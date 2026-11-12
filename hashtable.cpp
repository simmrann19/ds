#include "hashtable.h"
#include <iostream>

HashTable::HashTable(size_t size) {
    capacity = (size == 0) ? 101 : size;
    table.resize(capacity);
}

size_t HashTable::hashFunction(unsigned long long key) const {
    return static_cast<size_t>(key % capacity);
}

void HashTable::insert(unsigned long long key,
                       const std::string& documentName) {
    size_t index = hashFunction(key);

    for (const HashEntry& entry : table[index]) {
        if (entry.key == key && entry.documentName == documentName) {
            return;
        }
    }

    table[index].push_back({key, documentName});
}

std::vector<std::string> HashTable::search(
    unsigned long long key) const {
    std::vector<std::string> results;
    size_t index = hashFunction(key);

    for (const HashEntry& entry : table[index]) {
        if (entry.key == key) {
            results.push_back(entry.documentName);
        }
    }

    return results;
}

void HashTable::display() const {
    for (size_t i = 0; i < capacity; ++i) {
        if (!table[i].empty()) {
            std::cout << "Bucket " << i << ": ";

            for (const HashEntry& entry : table[i]) {
                std::cout << "[" << entry.key
                          << " -> " << entry.documentName << "] ";
            }

            std::cout << '\n';
        }
    }
}

size_t HashTable::getCapacity() const {
    return capacity;
}
