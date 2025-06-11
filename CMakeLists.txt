cmake_minimum_required(VERSION 3.29.6)
project(PROI_PROJEKT)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)


add_executable(PROI_PROJEKT main.cpp
        files_synchronizer_lib/files_synchronizer/files_synchronizer.cpp
        files_synchronizer_lib/files_synchronizer/files_synchronizer.h
        files_synchronizer_lib/file_handler/FilesHandler.cpp
        files_synchronizer_lib/file_handler/FilesHandler.h
        files_synchronizer_lib/sync_file/SyncFile.cpp
        files_synchronizer_lib/sync_file/SyncFile.h
        files_synchronizer_lib/modification_event/ModificationEvent.cpp
        files_synchronizer_lib/modification_event/ModificationEvent.h
        files_synchronizer_lib/update_info/update_info.cpp
        files_synchronizer_lib/update_info/update_info.h
        files_synchronizer_lib/who_enum/who_enum.cpp
        files_synchronizer_lib/who_enum/who_enum.h
        files_synchronizer_app/FilesSynchronizerApp.cpp
        files_synchronizer_app/FilesSynchronizerApp.h)

add_executable(MAIN main.cpp)

add_executable(JSON_test json_test.cpp)

add_subdirectory(google_test)