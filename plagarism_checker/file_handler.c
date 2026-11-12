
#include <stdio.h>
#include <stdlib.h>
#include "file_handler.h"

char *read_file(const char *filename)
{
    FILE *fp;
    char *buffer;
    long file_size;
    size_t bytes_read;

    // Open file in read mode
    fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        printf("Error: Cannot open file %s\n", filename);
        return NULL;
    }

    // Move to the end of the file
    fseek(fp, 0, SEEK_END);

    // Find the file size
    file_size = ftell(fp);

    if (file_size < 0)
    {
        fclose(fp);
        return NULL;
    }

    // Return to the beginning
    rewind(fp);

    // Allocate memory for file contents
    buffer = malloc((size_t)file_size + 1);

    if (buffer == NULL)
    {
        printf("Memory allocation failed!\n");
        fclose(fp);
        return NULL;
    }

    // Read file contents
    bytes_read = fread(buffer, 1, (size_t)file_size, fp);

    if (bytes_read != (size_t)file_size)
    {
        printf("Error reading file!\n");
        free(buffer);
        fclose(fp);
        return NULL;
    }

    // Add string termination character
    buffer[bytes_read] = '\0';

    fclose(fp);

    return buffer;
}
