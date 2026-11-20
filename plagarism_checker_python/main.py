"""
Main Plagiarism Detection Application (Python Version).
Faithfully matches main.cpp in C++ and main.c in C, with enhanced match location reporting.

Provides:
- Reference document corpus indexing with custom Hash Table and 5-gram fingerprints.
- Fast candidate lookup and dual verification via Rabin-Karp and KMP.
- Jaccard similarity scoring after stop-word removal.
- Detailed ranking and exact plagiarism location reporting (line numbers & context)
  to console and reports/report.txt.
- Direct pairwise document comparison matching C version (optional).
"""

import bisect
import os
import sys
from dataclasses import dataclass, field
from typing import List, Set, Tuple

# Handle relative or absolute imports seamlessly
try:
    from .preprocessing import normalize_text, tokenize, remove_stop_words
    from .hashtable import HashTable
    from .rabinkarp import calculate_hash, rabin_karp_search
    from .kmp import kmp_search
    from .similarity import calculate_jaccard_similarity
except ImportError:
    from preprocessing import normalize_text, tokenize, remove_stop_words
    from hashtable import HashTable
    from rabinkarp import calculate_hash, rabin_karp_search
    from kmp import kmp_search
    from similarity import calculate_jaccard_similarity


@dataclass
class Document:
    name: str = ""
    original_text: str = ""
    normalized_text: str = ""
    words: List[str] = field(default_factory=list)
    line_starts: List[int] = field(default_factory=list)


@dataclass
class PassageMatch:
    phrase: str = ""
    submission_lines: List[int] = field(default_factory=list)
    reference_lines: List[int] = field(default_factory=list)
    submission_snippets: List[str] = field(default_factory=list)
    reference_snippets: List[str] = field(default_factory=list)


@dataclass
class Result:
    document_name: str = ""
    similarity: float = 0.0
    matching_passages: int = 0
    passages: List[str] = field(default_factory=list)
    detailed_matches: List[PassageMatch] = field(default_factory=list)


def compute_line_starts(text: str) -> List[int]:
    """
    Computes starting character offsets for each line in original text.
    Line numbers are 1-indexed.
    """
    starts = [0]
    for idx, ch in enumerate(text):
        if ch == "\n":
            starts.append(idx + 1)
    return starts


def offset_to_line_number(line_starts: List[int], offset: int) -> int:
    """
    Converts a character offset into a 1-indexed line number.
    """
    return bisect.bisect_right(line_starts, offset)


def extract_context_snippet(original_text: str, line_starts: List[int], offset: int, phrase_len: int) -> str:
    """
    Extracts a readable context snippet around the matched offset.
    """
    lines = original_text.splitlines()
    line_num = offset_to_line_number(line_starts, offset)
    if 1 <= line_num <= len(lines):
        line = lines[line_num - 1].strip()
        if len(line) > 120:
            return line[:117] + "..."
        return line
    return ""


def find_phrase_locations(doc: Document, phrase: str) -> Tuple[List[int], List[str]]:
    """
    Locates all occurrences of phrase in document, returning unique line numbers and snippets.
    Uses KMP search on normalized text, which has 1-to-1 index alignment with original text.
    """
    positions = kmp_search(doc.normalized_text, phrase)
    line_numbers: List[int] = []
    snippets: List[str] = []
    seen_lines: Set[int] = set()

    for pos in positions:
        line_num = offset_to_line_number(doc.line_starts, pos)
        if line_num not in seen_lines:
            seen_lines.add(line_num)
            line_numbers.append(line_num)
            snippet = extract_context_snippet(doc.original_text, doc.line_starts, pos, len(phrase))
            if snippet:
                snippets.append(snippet)

    return line_numbers, snippets


def read_file(file_path: str) -> str:
    """
    Read the complete contents of a text file.
    Matches readFile in main.cpp.
    """
    if not os.path.isfile(file_path):
        return ""
    try:
        with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
            return f.read()
    except Exception:
        try:
            with open(file_path, "r", encoding="latin-1", errors="ignore") as f:
                return f.read()
        except Exception:
            return ""


