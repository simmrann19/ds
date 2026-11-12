#include "rabinkarp.h"
#include <cstddef>

namespace {
    const unsigned long long BASE = 256;
    const unsigned long long MOD = 1000000007ULL;
}

unsigned long long calculateHash(const std::string& text) {
    unsigned long long hash = 0;

    for (unsigned char ch : text) {
        hash = (hash * BASE + ch) % MOD;
    }

    return hash;
}

std::vector<size_t> rabinKarpSearch(
    const std::string& text,
    const std::string& pattern
) {
    std::vector<size_t> positions;

    size_t n = text.size();
    size_t m = pattern.size();

    if (m == 0 || m > n) {
        return positions;
    }

    unsigned long long patternHash = calculateHash(pattern);
    unsigned long long windowHash =
        calculateHash(text.substr(0, m));

    unsigned long long highestPower = 1;

    for (size_t i = 1; i < m; ++i) {
        highestPower = (highestPower * BASE) % MOD;
    }

    for (size_t i = 0; i <= n - m; ++i) {
        if (patternHash == windowHash &&
            text.compare(i, m, pattern) == 0) {
            positions.push_back(i);
        }

        if (i < n - m) {
            unsigned long long leading =
                (static_cast<unsigned char>(text[i]) *
                 highestPower) % MOD;

            windowHash = (windowHash + MOD - leading) % MOD;

            windowHash =
                (windowHash * BASE +
                 static_cast<unsigned char>(text[i + m])) % MOD;
        }
    }

    return positions;
}
