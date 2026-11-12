
#include "preprocessing.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <unordered_set>

std::string normalizeText(const std::string& text) {
    std::string result;

    for (unsigned char ch : text) {
        if (std::isalnum(ch) || std::isspace(ch)) {
            result += static_cast<char>(std::tolower(ch));
        } else {
            result += ' ';
        }
    }

    return result;
}

std::vector<std::string> tokenize(const std::string& text) {
    std::vector<std::string> words;
    std::stringstream stream(text);
    std::string word;

    while (stream >> word) {
        words.push_back(word);
    }

    return words;
}

std::vector<std::string> removeStopWords(
    const std::vector<std::string>& words
) {
    static const std::unordered_set<std::string> stopWords = {
        "the", "is", "are", "a", "an", "and", "of",
        "in", "to", "for", "on", "with", "by", "it",
        "this", "that", "was", "were", "be", "as",
        "at", "from", "or", "has", "have"
    };

    std::vector<std::string> filteredWords;

    for (const std::string& word : words) {
        if (stopWords.find(word) == stopWords.end()) {
            filteredWords.push_back(word);
        }
    }

    return filteredWords;
}
