//
// Created by grzeg on 25/05/11.
//

#include "FileHandler.h"

#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include "../update_info/update_info.h"

[[nodiscard]] std::string FileHandler::getMasterPath() const {
    return masterPath;
}

[[nodiscard]] std::string FileHandler::getSlavePath() const {
    return slavePath;
}

std::string const& FileHandler::pathFromWhoEnum(WhoEnum who) {
    return who == Master ? masterPath : slavePath;
}

void FileHandler::updateFile(std::string fileRelativePath, WhoEnum from) {

    std::filesystem::copy_file(pathFromWhoEnum(from) + fileRelativePath, pathFromWhoEnum(from == Master ? Slave : Master) + fileRelativePath);

}

time_t FileHandler::fileModificationDate(std::string fileRelativePath, WhoEnum who) {
    std::string absolutePath = pathFromWhoEnum(who) + fileRelativePath;
    time_t modificationDate = std::filesystem::last_write_time(absolutePath).time_since_epoch().count();
    return modificationDate;
}

void FileHandler::readSyncFiles() {


}

std::vector<std::string> FileHandler::getDirectoresPaths(WhoEnum who) {
    std::vector<std::string> dirs = {};
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(pathFromWhoEnum(who)))
        if (dir_entry.is_directory()) {
            dirs.push_back(dir_entry.path().string());
        }
    return dirs;
}

FileHandler::~FileHandler() {
}

FileHandler::FileHandler(std::string masterPath, std::string slavePath) {
    FileHandler::masterPath = masterPath;
    FileHandler::slavePath = slavePath;
    masterSyncFile = SyncFile();
    slaveSyncFile = SyncFile();
}

std::vector<std::string> FileHandler::getFilesPathsInDirectory(WhoEnum who) {
    std::vector<std::string> filePaths = {};
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(pathFromWhoEnum(who)))
        if (dir_entry.is_regular_file()) {
            filePaths.push_back(dir_entry.path().string());
        }
    return filePaths;
}

std::map<std::string, UpdateInfo> FileHandler::getDifferences() const {
    auto masterFiles = masterSyncFile.getFilesSynchronizationDates();
    auto slaveFiles = slaveSyncFile.getFilesSynchronizationDates();
    std::map<std::string, UpdateInfo> differencesMap = {};
    for (auto i = masterFiles.begin(); i != masterFiles.end(); ++i) {
        std::string const& masterPath = i->first;
        time_t const& slaveModificationDate = slaveFiles[masterPath];
        time_t const& masterModificationDate = i->second;
        if (slaveFiles.contains(masterPath)) {
            // TODO: this does not compile, diagnose why
            // const UpdateInfo updateInfo (
            //     masterModificationDate >= slaveModificationDate ? Master : Slave,
            //     masterModificationDate != slaveModificationDate ? Updated : None
            // );
            // slaveFiles.erase(masterPath);
            // differencesMap[masterPath] = updateInfo;
        }
    }
    // TODO: Add remaining changes from slave files to differences map
    return differencesMap;

}

void FileHandler::updateFiles() {

}

time_t FileHandler::getModificationDate() {
    return 0;
}

void FileHandler::updateSyncFilesPaths() {
}

void FileHandler::deleteSyncFiles() {
}
