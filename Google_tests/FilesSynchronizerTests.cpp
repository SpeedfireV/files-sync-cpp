#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <sstream>

#include "../FilesHandler/FilesHandler.h"
#include "../SyncJSON/SyncJSON.h"
#include "../FilesSynchronizer/FilesSynchronizer.h"

namespace fs = std::filesystem;

static void createFile(const fs::path& path, const std::string& content = "test content") {
    // Ensure parent directories exist
    fs::create_directories(path.parent_path());
    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        throw std::runtime_error("Failed to create file: " + path.string());
    }
    ofs << content;
    ofs.close();
}

static void touchFile(const fs::path& path) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    std::ofstream ofs(path, std::ios::app);
    if (!ofs.is_open()) {
        throw std::runtime_error("Failed to open file for touching: " + path.string());
    }
    ofs << " updated";
    ofs.close();
}

static std::string readFileContent(const fs::path& path) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + path.string());
    }
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    return buffer.str();
}

static void writeTestJSON(const fs::path& path, const nlohmann::json& content) {
    fs::create_directories(path.parent_path());
    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        throw std::runtime_error("Failed to open JSON file for writing: " + path.string());
    }
    ofs << std::setw(4) << content << std::endl;
    ofs.close();
}

class FilesSynchronizerTest : public ::testing::Test {
protected:
    fs::path tempBaseDir;
    fs::path jsonFile;
    fs::path dirA, dirB;
    fs::path dirC, dirD;

