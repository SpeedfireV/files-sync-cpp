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

FileHandler& FilesSynchronizer::getFileHandler(std::string const& path) {
    for (FileHandler& fh : dirsForSync) {
        if (fh.getMasterPath() == path || fh.getSlavePath() == path) {
            return fh;
        }
    }
    throw std::runtime_error("Spierdalaj, nie ma takiego FileHandlera");
}

void FilesSynchronizer::syncDirectories(std::string const &path) {
    FileHandler& fh = getFileHandler(path);
    fh.updateFiles();
}

void FilesSynchronizer::syncAllDirectories() {
    for (FileHandler& fh : dirsForSync) {
        fh.updateFiles();
    }
}
time_t FilesSynchronizer::lastSyncs(std::string const& path) {
    // return getFileHandler(path).lastSync();
    return 0; // Placeholder, implement actual logic
}

std::map<FileHandler, time_t> FilesSynchronizer::lastSyncs() {
    std::map<FileHandler, time_t> lastSyncs;
    for (FileHandler& fh : dirsForSync) {
        // lastSyncs[fh] = fh.lastSync();
        lastSyncs[fh] = 0; // Placeholder, implement actual logic
    }
    return lastSyncs;
}