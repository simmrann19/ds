#include "kmp.h"
#include <cstddef>

std::vector<size_t> computeLPS(const std::string& pattern) {
    std::vector<size_t> lps(pattern.size(), 0);
    size_t length = 0;
    size_t i = 1;

    while (i < pattern.size()) {
        if (pattern[i] == pattern[length]) {
            ++length;
            lps[i] = length;
            ++i;
        } else if (length != 0) {
            length = lps[length - 1];
        } else {
            lps[i] = 0;
            ++i;
        }
    }

    return lps;
}

std::vector<size_t> KMPSearch(
    const std::string& text,
    const std::string& pattern
) {
    std::vector<size_t> positions;

    if (pattern.empty() || pattern.size() > text.size()) {
        return positions;
    }

    std::vector<size_t> lps = computeLPS(pattern);

    size_t i = 0;
    size_t j = 0;

    while (i < text.size()) {
        if (text[i] == pattern[j]) {
            ++i;
            ++j;
        }

        if (j == pattern.size()) {
            positions.push_back(i - j);
            j = lps[j - 1];
        } else if (i < text.size() && text[i] != pattern[j]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                ++i;
            }
        }
    }

    return positions;
}
