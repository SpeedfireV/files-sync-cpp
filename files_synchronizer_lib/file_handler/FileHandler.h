//
// Created by grzeg on 25/05/11.
//

#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H
#include <string>
#include <vector>

#include "../sync_file/SyncFile.h"
#include "../who_enum/who_enum.h"


class FileHandler {
private:
    time_t modificationDate; // now
    std::string masterPath;
    std::string slavePath;
    SyncFile masterSyncFile;
    SyncFile slaveSyncFile;

    void updateFile(std::string fileRelativePath, WhoEnum from) const;
    time_t fileModificationDate(std::string fileRelativePath, WhoEnum who) const;
    void readSyncFiles();

public:
    ~FileHandler(); // Delete all json files

    FileHandler(std::string masterPath, std::string slavePath);

    [[nodiscard]] std::string getMasterPath() const;
    [[nodiscard]] std::string getSlavePath() const;

    std::vector<std::string> getFilesPathsInDirectory() const;

    std::map<std::string, UpdateInfo> getDifferences() const;

    void updateFiles();

    time_t getModificationDate();

    void updateSyncFilesPaths();

    void deleteSyncFiles(); // Basically destructor




};



#endif //FILE_HANDLER_H
