//
// Created by grzeg on 25/05/11.
//

#ifndef FILES_SYNCHRONIZER_H
#define FILES_SYNCHRONIZER_H
#include <map>
#include <string>
#include <vector>
#include "../file_handler/FilesHandler.h"

class FilesSynchronizer {
private:
    FilesHandler filesHandler = FilesHandler();
    std::vector<std::string> dirsForSync = {};
public:
    FilesSynchronizer();

    void addDirectories(std::string const& masterPath, std::string const& slavePath);

    void removeDirectories(std::string const& path); // Path might be master or slave
    void clearAllDirectories();

    FilesHandler& getFileHandler(std::string const& path);
    const std::vector<std::string>& getDirectories() const;

    // void prepareSyncFiles(std::string const& path);
    // void prepareAllSyncFiles();

    void syncFiles(std::string const& path); // Path might be master or slave
    void syncAllFiles();
    //
    // time_t lastSyncs(std::string const& path) ; // Path might be master or slave
    // std::map<FilesHandler, time_t> lastSyncs();
};



#endif //FILES_SYNCHRONIZER_H
