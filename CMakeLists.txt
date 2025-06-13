cmake_minimum_required(VERSION 3.31)
project(FileSynchronizer)

set(CMAKE_CXX_STANDARD 20)

add_executable(FileSynchronizer main.cpp
        SyncJSON/SyncJSON.h
        SyncJSON/SyncJSON.cpp
        UpdateData/UpdateData.h
        UpdateData/UpdateData.cpp
        FilesSynchronizerApp/FilesSynchronizerApp.h
        FilesSynchronizerApp/FilesSynchronizerApp.cpp
        FilesSynchronizer/FilesSynchronizer.h
        FilesSynchronizer/FilesSynchronizer.cpp
        FilesHandler/FilesHandler.h
        FilesHandler/FilesHandler.cpp)

add_executable(Test test.cpp)