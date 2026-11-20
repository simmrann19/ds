
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <algorithm>
#include <iomanip>

#include "preprocessing.h"
#include "hashtable.h"
#include "kmp.h"
#include "rabinkarp.h"
#include "similarity.h"

namespace fs = std::filesystem;

struct Document {
    std::string name;
    std::string originalText;
    std::string normalizedText;
    std::vector<std::string> words;
};

struct Result {
    std::string documentName;
    double similarity = 0.0;
    int matchingPassages = 0;
    std::vector<std::string> passages;
};

// Read the complete contents of a text file.
std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

// Convert a string to lowercase.
std::string toLowerCase(const std::string& text) {
    std::string result = text;

    std::transform(
        result.begin(),
        result.end(),
        result.begin(),
        [](unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        }
    );

    return result;
}

// Create sequences of consecutive words (n-grams).
std::vector<std::string> generateNGrams(
    const std::vector<std::string>& words,
    size_t n = 5
) {
    std::vector<std::string> ngrams;

    if (words.empty()) {
        return ngrams;
    }

    if (words.size() < n) {
        std::string phrase;

        for (const std::string& word : words) {
            if (!phrase.empty()) {
                phrase += " ";
            }
            phrase += word;
        }

        ngrams.push_back(phrase);
        return ngrams;
    }

    for (size_t i = 0; i <= words.size() - n; ++i) {
        std::string phrase;

        for (size_t j = 0; j < n; ++j) {
            if (!phrase.empty()) {
                phrase += " ";
            }

            phrase += words[i + j];
        }

        ngrams.push_back(phrase);
    }

    return ngrams;
}

// Check whether a file has a .txt extension.
bool isTextFile(const fs::path& path) {
    return toLowerCase(path.extension().string()) == ".txt";
}

// Load all reference documents from the documents folder.
std::vector<Document> loadReferenceDocuments(
    const std::string& folderPath
) {
    std::vector<Document> documents;

    if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
        std::cout << "Reference folder not found: "
                  << folderPath << '\n';
        return documents;
    }

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (!entry.is_regular_file() || !isTextFile(entry.path())) {
            continue;
        }

        Document doc;
        doc.name = entry.path().string();
        doc.originalText = readFile(doc.name);

        if (doc.originalText.empty()) {
            std::cout << "Skipping empty or unreadable file: "
                      << doc.name << '\n';
            continue;
        }

        doc.normalizedText = normalizeText(doc.originalText);

        std::vector<std::string> allWords =
            tokenize(doc.normalizedText);

        doc.words = removeStopWords(allWords);

        documents.push_back(doc);
    }

    return documents;
}

// Build a hash-table index of reference-document fingerprints.
HashTable buildIndex(const std::vector<Document>& references) {
    HashTable index(1009);

    for (const Document& doc : references) {
        std::vector<std::string> words =
            tokenize(doc.normalizedText);

        std::vector<std::string> ngrams =
            generateNGrams(words);

        for (const std::string& phrase : ngrams) {
            unsigned long long fingerprint =
                calculateHash(phrase);

            index.insert(fingerprint, doc.name);
        }
    }

    return index;
}

// Compare the submitted document with all reference documents.
std::vector<Result> compareDocuments(
    const Document& submitted,
    const std::vector<Document>& references,
    const HashTable& index
) {
    std::vector<Result> results;

    std::vector<std::string> submittedWords =
        tokenize(submitted.normalizedText);

    std::vector<std::string> submittedNGrams =
        generateNGrams(submittedWords);

    // Avoid checking identical n-grams repeatedly.
    std::unordered_set<std::string> uniqueNGrams(
        submittedNGrams.begin(),
        submittedNGrams.end()
    );

    for (const Document& reference : references) {
        Result result;
        result.documentName = reference.name;

        result.similarity = calculateJaccardSimilarity(
            submitted.words,
            reference.words
        );

        std::unordered_set<std::string> matchedPhrases;

        for (const std::string& phrase : uniqueNGrams) {
            unsigned long long fingerprint =
                calculateHash(phrase);

            // Use the custom hash table to find candidate documents.
            std::vector<std::string> candidates =
                index.search(fingerprint);

            bool isCandidate = false;

            for (const std::string& candidate : candidates) {
                if (candidate == reference.name) {
                    isCandidate = true;
                    break;
                }
            }

            if (!isCandidate) {
                continue;
            }

            // Rabin-Karp searches for the matching phrase.
            std::vector<size_t> rkMatches =
                rabinKarpSearch(reference.normalizedText, phrase);

            if (rkMatches.empty()) {
                continue;
            }

            // KMP independently verifies and locates the phrase.
            std::vector<size_t> kmpMatches =
                KMPSearch(reference.normalizedText, phrase);

            if (!kmpMatches.empty()) {
                matchedPhrases.insert(phrase);
            }
        }

        result.matchingPassages =
            static_cast<int>(matchedPhrases.size());

        for (const std::string& phrase : matchedPhrases) {
            result.passages.push_back(phrase);
        }

        results.push_back(result);
    }

    // Rank documents from highest to lowest similarity.
    std::sort(
        results.begin(),
        results.end(),
        [](const Result& a, const Result& b) {
            if (a.similarity != b.similarity) {
                return a.similarity > b.similarity;
            }

            return a.matchingPassages > b.matchingPassages;
        }
    );

    return results;
}

