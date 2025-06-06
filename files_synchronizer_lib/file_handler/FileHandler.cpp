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

}

void FileHandler::readSyncFiles() {
}

FileHandler::~FileHandler() {
}

FileHandler::FileHandler(std::string masterPath, std::string slavePath) {
}

std::vector<std::string> FileHandler::getFilesPathsInDirectory() const {
}

void FileHandler::updateFiles(std::string fileRelativePath) {
}

void FileHandler::updateSyncFilesPaths() {
}

void FileHandler::deleteSyncFiles() {
}
