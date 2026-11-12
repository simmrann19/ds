#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "preprocessor.h"

#define MAX_WORD_LENGTH 100

// Convert text to lowercase and replace punctuation
// with spaces.
char *normalize_text(const char *text)
{
    if (text == NULL)
        return NULL;

    size_t length = strlen(text);

    char *result = malloc(length + 1);

    if (result == NULL)
        return NULL;

    for (size_t i = 0; i < length; i++)
    {
        unsigned char ch = (unsigned char)text[i];

        if (isalnum(ch))
            result[i] = (char)tolower(ch);
        else
            result[i] = ' ';
    }

    result[length] = '\0';
    return result;
}

// Identify common English stop words.
int is_stop_word(const char *word)
{
    const char *stop_words[] = {
        "the", "is", "are", "a", "an",
        "and", "or", "of", "to", "in",
        "on", "for", "with", "this", "that",
        "it", "by", "be", "as", "was"
    };

    int count = sizeof(stop_words) / sizeof(stop_words[0]);

    for (int i = 0; i < count; i++)
    {
        if (strcmp(word, stop_words[i]) == 0)
            return 1;
    }

    return 0;
}

// Remove stop words while preserving other words.
char *remove_stop_words(const char *text)
{
    if (text == NULL)
        return NULL;

    size_t length = strlen(text);

    char *result = malloc(length + 1);

    if (result == NULL)
        return NULL;

    size_t output = 0;
    size_t i = 0;

    while (i < length)
    {
        while (i < length && isspace((unsigned char)text[i]))
            i++;

        size_t start = i;

        while (i < length && !isspace((unsigned char)text[i]))
            i++;

        size_t word_length = i - start;

        if (word_length == 0)
            continue;

        char word[MAX_WORD_LENGTH];

        if (word_length >= MAX_WORD_LENGTH)
            word_length = MAX_WORD_LENGTH - 1;

        memcpy(word, text + start, word_length);
        word[word_length] = '\0';

        if (!is_stop_word(word))
        {
            if (output > 0)
                result[output++] = ' ';

            memcpy(result + output, word, word_length);
            output += word_length;
        }
    }

    result[output] = '\0';
    return result;
}