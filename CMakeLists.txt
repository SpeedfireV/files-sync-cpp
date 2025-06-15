cmake_minimum_required(VERSION 3.31)
project(FileSynchronizer)

set(CMAKE_CXX_STANDARD 20)

# Core library
add_library(FileSynchronizerLib
        SyncJSON/SyncJSON.cpp
        UpdateData/UpdateData.cpp
        FilesSynchronizerApp/FilesSynchronizerApp.cpp
        FilesSynchronizer/FilesSynchronizer.cpp
        FilesHandler/FilesHandler.cpp
)

target_include_directories(FileSynchronizerLib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

# Main executable
add_executable(FileSynchronizer main.cpp)
target_link_libraries(FileSynchronizer FileSynchronizerLib)

add_subdirectory(Google_tests)