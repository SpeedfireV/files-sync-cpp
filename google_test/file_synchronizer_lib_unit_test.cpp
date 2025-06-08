#include <gtest/gtest.h>
#include "../files_synchronizer_lib/files_synchronizer/files_synchronizer.h""
#include <filesystem>

class FilesSynchronizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Tworzenie tymczasowych katalogów testowych
        std::filesystem::create_directory("test_master");
        std::filesystem::create_directory("test_slave");
    }

    void TearDown() override {
        // Sprzątanie po testach
        std::filesystem::remove_all("test_master");
        std::filesystem::remove_all("test_slave");
    }
};

TEST_F(FilesSynchronizerTest, ConstructorDefault) {
    FilesSynchronizer sync;
    // Sprawdzenie czy operator[] rzuci wyjątkiem (wektor powinien być pusty)
    EXPECT_THROW(sync[0], std::out_of_range);
}

TEST_F(FilesSynchronizerTest, ConstructorWithParams) {
    FilesSynchronizer sync("test_master", "test_slave");
    EXPECT_NO_THROW(sync[0]);
    EXPECT_EQ(sync[0].getMasterPath(), "test_master");
    EXPECT_EQ(sync[0].getSlavePath(), "test_slave");
}

TEST_F(FilesSynchronizerTest, AddDirectories) {
    FilesSynchronizer sync;
    
    // Dodanie poprawnych katalogów
    sync.addDirectories("test_master", "test_slave");
    EXPECT_NO_THROW(sync[0]);
    
    // Próba dodania tych samych ścieżek
    sync.addDirectories("test_master", "test_master");
    EXPECT_EQ(sync[0].getMasterPath(), "test_master");
    
    // Próba dodania pustych ścieżek
    sync.addDirectories("", "");
    EXPECT_EQ(sync[0].getMasterPath(), "test_master");
}

TEST_F(FilesSynchronizerTest, RemoveDirectories) {
    FilesSynchronizer sync;
    sync.addDirectories("test_master", "test_slave");
    
    // Usunięcie istniejącego katalogu
    EXPECT_NO_THROW(sync.removeDirectories("test_master"));
    
    // Próba usunięcia nieistniejącego katalogu
    EXPECT_THROW(sync.removeDirectories("nonexistent"), std::runtime_error);
}

TEST_F(FilesSynchronizerTest, ClearAllDirectories) {
    FilesSynchronizer sync;
    sync.addDirectories("test_master", "test_slave");
    sync.addDirectories("test_master2", "test_slave2");
    
    sync.clearAllDirectories();
    EXPECT_THROW(sync[0], std::out_of_range);
}

TEST_F(FilesSynchronizerTest, GetFileHandler) {
    FilesSynchronizer sync;
    sync.addDirectories("test_master", "test_slave");
    
    // Pobranie istniejącego handlera
    EXPECT_NO_THROW(sync.getFileHandler("test_master"));
    EXPECT_NO_THROW(sync.getFileHandler("test_slave"));
    
    // Próba pobrania nieistniejącego handlera
    EXPECT_THROW(sync.getFileHandler("nonexistent"), std::runtime_error);
}

TEST_F(FilesSynchronizerTest, OperatorSubscript) {
    FilesSynchronizer sync;
    sync.addDirectories("test_master", "test_slave");
    
    // Dostęp do istniejącego indeksu
    EXPECT_NO_THROW(sync[0]);
    
    // Dostęp do nieistniejącego indeksu
    EXPECT_THROW(sync[1], std::out_of_range);
}

TEST_F(FilesSynchronizerTest, PrepareSyncFiles) {
    FilesSynchronizer sync("test_master", "test_slave");
    
    // Przygotowanie synchronizacji dla istniejącego katalogu
    EXPECT_NO_THROW(sync.prepareSyncFiles("test_master"));
    
    // Próba przygotowania synchronizacji dla nieistniejącego katalogu
    EXPECT_THROW(sync.prepareSyncFiles("nonexistent"), std::runtime_error);
}

TEST_F(FilesSynchronizerTest, SyncDirectories) {
    FilesSynchronizer sync("test_master", "test_slave");
    
    // Synchronizacja istniejących katalogów
    EXPECT_NO_THROW(sync.syncFiles("test_master"));
    
    // Próba synchronizacji nieistniejącego katalogu
    EXPECT_THROW(sync.syncFiles("nonexistent"), std::runtime_error);
}