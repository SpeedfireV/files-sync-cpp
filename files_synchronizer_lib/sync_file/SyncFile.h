//
// Created by grzeg on 25/05/11.
//

#ifndef SYNC_FILE_H
#define SYNC_FILE_H
#include <ctime>
#include <map>
#include <string>

#include "../update_info/update_info.h"


class SyncFile {
private:
    std::map<std::string, time_t> filesSynchronizationDates = std::map<std::string, time_t>();

public:
    SyncFile(); // time_t - now,
    SyncFile(std::map<std::string, time_t> json); // time_t - now,


    ~SyncFile(); // Delete jsons

    void toJson(std::string absolutePath) const; // Save data from that class
    void fromJson(std::string absolutePath); // Save data to that class

    std::map<std::string, time_t> getFilesSynchronizationDates() const;
    time_t getFileSynchronizationDate(std::string relativePath);
};



#endif //SYNC_FILE_H
