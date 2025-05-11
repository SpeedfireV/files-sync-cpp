cmake_minimum_required(VERSION 3.31)
project(PROI_PROJEKT)

set(CMAKE_CXX_STANDARD 20)

add_executable(PROI_PROJEKT main.cpp
        files_synchronizer_lib/files_synchronizer/files_synchronizer.cpp
        files_synchronizer_lib/files_synchronizer/files_synchronizer.h
        files_synchronizer_lib/file_handler/file_handler.cpp
        files_synchronizer_lib/file_handler/file_handler.h
        files_synchronizer_lib/sync_file/sync_file.cpp
        files_synchronizer_lib/sync_file/sync_file.h
        files_synchronizer_lib/modification_event/modification_event.cpp
        files_synchronizer_lib/modification_event/modification_event.h
        files_synchronizer_lib/update_info/update_info.cpp
        files_synchronizer_lib/update_info/update_info.h
        files_synchronizer_lib/who_enum/who_enum.cpp
        files_synchronizer_lib/who_enum/who_enum.h)
