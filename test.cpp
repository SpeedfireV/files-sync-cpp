#include <iostream>
#include <filesystem>
#include <chrono>

time_t getModificationDate(const std::filesystem::path &path) {
    const auto ftime = std::filesystem::last_write_time(path);
    const auto sctp = std::chrono::clock_cast<std::chrono::system_clock>(ftime);
    return std::chrono::system_clock::to_time_t(sctp);
}

void files_test() {
    std::cout << "test\n";
    std::filesystem::path p("C:\\Users\\Wojciech/Documents/GitHub/Studia/Inne");
    std::filesystem::path p2 = std::filesystem::canonical(p);
    std::cout << p2 << "\n";
    std::cout << std::filesystem::canonical(p) << "\n";
    std::cout << std::filesystem::absolute(p) << "\n";
    for (std::filesystem::recursive_directory_iterator it(p); auto& entry : it) {
        if (entry.is_directory()) continue;
        std::cout << std::filesystem::relative(entry, p) << "\n";
    }
}

void time_test(time_t czas) {
    std::cout << std::put_time(localtime(&czas), "%a %d %b %Y, %H:%M:%S") << std::endl;
}

void time_test2() {
    std::filesystem::path p("C:/Users/Wojciech/Documents/GitHub/Studia/Inne/FileSynchronizer");
    std::cout << getModificationDate(p) << "\n";
    time_test(getModificationDate(p));
}

int main() {
    time_test2();
    return 0;
}

// bardziej ważny siusiak