#include "sample_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SUGGESTIONS 5

static int load_houses_from_file(HouseList *list, const char *map_name) {
  char path[256];
  FILE *f;
  char line[512];

  snprintf(path, sizeof(path), "maps/%s/houses.txt", map_name);
  f = fopen(path, "r");

  if (f == NULL) {
    snprintf(path, sizeof(path), "../maps/%s/houses.txt", map_name);
    f = fopen(path, "r");
  }

  if (f == NULL) {
    printf("Error opening houses file\n");
    return 0;
  }

  init_house_list(list);

  while (fgets(line, sizeof(line), f) != NULL) {
    char *street = strtok(line, ",");
    char *num = strtok(NULL, ",");
    char *lat = strtok(NULL, ",");
    char *lon = strtok(NULL, ",");

    if (street != NULL && num != NULL && lat != NULL && lon != NULL) {
      append_house(list, street, atoi(num), atof(lat), atof(lon));
    }
  }

  fclose(f);
  printf("%d houses loaded\n", list->count);
  return 1;
}

static int load_places_from_file(PlaceList *list, const char *map_name) {
  char path[256];
  FILE *f;
  char line[512];

  snprintf(path, sizeof(path), "maps/%s/places.txt", map_name);
  f = fopen(path, "r");

  if (f == NULL) {
    snprintf(path, sizeof(path), "../maps/%s/places.txt", map_name);
    f = fopen(path, "r");
  }

  if (f == NULL) {
    printf("Error opening places file\n");
    return 0;
  }

  init_place_list(list);

  while (fgets(line, sizeof(line), f) != NULL) {
    char *id = strtok(line, ",");
    char *name = strtok(NULL, ",");
    char *category = strtok(NULL, ",");
    char *lat = strtok(NULL, ",");
    char *lon = strtok(NULL, ",");

    if (id != NULL && name != NULL && category != NULL && lat != NULL &&
        lon != NULL) {
      trim_newline(name);
      trim_newline(lon);
      append_place(list, name, atof(lat), atof(lon));
    }
  }

  fclose(f);
  printf("%d places loaded\n\n", list->count);
  return 1;
}

static int load_streets_from_file(StreetList *list, const char *map_name) {
  char path[256];
  FILE *f;
  char line[512];

  snprintf(path, sizeof(path), "maps/%s/streets.txt", map_name);
  f = fopen(path, "r");

  if (f == NULL) {
    snprintf(path, sizeof(path), "../maps/%s/streets.txt", map_name);
    f = fopen(path, "r");
  }

  if (f == NULL) {
    printf("Error opening streets file\n");
    return 0;
  }

  init_street_list(list);

  while (fgets(line, sizeof(line), f) != NULL) {
    char *id1 = strtok(line, ",");
    char *lat1 = strtok(NULL, ",");
    char *lon1 = strtok(NULL, ",");
    char *id2 = strtok(NULL, ",");
    char *lat2 = strtok(NULL, ",");
    char *lon2 = strtok(NULL, ",");
    char *length = strtok(NULL, ",");
    char *name = strtok(NULL, "\n");

    if (id1 != NULL && lat1 != NULL && lon1 != NULL && id2 != NULL &&
        lat2 != NULL && lon2 != NULL && length != NULL && name != NULL) {

      trim_newline(name);

      append_street_segment(list, name, id1, id2, atof(lat1), atof(lon1),
                            atof(lat2), atof(lon2), atof(length));
    }
  }

  fclose(f);
  printf("%d streets loaded\n", list->count);
  return 1;
}

/* we also do a map validation*/

int is_valid_map_name(const char *name) {
  const char *valid[] = {"xs_1", "xs_2", "md_1", "lg_1", "xl_1", "2xl_1"};
  int i;

  for (i = 0; i < 6; i++) {
    if (strcmp(name, valid[i]) == 0) {
      return 1;
    }
  }

  return 0;
}

