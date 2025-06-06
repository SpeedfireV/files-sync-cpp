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

SyncFile::SyncFile(time_t modificationDate) {
    SyncFile::modificationDate = modificationDate;
}

SyncFile::SyncFile(std::map<std::string, time_t> json, time_t modificationDate) {
    SyncFile::modificationDate = modificationDate;
    filesSynchronizationDates = json;
}

SyncFile::~SyncFile() {
}

void SyncFile::toJson() const {
    std::ofstream JsonFile(std::filesystem::current_path() / "sync_file.json");
    JsonFile << std::to_string(modificationDate);
    JsonFile.close();
}

void SyncFile::fromJson() {
    std::string fileContents;
    std::ifstream JsonFile(std::filesystem::current_path() / "sync_file.json");
    while (std::getline(JsonFile, fileContents)) {
    }
    JsonFile.close();
}

std::map<std::string, UpdateInfo> SyncFile::getDifferences() {

}

time_t SyncFile::getModificationDate() {
    return modificationDate;
}

time_t SyncFile::getFileSynchronizationDate(std::string relativePath) {
    return filesSynchronizationDates[relativePath]; // TODO: Add assertions
}
