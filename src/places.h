#ifndef PLACES_H
#define PLACES_H

#include "types.h"

void init_place_list(PlaceList *list);
int append_place(PlaceList *list, const char *name, double lat, double lon);
void free_place_list(PlaceList *list);
Place *find_exact_place(PlaceList *list, const char *name);
int collect_similar_places(PlaceList *list, const char *input,
                           char suggestions[][MAX_NAME], int max_suggestions);

#endif