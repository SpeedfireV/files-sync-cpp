#ifndef FILES_HANDLER_H
#define FILES_HANDLER_H

#include <map>
#include <vector>
#include <filesystem>
#include "../UpdateData/UpdateData.h"

class FilesHandler {
private:
    time_t last_sync{}; // last sync time
    std::filesystem::path path1; // absolute path to the first directory
    std::filesystem::path path2; // absolute path to the second directory
    std::vector<std::filesystem::path> files; // an entry tracking all files in the entry
    std::map<std::filesystem::path, UpdateData> updates; // Helper entry for tracking updates

    void setLastSync(time_t last_sync); // Sets last sync time
    void setPath1(const std::filesystem::path& path1); // Sets path1
    void setPath2(const std::filesystem::path& path2); // Sets path2
    void setFiles(const std::vector<std::filesystem::path>& files); // Sets files
    void setUpdates(const std::map<std::filesystem::path, UpdateData>& updates); // Sets updates

    static time_t getModificationDate(const std::filesystem::path& path); // Returns modification date of the file
    void syncFile(const std::filesystem::path& path, const UpdateData& update_data) const; // Syncs file
public:
    FilesHandler(
        const std::filesystem::path& path1,
        const std::filesystem::path& path2,
        time_t last_sync,
        const std::vector<std::filesystem::path>& files);

    [[nodiscard]] const std::filesystem::path& getPath1() const; // Returns path1
    [[nodiscard]] const std::filesystem::path& getPath2() const; // Returns path2
    [[nodiscard]] time_t getLastSync() const; // Returns last sync time
    [[nodiscard]] const std::vector<std::filesystem::path>& getFiles() const; // Returns files
    [[nodiscard]] const std::map<std::filesystem::path, UpdateData>& getUpdates() const; // Returns updates
    [[nodiscard]] std::map<std::filesystem::path, ChangeType> getConflicts() const;

    static std::vector<std::filesystem::path> findFiles(const std::filesystem::path& path);
    void findUpdates(); // Searches for changes in the file system
    void syncFiles(); // sync files
};

std::ostream& operator<<(std::ostream& os, const FilesHandler& fh); // Overload operator<< for printing

#endif //FILES_HANDLER_H