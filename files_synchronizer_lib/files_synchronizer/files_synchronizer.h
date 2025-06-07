//
// Created by grzeg on 25/05/11.
//

#ifndef FILES_SYNCHRONIZER_H
#define FILES_SYNCHRONIZER_H
#include <map>
#include <string>
#include <vector>
#include "../file_handler/FileHandler.h"

class FilesSynchronizer {
private:
    std::vector<FileHandler> dirsForSync = std::vector<FileHandler>();
public:
    FilesSynchronizer();

    void addDirectories(std::string const& masterPath, std::string const& slavePath);

    void removeDirectories(std::string const& path); // Path might be master or slave
    void clearAllDirectories();

    FileHandler& getFileHandler(std::string const& path);

    void prepareSyncFiles(std::string const& path);
    void prepareAllSyncFiles();

    void syncDirectories(std::string const& path); // Path might be master or slave
    void syncAllDirectories();

    time_t lastSyncs(std::string const& path) ; // Path might be master or slave
    std::map<FileHandler, time_t> lastSyncs();

    FileHandler& operator[](const size_t& index);
};



#endif //FILES_SYNCHRONIZER_H
