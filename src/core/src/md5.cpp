
#include "../include/scanner_core/md5.h"
#include <array>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <math.h>

namespace scanner_core {
    using uint32 = uint32_t;
    using uint8 = uint8_t;

    static uint32 leftrotate(uint32 x, uint32 c) {
        return (x << c) | (x >> (32 - c));
    }

    std::string to_hex(const std::array<uint8, 16> &digest) {
        std::ostringstream ss;
        ss << std::hex << std::setfill('0');
        for (auto b: digest)
            ss << std::setw(2) << (static_cast<unsigned>(b) &
                                   0xFF);
        return ss.str();
    }

    std::array<uint8, 16> md5_binary(const std::vector<uint8> &msg) {
        uint32 s[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                      5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
                      4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                      6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};
        uint32 K[64];
        for (int i = 0; i < 64; ++i)
            K[i] =
                    static_cast<uint32>(std::floor(std::abs(std::sin(i + 1)) * (1ull << 32)));
        uint32 a0 = 0x67452301;
        uint32 b0 = 0xefcdab89;
        uint32 c0 = 0x98badcfe;
        uint32 d0 = 0x10325476;
        std::vector<uint8> msg2 = msg;
        uint64_t original_len_bits = static_cast<uint64_t>(msg.size()) * 8;
        msg2.push_back(0x80);
        while ((msg2.size() % 64) != 56) msg2.push_back(0);
        for (int i = 0; i < 8; ++i) {
            msg2.push_back(static_cast<uint8>(original_len_bits & 0xFF));
            original_len_bits >>= 8;
        }
        for (size_t offset = 0; offset < msg2.size(); offset += 64) {
            uint32 M[16];
            for (int i = 0; i < 16; ++i) {
                size_t idx = offset + i * 4;
                M[i] = static_cast<uint32>(msg2[idx]) |
                       (static_cast<uint32>(msg2[idx + 1]) << 8) | (static_cast<uint32>(msg2[idx + 2])
                        << 16) | (static_cast<uint32>(msg2[idx + 3]) << 24);
            }
            uint32 A = a0;
            uint32 B = b0;
            uint32 C = c0;
            uint32 D = d0;
            for (int i = 0; i < 64; ++i) {
                uint32 F, g;
                if (i < 16) {
                    F = (B & C) | ((~B) & D);
                    g = i;
                }
                else if (i < 32) {
                    F = (D & B) | ((~D) & C);
                    g = (5 * i + 1) %
                        16;
                } else if (i < 48) {
                    F = B ^ C ^ D;
                    g = (3 * i + 5) % 16;
                }
                else {
                    F = C ^ (B | (~D));
                    g = (7 * i) % 16;
                }
                uint32 temp = D;
                D = C;
                C = B;
                uint32 sum = A + F + K[i] + M[g];

                B = B + leftrotate(sum, s[i]);
                A = temp;
            }
            a0 += A;
            b0 += B;
            c0 += C;
            d0 += D;
        }
        std::array<uint8, 16> digest;
        uint32 parts[4] = {a0, b0, c0, d0};
        for (int i = 0; i < 4; ++i) {
            digest[i * 4 + 0] = static_cast<uint8>(parts[i] & 0xFF);
            digest[i * 4 + 1] = static_cast<uint8>((parts[i] >> 8) & 0xFF);
            digest[i * 4 + 2] = static_cast<uint8>((parts[i] >> 16) & 0xFF);
            digest[i * 4 + 3] = static_cast<uint8>((parts[i] >> 24) & 0xFF);
        }
        return digest;
    }

    std::string md5_of_file(const std::string &filepath) {
        const size_t BUF_SIZE = 4 * 1024 * 1024;
        std::ifstream in(filepath, std::ios::binary);
        if (!in) return std::string();
        std::vector<uint8> buffer;
        buffer.reserve(1024);
        std::vector<uint8> chunk;
        chunk.reserve(BUF_SIZE);
        while (true) {
            chunk.resize(BUF_SIZE);
            in.read(reinterpret_cast<char *>(chunk.data()),
                    static_cast<std::streamsize>(BUF_SIZE));
            std::streamsize got = in.gcount();
            if (got <= 0) break;
            chunk.resize(static_cast<size_t>(got));
            buffer.insert(buffer.end(), chunk.begin(), chunk.end());
        }
        auto digest = md5_binary(buffer);
        return to_hex(digest);
    }
}