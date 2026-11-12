#include <stdio.h>
#include <stdlib.h>

#include "file_handler.h"
#include "hash_table.h"

int main(void)
{
    char *text1 = read_file("documents/sample1.txt");
    char *text2 = read_file("documents/sample2.txt");

    if (text1 == NULL || text2 == NULL)
    {
        free(text1);
        free(text2);
        return 1;
    }

    HashNode *table[TABLE_SIZE];

    initialize_table(table);

    printf("\n====================================\n");
    printf("       PLAGIARISM CHECKER\n");
    printf("====================================\n");

    printf("\nComparing sample1.txt and sample2.txt\n");
    printf("------------------------------------\n");

    int n = 2;

    compare_documents(table, text1, text2, n);

    free_table(table);
    free(text1);
    free(text2);

    return 0;
}