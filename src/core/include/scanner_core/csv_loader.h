#ifndef CSV_LOADER_H
#define CSV_LOADER_H

#include <string>
#include <unordered_map>

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


    SCANNER_API std::unordered_map<std::string, std::string> load_csv_database(const std::string& csv_path);

}

#endif
