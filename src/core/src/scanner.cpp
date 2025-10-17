
#include "../include/scanner_core/scanner.h"
#include "../include/scanner_core/csv_loader.h"
#include "../include/scanner_core/md5.h"

#include <filesystem>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <fstream>
#include <atomic>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <chrono>

namespace fs = std::filesystem;

namespace scanner_core {

    struct Scanner::Impl {
        std::unordered_map<std::string, std::string> db;
        std::string log_path;
        unsigned int num_threads = 0;

        std::atomic<uint64_t> total_files{0};
        std::atomic<uint64_t> malicious_files{0};
        std::atomic<uint64_t> error_files{0};

        std::mutex q_mutex;
        std::condition_variable q_cv;
        std::queue<fs::path> q;
        bool finished_producing = false;

        std::mutex log_mutex;
        std::ofstream log_stream;

        Impl(const std::string& csv_path, const std::string& log_p, unsigned int threads) : log_path(log_p) {
            db = load_csv_database(csv_path);
            if (db.empty()) {
                std::cerr << "Warning: загруженная база хэшей пуста или не может быть прочитана: " << csv_path << std::endl;
            }
            num_threads = (threads == 0) ? std::max(1u, std::thread::hardware_concurrency()) : threads;

            log_stream.open(log_path, std::ios::out | std::ios::app);
            if (!log_stream) {
                std::cerr << "Warning: не удалось открыть лог-файл для записи: " << log_path << std::endl;
            }
        }

        ~Impl() {
            if (log_stream.is_open()) log_stream.close();
        }
    };

    Scanner::Scanner(const std::string& csv_path, const std::string& log_path, unsigned int threads) {
        pimpl = new Impl(csv_path, log_path, threads);
    }

    Scanner::~Scanner() {
        delete pimpl;
    }

    static std::string path_to_utf8(const fs::path& p) {
        try {
            return p.u8string();
        } catch (...) {
            try {
                return p.string();
            } catch (...) {
                return {};
            }
        }
    }

    ScanReport Scanner::scan(const std::string& root_path) {
        using clock = std::chrono::steady_clock;
        auto t0 = clock::now();

        Impl& I = *pimpl;

        auto worker = [&I]() {
            while (true) {
                fs::path file_path;
                {
                    std::unique_lock<std::mutex> lk(I.q_mutex);
                    I.q_cv.wait(lk, [&I]{ return I.finished_producing || !I.q.empty(); });
                    if (I.q.empty()) {
                        if (I.finished_producing) break;
                        else continue;
                    }
                    file_path = I.q.front();
                    I.q.pop();
                }

                try {
                    std::string fpath = path_to_utf8(file_path);
                    std::string hash = md5_of_file(fpath);

                    I.total_files.fetch_add(1, std::memory_order_relaxed);

                    if (hash.empty()) {
                        I.error_files.fetch_add(1, std::memory_order_relaxed);
                        continue;
                    }

                    std::transform(hash.begin(), hash.end(), hash.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });

                    auto it = I.db.find(hash);
                    if (it != I.db.end()) {
                        I.malicious_files.fetch_add(1, std::memory_order_relaxed);

                        std::lock_guard<std::mutex> lk(I.log_mutex);
                        if (I.log_stream) {
                            I.log_stream << fpath << ";" << hash << ";" << it->second << std::endl;
                            I.log_stream.flush();
                        }
                    }
                } catch (const std::exception& ex) {
                    I.error_files.fetch_add(1, std::memory_order_relaxed);
                } catch (...) {
                    I.error_files.fetch_add(1, std::memory_order_relaxed);
                }
            }
        };

        std::vector<std::thread> workers;
        workers.reserve(I.num_threads);
        for (unsigned int i = 0; i < I.num_threads; ++i) {
            workers.emplace_back(worker);
        }

        try {
            fs::path root(root_path);
            if (!fs::exists(root)) {
                std::cerr << "Warning: указанный корневой путь не существует: " << root_path << std::endl;
            } else {
                for (fs::recursive_directory_iterator it(root, fs::directory_options::skip_permission_denied), end; it != end; ++it) {
                    try {
                        if (it->is_regular_file()) {
                            {
                                std::lock_guard<std::mutex> lk(I.q_mutex);
                                I.q.push(it->path());
                            }
                            I.q_cv.notify_one();
                        }
                    } catch (const fs::filesystem_error& fe) {
                        I.error_files.fetch_add(1, std::memory_order_relaxed);
                    }
                }
            }
        } catch (const std::exception& ex) {
            std::cerr << "Error while traversing filesystem: " << ex.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown error during filesystem traversal." << std::endl;
        }

        {
            std::lock_guard<std::mutex> lk(I.q_mutex);
            I.finished_producing = true;
        }
        I.q_cv.notify_all();

        for (auto &t : workers) {
            if (t.joinable()) t.join();
        }

        auto t1 = clock::now();
        ScanReport r;
        r.total_files = I.total_files.load(std::memory_order_relaxed);
        r.malicious_files = I.malicious_files.load(std::memory_order_relaxed);
        r.error_files = I.error_files.load(std::memory_order_relaxed);
        r.duration_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0).count();
        return r;
    }

}
