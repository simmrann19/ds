#ifndef SIMILARITY_H
#define SIMILARITY_H

#include <string>
#include <vector>

double calculateJaccardSimilarity(
    const std::vector<std::string>& wordsA,
    const std::vector<std::string>& wordsB
);

#endif
