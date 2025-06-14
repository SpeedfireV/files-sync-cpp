#include "FilesSynchronizerApp.h"

#include <iostream>

void FilesSynchronizerApp::getInput() {
    std::string input;
    std::cout << ">_";
    std::getline(std::cin, input);

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

void FilesSynchronizerApp::stringToCode(std::string const &str) {
    if (str == "help") cmd = CommandCode::help;
    else if (str == "add") cmd = CommandCode::add;
    else if (str == "load") cmd = CommandCode::load;
    else if (str == "save") cmd = CommandCode::save;
    else if (str == "remove") cmd = CommandCode::remove;
    else if (str == "clear") cmd = CommandCode::clear;
    else if (str == "list") cmd = CommandCode::list;
    else if (str == "sync") cmd = CommandCode::sync;
    else if (str == "conflicts") cmd = CommandCode::conflicts;
    else if (str == "resolve") cmd = CommandCode::resolve;
    else if (str == "exit") cmd = CommandCode::exit;
    else if (str == "yes") cmd = CommandCode::yes;
    else if (str == "no") cmd = CommandCode::no;
    else cmd = CommandCode::none;
}

bool FilesSynchronizerApp::areYouSure() {
    std::cout << "Are you sure? (yes/no)\n";
    while (true) {
        getInput();
        if (argc != 0 or (cmd != CommandCode::yes and cmd != CommandCode::no)) {
            std::cout << "Invalid input, are you sure? (yes/no)\n";
            continue;
        }
        if (cmd == CommandCode::yes) {
            return true;
        }
        std::cout << "Aborted\n";
        return false;
    }
}

void FilesSynchronizerApp::help() const {
    if (argc != 0) {
        throw std::runtime_error("'help' does not accept arguments");
    }
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help - prints this message" << std::endl;
    std::cout << "  load <path> - loads data from JSON and changes file path" << std::endl;
    std::cout << "  load - loads data from JSON" << std::endl;
    std::cout << "  save <path> - saves data to JSON and changes file path" << std::endl;
    std::cout << "  save - saves data to JSON" << std::endl;
    std::cout << "  add <name> <path1> <path2> - adds an entry" << std::endl;
    std::cout << "  remove <name> - remove an entry" << std::endl;
    std::cout << "  clear - clears all entries" << std::endl;
    std::cout << "  list - lists all entries" << std::endl;
    std::cout << "  sync <name> - syncs an entry" << std::endl;
    std::cout << "  sync - syncs all entries" << std::endl;
    std::cout << "  conflicts <name> - prints conflicts in an entry" << std::endl;
    std::cout << "  conflicts - prints conflicts in all entries" << std::endl;
    std::cout << "  resolve <name> <source> - resolves conflicts in an entry (prioritizes changes in directory 1/2)" << std::endl;
    std::cout << "  exit - exits the program" << std::endl;
}

void FilesSynchronizerApp::load() {
    if (argc != 0 and argc != 1) {
        throw std::runtime_error("'load' accepts up to one argument");
    }
    if (argc == 1) {
        fs.setJSONPath(argv[0]);
        std::cout << "Set JSON path to " << argv[0] << "\n";
    }
    fs.loadData();
    std::cout << "Loaded data\n";
}

void FilesSynchronizerApp::save() {
    if (argc != 0 and argc != 1) {
        throw std::runtime_error("'save' accepts up to one argument");
    }
    if (argc == 1) {
        fs.setJSONPath(argv[0]);
        std::cout << "Set JSON path to " << argv[0] << "\n";
    }
    fs.saveData();
    std::cout << "Saved\n";
}

void FilesSynchronizerApp::add() {
    if (argc != 3) {
        throw std::runtime_error("'add' requires three arguments");
    }
    fs.addEntry(argv[0], argv[1], argv[2]);
    std::cout << "Added files to sync\n";
}

void FilesSynchronizerApp::remove() {
    if (argc != 1) {
        throw std::runtime_error("'remove' requires one argument");
    }
    fs.removeEntry(argv[0]);
}

void FilesSynchronizerApp::clear() {
    if (argc != 0) {
        throw std::runtime_error("'clear' does not accept arguments");
    }
    fs.removeAllEntries();
    std::cout << "Cleared all entries\n";
}

void FilesSynchronizerApp::list() const {
    if (argc != 0 && argc != 1) {
        throw std::runtime_error("'list' accepts up to one argument");
    }
    if (fs.getFileHandlers().empty()) {
        throw std::runtime_error("No entries to list");
    }

    if (argc == 1) {
        fs.listEntry(argv[0]);
    }
    else {
        std::cout << "All entries:\n\n";
        fs.listAllEntries();
    }
}

void FilesSynchronizerApp::sync() {
    if (argc != 0 && argc != 1) {
        throw std::runtime_error("'sync' accepts up to one argument");
    }
    if (!areYouSure()) {
        return;
    }
    if (argc == 1) {
        fs.syncEntry(argv[0]);
        std::cout << "Synced " << argv[0] << "\n";
    }
    else {
        fs.syncAllEntries();
        std::cout << "Synced all entries\n";
    }
    fs.saveData();
}

void FilesSynchronizerApp::conflicts() const {
    if (argc != 0 and argc != 1) {
        throw std::runtime_error("'conflicts' accepts up to one argument");
    }
    if (fs.getFileHandlers().empty()) {
        throw std::runtime_error("No entries");
    }
    if (argc == 1) {
        fs.listConflicts(argv[0]);
    }
    else {
        fs.listAllConflicts();
    }
}

void FilesSynchronizerApp::resolve() {
    if (argc != 2) {
        throw std::runtime_error("'resolve' requires two arguments");
    }
    const std::string &name = argv[0];
    const std::string &source = argv[1];
    if (source != "1" and source != "2") {
        throw std::runtime_error("Invalid source, must be 1 or 2");
    }
    if (!areYouSure()) {
        return;
    }
    fs.resolveConflicts(name, source == "1" ? ChangeSource::Path1 : ChangeSource::Path2);
    std::cout << "Resolved conflicts\n";
}

void FilesSynchronizerApp::exit() const {
    if (argc != 0) {
        throw std::runtime_error("'exit' does not accept arguments");
    }
    std::cout << "Exiting...\n";
}

FilesSynchronizerApp::FilesSynchronizerApp(const std::string &json_path)
    : fs(FilesSynchronizer(json_path)) {}

void FilesSynchronizerApp::run() {
    std::cout << "Welcome to files synchronizer\n";
    std::cout << "Type 'help' for list of commands\n";
    std::cout << "Type 'exit' to exit\n";
    fs.loadData();
    std::cout << "Loaded data from file\n";

    while (true) {
        getInput();
        try {
            switch (cmd) {
                case CommandCode::help: {
                    help();
                    break;
                }
                case CommandCode::load: {
                    load();
                    break;
                }
                case CommandCode::save: {
                    save();
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
                case CommandCode::conflicts: {
                    conflicts();
                    break;
                }
                case CommandCode::resolve: {
                    resolve();
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
