include(FetchContent)
FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(googletest
        SyncJsonTests.cpp
        FilesHandlerTTests.cpp
        FilesSynchronizerTests.cpp
)

target_link_libraries(googletest
        gtest_main
        FileSynchronizerLib)  # ✅ Use the library target
add_test(NAME googletest_test COMMAND googletest)
