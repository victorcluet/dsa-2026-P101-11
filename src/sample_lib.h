#ifndef SAMPLE_LIB_H
#define SAMPLE_LIB_H

#define MAX_NAME 128
#define MAX_INPUT 256

#define INTERSECTION_BUCKETS 1009

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

int is_valid_map_name(const char *name);
void run_program(void);

void init_house_list(HouseList *list);
int append_house(HouseList *list, const char *street_name, int number,
                 double lat, double lon);
void free_house_list(HouseList *list);
House *find_exact_house(HouseList *list, const char *street_name, int number);
int street_exists(HouseList *list, const char *street_name);
int collect_valid_numbers(HouseList *list, const char *street_name,
                          int numbers[], int max_numbers);

void init_place_list(PlaceList *list);
int append_place(PlaceList *list, const char *name, double lat, double lon);
void free_place_list(PlaceList *list);
Place *find_exact_place(PlaceList *list, const char *name);

int collect_similar_places(PlaceList *list, const char *input,
                           char suggestions[][MAX_NAME], int max_suggestions);

int collect_similar_streets(HouseList *list, const char *input,
                            char suggestions[][MAX_NAME], int max_suggestions);

typedef struct StreetSegment {
  char name[MAX_NAME];
  char id1[MAX_NAME];
  char id2[MAX_NAME];
  double lat1;
  double lon1;
  double lat2;
  double lon2;
  struct StreetSegment *next;
} StreetSegment;

typedef struct {
  StreetSegment *head;
  int count;
} StreetList;

void init_street_list(StreetList *list);
int append_street_segment(StreetList *list, const char *name,
                          const char *id1, const char *id2,
                          double lat1, double lon1,
                          double lat2, double lon2);
void free_street_list(StreetList *list);
StreetSegment *find_closest_street_segment(StreetList *list,
                                           double lat, double lon);
void print_connected_streets(StreetList *list, StreetSegment *segment);

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

void init_intersection_map(IntersectionMap *map);
int build_intersection_map(IntersectionMap *map, StreetList *streets);
void free_intersection_map(IntersectionMap *map);
void print_connected_streets_fast(IntersectionMap *map, StreetSegment *segment);

#endif
