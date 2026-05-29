#include "houses.h"
#include "normalize.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char street[MAX_NAME];
  int distance;
} StreetSuggestion;

void init_house_list(HouseList *list) {
  list->head = NULL;
  list->count = 0;
}

static House *create_house(const char *street_name, int number, double lat,
                           double lon) {
  House *new_house = (House *)malloc(sizeof(House));

  if (new_house == NULL) {
    return NULL;
  }

  strncpy(new_house->street_name, street_name, MAX_NAME - 1);
  new_house->street_name[MAX_NAME - 1] = '\0';
  new_house->number = number;
  new_house->lat = lat;
  new_house->lon = lon;
  new_house->next = NULL;

  return new_house;
}

int append_house(HouseList *list, const char *street_name, int number,
                 double lat, double lon) {
  House *new_house = create_house(street_name, number, lat, lon);
  House *current;

  if (new_house == NULL) {
    return 0;
  }

  if (list->head == NULL) {
    list->head = new_house;
  } else {
    current = list->head;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = new_house;
  }

  list->count++;
  return 1;
}

void free_house_list(HouseList *list) {
  House *current = list->head;
  House *next;

  while (current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }

  list->head = NULL;
  list->count = 0;
}

House *find_exact_house(HouseList *list, const char *street_name, int number) {
  House *current = list->head;
  char norm_input[MAX_INPUT];

  normalize_street(norm_input, street_name);

  while (current != NULL) {
    char norm_cur[MAX_INPUT];
    normalize_street(norm_cur, current->street_name);

    if (strcmp(norm_input, norm_cur) == 0 && current->number == number) {
      return current;
    }

    current = current->next;
  }

  return NULL;
}

int street_exists(HouseList *list, const char *street_name) {
  House *current = list->head;
  char norm_input[MAX_INPUT];

  normalize_street(norm_input, street_name);

  while (current != NULL) {
    char norm_cur[MAX_INPUT];
    normalize_street(norm_cur, current->street_name);

    if (strcmp(norm_input, norm_cur) == 0) {
      return 1;
    }

    current = current->next;
  }

  return 0;
}

int collect_valid_numbers(HouseList *list, const char *street_name,
                          int numbers[], int max_numbers) {
  House *current = list->head;
  char norm_input[MAX_INPUT];
  int count = 0;
  int i;
  int j;

  normalize_street(norm_input, street_name);

  while (current != NULL && count < max_numbers) {
    char norm_cur[MAX_INPUT];
    normalize_street(norm_cur, current->street_name);

    if (strcmp(norm_input, norm_cur) == 0) {
      numbers[count] = current->number;
      count++;
    }

    current = current->next;
  }

  for (i = 0; i < count - 1; i++) {
    for (j = i + 1; j < count; j++) {
      if (numbers[j] < numbers[i]) {
        int temp = numbers[i];
        numbers[i] = numbers[j];
        numbers[j] = temp;
      }
    }
  }

  return count;
}

static int normalized_street_in_list(StreetSuggestion arr[], int count,
                                     const char *street_name) {
  char norm_target[MAX_INPUT];
  int i;

  normalize_street(norm_target, street_name);

  for (i = 0; i < count; i++) {
    char norm_saved[MAX_INPUT];
    normalize_street(norm_saved, arr[i].street);

    if (strcmp(norm_saved, norm_target) == 0) {
      return 1;
    }
  }

  return 0;
}

int collect_similar_streets(HouseList *list, const char *input,
                            char suggestions[][MAX_NAME], int max_suggestions) {
  StreetSuggestion all[1000];
  House *current = list->head;
  char norm_input[MAX_INPUT];
  int count = 0;
  int i;
  int j;

  normalize_street(norm_input, input);

  while (current != NULL) {
    if (!normalized_street_in_list(all, count, current->street_name) &&
        count < 1000) {
      char norm_cur[MAX_INPUT];

      normalize_street(norm_cur, current->street_name);

      strncpy(all[count].street, current->street_name, MAX_NAME - 1);
      all[count].street[MAX_NAME - 1] = '\0';
      all[count].distance = levenshtein(norm_input, norm_cur);
      count++;
    }

    current = current->next;
  }

  for (i = 0; i < count - 1; i++) {
    for (j = i + 1; j < count; j++) {
      if (all[j].distance < all[i].distance) {
        StreetSuggestion temp = all[i];
        all[i] = all[j];
        all[j] = temp;
      }
    }
  }

  if (count > max_suggestions) {
    count = max_suggestions;
  }

  for (i = 0; i < count; i++) {
    strncpy(suggestions[i], all[i].street, MAX_NAME - 1);
    suggestions[i][MAX_NAME - 1] = '\0';
  }

  return count;
}
