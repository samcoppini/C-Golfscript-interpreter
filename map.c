// map.c
// Contains functions for manipulating maps
// Really only used to keep track of assigned variables

#include <stdlib.h>
#include <string.h>
#include "golf.h"

#define MAP_INIT_SIZE 64
#define MAP_MAX_LOAD_FACTOR 0.6

// Implements the djb2 hash function over a key
static uint32_t hash(String *key) {
  uint32_t hash_val = 5381;
  for (uint32_t i = 0; i < key->length; i++) {
    hash_val = ((hash_val << 5) + hash_val) + key->str_data[i];
  }
  return hash_val;
}

// Gets the appropriate slot for a given key in a given map
static uint32_t get_slot(Map *map, String *key) {
  uint32_t mask = map->allocated - 1;
  return hash(key) & mask;
}

Map new_map() {
  Map map = {
    .keys = calloc(MAP_INIT_SIZE, sizeof(String *)),
    .items = malloc(sizeof(Item) * MAP_INIT_SIZE),
    .num_items = 0,
    .allocated = MAP_INIT_SIZE
  };
  return map;
}

void free_map(Map *map) {
  for (uint32_t i = 0; i < map->allocated; i++) {
    if (map->keys[i] != NULL) {
      free_string(map->keys[i]);
      free(map->keys[i]);
      free_item(&map->items[i]);
    }
  }
  free(map->keys);
  free(map->items);
}

// Doubles the size of a map, rehashing all the keys
static void map_increase_size(Map *map) {
  String **old_keys = map->keys;
  Item *old_items = map->items;
  uint32_t old_size = map->allocated;

  map->allocated <<= 1;
  map->keys = calloc(map->allocated, sizeof(String *));
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

void map_set(Map *map, String key, Item item) {
  uint32_t slot = get_slot(map, &key);

  while (map->keys[slot] != NULL) {
    if (string_compare(map->keys[slot], &key) == 0) {
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

  map->keys[slot] = malloc(sizeof(String));
  *map->keys[slot] = key;
  map->items[slot] = item;
  map->num_items++;
  if (map->num_items >= map->allocated * MAP_MAX_LOAD_FACTOR) {
    map_increase_size(map);
  }
}

Item *map_get(Map *map, String *key) {
  uint32_t slot = get_slot(map, key);
  while (map->keys[slot] != NULL) {
    if (string_compare(map->keys[slot], key) == 0)
      return &map->items[slot];
    slot++;
    if (slot == map->allocated)
      slot = 0;
  }
  return NULL;
}
