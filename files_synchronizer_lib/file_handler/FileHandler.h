//
// Created by grzeg on 25/05/11.
//

#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H
#include <string>
#include <map>

#include "../sync_file/SyncFile.h"
#include "../who_enum/who_enum.h"
#include "../update_info/update_info.h"


class FileHandler {
private:
    time_t modificationDate; // last sync time
    std::string masterPath; // absolute path to master
    std::string slavePath; // absolute path to slave
    SyncFile syncFile; // sync file
    std::map<std::string, UpdateInfo> updates;

    [[nodiscard]] const std::string& pathFromWhoEnum(WhoEnum who) const;

    void updateFile(const std::string& fileRelativePath, UpdateInfo updateInfo);
    time_t fileModificationDate(const std::string& fileRelativePath, WhoEnum who) const;
    void readSyncFiles();

public:
    ~FileHandler() = default; // Save all JSON files

    FileHandler(const std::string& masterPath, const std::string& slavePath); // init with master and slave

    [[nodiscard]] const std::string& getMasterPath() const; // returns an absolute path to master
    [[nodiscard]] const std::string& getSlavePath() const; // returns an absolute path to slave

    std::map<std::string, time_t> getFilesPathsInDirectory(WhoEnum who) const; // returns all files in the directory

    void getDifferences(); // returns differences

    void updateAllFiles(); // updates all files in the directory
};



#endif //FILE_HANDLER_H