    void SetUp() override {
        tempBaseDir = fs::temp_directory_path() / ("fs_sync_test_" + std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
        fs::create_directories(tempBaseDir);

        jsonFile = tempBaseDir / "config.json";
        std::ofstream(jsonFile.string()).close();

        dirA = tempBaseDir / "sync_dir_A";
        dirB = tempBaseDir / "sync_dir_B";
        fs::create_directories(dirA);
        fs::create_directories(dirB);

        dirC = tempBaseDir / "sync_dir_C";
        dirD = tempBaseDir / "sync_dir_D";
        fs::create_directories(dirC);
        fs::create_directories(dirD);
    }

    void TearDown() override {
        fs::remove_all(tempBaseDir);
    }
    void setupSynchronizerWithInitialData() {
        nlohmann::json initialJson;
        initialJson["initialEntry"] = {
            {"firstPath", dirA.string()},
            {"secondPath", dirB.string()},
            {"lastSync", 0},
            {"files", nlohmann::json::array()}
        };
        writeTestJSON(jsonFile, initialJson);
    }
};


TEST_F(FilesSynchronizerTest, ConstructorThrowsOnInvalidJSONPath) {
    fs::path nonExistentPath = tempBaseDir / "non_existent.json";
    EXPECT_THROW(FilesSynchronizer synchronizer(nonExistentPath.string()), std::runtime_error);

    fs::path directoryAsPath = tempBaseDir; // A directory, not a regular file
    EXPECT_THROW(FilesSynchronizer synchronizer(directoryAsPath.string()), std::runtime_error);

    fs::path wrongExtension = tempBaseDir / "config.txt";
    createFile(wrongExtension);
    EXPECT_THROW(FilesSynchronizer synchronizer(wrongExtension.string()), std::runtime_error);
}


TEST_F(FilesSynchronizerTest, SetJSONPathThrowsOnEmptyPath) {
    FilesSynchronizer synchronizer(jsonFile.string());
    EXPECT_THROW(synchronizer.setJSONPath(""), std::runtime_error);
}

TEST_F(FilesSynchronizerTest, SetJSONPathThrowsOnNonRegularFile) {
    FilesSynchronizer synchronizer(jsonFile.string());
    EXPECT_THROW(synchronizer.setJSONPath(tempBaseDir.string()), std::runtime_error);
}

TEST_F(FilesSynchronizerTest, SetJSONPathThrowsOnWrongExtension) {
    FilesSynchronizer synchronizer(jsonFile.string());
    fs::path wrongExtension = tempBaseDir / "another.txt";
    createFile(wrongExtension);
    EXPECT_THROW(synchronizer.setJSONPath(wrongExtension.string()), std::runtime_error);
}

TEST_F(FilesSynchronizerTest, AddEntryAddsCorrectly) {
    FilesSynchronizer synchronizer(jsonFile.string());
    synchronizer.addEntry("mySync", dirA.string(), dirB.string());

    const auto& handlers = synchronizer.getFileHandlers();
    ASSERT_EQ(handlers.size(), 1);
    ASSERT_TRUE(handlers.contains("mySync"));

    const auto& handler = handlers.at("mySync");
    EXPECT_EQ(handler.getPath1(), fs::canonical(dirA));
    EXPECT_EQ(handler.getPath2(), fs::canonical(dirB));
    EXPECT_EQ(handler.getLastSync(), 0);
}

TEST_F(FilesSynchronizerTest, AddEntryThrowsOnDuplicateName) {
    FilesSynchronizer synchronizer(jsonFile.string());
    synchronizer.addEntry("mySync", dirA.string(), dirB.string());
    EXPECT_THROW(synchronizer.addEntry("mySync", dirC.string(), dirD.string()), std::runtime_error);
}

TEST_F(FilesSynchronizerTest, AddEntryThrowsOnExactPath11verlap) {
    FilesSynchronizer synchronizer(jsonFile.string());
    synchronizer.addEntry("sync1", dirA.string(), dirB.string());
    // Try to add another entry with the same path1
    EXPECT_THROW(synchronizer.addEntry("sync2", dirA.string(), dirD.string()), std::runtime_error);
}

TEST_F(FilesSynchronizerTest, AddEntryThrowsOnExactPath2Overlap) {
    FilesSynchronizer synchronizer(jsonFile.string());
    synchronizer.addEntry("sync1", dirA.string(), dirB.string());
    // Try to add another entry with the same path2
    EXPECT_THROW(synchronizer.addEntry("sync2", dirC.string(), dirB.string()), std::runtime_error);
}

TEST_F(FilesSynchronizerTest, RemoveEntryRemovesCorrectly) {
    FilesSynchronizer synchronizer(jsonFile.string());
    synchronizer.addEntry("toRemove", dirA.string(), dirB.string());
    ASSERT_EQ(synchronizer.getFileHandlers().size(), 1);

    synchronizer.removeEntry("toRemove");
    EXPECT_TRUE(synchronizer.getFileHandlers().empty());
}

TEST_F(FilesSynchronizerTest, RemoveEntryThrowsOnNonExistent) {
    FilesSynchronizer synchronizer(jsonFile.string());
    EXPECT_THROW(synchronizer.removeEntry("nonExistent"), std::runtime_error);
}

TEST_F(FilesSynchronizerTest, RemoveAllEntriesClearrsMap) {
    FilesSynchronizer synchronizer(jsonFile.string());
    synchronizer.addEntry("sync1", dirA.string(), dirB.string());
    synchronizer.addEntry("sync2", dirC.string(), dirD.string());
    ASSERT_EQ(synchronizer.getFileHandlers().size(), 2);

    synchronizer.removeAllEntries();
    EXPECT_TRUE(synchronizer.getFileHandlers().empty());
}


TEST_F(FilesSynchronizerTest, ListEntryThrowsOnNonExistent) {
    FilesSynchronizer synchronizer(jsonFile.string());
    EXPECT_THROW(synchronizer.listEntry("nonExistent"), std::runtime_error);
}


TEST_F(FilesSynchronizerTest, ListAllEntriesShowsNothingForEmpty) {
    FilesSynchronizer synchronizer(jsonFile.string());
    testing::internal::CaptureStdout();
    synchronizer.listAllEntries();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.empty());
}

TEST_F(FilesSynchronizerTest, SyncEntryWorksCorrectlyForCreatedFile) {
    FilesSynchronizer synchronizer(jsonFile.string());
    synchronizer.addEntry("testSync", dirA.string(), dirB.string());

    createFile(dirA / "file_A.txt", "content A");

    synchronizer.syncEntry("testSync");

    EXPECT_TRUE(fs::exists(dirB / "file_A.txt"));
    EXPECT_EQ(readFileContent(dirB / "file_A.txt"), "content A");
}

TEST_F(FilesSynchronizerTest, SyncEntryThrowsOnNonExistent) {
    FilesSynchronizer synchronizer(jsonFile.string());
    EXPECT_THROW(synchronizer.syncEntry("nonExistent"), std::runtime_error);
}

TEST_F(FilesSynchronizerTest, SyncAllEntriesWorksCorrectlyForMultipleCreatedFiles) {
    FilesSynchronizer synchronizer(jsonFile.string());
    synchronizer.addEntry("sync1", dirA.string(), dirB.string());
    synchronizer.addEntry("sync2", dirC.string(), dirD.string());

    createFile(dirA / "file1.txt", "content1");
    createFile(dirC / "file2.txt", "content2");

    synchronizer.syncAllEntries();

    EXPECT_TRUE(fs::exists(dirB / "file1.txt"));

    EXPECT_EQ(  readFileContent(dirB / "file1.txt"), "content1");

    EXPECT_TRUE(fs::exists(dirD / "file2.txt"));
    EXPECT_EQ(readFileContent(dirD / "file2.txt"), "content2");
}

