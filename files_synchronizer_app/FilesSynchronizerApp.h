#ifndef FILES_SYNCHRONIZER_APP_H
#define FILES_SYNCHRONIZER_APP_H

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "../files_synchronizer_lib/files_synchronizer/files_synchronizer.h"

enum class CommandCode {
    none = 0,
    help,
    add,
    remove,
    clear,
    list,
    sync,
    exit,
};

class FilesSynchronizerApp {
private:
    FilesSynchronizer fs;
    CommandCode cmd = CommandCode::none;
    unsigned char argc = 0;
    std::vector<std::string> argv;

    void stringToCode(std::string const& str);
    void parseCommand(const std::string& input);

    void help() const;
    void add();
    void remove();
    void clear();
    void list();
    void sync();
    void exit();

public:
    void run();
};

#endif //FILES_SYNCHRONIZER_APP_H
