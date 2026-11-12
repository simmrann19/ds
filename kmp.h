
#ifndef KMP_H
#define KMP_H

#include <string>
#include <vector>

std::vector<size_t> computeLPS(const std::string& pattern);

std::vector<size_t> KMPSearch(
    const std::string& text,
    const std::string& pattern
);

#endif
