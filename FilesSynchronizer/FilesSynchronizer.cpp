#include "FilesSynchronizer.h"

#include <iostream>
#include <filesystem>

FilesSynchronizer::FilesSynchronizer(const std::string &json_path)
    : syncJSON(std::filesystem::canonical(json_path)) {
    setJSONPath(json_path);
}

void FilesSynchronizer::setJSONPath(const std::string &json_path) {
    if (json_path.empty()) {
        throw std::runtime_error("JSON path cannot be empty");
    }
    if (!std::filesystem::is_regular_file(json_path)) {
        throw std::runtime_error("JSON path is not a regular file");
    }
    std::filesystem::path path = std::filesystem::canonical(json_path);
    if (path.extension() != ".json") {
        throw std::runtime_error("JSON path does not have .json extension");
    }
    syncJSON = SyncJSON(json_path);
}

const std::map<std::string, FilesHandler> & FilesSynchronizer::getFileHandlers() const {
    return fileHandlers;
}

const SyncJSON & FilesSynchronizer::getSyncJSON() const {
    return syncJSON;
}

void FilesSynchronizer::addEntry(const std::string &name, const std::string &path1, const std::string &path2) {
    if (fileHandlers.contains(name)) {
        throw std::runtime_error("Entry with name " + name + " already exists");
    }
    for (auto& [entry_name, handler] : fileHandlers) {
        if (handler.getPath1() == path1) {
            throw std::runtime_error(path1 + " already exists in entry " + entry_name);
        }
        if (handler.getPath2() == path2) {
            throw std::runtime_error(path2 + " already exists in entry " + entry_name);
        }
    }
    fileHandlers.emplace(name, FilesHandler(
        std::filesystem::path(path1),
        std::filesystem::path(path2),
        0, {}));
}

void FilesSynchronizer::removeEntry(const std::string &name) {
    if (!fileHandlers.contains(name)) {
        throw std::runtime_error("Entry with name " + name + " does not exist");
    }
    fileHandlers.erase(name);
}

void FilesSynchronizer::removeAllEntries() {
    fileHandlers.clear();
}

void FilesSynchronizer::listEntry(const std::string &name) const {
    if (!fileHandlers.contains(name)) {
        throw std::runtime_error("Entry with name " + name + " does not exist");
    }
    std::cout << "Entry " << name << ":\n";
    std::cout << fileHandlers.at(name);
}

void FilesSynchronizer::listAllEntries() const {
    for (auto& [name, handler] : fileHandlers) {
        std::cout << "Entry " << name << ":\n";
        std::cout << handler << "\n";
    }
}

void FilesSynchronizer::syncEntry(const std::string &name) {
    if (!fileHandlers.contains(name)) {
        throw std::runtime_error("Entry with name " + name + " does not exist");
    }
    fileHandlers.at(name).syncFiles();
}

void FilesSynchronizer::syncAllEntries() {
    for (auto &handler: fileHandlers | std::views::values) {
        handler.syncFiles();
    }
}

void FilesSynchronizer::listConflicts(const std::string &name) const {
    if (!fileHandlers.contains(name)) {
        throw std::runtime_error("Entry with name " + name + " does not exist");
    }
    std::map<std::filesystem::path, ChangeType> conflicts = fileHandlers.at(name).getConflicts();
    std::cout << "Conflicts in entry " << name << ":\n";
    if (conflicts.empty()) {
        std::cout << "No conflicts\n";
        return;
    }
    for (const auto &[path, change] : conflicts) {
        std::string change_str;
        if (change == Created) {
            change_str = "created";
        }
        else if (change == Updated) {
            change_str = "updated";
        }
        std::cout << path.generic_string() << " - " << change_str << "\n";
    }
}

void FilesSynchronizer::listAllConflicts() const {
    for (const auto &name: fileHandlers | std::views::keys) {
        listConflicts(name);
        std::cout << "\n";
    }
}

void FilesSynchronizer::loadData() {
    fileHandlers = syncJSON.loadJSON();
}

void FilesSynchronizer::saveData() const {
    syncJSON.saveJSON(fileHandlers);
}
