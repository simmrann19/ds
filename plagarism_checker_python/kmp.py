"""
Knuth-Morris-Pratt (KMP) String Matching Module.
Directly corresponds to kmp.h and kmp.cpp in C++.
"""

from typing import List


def compute_lps(pattern: str) -> List[int]:
    """
    Computes the Longest Prefix Suffix (LPS) array for the given pattern.
    Matches computeLPS in kmp.cpp.
    """
    lps: List[int] = [0] * len(pattern)
    length: int = 0
    i: int = 1

    while i < len(pattern):
        if pattern[i] == pattern[length]:
            length += 1
            lps[i] = length
            i += 1
        elif length != 0:
            length = lps[length - 1]
        else:
            lps[i] = 0
            i += 1

    return lps


def kmp_search(text: str, pattern: str) -> List[int]:
    """
    Searches for all occurrences of pattern in text using KMP.
    Matches KMPSearch in kmp.cpp.
    """
    positions: List[int] = []

    if not pattern or len(pattern) > len(text):
        return positions

    lps = compute_lps(pattern)

    i = 0
    j = 0

    while i < len(text):
        if text[i] == pattern[j]:
            i += 1
            j += 1

        if j == len(pattern):
            positions.append(i - j)
            j = lps[j - 1]
        elif i < len(text) and text[i] != pattern[j]:
            if j != 0:
                j = lps[j - 1]
            else:
                i += 1

    return positions
