#ifndef SCANNER_MD5_H
#define SCANNER_MD5_H

#include <string>

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

    SCANNER_API std::string md5_of_file(const std::string& filepath);

}

#endif
