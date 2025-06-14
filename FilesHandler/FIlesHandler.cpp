#include "FilesHandler.h"

#include <chrono>
#include <iostream>
#include <ranges>

time_t FilesHandler::getModificationDate(const std::filesystem::path &path) {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error(path.generic_string() + " does not exist");
    }
    if (!std::filesystem::is_regular_file(path)) {
        throw std::runtime_error(path.generic_string() + " is not a regular file");
    }
    const auto ftime = std::filesystem::last_write_time(path);
    const auto sctp = std::chrono::clock_cast<std::chrono::system_clock>(ftime);
    return std::chrono::system_clock::to_time_t(sctp);
}

void FilesHandler::syncFile(const std::filesystem::path &path, const UpdateData &update_data) const {
    if (update_data.source == Both) {
        throw std::runtime_error("Cannot sync file with conflicts");
    }
    const std::filesystem::path source = (update_data.source == Path1 ? path1 : path2) / path;
    const std::filesystem::path destination = (update_data.source == Path1 ? path2 : path1) / path;

    std::filesystem::remove(destination);
    if (update_data.change == Updated || update_data.change == Created) {
        if (!std::filesystem::exists(destination.parent_path())) {
            std::filesystem::create_directories(destination.parent_path());
        }
        std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing);
    }
}

FilesHandler::FilesHandler(
    const std::filesystem::path &path1,
    const std::filesystem::path &path2,
    const time_t last_sync,
    const std::vector<std::filesystem::path> &files) {
    if (path1 == path2) {
        throw std::runtime_error("Path1 and Path2 cannot be the same");
    }
    if (std::filesystem::canonical(path1).string().starts_with(std::filesystem::canonical(path2).string())) {
        throw std::runtime_error("Path1 cannot be a subdirectory of Path2");
    }
    if (std::filesystem::canonical(path2).string().starts_with(std::filesystem::canonical(path1).string())) {
        throw std::runtime_error("Path2 cannot be a subdirectory of Path1");
    }
    if (!std::filesystem::exists(path1)) {
        throw std::runtime_error(path1.generic_string() + " does not exist");
    }
    if (!std::filesystem::is_directory(path1)) {
        throw std::runtime_error(path1.generic_string() + " is not a directory");
    }
    this->path1 = std::filesystem::canonical(path1);

    if (!std::filesystem::exists(path2)) {
        throw std::runtime_error(path2.generic_string() + " does not exist");
    }
    if (!std::filesystem::is_directory(path2)) {
        throw std::runtime_error(path2.generic_string() + " is not a directory");
    }
    this->path2 = std::filesystem::canonical(path2);

    this->last_sync = last_sync;
    this->files = files;
    if (files.empty()) {
        this->files = findFiles(path1);
    }
}

const std::filesystem::path & FilesHandler::getPath1() const {
    return path1;
}

const std::filesystem::path & FilesHandler::getPath2() const {
    return path2;
}

time_t FilesHandler::getLastSync() const {
    return last_sync;
}

const std::vector<std::filesystem::path> & FilesHandler::getFiles() const {
    return files;
}

const std::map<std::filesystem::path, UpdateData> & FilesHandler::getUpdates() const {
    return updates;
}

std::map<std::filesystem::path, ChangeType> FilesHandler::getConflicts() const {
    std::map<std::filesystem::path, ChangeType> conflicts;
    for (const auto &[path, update_data] : updates) {
        if (update_data.source == Both) {
            conflicts.emplace(path, update_data.change);
        }
    }
    return conflicts;
}

std::vector<std::filesystem::path> FilesHandler::findFiles(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error(path.generic_string() + " does not exist");
    }
    if (!std::filesystem::is_directory(path)) {
        throw std::runtime_error(path.generic_string() + " is not a directory");
    }
    std::vector<std::filesystem::path> files;
    for (const auto &file : std::filesystem::recursive_directory_iterator(path)) {
        if (file.is_regular_file()){
            files.push_back(std::filesystem::relative(file.path(), path));
        }
    }
    return files;
}

