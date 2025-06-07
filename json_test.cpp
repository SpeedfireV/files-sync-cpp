//
// Created by wkrynski on 7.06.2025.
//
#include <iostream>
#include "files_synchronizer_lib/sync_file/SyncFile.cpp"


int main() {
    auto pairs = SyncFile::fromJson("C:/Users/wkrynski/Documents/GitHub/files-sync-cpp/sync_file.json");
    std::cout << pairs.size() << std::endl;
    std::cout << pairs[0].getMasterPath() << std::endl;
    std::cout << pairs[0].getSlavePath() << std::endl;
    pairs[0].setModificationDate(time_t(1656432000));
    std::cout << pairs[0].getModificationDate() <<std::endl;

    SyncFile::toJson(pairs[0], "C:/Users/wkrynski/Documents/GitHub/files-sync-cpp/sync_file.json");
    return 0;
}