
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
#include <vector>

struct HashEntry {
    unsigned long long key;
    std::string documentName;
};

class HashTable {
private:
    std::vector<std::vector<HashEntry>> table;
    size_t capacity;

    size_t hashFunction(unsigned long long key) const;

public:
    explicit HashTable(size_t size = 101);

    void insert(unsigned long long key,
                const std::string& documentName);

    std::vector<std::string> search(unsigned long long key) const;

    void display() const;

    size_t getCapacity() const;
};

#endif
