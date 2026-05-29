#ifndef GRAPH_H
#define GRAPH_H

#include "types.h"

void init_intersection_map(IntersectionMap *map);
int build_intersection_map(IntersectionMap *map, StreetList *streets);
void free_intersection_map(IntersectionMap *map);
void print_connected_streets_fast(IntersectionMap *map, StreetSegment *segment);
IntersectionEntry *find_intersection_entry(IntersectionMap *map,
                                           const char *id);

#endif