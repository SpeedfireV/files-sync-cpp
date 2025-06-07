//
// Created by grzeg on 25/05/11.
//

#include "FileHandler.h"

[[nodiscard]] std::string FileHandler::getMasterPath() const {
    return masterPath;
}

[[nodiscard]] std::string FileHandler::getSlavePath() const {
    return slavePath;
}

void FileHandler::updateFile(std::string fileRelativePath, WhoEnum from) const {
}

time_t FileHandler::fileModificationDate(std::string fileRelativePath) const {
    return 0;
}

void FileHandler::readSyncFiles() {
}

FileHandler::~FileHandler() {
}

FileHandler::FileHandler(std::string masterPath, std::string slavePath) {
}

std::vector<std::string> FileHandler::getFilesPathsInDirectory() const {
    return {};
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
            // UpdateInfo updateInfo {
            //     masterModificationDate >= slaveModificationDate ? Master : Slave,
            //     masterModificationDate != slaveModificationDate ? Updated : None
            // };
            // differencesMap[masterPath] = updateInfo;
        }
    }
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
