#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

template <typename KeyT, typename ValT>
class HashMap {
 private:
  struct ChainNode {
    const KeyT key;
    ValT value;
    ChainNode* next;

    ChainNode(KeyT key, ValT value)
        : key(key), value(value), next(nullptr) {
    }

    ChainNode(KeyT key, ValT value, ChainNode* next)
        : key(key), value(value), next(next) {
    }
  };

  ChainNode** data;
  size_t sz;
  size_t capacity;

  ChainNode* curr;
  size_t curr_idx;

  void init_buckets(size_t cap) {
    if (cap == 0) {
      throw invalid_argument("bad");
    }
    data = new ChainNode*[cap];
    for (size_t i = 0; i < cap; i++) {
      data[i] = nullptr;
    }
    sz = 0;
    capacity = cap;
    curr = nullptr;
    curr_idx = 0;
  }

  size_t bucket_index_for_capacity(const KeyT& key, size_t cap) const {
    if (cap == 0) {
      return 0;
    }
    size_t h = hash<KeyT>{}(key);
    return h % cap;
  }


  size_t bucket_index(const KeyT& key) const {
    return bucket_index_for_capacity(key, capacity);
  }

  void rehash(size_t new_cap) {
    if (new_cap == 0 || new_cap == capacity) {
      return;
    }
    ChainNode** new_data = new ChainNode*[new_cap];
    for (size_t i = 0; i < new_cap; i++) {
      new_data[i] = nullptr;
    }
    for (size_t i = 0; i < capacity; i++) {
      ChainNode* node = data[i];
      while (node != nullptr) {
        ChainNode* next = node->next;
        size_t idx = bucket_index_for_capacity(node->key, new_cap);
        node->next = new_data[idx];
        new_data[idx] = node;
        node = next;
      }
    }
    delete[] data;
    data = new_data;
    capacity = new_cap;
    curr = nullptr;
    curr_idx = 0;
  }

 public:
  HashMap() {
    init_buckets(10);
  }

  HashMap(size_t capacity) {
    init_buckets(capacity);
  }

  bool empty() const {
    return sz == 0;
  }

  size_t size() const {
    return sz;
  }

  void insert(KeyT key, ValT value) {
    size_t idx = bucket_index(key);
    ChainNode* node = data[idx];
    while (node != nullptr) {
      if (node->key == key) {
        return;
      }
      node = node->next;
    }

    ChainNode* new_node = new ChainNode(key, value, data[idx]);
    data[idx] = new_node;
    sz++;

    double load = 0.0;
    if (capacity != 0) {
      load = (double)sz / (double)capacity;
    }
    if (load > 1.5) {
      rehash(capacity * 2);
    }
  }

  ValT& at(const KeyT& key) const {
    size_t idx = bucket_index(key);
    ChainNode* node = data[idx];
    while (node != nullptr) {
      if (node->key == key) {
        return node->value;
      }
      node = node->next;
    }
    throw out_of_range("bad");
  }

  bool contains(const KeyT& key) const {
    size_t idx = bucket_index(key);
    ChainNode* node = data[idx];
    while (node != nullptr) {
      if (node->key == key) {
        return true;
      }
      node = node->next;
    }
    return false;
  }

  void clear() {
    for (size_t i = 0; i < capacity; i++) {
      ChainNode* node = data[i];
      while (node != nullptr) {
        ChainNode* next = node->next;
        delete node;
        node = next;
      }
      data[i] = nullptr;
    }
    sz = 0;
    curr = nullptr;
    curr_idx = 0;
  }

  ~HashMap() {
    clear();
    if (data != nullptr) {
      delete[] data;
    }
    data = nullptr;
    capacity = 0;
  }

  ValT erase(const KeyT& key) {
    size_t idx = bucket_index(key);
    ChainNode* node = data[idx];
    ChainNode* prev = nullptr;
    while (node != nullptr) {
      if (node->key == key) {
        ValT result = node->value;
        if (prev == nullptr) {
          data[idx] = node->next;
        } else {
          prev->next = node->next;
        }
        delete node;
        sz--;
        return result;
      }
      prev = node;
      node = node->next;
    }
    throw out_of_range("bad");
  }

  HashMap(const HashMap& other) {
    init_buckets(other.capacity);
    for (size_t i = 0; i < other.capacity; i++) {
      ChainNode* node = other.data[i];
      ChainNode* tail = nullptr;
      while (node != nullptr) {
        ChainNode* new_node = new ChainNode(node->key, node->value);
        if (tail == nullptr) {
          data[i] = new_node;
        } else {
          tail->next = new_node;
        }
        tail = new_node;
        sz++;
        node = node->next;
      }
    }
  }

  HashMap& operator=(const HashMap& other) {
    if (this == &other) {
      return *this;
    }

    clear();
    if (capacity != other.capacity) {
      if (data != nullptr) {
        delete[] data;
      }
      data = nullptr;
      capacity = 0;
      init_buckets(other.capacity);
    }

    sz = 0;
    for (size_t i = 0; i < other.capacity; i++) {
      ChainNode* node = other.data[i];
      ChainNode* tail = nullptr;
      while (node != nullptr) {
        ChainNode* new_node = new ChainNode(node->key, node->value);
        if (tail == nullptr) {
          data[i] = new_node;
        } else {
          tail->next = new_node;
        }
        tail = new_node;
        sz++;
        node = node->next;
      }
    }
    return *this;
  }

  bool operator==(const HashMap& other) const {
    if (sz != other.sz) {
      return false;
    }
    for (size_t i = 0; i < capacity; i++) {
      ChainNode* node = data[i];
      while (node != nullptr) {
        if (!other.contains(node->key)) {
          return false;
        }
        if (other.at(node->key) != node->value) {
          return false;
        }
        node = node->next;
      }
    }
    return true;
  }

  void begin() {
    curr = nullptr;
    curr_idx = 0;
  }

  bool next(KeyT& key, ValT& value) {
    if (capacity == 0) {
      return false;
    }

    if (curr == nullptr) {
      while (curr_idx < capacity && data[curr_idx] == nullptr) {
        curr_idx++;
      }
      if (curr_idx >= capacity) {
        return false;
      }
      curr = data[curr_idx];
    } else {
      if (curr->next != nullptr) {
        curr = curr->next;
      } else {
        curr_idx++;
        curr = nullptr;
        while (curr_idx < capacity && data[curr_idx] == nullptr) {
          curr_idx++;
        }
        if (curr_idx >= capacity) {
          return false;
        }
        curr = data[curr_idx];
      }
    }

    key = curr->key;
    value = curr->value;
    return true;
  }

  void* get_data() const {
    return this->data;
  }

  size_t get_capacity() {
    return this->capacity;
  }
};
