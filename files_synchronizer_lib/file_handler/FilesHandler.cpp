//
// Created by grzeg on 25/05/11.
//

#include "FilesHandler.h"

#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <direct.h>
#include <iostream>

#include "../update_info/update_info.h"
#include "../sync_file/SyncFile.h"

[[nodiscard]] const std::string& FilesHandler::pathFromWhoEnum(SyncFile syncFile, const WhoEnum who) const {
    return who == Master ? syncFile.getMasterPath() : syncFile.getSlavePath();
}

void FilesHandler::updateFile(SyncFile syncFile, const std::string &fileRelativePath, UpdateInfo updateInfo) {
    std::cout << "UPDATE FILE!" << std::endl;
    std::string fromPath = pathFromWhoEnum(syncFile, updateInfo.getNewer()) + fileRelativePath;
    std::string toPath =  pathFromWhoEnum(syncFile, updateInfo.getNewer() == Master ? Slave : Master) + fileRelativePath;
    if (updateInfo.getModificationEvent() == Removed && exists(std::filesystem::path(toPath))) {
        std::filesystem::remove(toPath);
    }
    else if (updateInfo.getModificationEvent() != None) {
        std::filesystem::copy(fromPath, toPath,std::filesystem::copy_options::overwrite_existing);
    }
}

void FilesHandler::updateAllFiles() {
    std::vector<SyncFile> syncFiles = SyncFile::fromJson();
    for (auto syncFile: syncFiles) {
        std::cout << syncFile.getMasterPath() << std::endl;
        auto updates = getDifferences(syncFile);
        std::vector<std::string> filesPaths = {};
        for (auto it = updates.begin(); it != updates.end(); ++it)
        {
            filesPaths.push_back(it->first);
            std::cout << it->second.getModificationEvent() << " " << it->first << std::endl;
            updateFile(syncFile, it->first, it->second);
        }
        auto filesMap = getFilesPathsInDirectory(syncFile, Master);
        syncFile.changeJsonContents(syncFile.getMasterPath(), syncFile.getModificationDate(), filesPaths);
    }
}

void FilesHandler::addPair(const std::string &masterPath, const std::string &slavePath) {
    SyncFile::addPair(masterPath, slavePath);
}


time_t FilesHandler::fileModificationDate(SyncFile syncFile, const std::string& fileRelativePath, const WhoEnum who) const {
    const std::string absolutePath = pathFromWhoEnum(syncFile, who) + "/" + fileRelativePath;
    const auto ftime = std::filesystem::last_write_time(absolutePath);
    const auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - std::filesystem::file_time_type::clock::now()
        + std::chrono::system_clock::now()
    );
    return std::chrono::system_clock::to_time_t(sctp);
}

FilesHandler::FilesHandler()
{
    syncFiles =  SyncFile::fromJson();
    masterPaths = {};
    for (auto syncFile: syncFiles) {
        masterPaths.push_back(syncFile.getMasterPath());
    }
}

std::map<std::string, time_t> FilesHandler::getFilesPathsInDirectory(SyncFile syncFile, const WhoEnum who) const {
    std::map<std::string, time_t> filePaths = {};
    std::string directoryPath = pathFromWhoEnum(syncFile, who);
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            auto relativePath = std::filesystem::relative(entry.path(), directoryPath);
            filePaths[relativePath.string()] = fileModificationDate(syncFile, relativePath.string(), who);
        }
    }
    return filePaths;
}

// TODO: getDifferences doesn't return differences although there are some
std::map<std::string, UpdateInfo> FilesHandler::getDifferences(SyncFile syncFile) {
    const auto oldFiles = syncFile.getFiles();
    const auto masterFiles = getFilesPathsInDirectory(syncFile, WhoEnum::Master);
    for (auto it = masterFiles.begin(); it != masterFiles.end(); ++it)
    {
        std::cout << it->first << " THERE IS THAT FILE" << std::endl;
    }
    const auto slaveFiles = getFilesPathsInDirectory(syncFile, WhoEnum::Slave);
    std::map<std::string, UpdateInfo> differencesMap = {};
    for (auto & filePath : oldFiles) {
        time_t masterModificationDate = 0;
        if (masterFiles.contains(filePath)) {masterModificationDate = masterFiles.at(filePath);}
        time_t slaveModificationDate = 0;
        if (slaveFiles.contains(filePath)) {slaveModificationDate = slaveFiles.at(filePath);}

        if (masterModificationDate == 0 and slaveModificationDate <= syncFile.getModificationDate()) {
            differencesMap[filePath] = UpdateInfo(WhoEnum::Master, ModificationEvent::Removed);
        } else if (masterModificationDate <= syncFile.getModificationDate() and slaveModificationDate == 0) {
            differencesMap[filePath] = UpdateInfo(WhoEnum::Slave, ModificationEvent::Removed);
        } else if (masterModificationDate >= slaveModificationDate) {
            differencesMap[filePath] = UpdateInfo(WhoEnum::Master, ModificationEvent::Updated);
        } else if (masterModificationDate < slaveModificationDate) {
            differencesMap[filePath] = UpdateInfo(WhoEnum::Slave, ModificationEvent::Updated);
        }
    }
    std::map<std::string, time_t> newFiles = {};
    for (auto & file : masterFiles) {
        if (std::find(oldFiles.begin(), oldFiles.end(), file.first) == oldFiles.end()) {
            newFiles[file.first] = file.second;
        }
    }
    for (auto & file : slaveFiles) {
        if (std::find(oldFiles.begin(), oldFiles.end(), file.first) != oldFiles.end()) {
            continue;
        } if (newFiles.contains(file.first) and newFiles.at(file.first) > file.second) {
            differencesMap[file.first] = UpdateInfo(WhoEnum::Master, ModificationEvent::Created);
        } else {
            differencesMap[file.first] = UpdateInfo(WhoEnum::Slave, ModificationEvent::Created);
        }
    }
    for (auto it = differencesMap.begin(); it != differencesMap.end(); ++it)
    {
        std::cout << it->first << " there's a difference";
    }
    this->updates = differencesMap;
    return differencesMap;
}
