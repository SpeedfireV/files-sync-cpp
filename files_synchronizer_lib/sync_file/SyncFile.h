//
// Created by grzeg on 25/05/11.
//

#ifndef SYNC_FILE_H
#define SYNC_FILE_H
#include <ctime>
#include <map>
#include <string>

#include "../update_info/update_info.h"


class SyncFile {private:
    time_t modificationDate; // now
    std::map<std::string, time_t> filesSynchronizationDates = std::map<std::string, time_t>();

public:
    SyncFile(); // time_t - now,
    SyncFile(std::map<std::string, time_t> json); // time_t - now,
    SyncFile(time_t modificationDate);
    SyncFile(std::map<std::string, time_t> json, time_t modificationDate);


    ~SyncFile(); // Delete jsons

    void toJson() const; // Save data from that class
    void fromJson(); // Save data to that class

    std::map<std::string, UpdateInfo> getDifferences();
    time_t getModificationDate();
    time_t getFileSynchronizationDate(std::string relativePath);
};



#endif //SYNC_FILE_H
