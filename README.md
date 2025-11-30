# C++ HashMap from Scratch

This project is a C++ implementation of a **chaining hash table**.  
I wrote it for a data structures course to practice pointers, dynamic memory, and testing.

---

## 1. What this project does

- Stores key–value pairs in a hash map (like `std::unordered_map` but built by hand).
- Handles collisions using **separate chaining** with linked lists.
- Automatically resizes when the load factor gets too high.
- Includes tests that check correctness and basic memory safety.

---

## 2. How the hash map is designed

At a high level:

1. The table is a dynamic array of buckets.
2. Each bucket is a singly linked list of nodes.
3. Each node stores a `key`, `value`, and a `next` pointer.
4. To find a bucket, the code computes `hash(key) % capacity`.
5. If the load factor (`size / capacity`) gets too large, the table is rehashed into a bigger array.

The core operations I implemented:

- `insert(key, value)` – add or update a mapping.
- `contains(key)` – check if a key exists.
- `at(key)` – return the value for a key (throws on missing key).
- `erase(key)` – remove a mapping if it exists.
- `clear()` – remove everything without leaking memory.
- Copy constructor / assignment operator – deep copy all chains.

---
