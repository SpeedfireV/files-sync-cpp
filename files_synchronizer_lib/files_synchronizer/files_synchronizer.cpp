//
// Created by grzeg on 25/05/11.
//

#include "files_synchronizer.h"
#include "../file_handler/FilesHandler.h"
#include <iostream>

FilesSynchronizer::FilesSynchronizer() {
    std::vector<SyncFile> syncFiles =  SyncFile::init();
}

void FilesSynchronizer::addDirectories(std::string const &masterPath, std::string const &slavePath) {
    if (masterPath.empty() || slavePath.empty()) {
        return;
    }

    if (masterPath == slavePath) {
        return;
    }
    filesHandler.addPair(masterPath, slavePath);
    dirsForSync.emplace_back(masterPath);

}

void FilesSynchronizer::removeDirectories(std::string const &path) {
    auto it = dirsForSync.begin();
    while (it != dirsForSync.end()) {
        if (*it == path) {
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

FilesHandler& FilesSynchronizer::getFileHandler(std::string const& path) {
    return filesHandler;
}

const std::vector<std::string> & FilesSynchronizer::getDirectories() const {
    return dirsForSync;
}


// TODO: :)
void FilesSynchronizer::syncFiles(std::string const &path) {
    filesHandler.updateAllFiles();
}

void FilesSynchronizer::syncAllFiles() {
    filesHandler.updateAllFiles();
}
