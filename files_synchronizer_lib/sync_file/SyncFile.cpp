//
// Created by grzeg on 25/05/11.
//

#include "SyncFile.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include "../nlohmann/json.hpp"
using json = nlohmann::json;

SyncFile::SyncFile(std::string masterPath, std::string slavePath, time_t modificationDate, std::vector<std::string> files) {
    SyncFile::masterPath = masterPath;
    SyncFile::slavePath = slavePath;
    SyncFile::modificationDate = modificationDate;
    SyncFile::files = files;
}

void SyncFile::toJson(SyncFile const& syncFile, const std::string& absolutePath) {
    // Step 1: Read the current JSON into memory
    std::vector<SyncFile> currentPairs = fromJson(absolutePath);

    // Step 2: Update or add the syncFile
    bool updated = false;
    for (auto& pair : currentPairs) {
        if (pair.masterPath == syncFile.masterPath) {
            pair = syncFile;
            std::cout << pair.modificationDate;
            updated = true;
            break;
        }
    }
    if (!updated) {
        currentPairs.push_back(syncFile);
    }

    // Step 3: Convert back to JSON structure
    json output;
    output["pairs"] = json::array();

    for (const auto& pair : currentPairs) {
        json entry;
        entry[pair.masterPath] = {
            {"slavePath", pair.slavePath},
            {"date", pair.modificationDate},
            {"files", pair.files}
        };
        output["pairs"].push_back(entry);
    }

    // Step 4: Write to file
    std::ofstream outFile(absolutePath);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open output file: " + absolutePath);
    }
    std::cout << output.dump(4);
    outFile << output.dump(4);  // Pretty-print with indentation
    outFile.close();
}


std::vector<SyncFile> SyncFile::fromJson(const std::string& absolutePath) {
    std::ifstream JsonFile(std::filesystem::current_path().string() + "/sync_file.json");
    if (!JsonFile.is_open()) {
        throw std::runtime_error("Could not open JSON file.");
    }
    json j;
    JsonFile >> j;
    JsonFile.close();
    std::vector<SyncFile> result;

    for (const auto& pair : j["pairs"]) {

        for (auto it = pair.begin(); it != pair.end(); ++it) {
            // TODO: Delete default values. Instead throw errors
            std::string masterPath = it.key(); // dynamic key
            const json& inner = it.value();
            std::string slavePath = inner.at("slavePath");
            time_t modificationDate = inner.at("date");
            std::vector<std::string> files = inner.at("files");
            auto syncFile = SyncFile(masterPath, slavePath, modificationDate, files);
            result.push_back(syncFile);
        }
    }

    return result;
}

SyncFile SyncFile::getSyncFile(std::string masterPath, const std::string& slavePath) {
    auto pairs = fromJson(masterPath);
    for (auto pair: pairs) {
        if (pair.masterPath == masterPath) {
            return pair;
        }
    }

    auto syncFile = SyncFile(masterPath, slavePath, time_t(), std::vector<std::string>());
    pairs.push_back(syncFile);

    return syncFile;
}

const std::vector<std::string>& SyncFile::getFiles() const {
    return files;
}

std::string& SyncFile::getMasterPath() {
    return masterPath;
}

std::string& SyncFile::getSlavePath() {
    return slavePath;
}

time_t SyncFile::getModificationDate() {
    return modificationDate;
}

void SyncFile::changeJsonContents(const std::string &masterPath,
    const time_t &modificationDate, const std::vector<std::string> &files) {
    std::vector<SyncFile> syncFiles = fromJson(std::filesystem::current_path().string());
    for (auto& syncFile: syncFiles) {
        if (syncFile.getMasterPath() == masterPath) {
            syncFile.setModificationDate(modificationDate);
            syncFile.setFiles(files);
            toJson(syncFile, std::filesystem::current_path().string());
            return;

        }
    }
    throw std::runtime_error("No sync file with specified master path");
}


void SyncFile::setModificationDate(time_t modificationDate) {
    std::cout << modificationDate;
    SyncFile::modificationDate = modificationDate;
}

void SyncFile::setFiles(std::vector<std::string> files) {
    this->files = files;
}

void SyncFile::init() {

    if (!exists(std::filesystem::path(std::filesystem::current_path().string() + "/sync_file.json"))) {
        json output;
        output["pairs"] = json::array();
        std::ofstream outFile(std::filesystem::current_path().string() + "/sync_file.json");
        if (!outFile.is_open()) {
            throw std::runtime_error("Failed to open output file: " + std::filesystem::current_path().string() + "/sync_file.json");
        }
        std::cout << output.dump(4);
        outFile << output.dump(4);  // Pretty-print with indentation
        outFile.close();
    }
}
