"""
Text Preprocessing Module.
Directly corresponds to preprocessing.h and preprocessing.cpp in C++.
"""

from typing import List, Set

# 25 common English stop words matching C++ preprocessing.cpp
STOP_WORDS: Set[str] = {
    "the", "is", "are", "a", "an", "and", "of",
    "in", "to", "for", "on", "with", "by", "it",
    "this", "that", "was", "were", "be", "as",
    "at", "from", "or", "has", "have"
}


def normalize_text(text: str) -> str:
    """
    Converts text to lowercase and replaces non-alphanumeric characters with spaces.
    Matches normalizeText in C++.
    """
    result = []
    for ch in text:
        if ('a' <= ch <= 'z') or ('A' <= ch <= 'Z') or ('0' <= ch <= '9') or ch in " \t\r\n\v\f":
            result.append(ch.lower())
        else:
            result.append(' ')
    return "".join(result)


def tokenize(text: str) -> List[str]:
    """
    Splits whitespace-normalized text into individual words.
    Matches tokenize in C++.
    """
    return text.split()


def remove_stop_words(words: List[str]) -> List[str]:
    """
    Removes common stop words from a list of tokens while preserving word order.
    Matches removeStopWords in C++.
    """
    return [word for word in words if word not in STOP_WORDS]
