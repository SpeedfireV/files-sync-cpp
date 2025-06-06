//
// Created by grzeg on 25/05/11.
//

#include "SyncFile.h"

#include <filesystem>
#include <fstream>

SyncFile::SyncFile() {
}

SyncFile::SyncFile(std::map<std::string, time_t> json) {
    filesSynchronizationDates = json;
}

SyncFile::~SyncFile() {
}

void SyncFile::toJson(std::string absolutePath) const {
    std::ofstream JsonFile(absolutePath + "sync_file.json");
    JsonFile.close();
}

// TODO: Implement fromJson
void SyncFile::fromJson(std::string absolutePath) {
    std::string fileContents;
    std::ifstream JsonFile(absolutePath + "sync_file.json");

    while (std::getline(JsonFile, fileContents)) {
    }
    JsonFile.close();
}

std::map<std::string, time_t> SyncFile::getFilesSynchronizationDates() const {
    return filesSynchronizationDates;
}

time_t SyncFile::getFileSynchronizationDate(std::string relativePath) {
    return filesSynchronizationDates[relativePath]; // TODO: Add assertions
}
