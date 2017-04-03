#include <stdlib.h>
#include <string.h>
#include "golf.h"

#define MAP_INIT_SIZE 8
#define MAP_MAX_LOAD_FACTOR 0.6

// Implements the djb2 hash function
static uint32_t hash(char *key) {
  uint32_t hash_val = 5381;
  while (*key) {
    hash_val = ((hash_val << 5) + hash_val) + *key;
    key++;
  }
  return hash_val;
}

static uint32_t get_slot(Map *map, char *key) {
  uint32_t mask = map->allocated - 1;
  return hash(key) & mask;
}

Map new_map() {
  Map map = {
    .keys = calloc(MAP_INIT_SIZE, sizeof(char *)),
    .items = malloc(sizeof(Item) * MAP_INIT_SIZE),
    .num_items = 0,
    .allocated = MAP_INIT_SIZE
  };
  return map;
}

bool map_has(Map *map, char *key) {
  uint32_t slot = get_slot(map, key);
  while (map->keys[slot] != NULL) {
    if (strcmp(map->keys[slot], key) == 0)
      return true;

    slot++;
    if (slot == map->allocated)
      slot = 0;
  }
  return false;
}

static void map_increase_size(Map *map) {
  char **old_keys = map->keys;
  Item *old_items = map->items;
  uint32_t old_size = map->allocated;

  map->allocated <<= 1;
  map->keys = calloc(map->allocated, sizeof(char *));
  map->items = malloc(sizeof(Item) * map->allocated);

  for (uint32_t i = 0; i < old_size; i++) {
    if (old_keys[i] != NULL) {
      uint32_t slot = get_slot(map, old_keys[i]);
      while (map->keys[slot] != NULL) {
        slot++;
        if (slot == map->allocated)
          slot = 0;
      }
      map->keys[slot] = old_keys[i];
      map->items[slot] = old_items[i];
    }
  }

  free(old_keys);
  free(old_items);
}

void map_set(Map *map, char *key, Item item) {
  uint32_t slot = get_slot(map, key);

  while (map->keys[slot] != NULL) {
    if (strcmp(map->keys[slot], key) == 0) {
      free_item(&map->items[slot]);
      map->items[slot] = item;
      return;
    }
    else {
      slot++;
      if (slot == map->allocated)
        slot = 0;
    }
  }

  map->keys[slot] = malloc(strlen(key) + 1);
  strcpy(map->keys[slot], key);
  map->items[slot] = item;
  map->num_items++;
  if (map->num_items >= map->allocated * MAP_MAX_LOAD_FACTOR) {
    map_increase_size(map);
  }
}

Item *map_get(Map *map, char *key) {
  uint32_t slot = get_slot(map, key);
  while (map->keys[slot] != NULL) {
    if (strcmp(map->keys[slot], key) == 0)
      return &map->items[slot];
    slot++;
    if (slot == map->allocated)
      slot = 0;
  }
  return NULL;
}