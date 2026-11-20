
#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include <string>
#include <vector>

std::string normalizeText(const std::string& text);

std::vector<std::string> tokenize(const std::string& text);

std::vector<std::string> removeStopWords(
    const std::vector<std::string>& words
);

#endif
