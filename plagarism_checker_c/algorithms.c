
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "algorithms.h"

// Construct the LPS array
void compute_lps(const char *pattern, int m, int lps[])
{
    int len = 0;
    int i = 1;

    lps[0] = 0;

    while (i < m)
    {
        if (pattern[i] == pattern[len])
        {
            len++;
            lps[i] = len;
            i++;
        }
        else
        {
            if (len != 0)
            {
                len = lps[len - 1];
            }
            else
            {
                lps[i] = 0;
                i++;
            }
        }
    }
}

// KMP searching algorithm
int kmp_search(const char *text, const char *pattern)
{
    int n = strlen(text);
    int m = strlen(pattern);

    if (m == 0)
        return 1;

    if (n < m)
        return 0;

    int lps[m];

    compute_lps(pattern, m, lps);

    int i = 0;
    int j = 0;

    while (i < n)
    {
        if (text[i] == pattern[j])
        {
            i++;
            j++;
        }

        if (j == m)
        {
            return 1;
        }
        else if (i < n && text[i] != pattern[j])
        {
            if (j != 0)
            {
                j = lps[j - 1];
            }
            else
            {
                i++;
            }
        }
    }

    return 0;
}

#define HASH_BASE 256
#define HASH_MOD 101

// Rabin-Karp string matching algorithm
int rabin_karp_search(const char *text, const char *pattern)
{
    int n = strlen(text);
    int m = strlen(pattern);

    if (m == 0)
        return 1;

    if (m > n)
        return 0;

    int pattern_hash = 0;
    int text_hash = 0;
    int highest_power = 1;

    // Calculate highest power of HASH_BASE
    for (int i = 0; i < m - 1; i++)
    {
        highest_power = (highest_power * HASH_BASE) % HASH_MOD;
    }

    // Calculate initial pattern and text hashes
    for (int i = 0; i < m; i++)
    {
        pattern_hash =
            (HASH_BASE * pattern_hash + (unsigned char)pattern[i])
            % HASH_MOD;

        text_hash =
            (HASH_BASE * text_hash + (unsigned char)text[i])
            % HASH_MOD;
    }

    // Slide the pattern across the text
    for (int i = 0; i <= n - m; i++)
    {
        // Check hashes
        if (pattern_hash == text_hash)
        {
            // Verify actual characters to avoid hash collisions
            if (strncmp(text + i, pattern, m) == 0)
                return 1;
        }

        // Calculate next rolling hash
        if (i < n - m)
        {
            text_hash =
                (HASH_BASE *
                (text_hash -
                (unsigned char)text[i] * highest_power)
                + (unsigned char)text[i + m]) % HASH_MOD;

            // Handle negative hash values
            if (text_hash < 0)
                text_hash += HASH_MOD;
        }
    }

    return 0;
}

#define MAX_WORDS 10000
#define MAX_WORD_LENGTH 100

// Convert text into individual words
int tokenize_text(const char *text, char words[][MAX_WORD_LENGTH])
{
    int count = 0;
    int length = 0;

    while (*text && count < MAX_WORDS)
    {
        if (isalnum((unsigned char)*text))
        {
            if (length < MAX_WORD_LENGTH - 1)
            {
                words[count][length++] =
                    tolower((unsigned char)*text);
            }
        }
        else if (length > 0)
        {
            words[count][length] = '\0';
            count++;
            length = 0;
        }

        text++;
    }

    if (length > 0 && count < MAX_WORDS)
    {
        words[count][length] = '\0';
        count++;
    }

    return count;
}

// Count the number of possible N-grams
int count_ngrams(const char *text, int n)
{
    if (n <= 0)
        return 0;

    char (*words)[MAX_WORD_LENGTH] =
        malloc(MAX_WORDS * sizeof(*words));

    if (words == NULL)
        return -1;

    int total_words = tokenize_text(text, words);

    int result = total_words >= n
        ? total_words - n + 1 : 0;

    free(words);

    return result;
}

// Display all generated N-grams
void display_ngrams(const char *text, int n)
{
    if (n <= 0)
    {
        printf("Invalid N-gram size.\n");
        return;
    }

    char (*words)[MAX_WORD_LENGTH] =
        malloc(MAX_WORDS * sizeof(*words));

    if (words == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    int total_words = tokenize_text(text, words);

    if (total_words < n)
    {
        printf("Not enough words to generate N-grams.\n");
        free(words);
        return;
    }

    printf("\nGenerated %d-grams:\n\n", n);

    for (int i = 0; i <= total_words - n; i++)
    {
        printf("%d. ", i + 1);

        for (int j = 0; j < n; j++)
        {
            printf("%s", words[i + j]);

            if (j < n - 1)
                printf(" ");
        }

        printf("\n");
    }

    free(words);
}
// Boyer-Moore bad-character rule
int boyer_moore_search(const char *text, const char *pattern)
{
    int n = strlen(text);
    int m = strlen(pattern);

    if (m == 0)
        return 1;

    if (m > n)
        return 0;

    int bad_char[256];

    // Initialize all characters as absent
    for (int i = 0; i < 256; i++)
        bad_char[i] = -1;

    // Store the last occurrence of each pattern character
    for (int i = 0; i < m; i++)
        bad_char[(unsigned char)pattern[i]] = i;

    int shift = 0;

    while (shift <= n - m)
    {
        int j = m - 1;

        // Compare from right to left
        while (j >= 0 &&
               pattern[j] == text[shift + j])
        {
            j--;
        }

        if (j < 0)
        {
            return 1;  // Pattern found
        }
        else
        {
            int last = bad_char[
                (unsigned char)text[shift + j]
            ];

            int movement = j - last;

            if (movement < 1)
                movement = 1;

            shift += movement;
        }
    }

    return 0;  // Pattern not found
}