#ifndef TYPES_H
#define TYPES_H

#define MAX_NAME 128
#define MAX_INPUT 256
#define INTERSECTION_BUCKETS 1009
#define MAX_PATH 10000

typedef struct House {
  char street_name[MAX_NAME];
  int number;
  double lat;
  double lon;
  struct House *next;
} House;

typedef struct {
  House *head;
  int count;
} HouseList;

typedef struct Place {
  char name[MAX_NAME];
  double lat;
  double lon;
  struct Place *next;
} Place;

typedef struct {
  Place *head;
  int count;
} PlaceList;

typedef struct StreetSegment {
  char name[MAX_NAME];
  char id1[MAX_NAME];
  char id2[MAX_NAME];
  double lat1;
  double lon1;
  double lat2;
  double lon2;
  double length_meters;
  struct StreetSegment *next;
} StreetSegment;

typedef struct {
  StreetSegment *head;
  int count;
} StreetList;

typedef struct ConnectionNode {
  StreetSegment *segment;
  struct ConnectionNode *next;
} ConnectionNode;

typedef struct IntersectionEntry {
  char intersection_id[MAX_NAME];
  ConnectionNode *segments;
  struct IntersectionEntry *next;
} IntersectionEntry;

typedef struct {
  IntersectionEntry *buckets[INTERSECTION_BUCKETS];
} IntersectionMap;

typedef struct {
  StreetSegment *segments[MAX_PATH];
  int length;
} Path;

#endif