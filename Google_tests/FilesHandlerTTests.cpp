#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include "../FilesHandler/FilesHandler.h"
#include <ctime>
#include <chrono>

namespace fs = std::filesystem;

void createFile(const fs::path& path, const std::string& content = "test") {
    std::ofstream ofs(path);
    ofs << content;
}

void touchFile(const fs::path& path) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::ofstream ofs(path, std::ios::app);
    ofs << "updated";
}

class FilesHandlerTest : public ::testing::Test {
protected:
    fs::path dir1, dir2;
    time_t last_sync = 0;

    void SetUp() override {
        dir1 = fs::temp_directory_path() / "sync_test_dir1";
        dir2 = fs::temp_directory_path() / "sync_test_dir2";

        fs::create_directories(dir1);
        fs::create_directories(dir2);

        last_sync = time(nullptr) - 10;
    }

    void TearDown() override {
        fs::remove_all(dir1);
        fs::remove_all(dir2);
    }
};

TEST_F(FilesHandlerTest, ConstructorThrowsOnSamePaths) {
    EXPECT_THROW(FilesHandler fh(dir1, dir1, last_sync, {}), std::runtime_error);
}

TEST_F(FilesHandlerTest, ConstructorThrowsOnInvalidPaths) {
    fs::remove_all(dir2);
    EXPECT_THROW(FilesHandler fh(dir1, dir2, last_sync, {}), std::runtime_error);
}

TEST_F(FilesHandlerTest, FindFilesReturnsCorrectRelativePaths) {
    createFile(dir1 / "file1.txt");
    fs::create_directories(dir1 / "sub");
    createFile(dir1 / "sub" / "file2.txt");

    auto files = FilesHandler::findFiles(dir1);
    EXPECT_EQ(files.size(), 2);
    EXPECT_NE(std::find(files.begin(), files.end(), fs::path("file1.txt")), files.end());
    EXPECT_NE(std::find(files.begin(), files.end(), fs::path("sub/file2.txt")), files.end());
}


TEST_F(FilesHandlerTest, DetectsConflicts) {
    createFile(dir1 / "conflict.txt");
    createFile(dir2 / "conflict.txt");

    FilesHandler fh(dir1, dir2, last_sync, {});
    touchFile(dir1 / "conflict.txt");
    touchFile(dir2 / "conflict.txt");
    fh.findUpdates();

    auto conflicts = fh.getConflicts();
    ASSERT_EQ(conflicts.size(), 1);
    EXPECT_EQ(conflicts.begin()->first, fs::path("conflict.txt"));
}

TEST_F(FilesHandlerTest, ThrowsOnSyncWithConflicts) {
    createFile(dir1 / "file.txt");
    createFile(dir2 / "file.txt");

    FilesHandler fh(dir1, dir2, last_sync, {});
    touchFile(dir1 / "file.txt");
    touchFile(dir2 / "file.txt");
    fh.findUpdates();

    EXPECT_THROW(fh.syncFiles(), std::runtime_error);
}

TEST_F(FilesHandlerTest, ResolvesConflictsByDeletingFromSpecifiedSource) {
    createFile(dir1 / "file.txt");
    createFile(dir2 / "file.txt");

    FilesHandler fh(dir1, dir2, last_sync, {});
    touchFile(dir1 / "file.txt");
    touchFile(dir2 / "file.txt");
    fh.findUpdates();

    fh.resolveConflicts(Path1);
    EXPECT_FALSE(fs::exists(dir2 / "file.txt"));
}

TEST_F(FilesHandlerTest, SyncsCreatedFileCorrectly) {
    createFile(dir1 / "syncme.txt");

    FilesHandler fh(dir1, dir2, last_sync, {});
    fh.syncFiles();

    EXPECT_TRUE(fs::exists(dir2 / "syncme.txt"));
}
