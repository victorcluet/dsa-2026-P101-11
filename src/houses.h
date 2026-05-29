#ifndef HOUSES_H
#define HOUSES_H

#include "types.h"

void init_house_list(HouseList *list);
int append_house(HouseList *list, const char *street_name, int number,
                 double lat, double lon);
void free_house_list(HouseList *list);
House *find_exact_house(HouseList *list, const char *street_name, int number);
int street_exists(HouseList *list, const char *street_name);
int collect_valid_numbers(HouseList *list, const char *street_name,
                          int numbers[], int max_numbers);
int collect_similar_streets(HouseList *list, const char *input,
                            char suggestions[][MAX_NAME], int max_suggestions);

#endif