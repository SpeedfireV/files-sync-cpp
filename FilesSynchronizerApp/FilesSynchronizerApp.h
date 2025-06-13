#ifndef FILES_SYNCHRONIZER_APP_H
#define FILES_SYNCHRONIZER_APP_H

#include <string>
#include <vector>
#include "../FilesSynchronizer/FilesSynchronizer.h"

enum class CommandCode {
    none = 0,
    help,
    load,
    save,
    add,
    remove,
    clear,
    list,
    sync,
    conflicts,
    exit,
    yes,
    no
};

class FilesSynchronizerApp {
private:
    FilesSynchronizer fs;
    CommandCode cmd = CommandCode::none;
    unsigned char argc = 0;
    std::vector<std::string> argv;

    void getInput();
    void stringToCode(std::string const& str);

    void help() const;
    void load();
    void save();
    void add();
    void remove();
    void clear();
    void list() const;
    void sync();
    void conflicts() const;
    void exit() const;

public:
    explicit FilesSynchronizerApp(const std::string& json_path);
    void run();
};

#endif //FILES_SYNCHRONIZER_APP_H
