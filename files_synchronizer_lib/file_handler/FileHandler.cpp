//
// Created by grzeg on 25/05/11.
//

#include "FileHandler.h"

#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <direct.h>

#include "../update_info/update_info.h"
#include "../sync_file/SyncFile.h"

[[nodiscard]] const std::string& FileHandler::pathFromWhoEnum(const WhoEnum who) const {
    return who == Master ? masterPath : slavePath;
}

void FileHandler::updateFile(const std::string &fileRelativePath, UpdateInfo updateInfo) {
    std::string fromPath = pathFromWhoEnum(updateInfo.getNewer()) + fileRelativePath;
    std::string toPath =  pathFromWhoEnum(updateInfo.getNewer() == Master ? Slave : Master) + fileRelativePath;
    if (updateInfo.getModificationEvent() == Removed && exists(std::filesystem::path(toPath))) {
        std::filesystem::remove(toPath);
    }
    else if (updateInfo.getModificationEvent() != None) {
        std::filesystem::copy(fromPath, toPath,std::filesystem::copy_options::overwrite_existing);
    }
}

void FileHandler::updateAllFiles() {
    for (auto file : updates) {
        updateFile(file.first, file.second);
    }
    auto filesMap = getFilesPathsInDirectory(Master);
    std::vector<std::string> files = {};
    for (auto it = filesMap.begin(); it != filesMap.end(); ++it)
    {
        // Add the key to the vector
        files.push_back(it->first);
    }
    syncFile.changeJsonContents(masterPath, modificationDate, files);
}

[[nodiscard]] const std::string& FileHandler::getMasterPath() const {
    return masterPath;
}

[[nodiscard]] const std::string& FileHandler::getSlavePath() const {
    return slavePath;
}

time_t FileHandler::fileModificationDate(const std::string& fileRelativePath, const WhoEnum who) const {
    const std::string absolutePath = pathFromWhoEnum(who) + "/" + fileRelativePath;
    const auto ftime = std::filesystem::last_write_time(absolutePath);
    const auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - std::filesystem::file_time_type::clock::now()
        + std::chrono::system_clock::now()
    );
    return std::chrono::system_clock::to_time_t(sctp);
}

FileHandler::FileHandler(const std::string& masterPath, const std::string& slavePath)
    : modificationDate(0)
    , masterPath(masterPath)
    , slavePath(slavePath)
    , syncFile(SyncFile::getSyncFile(masterPath, slavePath)) {
    SyncFile::addPair(masterPath, slavePath);
    auto filesMap = getFilesPathsInDirectory(Master);
    std::vector<std::string> files {};
    for (auto it = filesMap.begin(); it != filesMap.end(); ++it)
    {
        // Add the key to the vector
        files.push_back(it->first);
    }
    auto syncFile = SyncFile(masterPath, slavePath, time_t(), files);
}

std::map<std::string, time_t> FileHandler::getFilesPathsInDirectory(const WhoEnum who) const {
    std::map<std::string, time_t> filePaths = {};
    std::string directoryPath = pathFromWhoEnum(who);
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            auto relativePath = std::filesystem::relative(entry.path(), directoryPath);
            filePaths[relativePath.string()] = fileModificationDate(relativePath.string(), who);
        }
    }
    return filePaths;
}

void FileHandler::getDifferences() {
    const auto oldFiles = syncFile.getFiles();
    const auto masterFiles = getFilesPathsInDirectory(WhoEnum::Master);
    const auto slaveFiles = getFilesPathsInDirectory(WhoEnum::Slave);
    std::map<std::string, UpdateInfo> differencesMap = {};
    for (auto & filePath : oldFiles) {
        time_t masterModificationDate = 0;
        if (masterFiles.contains(filePath)) {masterModificationDate = masterFiles.at(filePath);}
        time_t slaveModificationDate = 0;
        if (slaveFiles.contains(filePath)) {slaveModificationDate = slaveFiles.at(filePath);}

        if (masterModificationDate == 0 and slaveModificationDate <= modificationDate) {
            differencesMap[filePath] = UpdateInfo(WhoEnum::Master, ModificationEvent::Removed);
        } else if (masterModificationDate <= modificationDate and slaveModificationDate == 0) {
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
    this->updates = differencesMap;
}
