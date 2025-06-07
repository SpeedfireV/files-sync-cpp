//
// Created by grzeg on 25/05/11.
//

#include "files_synchronizer.h"
#include "../file_handler/FileHandler.h"
#include <iostream>

FilesSynchronizer::FilesSynchronizer() {
    SyncFile::init();
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
    throw std::runtime_error("No file handlers with specified path");
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
    throw std::runtime_error("No file handlers with specified path");
}

void FilesSynchronizer::prepareSyncFiles(std::string const &path) {
    FileHandler& fh = getFileHandler(path);
    fh.getDifferences();
}

void FilesSynchronizer::prepareAllSyncFiles() {
    for (FileHandler& fh : dirsForSync) {
        fh.getDifferences();
    }
}

void FilesSynchronizer::syncDirectories(std::string const &path) {
    FileHandler& fh = getFileHandler(path);
    fh.updateAllFiles();
}

void FilesSynchronizer::syncAllDirectories() {
    for (FileHandler& fh : dirsForSync) {
        fh.updateAllFiles();
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
        // lastSyncs[fh] = 0; // Placeholder, implement actual logic
    }
    return lastSyncs;
}

FileHandler& FilesSynchronizer::operator[](const size_t& index) {
    return dirsForSync[index];
}