#include "SyncJSON.h"

#include <fstream>
#include <iomanip>

SyncJSON::SyncJSON(const std::filesystem::path& filePath) {
    if (!std::filesystem::exists(filePath)) {
        throw std::runtime_error("File does not exist");
    }
    if (!std::filesystem::is_regular_file(filePath)) {
        throw std::runtime_error("File is not a regular file");
    }
    this->filePath = std::filesystem::canonical(filePath);
}

std::map<std::string, FilesHandler> SyncJSON::loadJSON() const {
    if (filePath.empty()) {
        throw std::runtime_error("JSON file path is empty");
    }
    std::map<std::string, FilesHandler> result;

    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("JSON file could not be opened");
    }

    nlohmann::json json;
    file >> json;

    for (const auto& [entryName, entry] : json.items()) {
        std::filesystem::path firstPath = std::filesystem::canonical(entry["firstPath"]);
        std::filesystem::path secondPath = std::filesystem::canonical(entry["secondPath"]);
        time_t lastSync = entry["lastSync"];
        std::vector<std::string> files_string = entry["files"];
        std::vector<std::filesystem::path> files;
        for (auto& file_string : files_string) {
            files.emplace_back(file_string);
        }

        result.try_emplace(entryName, FilesHandler(firstPath, secondPath, lastSync, files));
    }

    return result;
}

void SyncJSON::saveJSON(const std::map<std::string, FilesHandler> &data) const {
    if (filePath.empty()) {
        throw std::runtime_error("JSON file path is empty");
    }
    if (std::filesystem::exists(filePath)) {
        std::filesystem::remove(filePath.string() + ".old");
        std::filesystem::copy_file(
            filePath,
            filePath.string() + ".old",
            std::filesystem::copy_options::overwrite_existing
        );
    }

    try {
        nlohmann::json json;

        for (const auto& [entryName, handler] : data) {
            std::vector<std::string> files_string;
            for (auto& file : handler.getFiles()) {
                files_string.emplace_back(file.generic_string());
            }
            json[entryName] = {
                {"firstPath", handler.getPath1().generic_string()},
                {"secondPath", handler.getPath2().generic_string()},
                {"lastSync", handler.getLastSync()},
                {"files", files_string}
            };
        }

        std::ofstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("JSON file could not be opened");
        }

        file << std::setw(4) << json << std::endl;
        if (file.fail()) {
           throw std::runtime_error("JSON file could not be written");
        }
    }
    catch (std::exception& e) {
        if (std::filesystem::exists(filePath.string() + ".old")) {
            std::filesystem::copy_file(
                filePath.string() + ".old",
                filePath,
                std::filesystem::copy_options::overwrite_existing
            );
        }
        throw;
    }
}
