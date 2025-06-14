#ifndef FILES_SYNCHRONIZER_H
#define FILES_SYNCHRONIZER_H

#include <string>
#include <map>
#include "../FilesHandler/FilesHandler.h"
#include "../SyncJSON/SyncJSON.h"

class FilesSynchronizer {
private:
    std::map<std::string, FilesHandler> fileHandlers; // all FilesHandler objects to synchronize
    SyncJSON syncJSON; // An object for saving and loading a JSON file
public:
    explicit FilesSynchronizer(const std::string& json_path); // Specify JSON file path at initialization

    void setJSONPath(const std::string& json_path); // Set JSON file path

    [[nodiscard]] const std::map<std::string, FilesHandler>& getFileHandlers() const;
    [[nodiscard]] const SyncJSON& getSyncJSON() const;

    void addEntry(const std::string& name, const std::string& path1, const std::string& path2);

    void removeEntry(const std::string& name);
    void removeAllEntries();

    void listEntry(const std::string& name) const;
    void listAllEntries() const;

    void syncEntry(const std::string& name);
    void syncAllEntries();

    void listConflicts(const std::string& name) const;
    void listAllConflicts() const;

    void resolveConflicts(const std::string& name, ChangeSource source);

    void loadData();
    void saveData() const;
};

#endif //FILES_SYNCHRONIZER_H