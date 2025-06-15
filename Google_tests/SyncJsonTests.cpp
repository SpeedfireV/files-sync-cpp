#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include "../SyncJSON/SyncJSON.h"

namespace fs = std::filesystem;

void writeTestJSON(const fs::path& path, const std::string& content) {
    std::ofstream ofs(path);
    ofs << content;
    ofs.close();
}

class SyncJSONTest : public ::testing::Test {
protected:
    fs::path tempDir;
    fs::path jsonFile;
    fs::path dir1, dir2;

    void SetUp() override {
        tempDir = fs::temp_directory_path() / "sync_json_test";
        fs::create_directories(tempDir);
        jsonFile = tempDir / "config.json";
        dir1 = tempDir / "dir1";
        dir2 = tempDir / "dir2";
        fs::create_directories(dir1);
        fs::create_directories(dir2);
    }

    void TearDown() override {
        fs::remove_all(tempDir);
    }
};


TEST_F(SyncJSONTest, LoadsValidJSONCorrectly) {
    std::string content = R"({
        "entry1": {
            "firstPath": ")" + dir1.generic_string() + R"(",
            "secondPath": ")" + dir2.generic_string() + R"(",
            "lastSync": 1000,
            "files": ["a.txt", "b.txt"]
        }
    })";

    writeTestJSON(jsonFile, content);
    SyncJSON sync(jsonFile);

    auto data = sync.loadJSON();
    ASSERT_EQ(data.size(), 1);
    const FilesHandler& handler = data.at("entry1");

    EXPECT_EQ(fs::canonical(handler.getPath1()), fs::canonical(dir1));
    EXPECT_EQ(fs::canonical(handler.getPath2()), fs::canonical(dir2));
    EXPECT_EQ(handler.getLastSync(), 1000);
    auto files = handler.getFiles();
    EXPECT_EQ(files.size(), 2);
    EXPECT_NE(std::find(files.begin(), files.end(), fs::path("a.txt")), files.end());
    EXPECT_NE(std::find(files.begin(), files.end(), fs::path("b.txt")), files.end());
}

TEST_F(SyncJSONTest, SavesJSONCorrectly) {
    FilesHandler handler(dir1, dir2, 1234, { "a.txt", "b.txt" });
    std::map<std::string, FilesHandler> data = { { "testEntry", handler } };

    // Create an empty file first
    std::ofstream touch(jsonFile);
    touch.close();
    ASSERT_TRUE(fs::exists(jsonFile));

    SyncJSON sync(jsonFile);
    sync.saveJSON(data);


    std::ifstream ifs(jsonFile);
    nlohmann::json json;
    ifs >> json;

    ASSERT_TRUE(json.contains("testEntry"));
    auto entry = json["testEntry"];

    EXPECT_EQ(entry["firstPath"], dir1.generic_string());
    EXPECT_EQ(entry["secondPath"], dir2.generic_string());
    EXPECT_EQ(entry["lastSync"], 1234);
    std::vector<std::string> expected = { "a.txt", "b.txt" };
    EXPECT_EQ(entry["files"], expected);
}