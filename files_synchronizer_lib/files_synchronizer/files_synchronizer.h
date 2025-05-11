//
// Created by grzeg on 25/05/11.
//

#ifndef FILES_SYNCHRONIZER_H
#define FILES_SYNCHRONIZER_H
#include <map>
#include <string>
#include <vector>


class FilesSynchronizer {
private:
    std::string childPathFromParentPath(std::string parentPath);
public:
    FilesSynchronizer();
    FilesSynchronizer(std::string const& parentPath, std::string const& childPath);
    FilesSynchronizer(std::map<std::string const&, std::string const&> dirs);

    void addDirectories(std::string const& parentPath, std::string const& childPath);


    void removeDirectories(std::string const& parentPath);
    void removeDirectory(std::string const& parentPath, std::string const& childPath);
    void removeDirectories(std::map<std::string const&, std::string const&> const& dirs);
    void clearAllDirectories();

    void syncDirectories(std::string const& parentPath);
    void syncDirectory(std::string const& parentPath, std::string const& childPath);
    void syncDirectories(std::map<std::string const &, std::string const &> const& paths);
    void syncAllDirectories();

    std::vector<time_t> lastSyncs(std::string const& parentPath) const;
    time_t lastSync(std::string const& parentPath, std::string const& childPath) const;
    std::vector<time_t> lastSyncs(std::map<std::string const &, std::string const & > const &) const;
    std::map<std::string, time_t> lastSyncs();
};



#endif //FILES_SYNCHRONIZER_H
