//
// Created by grzeg on 25/05/11.
//

#ifndef SYNC_FILE_H
#define SYNC_FILE_H
#include <map>
#include <string>
#include <vector>


class SyncFile {
private:
    std::string masterPath;
    std::string slavePath;
    time_t modificationDate; // last sync time
    std::vector<std::string> files;

public:
    SyncFile(std::string masterPath, std::string slavePath, time_t modificationDate, std::vector<std::string> files);

    static void toJson(SyncFile const& syncFile, const std::string&); // Save data from that class
    static std::vector<SyncFile> fromJson(const std::string&);

    static SyncFile getSyncFile(std::string masterPath, const std::string& slavePath);

    std::string& getMasterPath();
    std::string& getSlavePath();
    time_t getModificationDate();
    void changeJsonContents(const std::string &masterPath,
        const time_t &modificationDate, const std::vector<std::string> &files);
    const std::vector<std::string>& getFiles() const;
    void setModificationDate(time_t modificationDate);
    void setFiles(std::vector<std::string> files);

    static void init();
};



#endif //SYNC_FILE_H
