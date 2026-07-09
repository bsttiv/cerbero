#include "../include/hashtable.h"
#include "../include/siphash.h"
#include "../include/veb.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <sys/types.h>

void hashtable_resize(HashTable *ht) {
  // if hashtable exceeds load factor of 70%, we resize it
  if (ht->size * 10 >= ht->capacity * 7) {
    // Table Resizing
    HashEntry *hash_table = ht->table;
    HashEntry *new_table = calloc(ht->capacity * 2, sizeof(HashEntry));
    for (int i = 0; i < ht->capacity; i++) {
      if (ht->table[i].cluster != NULL) {
        u64 old_key = ht->table[i].key;
        u64 rehash = siphash(ht->secret_key, (u8 *)&old_key, 8);
        u64 new_index = rehash % (ht->capacity * 2);
        while (new_table[new_index].cluster != NULL)
          new_index = (new_index + 1) % (ht->capacity * 2);
        new_table[new_index].cluster = hash_table[i].cluster;
        new_table[new_index].key = old_key;
      }
    }
    ht->table = new_table;
    ht->capacity *= 2;
    free(hash_table);
  }
}

void generate_key(u8 secret_key[16]) {
  ssize_t result;
  do {
    result = getrandom(secret_key, 16, 0);
  } while (result != 16);
}

HashTable *hashtable_create(int capacity, u8 children_cluster_size) {
  HashTable *ht = malloc(sizeof(HashTable));
  ht->capacity = capacity;
  ht->children_cluster_size = children_cluster_size;
  ht->size = 0;
  generate_key(ht->secret_key);
  ht->table = calloc(capacity, sizeof(HashEntry));
  return ht;
}

void hashtable_insert(HashTable *ht, u64 key, vEBNode *cluster) {
  hashtable_resize(ht);
  u64 hash = siphash(ht->secret_key, (u8 *)&key, 8);
  u64 index = hash % ht->capacity;
  while (ht->table[index].cluster != NULL) {
    if (ht->table[index].key == key) {
      ht->table[index].cluster = cluster;
      return;
    }
    index = (index + 1) % ht->capacity;
  }
  ht->table[index].cluster = cluster;
  ht->table[index].key = key;
  ht->size++;
}

vEBNode *hashtable_get(HashTable *ht, u64 key) {
  hashtable_resize(ht);
  u64 hash = siphash(ht->secret_key, (u8 *)&key, 8);
  u64 index = hash % ht->capacity;
  while (ht->table[index].cluster != NULL) {
    if (ht->table[index].key == key)
      return ht->table[index].cluster;
    index = (index + 1) % ht->capacity;
  }
  return NULL;
}

vEBNode *hashtable_get_or_create(HashTable *ht, u64 key) {
  hashtable_resize(ht);
  u64 hash = siphash(ht->secret_key, (u8 *)&key, 8);
  u64 index = hash % ht->capacity;
  while (ht->table[index].cluster != NULL) {
    if (ht->table[index].key == key)
      return ht->table[index].cluster;
    index = (index + 1) % ht->capacity;
  }
  vEBNode *node = veb_create(ht->children_cluster_size);
  ht->table[index].key = key;
  ht->table[index].cluster = node;
  ht->size++;
  return node;
}
