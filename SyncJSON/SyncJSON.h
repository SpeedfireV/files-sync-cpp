#ifndef SYNC_JSON_H
#define SYNC_JSON_H



#include <string>
#include <map>
#include <filesystem>
#include "json.hpp"
#include "../FilesHandler/FilesHandler.h"

class SyncJSON {
private:
    std::filesystem::path filePath;
public:
    explicit SyncJSON(const std::filesystem::path& filePath); // Specify the JSON file path as a run argument

    [[nodiscard]] std::map<std::string, FilesHandler> loadJSON() const; // Load data from JSON and return as a map
    void saveJSON(const std::map<std::string, FilesHandler>& data) const; // Save data to JSON from a map
};



#endif // SYNC_JSON_H