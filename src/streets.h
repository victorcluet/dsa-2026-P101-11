#ifndef STREETS_H
#define STREETS_H

#include "types.h"

void init_street_list(StreetList *list);
void init_street_list(StreetList *list);
int append_street_segment(StreetList *list, const char *name, const char *id1,
                          const char *id2, double lat1, double lon1,
                          double lat2, double lon2, double length_meters);
void free_street_list(StreetList *list);
StreetSegment *find_closest_street_segment(StreetList *list, double lat,
                                           double lon);
void print_connected_streets(StreetList *list, StreetSegment *segment);

#endif