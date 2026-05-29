#include "places.h"
#include "normalize.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char name[MAX_NAME];
  int distance;
} PlaceSuggestion;

void init_place_list(PlaceList *list) {
  list->head = NULL;
  list->count = 0;
}

static Place *create_place(const char *name, double lat, double lon) {
  Place *new_place = (Place *)malloc(sizeof(Place));

  if (new_place == NULL) {
    return NULL;
  }

  strncpy(new_place->name, name, MAX_NAME - 1);
  new_place->name[MAX_NAME - 1] = '\0';
  new_place->lat = lat;
  new_place->lon = lon;
  new_place->next = NULL;

  return new_place;
}

int append_place(PlaceList *list, const char *name, double lat, double lon) {
  Place *new_place = create_place(name, lat, lon);
  Place *current;

  if (new_place == NULL) {
    return 0;
  }

  if (list->head == NULL) {
    list->head = new_place;
  } else {
    current = list->head;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = new_place;
  }

  list->count++;
  return 1;
}

void free_place_list(PlaceList *list) {
  Place *current = list->head;
  Place *next;

  while (current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }

  list->head = NULL;
  list->count = 0;
}

Place *find_exact_place(PlaceList *list, const char *name) {
  Place *current = list->head;
  char norm_input[MAX_INPUT];

  normalize_place(norm_input, name);

  while (current != NULL) {
    char norm_cur[MAX_INPUT];
    normalize_place(norm_cur, current->name);

    if (strcmp(norm_input, norm_cur) == 0) {
      return current;
    }

    current = current->next;
  }

  return NULL;
}

static int normalized_place_in_list(PlaceSuggestion arr[], int count,
                                    const char *place_name) {
  char norm_target[MAX_INPUT];
  int i;

  normalize_place(norm_target, place_name);

  for (i = 0; i < count; i++) {
    char norm_saved[MAX_INPUT];
    normalize_place(norm_saved, arr[i].name);

    if (strcmp(norm_saved, norm_target) == 0) {
      return 1;
    }
  }

  return 0;
}

int collect_similar_places(PlaceList *list, const char *input,
                           char suggestions[][MAX_NAME], int max_suggestions) {
  PlaceSuggestion *all;
  Place *current = list->head;
  char norm_input[MAX_INPUT];
  int count = 0;
  int i;
  int j;

  all = malloc(sizeof(PlaceSuggestion) * list->count);

  if (all == NULL) {
    return 0;
  }

  normalize_place(norm_input, input);

  while (current != NULL) {
    if (!normalized_place_in_list(all, count, current->name) &&
        count < list->count) {
      char norm_cur[MAX_INPUT];

      normalize_place(norm_cur, current->name);

      strncpy(all[count].name, current->name, MAX_NAME - 1);
      all[count].name[MAX_NAME - 1] = '\0';
      all[count].distance = levenshtein(norm_input, norm_cur);
      count++;
    }

    current = current->next;
  }

  for (i = 0; i < count - 1; i++) {
    for (j = i + 1; j < count; j++) {
      if (all[j].distance < all[i].distance) {
        PlaceSuggestion temp = all[i];
        all[i] = all[j];
        all[j] = temp;
      }
    }
  }

  if (count > max_suggestions) {
    count = max_suggestions;
  }

  for (i = 0; i < count; i++) {
    strncpy(suggestions[i], all[i].name, MAX_NAME - 1);
    suggestions[i][MAX_NAME - 1] = '\0';
  }

  free(all);

  return count;
}
