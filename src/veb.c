#include "../include/hashtable.h"
#include "../include/veb.h"
#include <stddef.h>
#include <stdlib.h>

vEBNode *veb_create(u8 capacity, u8 *secret_key) {
  vEBNode *node = malloc(sizeof(vEBNode));
  node->capacity = capacity;
  node->is_empty = true;
  node->top = NULL;
  node->secret_key = secret_key;
  if (capacity > 1) {
    node->bottom = hashtable_create(64, capacity / 2, secret_key);
  } else {
    node->bottom = NULL;
  }
  return node;
}

#define HIGH(x, b) ((x) >> (b))
#define LOW(x, b) ((x) & ((1ULL << b) - 1))

void veb_insert(vEBNode *veb, u32 x) {
  if (veb->is_empty) {
    veb->min = x;
    veb->max = x;
    veb->is_empty = false;
    return;
  }
  if (x < veb->min) {
    u32 temp = veb->min;
    veb->min = x;
    x = temp;
  }
  if (x > veb->max) {
    veb->max = x;
  }

  if (veb->capacity <= 1)
    return;

  u8 lower_bits = veb->capacity / 2;
  u8 upper_bits = veb->capacity - lower_bits;
  u32 high = HIGH(x, lower_bits);
  u32 low = LOW(x, lower_bits);
  vEBNode *bottom = hashtable_get_or_create(veb->bottom, high);
  if (bottom->is_empty) {
    if (veb->top == NULL)
      veb->top = veb_create(upper_bits, veb->secret_key);
    veb_insert(veb->top, high);
  }
  veb_insert(bottom, low);
}

bool veb_contains(vEBNode *veb, u32 x) {
  if (veb->is_empty)
    return false;

  if (x == veb->min || x == veb->max)
    return true;

  if (veb->capacity <= 1)
    return false;

  u8 lower_bits = veb->capacity / 2;
  u32 high = HIGH(x, lower_bits);
  u32 low = LOW(x, lower_bits);
  vEBNode *bottom = hashtable_get(veb->bottom, high);
  if (bottom == NULL)
    return false;
  return veb_contains(bottom, low);
}

void veb_destroy(vEBNode *veb) {
  if (veb == NULL) {
    return;
  }
  if (veb->top != NULL) {
    vEBNode *to_free = veb->top;
    veb->top = NULL;
    veb_destroy(to_free);
  }
  if (veb->bottom != NULL) {
    HashTable *to_free = veb->bottom;
    veb->bottom = NULL;
    hashtable_destroy(to_free);
  }

  free(veb);
}
