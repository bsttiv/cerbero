#include <stdint.h>

typedef uint64_t u64;
typedef uint8_t u8;

typedef struct vEBNode vEBNode;

typedef struct HashEntry {
  u64 key;
  vEBNode *cluster;
} HashEntry;

typedef struct HashTable {
  HashEntry *table;
  int capacity;
  int size;
  u8 secret_key[16];
  u8 children_cluster_size;
} HashTable;

HashTable *hashtable_create(int capacity, u8 children_cluster_size);
void hashtable_insert(HashTable *ht, u64 key, vEBNode *cluster);
vEBNode *hashtable_get(HashTable *ht, u64 key);
vEBNode *hashtable_get_or_create(HashTable *ht, u64 key);
