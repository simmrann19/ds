"""
Plagiarism Checker Package (Python Implementation).
Replicates the data structures and algorithms of the C and C++ versions:
- Custom Chained Hash Table
- Polynomial Rolling Hash (Rabin-Karp)
- Longest Prefix Suffix (KMP)
- N-gram generation
- Jaccard Similarity index
- Text Normalization and Stop-words Filtering
"""

from .preprocessing import normalize_text, tokenize, remove_stop_words
from .hashtable import HashTable, HashEntry
from .rabinkarp import calculate_hash, rabin_karp_search
from .kmp import compute_lps, kmp_search
from .similarity import calculate_jaccard_similarity

__all__ = [
    "normalize_text",
    "tokenize",
    "remove_stop_words",
    "HashTable",
    "HashEntry",
    "calculate_hash",
    "rabin_karp_search",
    "compute_lps",
    "kmp_search",
    "calculate_jaccard_similarity",
]
