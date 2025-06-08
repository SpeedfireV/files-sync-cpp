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


class FilesHandler {
private:
    std::vector<SyncFile> syncFiles;
    std::vector<std::string> masterPaths;

    std::map<std::string, UpdateInfo> updates;
    [[nodiscard]] const std::string& pathFromWhoEnum(SyncFile syncFile, const WhoEnum who) const;


    void updateFile(SyncFile SyncFile, const std::string& fileRelativePath, UpdateInfo updateInfo);
    time_t fileModificationDate(SyncFile syncFile, const std::string& fileRelativePath, WhoEnum who) const;
    void readSyncFiles();

public:
    ~FilesHandler() = default; // Save all JSON files

    FilesHandler(); // init with master and slave

    std::map<std::string, time_t> getFilesPathsInDirectory(SyncFile syncFile, WhoEnum who) const; // returns all files in the directory

    std::map<std::string, UpdateInfo> getDifferences(SyncFile syncFile); // returns differences

    void updateAllFiles(); // updates all files in the directory

    void addPair(const std::string& masterPath, const std::string& slavePath);
};



#endif //FILE_HANDLER_H
