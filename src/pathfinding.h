#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "graph.h"
#include "types.h"

int bfs_route(IntersectionMap *map, StreetSegment *origin,
              StreetSegment *destination, Path *result);
void print_route(Path *path);

#endif