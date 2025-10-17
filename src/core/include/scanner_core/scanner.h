#ifndef SCANNER_CORE_H
#define SCANNER_CORE_H

#include <string>
#include <unordered_map>
#include <chrono>

#ifdef _WIN32
#ifdef SCANNER_CORE_EXPORTS
#define SCANNER_API __declspec(dllexport)
#else
#define SCANNER_API __declspec(dllimport)
#endif
#else
#define SCANNER_API
#endif
namespace scanner_core {
    struct SCANNER_API ScanReport {
        uint64_t total_files = 0;
        uint64_t malicious_files = 0;
        uint64_t error_files = 0;
        double duration_seconds = 0.0;
    };

    class SCANNER_API Scanner {
    public:

        Scanner(const std::string &csv_path, const std::string &log_path,
                unsigned int threads = 0);

        ~Scanner();


        ScanReport scan(const std::string &root_path);

    private:
        struct Impl;
        Impl *pimpl;
    };
}
#endif