"""
Custom Hash Table Module.
Directly corresponds to hashtable.h and hashtable.cpp in C++.
Implements separate chaining using buckets of HashEntry nodes.
"""

from typing import List, Optional


class HashEntry:
    """
    Represents an entry in the hash table with a 64-bit key and associated document name.
    Matches HashEntry in hashtable.h.
    """
    def __init__(self, key: int, document_name: str):
        self.key: int = key
        self.document_name: str = document_name

    def __repr__(self) -> str:
        return f"[{self.key} -> {self.document_name}]"


class HashTable:
    """
    Custom separate chaining hash table.
    Matches HashTable class in hashtable.h / hashtable.cpp.
    """
    def __init__(self, size: int = 101):
        self.capacity: int = 101 if size == 0 else size
        self.table: List[List[HashEntry]] = [[] for _ in range(self.capacity)]

    def hash_function(self, key: int) -> int:
        """
        Modulo hash function matching size_t hashFunction(key).
        """
        return key % self.capacity

    def insert(self, key: int, document_name: str) -> None:
        """
        Inserts a key-document pair into the hash table if not already present.
        Matches HashTable::insert in hashtable.cpp.
        """
        index = self.hash_function(key)

        for entry in self.table[index]:
            if entry.key == key and entry.document_name == document_name:
                return

        self.table[index].append(HashEntry(key, document_name))

    def search(self, key: int) -> List[str]:
        """
        Searches for a key and returns all matching document names.
        Matches HashTable::search in hashtable.cpp.
        """
        results: List[str] = []
        index = self.hash_function(key)

        for entry in self.table[index]:
            if entry.key == key:
                results.append(entry.document_name)

        return results

    def display(self) -> None:
        """
        Displays non-empty buckets of the hash table.
        Matches HashTable::display in hashtable.cpp.
        """
        for i in range(self.capacity):
            if self.table[i]:
                entries = " ".join(f"[{entry.key} -> {entry.document_name}]" for entry in self.table[i])
                print(f"Bucket {i}: {entries} ")

    def get_capacity(self) -> int:
        """
        Returns the bucket capacity.
        Matches HashTable::getCapacity in hashtable.cpp.
        """
        return self.capacity
