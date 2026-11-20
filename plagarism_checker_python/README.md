# Plagiarism Checker (Python Implementation)

A faithful Python implementation of the Plagiarism Detection System, mirroring the exact algorithms and logic from the C and C++ implementations, with extended location reporting.

## Features & Implemented Logic

- **Custom Hash Table (`hashtable.py`)**:
  - Implements separate chaining with dynamic buckets.
  - Modulo hash function matching `HashTable::hashFunction(key)`.
  - Collision resolution and duplicate prevention matching C++ `HashTable::insert` and `HashTable::search`.

- **Polynomial Rolling Hash & Rabin-Karp (`rabinkarp.py`)**:
  - Base 256, Prime Modulo $10^9 + 7$ (`1000000007`).
  - Rolling hash calculation matching `calculateHash` in C++.
  - Sliding window pattern search with character-by-character collision verification matching `rabinKarpSearch`.

- **Knuth-Morris-Pratt Algorithm (`kmp.py`)**:
  - Longest Prefix Suffix (LPS) array computation matching `computeLPS`.
  - Deterministic $O(N + M)$ pattern search verification matching `KMPSearch`.

- **Text Preprocessing & Filtering (`preprocessing.py`)**:
  - Alphanumeric lowercasing and whitespace normalization matching `normalizeText`.
  - Word tokenization matching `tokenize`.
  - Common English stop-word filtering (25 words) matching `removeStopWords`.

- **Jaccard Similarity Calculation (`similarity.py`)**:
  - Set-based word intersection and union calculation:
    $$\text{Similarity} = \frac{|A \cap B|}{|A \cup B|} \times 100\%$$

- **Exact Plagiarism Location Reporting (`main.py`)**:
  - Pinpoints the **exact line numbers** in both the submitted document and the reference document.
  - Displays context snippets showing the exact matching sentence/heading.
  - Builds 5-gram fingerprint index across all reference documents in `documents/`.
  - Candidate document filtering using the custom Hash Table.
  - Dual matching verification via Rabin-Karp and KMP.
  - Ranks candidate documents from highest similarity to lowest.
  - Automatically exports the full report to `reports/report.txt`.
  - Supports direct 2-document comparison matching the C version (`main.c`).

## Usage

### 1. Multi-Document Verification (with Location Reporting)

Interactive prompt:
```bash
python ds/plagarism_checker_python/main.py
```
When prompted, enter the path to the document to check:
```text
Enter the path of the submitted .txt file: ds/input/submission.txt
```

Or pass the path directly as a command-line argument:
```bash
python ds/plagarism_checker_python/main.py ds/input/submission.txt
```

#### Example Report Output with Locations
```text
========== PLAGIARISM DETECTION REPORT ==========

Rank: 1
Reference: ds\documents\reference3.txt
Similarity: 19.32%
Matching passages: 3
  Matching text: "neural networks and deep learning"
    -> In submitted document : Line 40, Line 68
    -> In reference document : Line 16
       Snippet (Submission): "4. Neural Networks and Deep Learning"
  Matching text: "of computer science concerned with"
    -> In submitted document : Line 3
    -> In reference document : Line 28
       Snippet (Submission): "Artificial Intelligence (AI) is a field of computer science concerned with developing machines..."
  Matching text: "of artificial intelligence that enables"
    -> In submitted document : Line 8
    -> In reference document : Line 16
       Snippet (Submission): "Machine Learning (ML) is a specialised area of artificial intelligence that enables computer systems..."
```

### 2. Pairwise Document Comparison (C style)

Compare two files directly:
```bash
python ds/plagarism_checker_python/main.py ds/plagarism_checker_c/sample1.txt ds/plagarism_checker_c/sample2.txt
```