def to_lower_case(text: str) -> str:
    """
    Convert a string to lowercase.
    Matches toLowerCase in main.cpp.
    """
    return text.lower()


def generate_ngrams(words: List[str], n: int = 5) -> List[str]:
    """
    Create sequences of consecutive words (n-grams).
    Matches generateNGrams in main.cpp.
    """
    ngrams: List[str] = []

    if not words:
        return ngrams

    if len(words) < n:
        return [" ".join(words)]

    for i in range(len(words) - n + 1):
        ngrams.append(" ".join(words[i : i + n]))

    return ngrams


def is_text_file(path_str: str) -> bool:
    """
    Check whether a file has a .txt extension.
    Matches isTextFile in main.cpp.
    """
    _, ext = os.path.splitext(path_str)
    return ext.lower() == ".txt"


def find_documents_folder(hint: str = "documents") -> str:
    """
    Locates the reference documents folder from current working directory
    or standard project directories.
    """
    candidate_paths = [
        hint,
        os.path.join("ds", hint),
        os.path.join(os.path.dirname(__file__), "..", hint),
        os.path.join(os.path.dirname(__file__), hint),
    ]
    for p in candidate_paths:
        norm = os.path.normpath(p)
        if os.path.isdir(norm):
            return norm
    return hint


def load_reference_documents(folder_path: str) -> List[Document]:
    """
    Load all reference documents from the documents folder.
    Matches loadReferenceDocuments in main.cpp.
    """
    documents: List[Document] = []

    if not os.path.exists(folder_path) or not os.path.isdir(folder_path):
        print(f"Reference folder not found: {folder_path}")
        return documents

    for filename in sorted(os.listdir(folder_path)):
        full_path = os.path.join(folder_path, filename)
        if not os.path.isfile(full_path) or not is_text_file(full_path):
            continue

        doc = Document()
        doc.name = os.path.normpath(full_path)
        doc.original_text = read_file(doc.name)

        if not doc.original_text:
            print(f"Skipping empty or unreadable file: {doc.name}")
            continue

        doc.normalized_text = normalize_text(doc.original_text)
        doc.line_starts = compute_line_starts(doc.original_text)
        all_words = tokenize(doc.normalized_text)
        doc.words = remove_stop_words(all_words)

        documents.append(doc)

    return documents


def build_index(references: List[Document]) -> HashTable:
    """
    Build a hash-table index of reference-document fingerprints.
    Matches buildIndex in main.cpp.
    """
    index = HashTable(1009)

    for doc in references:
        words = tokenize(doc.normalized_text)
        ngrams = generate_ngrams(words, n=5)

        for phrase in ngrams:
            fingerprint = calculate_hash(phrase)
            index.insert(fingerprint, doc.name)

    return index


def compare_documents(
    submitted: Document,
    references: List[Document],
    index: HashTable
) -> List[Result]:
    """
    Compare the submitted document with all reference documents.
    Matches compareDocuments in main.cpp, with added location detection.
    """
    results: List[Result] = []

    submitted_words = tokenize(submitted.normalized_text)
    submitted_ngrams = generate_ngrams(submitted_words, n=5)

    unique_ngrams: Set[str] = set(submitted_ngrams)

    for reference in references:
        result = Result()
        result.document_name = reference.name

        result.similarity = calculate_jaccard_similarity(
            submitted.words,
            reference.words
        )

        matched_phrases: Set[str] = set()
        detailed_matches: List[PassageMatch] = []

        for phrase in unique_ngrams:
            fingerprint = calculate_hash(phrase)

            # Use custom hash table to find candidate documents
            candidates = index.search(fingerprint)

            if reference.name not in candidates:
                continue

            # Rabin-Karp searches for the matching phrase
            rk_matches = rabin_karp_search(reference.normalized_text, phrase)
            if not rk_matches:
                continue

            # KMP independently verifies and locates the phrase
            kmp_matches = kmp_search(reference.normalized_text, phrase)
            if kmp_matches:
                matched_phrases.add(phrase)

                # Locate line numbers and context snippets in both documents
                sub_lines, sub_snippets = find_phrase_locations(submitted, phrase)
                ref_lines, ref_snippets = find_phrase_locations(reference, phrase)

                match_detail = PassageMatch(
                    phrase=phrase,
                    submission_lines=sub_lines,
                    reference_lines=ref_lines,
                    submission_snippets=sub_snippets,
                    reference_snippets=ref_snippets,
                )
                detailed_matches.append(match_detail)

        result.matching_passages = len(matched_phrases)
        result.passages = list(matched_phrases)
        result.detailed_matches = detailed_matches
        results.append(result)

    # Rank documents from highest to lowest similarity, then by matching passages
    results.sort(
        key=lambda r: (r.similarity, r.matching_passages),
        reverse=True
    )

    return results


