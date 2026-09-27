#include "../include/cerbero.h"
#include <sys/random.h>
#include <stdlib.h>
#include <unistd.h>

void generate_key(u8 secret_key[16]) {
  ssize_t result;
  do {
    result = getrandom(secret_key, 16, 0);
  } while (result != 16);
}

CerberoEngine *cerbero_engine_create(void) {
  CerberoEngine *engine = malloc(sizeof(CerberoEngine));
  if (!engine) {
    return NULL;
  }

  generate_key(engine->secret_key);
  engine->tree_root = veb_create(32, engine->secret_key);
  if (!engine->tree_root) {
    free(engine);
    return NULL;
  }

  return engine;
}

void cerbero_destroy(CerberoEngine *engine) {
  if (!engine) {
    return;
  }
  if (engine->tree_root) {
    veb_destroy(engine->tree_root);
    engine->tree_root = NULL;
  }
  free(engine);
}