void FilesHandler::findUpdates() {
    updates.clear();
    std::vector<std::filesystem::path> files1 = findFiles(path1);
    std::vector<std::filesystem::path> files2 = findFiles(path2);
    if (files.empty()) {
        this->files = files1;
    }

    // Find all modified / deleted files
    for (const auto &file : files) {
        std::filesystem::path path1_file = path1 / file;
        std::filesystem::path path2_file = path2 / file;
        auto file1 = std::ranges::find(files1, file);
        auto file2 = std::ranges::find(files2, file);
        time_t modification_date1 = 0;
        time_t modification_date2 = 0;
        if (file1 != std::end(files1)) {
            modification_date1 = getModificationDate(path1_file);
            files1.erase(file1);
        }
        if (file2 != std::end(files2)) {
            modification_date2 = getModificationDate(path2_file);
            files2.erase(file2);
        }

        if (modification_date1 == modification_date2) {
            // Are equal -> do nothing
        }
        else if (modification_date1 == 0 and modification_date2 == 0) {
            // 1 deleted, 2 deleted -> do nothing
        }
        else if (modification_date1 == 0 and modification_date2 <= last_sync) {
            // 1 deleted, 2 unchanged -> delete 1
            updates.emplace(file, UpdateData(Deleted, Path1));
        }
        else if (modification_date1 <= last_sync and modification_date2 == 0) {
            // 1 unchanged, 2 deleted -> delete 1
            updates.emplace(file, UpdateData(Deleted, Path2));
        }
        else if (modification_date1 > last_sync and modification_date2 <= last_sync) {
            // 1 updated, 2 deleted/unchanged -> copy 1 to 2
            updates.emplace(file, UpdateData(Updated, Path1));
        }
        else if (modification_date1 <= last_sync and modification_date2 > last_sync) {
            // 1 deleted/unchanged, 2 updated -> copy 2 to 1
            updates.emplace(file, UpdateData(Updated, Path2));
        }
        else if (modification_date1 > last_sync and modification_date2 > last_sync) {
            // 1 updated, 2 updated -> conflict
            updates.emplace(file, UpdateData(Updated, Both));
        }
        else {
            // No changes?
            // throw std::runtime_error("Something went wrong");
        }
    }

    // find created files in 1
    for (const auto &file1 : files1) {
        if (auto file2 = std::ranges::find(files2, file1); file2 == std::end(files2)) {
            // 1 created, 2 non-existent
            updates.emplace(file1, UpdateData(Created, Path1));
        }
        else {
            // 1 created, 2 created
            if (getModificationDate(path1 / file1) == getModificationDate(path2 / *file2)) {
                continue;
            }
            updates.emplace(file1, UpdateData(Created, Both));
            files2.erase(file2);
        }
    }

    // find created files in 2
    for (const auto &file2 : files2) {
        // 1 non-existent, 2 created
        updates.emplace(file2, UpdateData(Created, Path2));
    }
}

void FilesHandler::resolveConflicts(const ChangeSource source) {
    if (source == Both) {
        throw std::runtime_error("Cannot resolve conflicts for both sources");
    }
    if (updates.empty()) {
        throw std::runtime_error("There are no conflicts to resolve");
    }
    for (auto &[path, updateData] : updates) {
        if (updateData.source == Both) {
            syncFile(path, UpdateData(Deleted, source));
        }
    }
}

void FilesHandler::syncFiles() {
    findUpdates();
    if (updates.empty()) {
        throw std::runtime_error("There are no changes in the files");
    }
    int conflict_count = 0;
    for (const auto &[change, source]: updates | std::views::values) {
        if (source == Both) {
            conflict_count++;
        }
    }
    if (conflict_count > 0) {
        throw std::runtime_error("There are " + std::to_string(conflict_count) + " files with conflicts");
    }
    for (auto &[path, update_data] : updates) {
        syncFile(path, update_data);
    }
    this->last_sync = time(nullptr);
    this->files = findFiles(path1);
}

std::ostream & operator<<(std::ostream &os, const FilesHandler &fh) {
    const time_t time = fh.getLastSync();
    os << "Path1: " << fh.getPath1() << "\n";
    os << "Path2: " << fh.getPath2() << "\n";
    os << "Last sync: ";
    if (time == 0) {
        os << "Never\n";
    } else {
        os << std::put_time(localtime(&time), "%a %d %b %Y, %H:%M:%S") << "\n";
    }

    return os;
}
