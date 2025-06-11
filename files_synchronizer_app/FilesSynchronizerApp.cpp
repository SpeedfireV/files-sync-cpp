#include "FilesSynchronizerApp.h"

void FilesSynchronizerApp::stringToCode(std::string const &str) {
    if (str == "help") cmd = CommandCode::help;
    else if (str == "add") cmd = CommandCode::add;
    else if (str == "remove") cmd = CommandCode::remove;
    else if (str == "clear") cmd = CommandCode::clear;
    else if (str == "list") cmd = CommandCode::list;
    else if (str == "sync") cmd = CommandCode::sync;
    else if (str == "exit") cmd = CommandCode::exit;
    else cmd = CommandCode::none;
}

void FilesSynchronizerApp::parseCommand(const std::string &input) {
    std::istringstream iss(input);

    std::string command;
    iss >> command;

    stringToCode(command);

    std::string arg;

    argc = 0;
    argv.clear();
    while (iss >> std::quoted(arg)) {
        argc++;
        argv.push_back(arg);
    }
}

void FilesSynchronizerApp::help() const {
    if (argc != 0) {
        throw std::runtime_error("'help' does not accept arguments");
    }
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help - prints this message" << std::endl;
    std::cout << "  add <name> <path1> <path2> - adds an entry" << std::endl;
    std::cout << "  remove <name> - remove an entry" << std::endl;
    std::cout << "  clear - clears all entries" << std::endl;
    std::cout << "  list - lists all entries" << std::endl;
    std::cout << "  sync <name> - syncs an entry" << std::endl;
    std::cout << "  sync - syncs all entries" << std::endl;
    std::cout << "  exit - exits the program" << std::endl;
}

void FilesSynchronizerApp::add() {
    if (argc != 2) {
        throw std::runtime_error("'add' requires two arguments");
    }
    fs.addDirectories(argv[0], argv[1]);
    std::cout << "Added files to sync\n";
}

void FilesSynchronizerApp::remove() {
    if (argc != 1) {
        throw std::runtime_error("'remove' requires one argument");
    }
    fs.removeDirectories(argv[0]);
    std::cout << "Removed directory from sync\n";
}

void FilesSynchronizerApp::clear() {
    if (argc != 0) {
        throw std::runtime_error("'clear' does not accept arguments");
    }
    fs.clearAllDirectories();
    std::cout << "Cleared all directories from sync\n";
}

void FilesSynchronizerApp::list() {
    if (argc != 0) {
        throw std::runtime_error("'list' does not accept arguments");
    }
    std::cout << "Directories for sync:\n";
    if (fs.getDirectories().empty()) {
        std::cout << "    None\n";
        return;
    }
    for (const auto directories = fs.getDirectories(); const auto& directory: directories) {
        std::cout << "    " << directory << "\n";
    }
}

void FilesSynchronizerApp::sync() {
    if (argc != 0) {
        throw std::runtime_error("'sync' does not accept arguments");
    }
    fs.syncAllFiles();
    std::cout << "Synchronization complete\n";
}

void FilesSynchronizerApp::exit() {
    if (argc != 0) {
        throw std::runtime_error("'exit' does not accept arguments");
    }
    std::cout << "Exiting...\n";
}

void FilesSynchronizerApp::run() {
    std::cout << "Welcome to files synchronizer\n";
    std::cout << "Type 'help' for list of commands\n";

    while (true) {
        std::string input;
        std::cout << ">_";
        std::getline(std::cin, input);

        parseCommand(input);

        try {
            switch (cmd) {
                case CommandCode::help: {
                    help();
                    break;
                }
                case CommandCode::add: {
                    add();
                    break;
                }
                case CommandCode::remove: {
                    remove();
                    break;
                }
                case CommandCode::clear: {
                    clear();
                    break;
                }
                case CommandCode::list: {
                    list();
                    break;
                }
                case CommandCode::sync: {
                    sync();
                    break;
                }
                case CommandCode::exit: {
                    exit();
                    return;
                }
                default:
                    throw std::runtime_error("incorrect command, try 'help' for list of commands");
            }
        }
        catch (std::exception& e) {
            std::cout << "ERROR: " << e.what() << std::endl;
        }
    }
}
