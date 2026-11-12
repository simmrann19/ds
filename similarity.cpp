#include "similarity.h"
#include <unordered_set>

double calculateJaccardSimilarity(
    const std::vector<std::string>& wordsA,
    const std::vector<std::string>& wordsB
) {
    std::unordered_set<std::string> setA(
        wordsA.begin(), wordsA.end()
    );

    std::unordered_set<std::string> setB(
        wordsB.begin(), wordsB.end()
    );

    if (setA.empty() && setB.empty()) {
        return 0.0;
    }

    size_t intersectionCount = 0;

    for (const std::string& word : setA) {
        if (setB.find(word) != setB.end()) {
            ++intersectionCount;
        }
    }

    size_t unionCount =
        setA.size() + setB.size() - intersectionCount;

    return (static_cast<double>(intersectionCount) /
            unionCount) * 100.0;
}
