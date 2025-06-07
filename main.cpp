#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "files_synchronizer_lib/files_synchronizer/files_synchronizer.h"

struct Command {
    std::string name;
    std::vector<std::string> arguments;
};

enum class StringCode {
    none = 0,
    help,
    add,
    remove,
    list,
    clear,
    exit
};

StringCode stringToCode(std::string const& str) {
    if (str == "help") return StringCode::help;
    if (str == "add") return StringCode::add;
    if (str == "remove") return StringCode::remove;
    if (str == "list") return StringCode::list;
    if (str == "clear") return StringCode::clear;
    if (str == "exit") return StringCode::exit;
    return StringCode::none;
}

Command parseCommand(const std::string& input) {
    std::istringstream iss(input);
    Command cmd;

    iss >> cmd.name;

    std::string arg;
    while (iss >> std::quoted(arg)) {
        cmd.arguments.push_back(arg);
    }
    
    return cmd;
}

void printHelp() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help - prints this message" << std::endl;
    std::cout << "  add <master_path> <slave_path> - adds two files to synchronize" << std::endl;
    std::cout << "  remove <path> - removes directory from synchronization" << std::endl;
    std::cout << "  list - lists all directories for synchronization" << std::endl;
    std::cout << "  clear - clears all directories for synchronization" << std::endl;
    std::cout << "  exit - exits the program" << std::endl;
}

int main() {
    FilesSynchronizer fs;
    while (true) {
        std::string input;
        std::cout << ">_";
        std::getline(std::cin,  input);

        Command cmd = parseCommand(input);

        try {
            switch (stringToCode(cmd.name)) {
                case StringCode::help: {
                    if (!cmd.arguments.empty()) {
                        std::cout << "ERROR: 'help' does not accept arguments\n";
                    } else {
                        printHelp();
                    }
                    break;
                }
                case StringCode::add: {
                    if (cmd.arguments.size() == 2) {
                        fs.addDirectories(cmd.arguments[0], cmd.arguments[1]);
                        std::cout << "Added files to sync\n";
                        std::string masterPath = cmd.arguments[0];
                        std::string slavePath = cmd.arguments[1];
                    } else {
                        std::cout << "ERROR: 'add' requires two arguments\n";
                    }
                    break;
                }
                case StringCode::remove: {
                    if (cmd.arguments.size() == 1) {
                        std::cout << "Removed directory from sync\n";
                        std::string path = cmd.arguments[0];
                    } else {
                        std::cout << "ERROR: 'remove' requires one argument\n";
                    }
                }
                default:
                    std::cout << "ERROR: incorrect command, try help\n";
            }
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
}