TEST_F(FilesSynchronizerTest, ListConflictsShowsCorrectInfo) {
    FilesSynchronizer synchronizer(jsonFile.string());
    synchronizer.addEntry("conflictSync", dirA.string(), dirB.string());

    createFile(dirA / "conflict.txt", "version A");
    createFile(dirB / "conflict.txt", "version B");

    touchFile(dirA / "conflict.txt");
    touchFile(dirB / "conflict.txt");



    EXPECT_THROW(synchronizer.syncEntry("conflictSync"), std::runtime_error);

    testing::internal::CaptureStdout();
    synchronizer.listConflicts("conflictSync");
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("Conflicts in entry conflictSync:") != std::string::npos);
    EXPECT_TRUE(output.find("conflict.txt - updated") != std::string::npos);
}

TEST_F(FilesSynchronizerTest, ListConflictsShowsNoConflictsMessage) {
    FilesSynchronizer synchronizer(jsonFile.string());
    synchronizer.addEntry("noConflictSync", dirA.string(), dirB.string());

    // Ensure no actual updates
    createFile(dirA / "file.txt");
    createFile(dirB / "file.txt");


    createFile(dirA / "new_file.txt");
    synchronizer.syncEntry("noConflictSync");


    testing::internal::CaptureStdout();
    synchronizer.listConflicts("noConflictSync");
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("No conflicts") != std::string::npos);
}

TEST_F(FilesSynchronizerTest, ListConflictsThrowsOnNonExistent) {
    FilesSynchronizer synchronizer(jsonFile.string());
    EXPECT_THROW(synchronizer.listConflicts("nonExistent"), std::runtime_error);
}

TEST_F(FilesSynchronizerTest, ListAllConflictsShowsAllConflicts) {
    FilesSynchronizer synchronizer(jsonFile.string());
    synchronizer.addEntry("sync1", dirA.string(), dirB.string());
    synchronizer.addEntry("sync2", dirC.string(), dirD.string());

    createFile(dirA / "conflict1.txt", "A1");
    createFile(dirB / "conflict1.txt", "B1");


    touchFile(dirA / "conflict1.txt");
    touchFile(dirB / "conflict1.txt");

    createFile(dirC / "conflict2.txt", "C2");
    createFile(dirD / "conflict2.txt", "D2");
    touchFile(dirC / "conflict2.txt");
    touchFile(dirD / "conflict2.txt");

    EXPECT_THROW(synchronizer.syncEntry("sync1"), std::runtime_error);
    EXPECT_THROW(synchronizer.syncEntry("sync2"), std::runtime_error);

    testing::internal::CaptureStdout();
    synchronizer.listAllConflicts();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("Conflicts in entry sync1:") != std::string::npos);
    EXPECT_TRUE(output.find("conflict1.txt - updated") != std::string::npos);
    EXPECT_TRUE(output.find("Conflicts in entry sync2:") != std::string::npos);
    EXPECT_TRUE(output.find("conflict2.txt - updated") != std::string::npos);
}

// --- resolveConflicts Tests ---

TEST_F(FilesSynchronizerTest, ResolveConflictsResolvesCorrectly) {
    FilesSynchronizer synchronizer(jsonFile.string());
    synchronizer.addEntry("resSync", dirA.string(), dirB.string());

    createFile(dirA / "resolvable.txt", "version A");
    createFile(dirB / "resolvable.txt", "version B");
    touchFile(dirA / "resolvable.txt");
    touchFile(dirB / "resolvable.txt");

    EXPECT_THROW(synchronizer.syncEntry("resSync"), std::runtime_error);

    synchronizer.resolveConflicts("resSync", Path1);
    synchronizer.syncEntry("resSync");

    EXPECT_TRUE(fs::exists(dirB / "resolvable.txt"));
    EXPECT_EQ(readFileContent(dirB / "resolvable.txt"), "version A updated");
}

TEST_F(FilesSynchronizerTest, ResolveConflictsThrowsOnNonExistent) {
    FilesSynchronizer synchronizer(jsonFile.string());
    EXPECT_THROW(synchronizer.resolveConflicts("nonExistent", Path1), std::runtime_error);
}

