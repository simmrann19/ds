
#ifndef RABINKARP_H
#define RABINKARP_H

#include <string>
#include <vector>

std::vector<size_t> rabinKarpSearch(
    const std::string& text,
    const std::string& pattern
);

unsigned long long calculateHash(const std::string& text);

#endif
