#include "streets.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_street_list(StreetList *list) {
  list->head = NULL;
  list->count = 0;
}

int append_street_segment(StreetList *list, const char *name, const char *id1,
                          const char *id2, double lat1, double lon1,
                          double lat2, double lon2, double length_meters) {
  StreetSegment *new_segment;
  StreetSegment *current;

  new_segment = (StreetSegment *)malloc(sizeof(StreetSegment));

  if (new_segment == NULL) {
    return 0;
  }

  strncpy(new_segment->name, name, MAX_NAME - 1);
  new_segment->name[MAX_NAME - 1] = '\0';

  strncpy(new_segment->id1, id1, MAX_NAME - 1);
  new_segment->id1[MAX_NAME - 1] = '\0';

  strncpy(new_segment->id2, id2, MAX_NAME - 1);
  new_segment->id2[MAX_NAME - 1] = '\0';

  new_segment->lat1 = lat1;
  new_segment->lon1 = lon1;
  new_segment->lat2 = lat2;
  new_segment->lon2 = lon2;
  new_segment->length_meters = length_meters;
  new_segment->next = NULL;

  if (list->head == NULL) {
    list->head = new_segment;
  } else {
    current = list->head;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = new_segment;
  }

  list->count++;
  return 1;
}

void free_street_list(StreetList *list) {
  StreetSegment *current = list->head;
  StreetSegment *next;

  while (current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }

  list->head = NULL;
  list->count = 0;
}

static double degrees_to_radians(double degrees) {
  return degrees * 3.141592653589793 / 180.0;
}

static double haversine_distance(double lat1, double lon1, double lat2,
                                 double lon2) {
  double earth_radius = 6371000.0;
  double dlat = degrees_to_radians(lat2 - lat1);
  double dlon = degrees_to_radians(lon2 - lon1);
  double a;
  double c;

  lat1 = degrees_to_radians(lat1);
  lat2 = degrees_to_radians(lat2);

  a = sin(dlat / 2.0) * sin(dlat / 2.0) +
      cos(lat1) * cos(lat2) * sin(dlon / 2.0) * sin(dlon / 2.0);

  c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

  return earth_radius * c;
}

StreetSegment *find_closest_street_segment(StreetList *list, double lat,
                                           double lon) {
  StreetSegment *current = list->head;
  StreetSegment *best = NULL;
  double best_distance = -1.0;

  while (current != NULL) {
    double mid_lat;
    double mid_lon;
    double distance;

    mid_lat = (current->lat1 + current->lat2) / 2.0;
    mid_lon = (current->lon1 + current->lon2) / 2.0;

    distance = haversine_distance(lat, lon, mid_lat, mid_lon);

    if (best == NULL || distance < best_distance) {
      best = current;
      best_distance = distance;
    }

    current = current->next;
  }

  return best;
}

static int print_next_different_connected_streets(StreetList *list,
                                                  StreetSegment *segment) {
  StreetSegment *base = segment;
  int steps = 0;

  while (base != NULL && steps < 1000) {
    StreetSegment *current = list->head;
    StreetSegment *next_same = NULL;
    int printed = 0;

    while (current != NULL) {
      if (current != base && strcmp(base->id2, current->id1) == 0) {

        if (strcmp(current->name, segment->name) != 0) {
          printf("        - %s\n", current->name);
          printed = 1;
        } else if (next_same == NULL) {
          next_same = current;
        }
      }

      current = current->next;
    }

    if (printed) {
      return 1;
    }

    base = next_same;
    steps++;
  }

  return 0;
}

void print_connected_streets(StreetList *list, StreetSegment *segment) {
  // StreetSegment *current = list->head;

  printf("\nClosest street: %s\n", segment->name);
  printf("Between %s (%.6f, %.6f) and %s (%.6f, %.6f)\n", segment->id1,
         segment->lat1, segment->lon1, segment->id2, segment->lat2,
         segment->lon2);

  printf("\nFrom this street segment, you can go to:\n");
  printf("    - %s\n", segment->name);
  printf("  Which is connected to:\n");

  print_next_different_connected_streets(list, segment);
}
