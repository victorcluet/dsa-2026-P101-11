#include "pathfinding.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VISITED_BUCKETS 1009

typedef struct QueueNode {
  Path path;
  struct QueueNode *next;
} QueueNode;

static void enqueue(QueueNode **front, QueueNode **back, Path path) {
  QueueNode *node = malloc(sizeof(QueueNode));
  if (node == NULL)
    return;

  node->path = path;
  node->next = NULL;

  if (*back == NULL) {
    *front = node;
    *back = node;
  } else {
    (*back)->next = node;
    *back = node;
  }
}

static int dequeue(QueueNode **front, QueueNode **back, Path *path) {
  QueueNode *temp;

  if (*front == NULL)
    return 0;

  temp = *front;
  *path = temp->path;
  *front = (*front)->next;

  if (*front == NULL) {
    *back = NULL;
  }

  free(temp);
  return 1;
}

typedef struct VisitedNode {
  StreetSegment *segment;
  struct VisitedNode *next;
} VisitedNode;

typedef struct {
  VisitedNode *buckets[VISITED_BUCKETS];
} VisitedSet;

static unsigned int hash_segment_pointer(StreetSegment *segment) {
  return ((unsigned long)segment) % VISITED_BUCKETS;
}

static void init_visited_set(VisitedSet *set) {
  int i;

  for (i = 0; i < VISITED_BUCKETS; i++) {
    set->buckets[i] = NULL;
  }
}

static int visited_contains(VisitedSet *set, StreetSegment *segment) {
  unsigned int index = hash_segment_pointer(segment);
  VisitedNode *current = set->buckets[index];

  while (current != NULL) {
    if (current->segment == segment) {
      return 1;
    }

    current = current->next;
  }

  return 0;
}

static int visited_add(VisitedSet *set, StreetSegment *segment) {
  unsigned int index;
  VisitedNode *node;

  if (visited_contains(set, segment)) {
    return 1;
  }

  index = hash_segment_pointer(segment);
  node = malloc(sizeof(VisitedNode));

  if (node == NULL) {
    return 0;
  }

  node->segment = segment;
  node->next = set->buckets[index];
  set->buckets[index] = node;

  return 1;
}

static void free_visited_set(VisitedSet *set) {
  int i;

  for (i = 0; i < VISITED_BUCKETS; i++) {
    VisitedNode *current = set->buckets[i];

    while (current != NULL) {
      VisitedNode *next = current->next;
      free(current);
      current = next;
    }

    set->buckets[i] = NULL;
  }
}

int bfs_route(IntersectionMap *map, StreetSegment *origin,
              StreetSegment *destination, Path *result) {
  QueueNode *front = NULL;
  QueueNode *back = NULL;
  VisitedSet visited;
  Path initial;

  init_visited_set(&visited);

  initial.length = 1;
  initial.segments[0] = origin;

  enqueue(&front, &back, initial);
  visited_add(&visited, origin);

  while (dequeue(&front, &back, result)) {
    StreetSegment *last;
    IntersectionEntry *entry;
    ConnectionNode *node;

    last = result->segments[result->length - 1];

    if (last == destination) {
      free_visited_set(&visited);
      return 1;
    }

    entry = find_intersection_entry(map, last->id2);

    if (entry != NULL) {
      node = entry->segments;

      while (node != NULL) {
        if (!visited_contains(&visited, node->segment) &&
            result->length < MAX_PATH) {
          Path new_path = *result;
          new_path.segments[new_path.length] = node->segment;
          new_path.length++;
          visited_add(&visited, node->segment);
          enqueue(&front, &back, new_path);
        }

        node = node->next;
      }
    }
  }

  free_visited_set(&visited);
  return 0;
}

static double cross_product(StreetSegment *a, StreetSegment *b) {
  double ax = a->lon2 - a->lon1;
  double ay = a->lat2 - a->lat1;
  double bx = b->lon2 - b->lon1;
  double by = b->lat2 - b->lat1;

  return ax * by - ay * bx;
}

static const char *turn_direction(StreetSegment *a, StreetSegment *b) {
  double cross = cross_product(a, b);

  if (cross > 0.00000001) {
    return "Turn left";
  }

  if (cross < -0.00000001) {
    return "Turn right";
  }

  return "Continue straight";
}

void print_route(Path *path) {
  int i;

  printf("\n--- ROUTE ---\n");

  if (path->length == 0) {
    printf("No route found\n");
    return;
  }

  printf("Start at %s\n", path->segments[0]->name);

  i = 1;

  while (i < path->length) {
    const char *street_name;
    const char *direction;
    double total_distance;

    street_name = path->segments[i]->name;
    direction = turn_direction(path->segments[i - 1], path->segments[i]);
    total_distance = 0;

    while (i < path->length &&
           strcmp(path->segments[i]->name, street_name) == 0) {
      total_distance += path->segments[i]->length_meters;
      i++;
    }

    printf("%s to %s and continue for %.0fm\n", direction, street_name,
           total_distance);
  }

  printf("You have arrived to %s\n", path->segments[path->length - 1]->name);
}