static int ask_position(const char *title, HouseList *houses, PlaceList *places,
                        double *lat, double *lon) {
  char choice[16];
  char street[MAX_INPUT];
  char numstr[16];
  char place_name[MAX_INPUT];
  int number;
  House *h;
  Place *p;

  printf("\n--- %s ---\n", title);
  printf("Where are you? Address (1), Place (2) or Coordinate (3)? ");
  fgets(choice, sizeof(choice), stdin);
  trim_newline(choice);

  if (strcmp(choice, "3") == 0) {
    printf("Enter latitude: ");
    fgets(numstr, sizeof(numstr), stdin);
    *lat = atof(numstr);

    printf("Enter longitude: ");
    fgets(numstr, sizeof(numstr), stdin);
    *lon = atof(numstr);

    printf("\nFound at (%.6f, %.6f)\n", *lat, *lon);
    return 1;
  }

  if (strcmp(choice, "2") == 0) {
    printf("Enter place name: ");
    fgets(place_name, sizeof(place_name), stdin);
    trim_newline(place_name);

    p = find_exact_place(places, place_name);

    if (p == NULL) {
      char suggestions[MAX_SUGGESTIONS][MAX_NAME];
      int n;
      int option;
      int i;

      n = collect_similar_places(places, place_name, suggestions,
                                 MAX_SUGGESTIONS);

      if (n <= 0) {
        printf("Place not found\n");
        return 0;
      }

      printf("Place not found. Did you mean:\n");

      for (i = 0; i < n; i++) {
        printf("%d. %s\n", i + 1, suggestions[i]);
      }

      printf("Choose a place (1-%d): ", n);
      fgets(numstr, sizeof(numstr), stdin);
      option = atoi(numstr);

      if (option < 1 || option > n) {
        printf("Invalid option.\n");
        return 0;
      }

      strcpy(place_name, suggestions[option - 1]);
      p = find_exact_place(places, place_name);
    }

    if (p != NULL) {
      *lat = p->lat;
      *lon = p->lon;

      printf("\nFound at (%.6f, %.6f)\n", *lat, *lon);
      return 1;
    }

    return 0;
  }

  if (strcmp(choice, "1") == 0) {
    printf("Enter street name (e.g. 'Carrer de Roc Boronat'): ");
    fgets(street, sizeof(street), stdin);
    trim_newline(street);

    if (!street_exists(houses, street)) {
      char suggestions[MAX_SUGGESTIONS][MAX_NAME];
      int n;
      int option;
      int i;

      n = collect_similar_streets(houses, street, suggestions, MAX_SUGGESTIONS);

      if (n <= 0) {
        printf("Street not found\n");
        return 0;
      }

      printf("Street not found. Did you mean:\n");

      for (i = 0; i < n; i++) {
        printf("%d. %s\n", i + 1, suggestions[i]);
      }

      printf("Choose a street (1-%d): ", n);
      fgets(numstr, sizeof(numstr), stdin);
      option = atoi(numstr);

      if (option < 1 || option > n) {
        printf("Invalid option.\n");
        return 0;
      }

      strcpy(street, suggestions[option - 1]);
    }

    printf("Enter street number (e.g. '138'): ");
    fgets(numstr, sizeof(numstr), stdin);
    number = atoi(numstr);

    h = find_exact_house(houses, street, number);

    if (h == NULL) {
      int nums[1000];
      int n;
      int i;

      n = collect_valid_numbers(houses, street, nums, 1000);

      if (n > 0) {
        printf("Invalid number. Valid numbers:\n");

        for (i = 0; i < n; i++) {
          printf("%d ", nums[i]);
        }

        printf("\nChoose one number: ");
        fgets(numstr, sizeof(numstr), stdin);
        number = atoi(numstr);

        h = find_exact_house(houses, street, number);
      }
    }

    if (h != NULL) {
      *lat = h->lat;
      *lon = h->lon;

      printf("\nFound at (%.6f, %.6f)\n", *lat, *lon);
      return 1;
    }

    printf("Address not found\n");
    return 0;
  }

  printf("Invalid option.\n");
  return 0;
}

/* and here comes the main program */

void run_program(void) {
  HouseList houses;
  PlaceList places;
  StreetList streets;
  IntersectionMap graph;

  char map[32];

  double origin_lat;
  double origin_lon;
  double destination_lat;
  double destination_lon;

  StreetSegment *origin_segment;
  StreetSegment *destination_segment;

  Path route;

  printf("Enter map name (e.g. 'xs_2' or 'xl_1'): ");
  fgets(map, sizeof(map), stdin);
  trim_newline(map);

  if (!is_valid_map_name(map)) {
    printf("Invalid map\n");
    return;
  }

  if (!load_houses_from_file(&houses, map)) {
    return;
  }

  if (!load_places_from_file(&places, map)) {
    free_house_list(&houses);
    return;
  }

  if (!load_streets_from_file(&streets, map)) {
    free_house_list(&houses);
    free_place_list(&places);
    return;
  }

  init_intersection_map(&graph);
  build_intersection_map(&graph, &streets);

  if (!ask_position("ORIGIN", &houses, &places, &origin_lat, &origin_lon)) {
    free_house_list(&houses);
    free_place_list(&places);
    free_intersection_map(&graph);
    free_street_list(&streets);
    return;
  }

  origin_segment =
      find_closest_street_segment(&streets, origin_lat, origin_lon);

  if (origin_segment == NULL) {
    printf("Could not find origin street\n");
    free_house_list(&houses);
    free_place_list(&places);
    free_intersection_map(&graph);
    free_street_list(&streets);
    return;
  }

  print_connected_streets(&streets, origin_segment);
  print_connected_streets_fast(&graph, origin_segment);

  if (!ask_position("DESTINATION", &houses, &places, &destination_lat,
                    &destination_lon)) {
    free_house_list(&houses);
    free_place_list(&places);
    free_intersection_map(&graph);
    free_street_list(&streets);
    return;
  }

  destination_segment =
      find_closest_street_segment(&streets, destination_lat, destination_lon);

  if (destination_segment == NULL) {
    printf("Could not find destination street\n");
    free_house_list(&houses);
    free_place_list(&places);
    free_intersection_map(&graph);
    free_street_list(&streets);
    return;
  }

  printf("\nDestination closest street: %s\n", destination_segment->name);
  printf("Between %s (%.6f, %.6f) and %s (%.6f, %.6f)\n",
         destination_segment->id1, destination_segment->lat1,
         destination_segment->lon1, destination_segment->id2,
         destination_segment->lat2, destination_segment->lon2);

  if (bfs_route(&graph, origin_segment, destination_segment, &route)) {
    print_route(&route);
  } else {
    printf("\nNo route found\n");
  }

  free_house_list(&houses);
  free_place_list(&places);
  free_intersection_map(&graph);
  free_street_list(&streets);
}
