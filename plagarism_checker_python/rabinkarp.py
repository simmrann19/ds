"""
Rabin-Karp String Matching and Rolling Hash Module.
Directly corresponds to rabinkarp.h and rabinkarp.cpp in C++.
"""

from typing import List

BASE: int = 256
MOD: int = 1000000007


def calculate_hash(text: str) -> int:
    """
    Computes 64-bit polynomial rolling hash of text.
    Matches calculateHash in rabinkarp.cpp.
    """
    h: int = 0
    for ch in text:
        h = (h * BASE + ord(ch)) % MOD
    return h


def rabin_karp_search(text: str, pattern: str) -> List[int]:
    """
    Searches for all occurrences of pattern in text using the Rabin-Karp algorithm.
    Matches rabinKarpSearch in rabinkarp.cpp.
    """
    positions: List[int] = []

    n = len(text)
    m = len(pattern)

    if m == 0 or m > n:
        return positions

    pattern_hash = calculate_hash(pattern)
    window_hash = calculate_hash(text[:m])

    highest_power = 1
    for _ in range(1, m):
        highest_power = (highest_power * BASE) % MOD

    for i in range(n - m + 1):
        if pattern_hash == window_hash and text[i:i + m] == pattern:
            positions.append(i)

        if i < n - m:
            leading = (ord(text[i]) * highest_power) % MOD
            window_hash = (window_hash + MOD - leading) % MOD
            window_hash = (window_hash * BASE + ord(text[i + m])) % MOD

    return positions
