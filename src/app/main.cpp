
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#ifdef _WIN32
#include <windows.h>
#endif

#include "../core/include/scanner_core/scanner.h"

namespace fs = std::filesystem;

void print_usage() {
    std::cout << "Usage:\n  scanner_app --base <path_to_csv> --log <path_to_log> --path <root_folder> [--threads N] [--verbose]\n";
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    if (argc < 7) {
        print_usage();
        return 1;
    }
    std::string csv_path;
    std::string log_path;
    std::string root_path;
    unsigned int threads = 0;
    bool verbose = false;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--base" && i + 1 < argc) { csv_path = argv[++i]; }
        else if (a == "--log" && i + 1 < argc) { log_path = argv[++i]; }
        else if (a == "--path" && i + 1 < argc) { root_path = argv[++i]; }
        else if (a == "--threads" && i + 1 < argc) { threads = static_cast<unsigned int>(std::stoi(argv[++i])); }
        else if (a == "--verbose") { verbose = true; }
        else {
            std::cerr << "Unknown arg: " << a << "\n";
        }
    }

    if (csv_path.empty() || log_path.empty() || root_path.empty()) {
        print_usage();
        return 2;
    }

    if (!fs::exists(csv_path)) {
        std::cerr << "Ошибка: CSV-файл базы не найден: " << csv_path << "\n";
        return 3;
    }
    if (!fs::exists(root_path)) {
        std::cerr << "Ошибка: Корневая папка для сканирования не найдена: " << root_path << "\n";
        return 4;
    }

    std::cout << "CSV:   " << csv_path << "\n";
    std::cout << "LOG:   " << log_path << "\n";
    std::cout << "PATH:  " << root_path << "\n";
    std::cout << "THREADS (0 = auto): " << threads << "\n";
    std::cout << "VERBOSE: " << (verbose ? "ON" : "OFF") << std::endl;
    std::cout.flush();

    try {
        scanner_core::Scanner scanner(csv_path, log_path, threads);
        if (verbose) std::cout << "Запуск сканирования...\\n" << std::flush;
        auto report = scanner.scan(root_path);
        std::cout << "Scan report:\\n";
        std::cout << "  Total files processed: " << report.total_files << "\\n";
        std::cout << "  Malicious files found: " << report.malicious_files << "\n";
        std::cout << "  Errors during analysis: " << report.error_files << "\\n";
        std::cout << "  Duration (seconds): " << report.duration_seconds << "\\n";
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << "\\n";
        return 5;
    }

    return 0;
}
