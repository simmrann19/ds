#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#define TABLE_SIZE 101

typedef struct HashNode
{
    char *gram;
    struct HashNode *next;
} HashNode;

void initialize_table(HashNode *table[]);
void insert_gram(HashNode *table[], const char *gram);
int search_gram(HashNode *table[], const char *gram);
void free_table(HashNode *table[]);
int compare_documents(HashNode *table[],
                      const char *text1,
                      const char *text2,
                      int n);
#endif
