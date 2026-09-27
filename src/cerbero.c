#include "../include/cerbero.h"
#include "sys/random.h"
#include <stdlib.h>

void generate_key(u8 secret_key[16]) {
  ssize_t result;
  do {
    result = getrandom(secret_key, 16, 0);
  } while (result != 16);
}

CerberoEngine *cerbero_engine_create() {
  CerberoEngine *engine = malloc(sizeof(CerberoEngine));
  u8 secret_key[16];
  generate_key(secret_key);
  engine->tree_root = veb_create(32, secret_key);
  return engine;
}

void cerbero_destroy(CerberoEngine *engine) {
  free(engine->tree_root);
  free(engine->secret_key);
  free(engine);
}
