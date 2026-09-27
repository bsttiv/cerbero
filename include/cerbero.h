#include "../include/veb.h"

typedef uint8_t u8;

typedef struct {
  u8 secret_key[16];
  vEBNode *tree_root;
} CerberoEngine;

CerberoEngine *cerbero_engine_create();
void cerbero_destroy(CerberoEngine *engine);
