#include "../include/scanner_core/csv_loader.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace scanner_core {
    static inline std::string trim(const std::string &s) {
        size_t a = 0;
        while (a < s.size() && isspace((unsigned char) s[a])) ++a;
        size_t b = s.size();
        while (b > a && isspace((unsigned char) s[b - 1])) --b;
        return s.substr(a, b - a);
    }

    std::unordered_map<std::string, std::string> load_csv_database(const
                                                                   std::string &csv_path) {
        std::unordered_map<std::string, std::string> db;
        std::ifstream in(csv_path);
        if (!in) return db;
        std::string line;
        while (std::getline(in, line)) {
            std::string s = trim(line);
            if (s.empty()) continue;
            if (s[0] == '#') continue;
            size_t pos = s.find(';');
            if (pos == std::string::npos) continue;
            std::string h = s.substr(0, pos);
            std::string verdict = s.substr(pos + 1);
            h = trim(h);
            verdict = trim(verdict);

            std::transform(h.begin(), h.end(), h.begin(), [](unsigned char c) {
                return std::tolower(c);
            });
            db.emplace(h, verdict);
        }
        return db;
    }
}