// Display and save the comparison results.
void displayAndSaveResults(const std::vector<Result>& results) {
    fs::create_directories("reports");

    std::ofstream report("reports/report.txt");

    if (!report.is_open()) {
        std::cout << "Could not create the report file.\n";
        return;
    }

    std::cout << "\n========== PLAGIARISM DETECTION REPORT ==========\n";

    report << "========== PLAGIARISM DETECTION REPORT ==========\n";

    if (results.empty()) {
        std::cout << "No reference documents were available.\n";
        report << "No reference documents were available.\n";
        return;
    }

    int rank = 1;

    for (const Result& result : results) {
        std::cout << "\nRank: " << rank
                  << "\nReference: " << result.documentName
                  << "\nSimilarity: " << std::fixed
                  << std::setprecision(2)
                  << result.similarity << "%"
                  << "\nMatching passages: "
                  << result.matchingPassages << '\n';

        report << "\nRank: " << rank
               << "\nReference: " << result.documentName
               << "\nSimilarity: " << std::fixed
               << std::setprecision(2)
               << result.similarity << "%"
               << "\nMatching passages: "
               << result.matchingPassages << '\n';

        int displayed = 0;

        for (const std::string& phrase : result.passages) {
            if (displayed == 5) {
                break;
            }

            std::cout << "  Matching text: " << phrase << '\n';
            report << "  Matching text: " << phrase << '\n';

            ++displayed;
        }

        ++rank;
    }

    std::cout << "\nReport saved to reports/report.txt\n";

    report << "\nNote: Similarity is an indicator, not proof of plagiarism.\n";
    report.close();
}

// Main application.
int main() {
    std::cout << "========================================\n";
    std::cout << "       PLAGIARISM DETECTION SYSTEM\n";
    std::cout << "========================================\n";

    std::cout << "\nLoading reference documents...\n";

    std::vector<Document> references =
        loadReferenceDocuments("documents");

    if (references.empty()) {
        std::cout << "\nNo reference documents found.\n";
        std::cout << "Reference .txt files will be needed in the "
                     "documents folder before comparison.\n";
        return 0;
    }

    std::cout << references.size()
              << " reference document(s) loaded.\n";

    std::cout << "\nEnter the path of the submitted .txt file: ";

    std::string submittedPath;
    std::getline(std::cin, submittedPath);

    // Allow paths entered with surrounding quotation marks.
    if (submittedPath.size() >= 2 &&
        submittedPath.front() == '"' &&
        submittedPath.back() == '"') {
        submittedPath = submittedPath.substr(
            1, submittedPath.size() - 2
        );
    }

    if (!fs::exists(submittedPath) ||
        !fs::is_regular_file(submittedPath) ||
        !isTextFile(submittedPath)) {
        std::cout << "\nInvalid file path or file type.\n";
        std::cout << "Please provide an existing .txt file.\n";
        return 1;
    }

    Document submitted;
    submitted.name = submittedPath;
    submitted.originalText = readFile(submittedPath);

    if (submitted.originalText.empty()) {
        std::cout << "\nThe submitted file is empty or unreadable.\n";
        return 1;
    }

    submitted.normalizedText =
        normalizeText(submitted.originalText);

    std::vector<std::string> allWords =
        tokenize(submitted.normalizedText);

    submitted.words = removeStopWords(allWords);

    if (submitted.words.empty()) {
        std::cout << "\nThe submitted file contains no useful words.\n";
        return 1;
    }

    std::cout << "\nBuilding reference fingerprint index...\n";

    HashTable index = buildIndex(references);

    std::cout << "Comparing documents...\n";

    std::vector<Result> results =
        compareDocuments(submitted, references, index);

    displayAndSaveResults(results);

    std::cout << "\nAnalysis completed successfully.\n";

    return 0;
}
