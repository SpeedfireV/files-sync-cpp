//
// Created by grzeg on 25/05/11.
//

#include "files_synchronizer.h"
#include "../file_handler/FileHandler.h"
#include <iostream>

FilesSynchronizer::FilesSynchronizer(std::string const &masterPath, std::string const &slavePath) {
    addDirectories(masterPath, slavePath);
}

void FilesSynchronizer::addDirectories(std::string const &masterPath, std::string const &slavePath) {
    if (masterPath.empty() || slavePath.empty()) {
        return;
    }

    if (masterPath == slavePath) {
        return;
    }

    dirsForSync.emplace_back(masterPath, slavePath);
}

void FilesSynchronizer::removeDirectories(std::string const &path) {
    auto it = dirsForSync.begin();
    while (it != dirsForSync.end()) {
        if (it->getMasterPath() == path || it->getSlavePath() == path) {
            std::cout << "Removed directory" << std::endl;
            dirsForSync.erase(it);
            return;
        }
        ++it;
    }
    std::cout << "Directory not found" << std::endl;
}

void FilesSynchronizer::clearAllDirectories() {
    dirsForSync.clear();
}

void FilesSynchronizer::syncDirectories(std::string const &path) {
}

void FilesSynchronizer::syncAllDirectories() {
}

[[nodiscard]] std::vector<time_t> FilesSynchronizer::lastSyncs(std::string const &path) {
    return {};
}

std::map<std::string, time_t> FilesSynchronizer::lastSyncs() {
    return {};
}
