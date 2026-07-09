#include <stdbool.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef struct HashTable HashTable;

typedef struct vEBNode {
  u32 min;
  u32 max;
  bool is_empty;
  int capacity;
  struct vEBNode *top;
  HashTable *bottom;
} vEBNode;

vEBNode *veb_create(u8 capacity);
void veb_insert(vEBNode *veb, u32 x);
bool veb_contains(vEBNode *veb, u32 x);