def display_and_save_results(
    results: List[Result],
    report_path: str = "reports/report.txt"
) -> None:
    """
    Display and save comparison results with exact plagiarism locations.
    """
    report_dir = os.path.dirname(report_path)
    if report_dir:
        os.makedirs(report_dir, exist_ok=True)

    header = "========== PLAGIARISM DETECTION REPORT =========="
    print(f"\n{header}\n")

    try:
        report_file = open(report_path, "w", encoding="utf-8")
        report_file.write(f"{header}\n\n")
    except Exception as e:
        print(f"Could not create the report file: {e}")
        report_file = None

    if not results:
        msg = "No reference documents were available.\n"
        print(msg)
        if report_file:
            report_file.write(msg)
            report_file.close()
        return

    rank = 1
    for res in results:
        output_block = (
            f"Rank: {rank}\n"
            f"Reference: {res.document_name}\n"
            f"Similarity: {res.similarity:.2f}%\n"
            f"Matching passages: {res.matching_passages}\n"
        )
        print(output_block, end="")
        if report_file:
            report_file.write(f"\nRank: {rank}\n"
                              f"Reference: {res.document_name}\n"
                              f"Similarity: {res.similarity:.2f}%\n"
                              f"Matching passages: {res.matching_passages}\n")

        displayed = 0
        for match in res.detailed_matches:
            if displayed == 5:
                break

            sub_line_str = ", ".join(f"Line {ln}" for ln in match.submission_lines) if match.submission_lines else "Unknown"
            ref_line_str = ", ".join(f"Line {ln}" for ln in match.reference_lines) if match.reference_lines else "Unknown"

            match_text = f"  Matching text: \"{match.phrase}\"\n"
            loc_text = (
                f"    -> In submitted document : {sub_line_str}\n"
                f"    -> In reference document : {ref_line_str}\n"
            )
            context_text = ""
            if match.submission_snippets:
                context_text += f"       Snippet (Submission): \"{match.submission_snippets[0]}\"\n"

            block = match_text + loc_text + context_text
            print(block, end="")
            if report_file:
                report_file.write(block)
            displayed += 1

        print()
        rank += 1

    print(f"Report saved to {report_path}\n")
    note = "Note: Similarity is an indicator, not proof of plagiarism.\n"
    print(note)
    if report_file:
        report_file.write(f"\n{note}")
        report_file.close()