TEST_F(FilesSynchronizerTest, ResolveConflictsThrowsOnBothSource) {
    FilesSynchronizer synchronizer(jsonFile.string());
    synchronizer.addEntry("resSync", dirA.string(), dirB.string());
    createFile(dirA / "resolvable.txt", "version A");
    createFile(dirB / "resolvable.txt", "version B");
    touchFile(dirA / "resolvable.txt");
    touchFile(dirB / "resolvable.txt");




    EXPECT_THROW(synchronizer.syncEntry("resSync"), std::runtime_error);
    EXPECT_THROW(synchronizer.resolveConflicts("resSync", Both), std::runtime_error);
}

TEST_F(FilesSynchronizerTest, SaveDataSavesCorrectly) {
    FilesSynchronizer synchronizer(jsonFile.string());
    synchronizer.addEntry("testEntry1", dirA.string(), dirB.string());
    synchronizer.addEntry("testEntry2", dirC.string(), dirD.string());

    createFile(dirA / "file_in_A.txt");
    synchronizer.syncEntry("testEntry1");





    synchronizer.saveData();
    ASSERT_TRUE(fs::exists(jsonFile));
    std::ifstream ifs(jsonFile);
    nlohmann::json savedJson;
    ifs >> savedJson;

    ASSERT_TRUE(savedJson.contains("testEntry1"));
    auto entry1 = savedJson["testEntry1"];
    EXPECT_EQ(entry1["firstPath"], fs::canonical(dirA).generic_string());
    EXPECT_EQ(entry1["secondPath"], fs::canonical(dirB).generic_string());
    EXPECT_GT(entry1["lastSync"].get<time_t>(), 0);
    ASSERT_TRUE(entry1.contains("files"));
    EXPECT_NE(std::find(entry1["files"].begin(), entry1["files"].end(), "file_in_A.txt"), entry1["files"].end());

    ASSERT_TRUE(savedJson.contains("testEntry2"));
    auto entry2 = savedJson["testEntry2"];
    EXPECT_EQ(entry2["firstPath"], fs::canonical(dirC).generic_string());
    EXPECT_EQ(entry2["secondPath"], fs::canonical(dirD).generic_string());
    EXPECT_EQ(entry2["lastSync"], 0);

}



TEST_F(FilesSynchronizerTest, LoadDataLoadsCorrectly) {

    nlohmann::json initialJson;
    initialJson["loadedEntry1"] = {
        {"firstPath", dirA.string()},
        {"secondPath", dirB.string()},
        {"lastSync", 5000},
        {"files", {"file1.txt", "sub/file2.txt"}}
    };
    initialJson["loadedEntry2"] = {
        {"firstPath", dirC.string()},
        {"secondPath", dirD.string()},
        {"lastSync", 6000},
        {"files", {"another.txt"}}
    };
    writeTestJSON(jsonFile, initialJson);

    FilesSynchronizer synchronizer(jsonFile.string());
    synchronizer.loadData();

    const auto& handlers = synchronizer.getFileHandlers();
    ASSERT_EQ(handlers.size(), 2);

    ASSERT_TRUE(handlers.contains("loadedEntry1"));
    const auto& handler1 = handlers.at("loadedEntry1");
    EXPECT_EQ(handler1.getPath1(), fs::canonical(dirA));
    EXPECT_EQ(handler1.getPath2(), fs::canonical(dirB));
    EXPECT_EQ(handler1.getLastSync(), 5000);
    EXPECT_EQ(handler1.getFiles().size(), 2);
    EXPECT_NE(std::find(handler1.getFiles().begin(), handler1.getFiles().end(), fs::path("file1.txt")), handler1.getFiles().end());
    EXPECT_NE(std::find(handler1.getFiles().begin(), handler1.getFiles().end(), fs::path("sub/file2.txt")), handler1.getFiles().end());


    ASSERT_TRUE(handlers.contains("loadedEntry2"));
    const auto& handler2 = handlers.at("loadedEntry2");
    EXPECT_EQ(handler2.getPath1(), fs::canonical(dirC));
    EXPECT_EQ(handler2.getPath2(), fs::canonical(dirD));
    EXPECT_EQ(handler2.getLastSync(), 6000);
    EXPECT_EQ(handler2.getFiles().size(), 1);
    EXPECT_NE(std::find(handler2.getFiles().begin(), handler2.getFiles().end(), fs::path("another.txt")), handler2.getFiles().end());
}


TEST_F(FilesSynchronizerTest, LoadDataHandlesEmptyJSONContent) {
    nlohmann::json emptyJson = nlohmann::json::object();
    writeTestJSON(jsonFile, emptyJson);


    FilesSynchronizer synchronizer(jsonFile.string());
    synchronizer.loadData();
    EXPECT_TRUE(synchronizer.getFileHandlers().empty());
}
