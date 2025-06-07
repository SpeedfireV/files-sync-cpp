//
// Created by grzeg on 25/05/11.
//

#include "FileHandler.h"

#include <filesystem>

[[nodiscard]] std::string FileHandler::getMasterPath() const {
    return masterPath;
}

[[nodiscard]] std::string FileHandler::getSlavePath() const {
    return slavePath;
}

void FileHandler::updateFile(std::string fileRelativePath, WhoEnum from) const {
}

time_t FileHandler::fileModificationDate(std::string fileRelativePath, WhoEnum who) const {
    std::string absolutePath = (who == WhoEnum::Master ? masterPath : slavePath) + fileRelativePath;
    time_t modificationDate = std::filesystem::last_write_time(absolutePath).time_since_epoch().count();
    return modificationDate;
}

void FileHandler::readSyncFiles() {
}

FileHandler::~FileHandler() {
}

FileHandler::FileHandler(std::string masterPath, std::string slavePath) {
    FileHandler::masterPath = masterPath;
    FileHandler::slavePath = slavePath;
    masterSyncFile = SyncFile();
    slaveSyncFile = SyncFile();
}

std::vector<std::string> FileHandler::getFilesPathsInDirectory() const {
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
            UpdateInfo updateInfo {
                masterModificationDate >= slaveModificationDate ? Master : Slave,
                masterModificationDate != slaveModificationDate ? Updated : None
            };
            differencesMap[masterPath] = updateInfo;
        }
    }
    return differencesMap;

}

void FileHandler::updateFiles() {

}

time_t FileHandler::getModificationDate() {
}

void FileHandler::updateSyncFilesPaths() {
}

void FileHandler::deleteSyncFiles() {
}
