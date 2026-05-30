#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned int hash_intersection_id(const char *id) {
  unsigned int hash = 0;
  int i = 0;

  while (id[i] != '\0') {
    hash = hash * 31 + (unsigned char)id[i];
    i++;
  }

  return hash % INTERSECTION_BUCKETS;
}

void init_intersection_map(IntersectionMap *map) {
  int i;

  for (i = 0; i < INTERSECTION_BUCKETS; i++) {
    map->buckets[i] = NULL;
  }
}

IntersectionEntry *find_intersection_entry(IntersectionMap *map,
                                           const char *id) {
  unsigned int index = hash_intersection_id(id);
  IntersectionEntry *current = map->buckets[index];

  while (current != NULL) {
    if (strcmp(current->intersection_id, id) == 0) {
      return current;
    }

    current = current->next;
  }

  return NULL;
}

static IntersectionEntry *create_intersection_entry(IntersectionMap *map,
                                                    const char *id) {
  unsigned int index = hash_intersection_id(id);
  IntersectionEntry *entry;

  entry = (IntersectionEntry *)malloc(sizeof(IntersectionEntry));

  if (entry == NULL) {
    return NULL;
  }

  strncpy(entry->intersection_id, id, MAX_NAME - 1);
  entry->intersection_id[MAX_NAME - 1] = '\0';
  entry->segments = NULL;

  entry->next = map->buckets[index];
  map->buckets[index] = entry;

  return entry;
}

static int add_segment_to_intersection(IntersectionMap *map, const char *id,
                                       StreetSegment *segment) {
  IntersectionEntry *entry;
  ConnectionNode *node;

  entry = find_intersection_entry(map, id);

  if (entry == NULL) {
    entry = create_intersection_entry(map, id);
  }

  if (entry == NULL) {
    return 0;
  }

  node = (ConnectionNode *)malloc(sizeof(ConnectionNode));

  if (node == NULL) {
    return 0;
  }

  node->segment = segment;
  node->next = entry->segments;
  entry->segments = node;

  return 1;
}

int build_intersection_map(IntersectionMap *map, StreetList *streets) {
  StreetSegment *current = streets->head;

  while (current != NULL) {
    if (!add_segment_to_intersection(map, current->id1, current)) {
      return 0;
    }

    if (!add_segment_to_intersection(map, current->id2, current)) {
      return 0;
    }

    current = current->next;
  }

  return 1;
}

void free_intersection_map(IntersectionMap *map) {
  int i;

  for (i = 0; i < INTERSECTION_BUCKETS; i++) {
    IntersectionEntry *entry = map->buckets[i];

    while (entry != NULL) {
      IntersectionEntry *next_entry = entry->next;
      ConnectionNode *node = entry->segments;

      while (node != NULL) {
        ConnectionNode *next_node = node->next;
        free(node);
        node = next_node;
      }

      free(entry);
      entry = next_entry;
    }

    map->buckets[i] = NULL;
  }
}

static IntersectionEntry *find_next_different_entry(IntersectionMap *map,
                                                    StreetSegment *segment) {
  StreetSegment *base = segment;
  IntersectionEntry *entry;
  ConnectionNode *node;
  int steps = 0;

  while (base != NULL && steps < 1000) {
    entry = find_intersection_entry(map, base->id2);

    if (entry == NULL) {
      return NULL;
    }

    node = entry->segments;

    while (node != NULL) {
      if (node->segment != base &&
          strcmp(node->segment->name, segment->name) != 0) {
        return entry;
      }

      node = node->next;
    }

    node = entry->segments;
    base = NULL;

    while (node != NULL) {
      if (node->segment != segment &&
          strcmp(node->segment->name, segment->name) == 0) {
        base = node->segment;
      }

      node = node->next;
    }

    steps++;
  }

  return NULL;
}

void print_connected_streets_fast(IntersectionMap *map,
                                  StreetSegment *segment) {
  IntersectionEntry *entry;
  ConnectionNode *node;

  printf("\nFAST HASHMAP VERSION\n");
  printf("From this street segment, you can go to:\n");
  printf("- %s\n", segment->name);
  printf("  Which is connected to:\n");

  entry = find_next_different_entry(map, segment);

  if (entry == NULL) {
    printf("  No connected streets\n");
    return;
  }

  node = entry->segments;

  while (node != NULL) {
    if (node->segment != segment &&
        strcmp(node->segment->name, segment->name) != 0) {
      printf("        - %s\n", node->segment->name);
    }

    node = node->next;
  }
}
