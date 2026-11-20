"""
Similarity Calculation Module.
Directly corresponds to similarity.h and similarity.cpp in C++.
"""

from typing import List, Set


def calculate_jaccard_similarity(words_a: List[str], words_b: List[str]) -> float:
    """
    Calculates Jaccard similarity percentage between two word token sets.
    Matches calculateJaccardSimilarity in similarity.cpp.
    """
    set_a: Set[str] = set(words_a)
    set_b: Set[str] = set(words_b)

    if not set_a and not set_b:
        return 0.0

    intersection_count = len(set_a.intersection(set_b))
    union_count = len(set_a) + len(set_b) - intersection_count

    if union_count == 0:
        return 0.0

    return (float(intersection_count) / float(union_count)) * 100.0
