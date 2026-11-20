
#ifndef ALGORITHMS_H
#define ALGORITHMS_H

// KMP string matching function
int kmp_search(const char *text, const char *pattern);

int rabin_karp_search(const char *text, const char *pattern);
// N-gram functions
int count_ngrams(const char *text, int n);

void display_ngrams(const char *text, int n);
int boyer_moore_search(const char *text, const char *pattern);
#endif