def compare_two_files(file1_path: str, file2_path: str, n: int = 2) -> None:
    """
    Compare two specific text files directly using n-grams.
    Matches compare_documents in C version with added location reporting.
    """
    print("\n====================================")
    print("       PLAGIARISM CHECKER")
    print("====================================")
    print(f"\nComparing {os.path.basename(file1_path)} and {os.path.basename(file2_path)}")
    print("------------------------------------")

    text1 = read_file(file1_path)
    text2 = read_file(file2_path)

    if not text1 or not text2:
        print("Error: Could not read one or both input files.")
        return

    doc1 = Document(
        name=file1_path,
        original_text=text1,
        normalized_text=normalize_text(text1),
        line_starts=compute_line_starts(text1)
    )
    doc2 = Document(
        name=file2_path,
        original_text=text2,
        normalized_text=normalize_text(text2),
        line_starts=compute_line_starts(text2)
    )

    words1 = tokenize(doc1.normalized_text)
    words2 = tokenize(doc2.normalized_text)

    if len(words1) < n or len(words2) < n:
        print("Not enough words to generate N-grams.")
        return

    # Use custom HashTable to store n-grams from Document 1
    table = HashTable(101)
    for i in range(len(words1) - n + 1):
        gram = " ".join(words1[i : i + n])
        table.insert(calculate_hash(gram), gram)

    total = 0
    common = 0
    for i in range(len(words2) - n + 1):
        gram = " ".join(words2[i : i + n])
        total += 1
        matches = table.search(calculate_hash(gram))
        if gram in matches:
            common += 1
            l1, _ = find_phrase_locations(doc1, gram)
            l2, _ = find_phrase_locations(doc2, gram)
            loc1 = f"Doc1: Line {', '.join(str(x) for x in l1)}" if l1 else ""
            loc2 = f"Doc2: Line {', '.join(str(x) for x in l2)}" if l2 else ""
            print(f"Common phrase: \"{gram}\" [{loc1}; {loc2}]")

    similarity = (common / total * 100.0) if total > 0 else 0.0
    print(f"\nTotal N-grams in Document 2: {total}")
    print(f"Common N-grams: {common}")
    print(f"Similarity: {similarity:.2f}%\n")


def main() -> int:
    """
    Main application loop.
    Supports interactive mode or CLI parameters:
      python main.py
      python main.py <submission_file>
      python main.py <file1> <file2>
    """
    if len(sys.argv) == 3:
        compare_two_files(sys.argv[1], sys.argv[2])
        return 0

    print("========================================")
    print("       PLAGIARISM DETECTION SYSTEM")
    print("========================================")
    print("\nLoading reference documents...")

    docs_folder = find_documents_folder("documents")
    references = load_reference_documents(docs_folder)

    if not references:
        print("\nNo reference documents found.")
        print("Reference .txt files will be needed in the "
              "documents folder before comparison.")
        return 0

    print(f"{len(references)} reference document(s) loaded.")

    if len(sys.argv) == 2:
        submitted_path = sys.argv[1]
    else:
        try:
            submitted_path = input("\nEnter the path of the submitted .txt file: ").strip()
        except (KeyboardInterrupt, EOFError):
            print("\nExiting.")
            return 0

    # Allow paths entered with surrounding quotation marks
    if (len(submitted_path) >= 2 and
            submitted_path.startswith('"') and submitted_path.endswith('"')):
        submitted_path = submitted_path[1:-1]

    # Resolve path if relative
    if not os.path.isfile(submitted_path):
        candidate_submission = os.path.join("ds", submitted_path)
        if os.path.isfile(candidate_submission):
            submitted_path = candidate_submission

    if (not os.path.exists(submitted_path) or
            not os.path.isfile(submitted_path) or
            not is_text_file(submitted_path)):
        print("\nInvalid file path or file type.")
        print("Please provide an existing .txt file.")
        return 1

    submitted = Document()
    submitted.name = os.path.normpath(submitted_path)
    submitted.original_text = read_file(submitted_path)

    if not submitted.original_text:
        print("\nThe submitted file is empty or unreadable.")
        return 1

    submitted.normalized_text = normalize_text(submitted.original_text)
    submitted.line_starts = compute_line_starts(submitted.original_text)
    all_words = tokenize(submitted.normalized_text)
    submitted.words = remove_stop_words(all_words)

    if not submitted.words:
        print("\nThe submitted file contains no useful words.")
        return 1

    print("\nBuilding reference fingerprint index...")
    index = build_index(references)

    print("Comparing documents...")
    results = compare_documents(submitted, references, index)

    # Output report path in reports/report.txt (resolving to ds/reports if ds exists)
    report_target = "reports/report.txt"
    if os.path.isdir("ds") and not os.path.isdir("reports"):
        report_target = os.path.join("ds", "reports", "report.txt")

    display_and_save_results(results, report_target)

    print("Analysis completed successfully.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
