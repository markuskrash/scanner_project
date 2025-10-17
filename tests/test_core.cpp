#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include "../src/core//include/scanner_core/csv_loader.h"
#include "../src/core/include/scanner_core/md5.h"
namespace fs = std::filesystem;
TEST(CsvLoader, LoadSimpleDb) {
auto tmp = fs::temp_directory_path() / "test_db.csv";
std::ofstream out(tmp);
out << "aabbccdd00112233445566778899aabb;TestVerdict\n";
out << "# comment\n";
out << "0123456789abcdef0123456789abcdef;Another\n";
out.close();
auto db = scanner_core::load_csv_database(tmp.string());
EXPECT_EQ(db.size(), 2);
EXPECT_EQ(db.at("aabbccdd00112233445566778899aabb"), "TestVerdict");
fs::remove(tmp);
}
TEST(MD5, KnownString) {
auto tmp = fs::temp_directory_path() / "md5_test.txt";
std::ofstream out(tmp);
out << "hello";
out.close();

std::string got = scanner_core::md5_of_file(tmp.string());

std::transform(got.begin(), got.end(), got.begin(), ::tolower);
EXPECT_EQ(got, "5d41402abc4b2a76b9719d911017c592");
fs::remove(tmp);
}
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}