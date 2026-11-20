#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hash_table.h"

// Generate a hash value for a phrase
unsigned int hash_function(const char *str)
{
    unsigned long hash = 5381;

    while (*str)
    {
        hash = ((hash << 5) + hash) + (unsigned char)*str;
        str++;
    }

    return hash % TABLE_SIZE;
}

// Initialize all buckets
void initialize_table(HashNode *table[])
{
    for (int i = 0; i < TABLE_SIZE; i++)
        table[i] = NULL;
}

// Insert a phrase into the hash table
void insert_gram(HashNode *table[], const char *gram)
{
    unsigned int index = hash_function(gram);

    // Avoid inserting duplicates
    if (search_gram(table, gram))
        return;

    HashNode *new_node = malloc(sizeof(HashNode));

    if (new_node == NULL)
        return;

    new_node->gram = malloc(strlen(gram) + 1);

    if (new_node->gram == NULL)
    {
        free(new_node);
        return;
    }

    strcpy(new_node->gram, gram);

    new_node->next = table[index];
    table[index] = new_node;
}

// Search for a phrase
int search_gram(HashNode *table[], const char *gram)
{
    unsigned int index = hash_function(gram);

    HashNode *current = table[index];

    while (current != NULL)
    {
        if (strcmp(current->gram, gram) == 0)
            return 1;

        current = current->next;
    }

    return 0;
}

// Free allocated memory
void free_table(HashNode *table[])
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        HashNode *current = table[i];

        while (current != NULL)
        {
            HashNode *temp = current;
            current = current->next;

            free(temp->gram);
            free(temp);
        }

        table[i] = NULL;
    }
}
#define MAX_WORDS 10000
#define MAX_WORD_LENGTH 100

// Convert text into lowercase words
int split_words(const char *text,
                char words[][MAX_WORD_LENGTH])
{
    int count = 0;
    int length = 0;

    while (*text && count < MAX_WORDS)
    {
        if (isalnum((unsigned char)*text))
        {
            if (length < MAX_WORD_LENGTH - 1)
                words[count][length++] =
                    tolower((unsigned char)*text);
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

// Create a phrase from consecutive words
void make_gram(char *gram,
               char words[][MAX_WORD_LENGTH],
               int start,
               int n)
{
    gram[0] = '\0';

    for (int i = 0; i < n; i++)
    {
        if (i > 0)
            strcat(gram, " ");

        strcat(gram, words[start + i]);
    }
}

// Compare two documents
int compare_documents(HashNode *table[],
                      const char *text1,
                      const char *text2,
                      int n)
{
    if (n <= 0)
        return -1;

    char (*words1)[MAX_WORD_LENGTH] =
        malloc(MAX_WORDS * sizeof(*words1));

    char (*words2)[MAX_WORD_LENGTH] =
        malloc(MAX_WORDS * sizeof(*words2));

    if (words1 == NULL || words2 == NULL)
    {
        free(words1);
        free(words2);
        return -1;
    }

    int count1 = split_words(text1, words1);
    int count2 = split_words(text2, words2);

    if (count1 < n || count2 < n)
    {
        free(words1);
        free(words2);
        return 0;
    }

    char gram[MAX_WORD_LENGTH * 2];

    // Store unique N-grams from Document 1
    for (int i = 0; i <= count1 - n; i++)
    {
        make_gram(gram, words1, i, n);
        insert_gram(table, gram);
    }

    // Search for N-grams from Document 2
    int total = 0;
    int common = 0;

    for (int i = 0; i <= count2 - n; i++)
    {
        make_gram(gram, words2, i, n);
        total++;

        if (search_gram(table, gram))
        {
            common++;
            printf("Common phrase: %s\n", gram);
        }
    }

    double similarity = (double)common / total * 100.0;

    printf("\nTotal N-grams in Document 2: %d\n", total);
    printf("Common N-grams: %d\n", common);
    printf("Similarity: %.2f%%\n", similarity);

    free(words1);
    free(words2);

    return common;
}
double jaccard_similarity(
    HashNode *table[],
    const char *text1,
    const char *text2,
    int n)
{
    if (n <= 0)
        return -1.0;

    char (*words1)[MAX_WORD_LENGTH] =
        malloc(MAX_WORDS * sizeof(*words1));

    char (*words2)[MAX_WORD_LENGTH] =
        malloc(MAX_WORDS * sizeof(*words2));

    if (words1 == NULL || words2 == NULL)
    {
        free(words1);
        free(words2);
        return -1.0;
    }

    int count1 = split_words(text1, words1);
    int count2 = split_words(text2, words2);

    if (count1 < n || count2 < n)
    {
        free(words1);
        free(words2);
        return 0.0;
    }

    // Insert unique N-grams from Document 1
    for (int i = 0; i <= count1 - n; i++)
    {
        char gram[MAX_WORD_LENGTH * 2];

        make_gram(gram, words1, i, n);
        insert_gram(table, gram);
    }

    int intersection = 0;
    int unique_b = 0;

    // Count unique N-grams in Document 2
    for (int i = 0; i <= count2 - n; i++)
    {
        char gram[MAX_WORD_LENGTH * 2];

        make_gram(gram, words2, i, n);

        if (!search_gram(table, gram))
        {
            unique_b++;
            insert_gram(table, gram);
        }
        else
        {
            // Check whether this phrase was already
            // counted in Document 2.
            // A temporary marker is not used here,
            // so duplicates need separate handling.
            intersection++;
        }
    }

    int unique_a = 0;

    for (int i = 0; i < TABLE_SIZE; i++)
    {
        HashNode *current = table[i];

        while (current != NULL)
        {
            unique_a++;
            current = current->next;
        }
    }

    int union_count = unique_a;

    if (union_count == 0)
    {
        free(words1);
        free(words2);
        return 0.0;
    }

    // This implementation is a preliminary demonstration.
    // A proper unique-set Jaccard calculation is added next.

    double result =
        (double)intersection / union_count * 100.0;

    free(words1);
    free(words2);

    return result;
}