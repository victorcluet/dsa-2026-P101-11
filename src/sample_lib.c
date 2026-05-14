#include "sample_lib.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SUGGESTIONS 5

typedef struct {
  char street[MAX_NAME];
  int distance;
} StreetSuggestion;

static void trim_newline(char *s) {
  if (s == NULL) {
    return;
  }

  int len = (int)strlen(s);
  if (len > 0 && s[len - 1] == '\n') {
    s[len - 1] = '\0';
  }
}

static void to_lowercase(char *dst, const char *src) {
  int i = 0;
  while (src[i] != '\0') {
    dst[i] = (char)tolower((unsigned char)src[i]);
    i++;
  }
  dst[i] = '\0';
}

static void expand_abbreviations(char *s) {
  char temp[MAX_INPUT];
  int i = 0;
  int j = 0;

  while (s[i] != '\0' && isspace((unsigned char)s[i])) {
    i++;
  }

  while (s[i] != '\0') {
    temp[j] = s[i];
    i++;
    j++;
  }
  temp[j] = '\0';

  if (strncmp(temp, "c. ", 3) == 0) {
    snprintf(s, MAX_INPUT, "carrer %s", temp + 3);
  } else if (strncmp(temp, "c/", 2) == 0) {
    snprintf(s, MAX_INPUT, "carrer %s", temp + 2);
  } else {
    strcpy(s, temp);
  }
}

static void normalize_spaces(char *s) {
  char temp[MAX_INPUT];
  int i = 0;
  int j = 0;
  int last_was_space = 1;

  while (s[i] != '\0') {
    if (isspace((unsigned char)s[i])) {
      if (!last_was_space) {
        temp[j] = ' ';
        j++;
      }
      last_was_space = 1;
    } else {
      temp[j] = s[i];
      j++;
      last_was_space = 0;
    }
    i++;
  }

  if (j > 0 && temp[j - 1] == ' ') {
    j--;
  }

  temp[j] = '\0';
  strcpy(s, temp);
}

static void normalize_street(char *dst, const char *src) {
  char temp[MAX_INPUT];
  to_lowercase(temp, src);
  expand_abbreviations(temp);
  normalize_spaces(temp);
  strcpy(dst, temp);
}

static void normalize_place(char *dst, const char *src) {
  to_lowercase(dst, src);
  normalize_spaces(dst);
}

static int min3(int a, int b, int c) {
  int min = a;
  if (b < min) {
    min = b;
  }
  if (c < min) {
    min = c;
  }
  return min;
}

static int levenshtein(const char *s1, const char *s2) {
  int len1 = (int)strlen(s1);
  int len2 = (int)strlen(s2);
  int v0[256];
  int v1[256];
  int i;
  int j;

  for (i = 0; i <= len2; i++) {
    v0[i] = i;
  }

  for (i = 0; i < len1; i++) {
    v1[0] = i + 1;

    for (j = 0; j < len2; j++) {
      int cost;
      if (s1[i] == s2[j]) {
        cost = 0;
      } else {
        cost = 1;
      }

      v1[j + 1] = min3(v1[j] + 1, v0[j + 1] + 1, v0[j] + cost);
    }

    for (j = 0; j <= len2; j++) {
      v0[j] = v1[j];
    }
  }

  return v0[len2];
}

/* we use houses and places linked lists as it is required and it is easier*/

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

/* now using the Levensthein distance we try to find the most similar string to
 * the given by the user */

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

/* here we do all the file loading that we will need*/

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

/* we also do a map validation.*/

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

/* and here comes the main program */

void run_program(void) {
  HouseList houses;
  PlaceList places;
  char map[32];
  char choice[16];
  char street[MAX_INPUT];
  char numstr[16];
  char place_name[MAX_INPUT];
  int number;
  House *h;
  Place *p;

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

  printf("\n--- ORIGIN ---\n");
  printf("Where are you? Address (1), Place (2) or Coordinate (3)? ");
  fgets(choice, sizeof(choice), stdin);
  trim_newline(choice);

  if (strcmp(choice, "3") == 0) {
    printf("Not implemented yet.\n");
    free_house_list(&houses);
    free_place_list(&places);
    return;
  }

  if (strcmp(choice, "2") == 0) {
    printf("Enter place name: ");
    fgets(place_name, sizeof(place_name), stdin);
    trim_newline(place_name);

    p = find_exact_place(&places, place_name);

    if (p != NULL) {
      printf("\nFound at (%.6f, %.6f)\n", p->lat, p->lon);
    } else {
      printf("Place not found\n");
    }

    free_house_list(&houses);
    free_place_list(&places);
    return;
  }

  if (strcmp(choice, "1") != 0) {
    printf("Invalid option.\n");
    free_house_list(&houses);
    free_place_list(&places);
    return;
  }

  printf("Enter street name (e.g. 'Carrer de Roc Boronat'): ");
  fgets(street, sizeof(street), stdin);
  trim_newline(street);

  if (!street_exists(&houses, street)) {
    char suggestions[MAX_SUGGESTIONS][MAX_NAME];
    int n;
    int option;

    n = collect_similar_streets(&houses, street, suggestions, MAX_SUGGESTIONS);

    if (n > 0) {
      int i;

      printf("Street not found. Did you mean:\n");
      for (i = 0; i < n; i++) {
        printf("%d. %s\n", i + 1, suggestions[i]);
      }

      printf("Choose a street (1-%d): ", n);
      fgets(numstr, sizeof(numstr), stdin);
      option = atoi(numstr);

      if (option >= 1 && option <= n) {
        strcpy(street, suggestions[option - 1]);
      } else {
        printf("Invalid option.\n");
        free_house_list(&houses);
        free_place_list(&places);
        return;
      }
    } else {
      printf("Street not found\n");
      free_house_list(&houses);
      free_place_list(&places);
      return;
    }
  }

  printf("Enter street number (e.g. '138'): ");
  fgets(numstr, sizeof(numstr), stdin);
  number = atoi(numstr);

  h = find_exact_house(&houses, street, number);

  if (h == NULL) {
    int nums[1000];
    int n;
    int i;

    n = collect_valid_numbers(&houses, street, nums, 1000);

    if (n > 0) {
      printf("Invalid number. Valid numbers:\n");
      for (i = 0; i < n; i++) {
        printf("%d ", nums[i]);
      }
      printf("\nChoose one number: ");
      fgets(numstr, sizeof(numstr), stdin);
      number = atoi(numstr);

      h = find_exact_house(&houses, street, number);
    }
  }

  if (h != NULL) {
    printf("\nFound at (%.6f, %.6f)\n", h->lat, h->lon);
  } else {
    printf("Address not found\n");
  }

  free_house_list(&houses);
  free_place_list(&places);
}