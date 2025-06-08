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
    exit,
    syncAll  // <-- Added syncAll
};

StringCode stringToCode(std::string const& str) {
    if (str == "help") return StringCode::help;
    if (str == "add") return StringCode::add;
    if (str == "remove") return StringCode::remove;
    if (str == "list") return StringCode::list;
    if (str == "clear") return StringCode::clear;
    if (str == "exit") return StringCode::exit;
    if (str == "syncAll") return StringCode::syncAll;  // <-- Added syncAll
    return StringCode::none;
}

void printHelp() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help - prints this message" << std::endl;
    std::cout << "  add <master_path> <slave_path> - adds two files to synchronize" << std::endl;
    std::cout << "  remove <path> - removes directory from synchronization" << std::endl;
    std::cout << "  list - lists all directories for synchronization" << std::endl;
    std::cout << "  clear - clears all directories for synchronization" << std::endl;
    std::cout << "  syncAll - synchronizes all registered files" << std::endl;  // <-- Added line
    std::cout << "  exit - exits the program" << std::endl;
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

int main() {
    FilesSynchronizer fs;
    std::cout << "Welcome to files synchronizer\n";
    std::cout << "Type 'help' for list of commands\n";
    while (true) {
        std::string input;
        std::cout << ">_";
        std::getline(std::cin,  input);

        Command cmd = parseCommand(input);

        try {
            switch (stringToCode(cmd.name)) {
                case StringCode::help: {
                    if (cmd.arguments.empty()) {
                        printHelp();
                    } else {
                        std::cout << "ERROR: 'help' does not accept arguments\n";
                    }
                    break;
                }
                case StringCode::add: {
                    if (cmd.arguments.size() == 2) {
                        fs.addDirectories(cmd.arguments[0], cmd.arguments[1]);
                        std::cout << "Added files to sync\n";
                    } else {
                        std::cout << "ERROR: 'add' requires two arguments\n";
                    }
                    break;
                }
                case StringCode::syncAll: {
                    if (cmd.arguments.empty()) {
                        fs.syncAllFiles();
                        std::cout << "Synchronization complete\n";
                    } else {
                        std::cout << "ERROR: 'syncAll' does not accept arguments\n";
                    }
                    break;
                }
                case StringCode::remove: {
                    if (cmd.arguments.size() == 1) {
                        fs.removeDirectories(cmd.arguments[0]);
                        std::cout << "Removed directory from sync\n";
                    } else {
                        std::cout << "ERROR: 'remove' requires one argument\n";
                    }
                    break;
                }
                case StringCode::list: {
                    if (cmd.arguments.empty()) {
                        // TODO: implement list command
                        std::cout << "Directories for sync:\n";
                        if (fs.getDirectories().empty()) {
                            std::cout << "    None\n";
                            break;
                        }
                        for (auto directories = fs.getDirectories(); const auto& directory: directories) {
                            std::cout << "    " << directory << "\n";
                        }
                    } else {
                        std::cout << "ERROR: 'list' does not accept arguments\n";
                    }
                    break;
                }
                case StringCode::clear: {
                    if (cmd.arguments.empty()) {
                        fs.clearAllDirectories();
                        std::cout << "Cleared all directories\n";
                    } else {
                        std::cout << "ERROR: 'clear' does not accept arguments\n";
                    }
                    break;
                }
                case StringCode::exit: {
                    if (cmd.arguments.empty()) {
                        std::cout << "Exiting...\n";
                        return 0;
                    }
                    std::cout << "ERROR: 'exit' does not accept arguments\n";
                    break;
               }
                default:
                    std::cout << "ERROR: incorrect command, try 'help' for list of commands\n";
            }
        }
        catch (std::exception& e) {
            std::cout << "ERROR: " << e.what() << std::endl;
        }
    }
}