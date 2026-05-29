#include "sample_lib.h"
#include "../test/utils.h"
#include <assert.h>
#include <stddef.h>
#include <string.h>

void test_valid_map_name_xl1() {
  int result;

  runningtest("test_valid_map_name_xl1");

  result = is_valid_map_name("xl_1");
  assertEqualsInt(result, 1);

  successtest();
}

void test_valid_map_name_xs1() {
  int result;

  runningtest("test_valid_map_name_xs1");

  result = is_valid_map_name("xs_1");
  assertEqualsInt(result, 1);

  successtest();
}

void test_invalid_map_name() {
  int result;

  runningtest("test_invalid_map_name");

  result = is_valid_map_name("abc");
  assertEqualsInt(result, 0);

  successtest();
}

void test_init_house_list() {
  HouseList list;

  runningtest("test_init_house_list");

  init_house_list(&list);

  assert(list.head == NULL);
  assertEqualsInt(list.count, 0);

  successtest();
}

void test_append_house_increases_count() {
  HouseList list;

  runningtest("test_append_house_increases_count");

  init_house_list(&list);
  append_house(&list, "Carrer de Roc Boronat", 138, 41.403981, 2.193255);

  assertEqualsInt(list.count, 1);

  free_house_list(&list);
  successtest();
}

void test_append_two_houses_links_nodes() {
  HouseList list;

  runningtest("test_append_two_houses_links_nodes");

  init_house_list(&list);

  append_house(&list, "Carrer de Roc Boronat", 138, 41.403981, 2.193255);
  append_house(&list, "Carrer de Roc Boronat", 140, 41.404050, 2.193400);

  assertEqualsInt(list.count, 2);
  assertEqualsInt(list.head != NULL, 1);
  assertEqualsInt(list.head->number, 138);
  assertEqualsInt(list.head->next != NULL, 1);
  assertEqualsInt(list.head->next->number, 140);

  free_house_list(&list);
  successtest();
}

void test_find_exact_house_found() {
  HouseList list;
  House *found;

  runningtest("test_find_exact_house_found");

  init_house_list(&list);
  append_house(&list, "Carrer de Roc Boronat", 138, 41.403981, 2.193255);

  found = find_exact_house(&list, "Carrer de Roc Boronat", 138);

  assertEqualsInt(found != NULL, 1);
  assertEqualsInt(found->number, 138);

  free_house_list(&list);
  successtest();
}

void test_find_exact_house_with_abbreviation() {
  HouseList list;
  House *found;

  runningtest("test_find_exact_house_with_abbreviation");

  init_house_list(&list);
  append_house(&list, "Carrer de Roc Boronat", 138, 41.403981, 2.193255);

  found = find_exact_house(&list, "c. de Roc Boronat", 138);

  assertEqualsInt(found != NULL, 1);
  assertEqualsInt(found->number, 138);

  free_house_list(&list);
  successtest();
}

void test_find_exact_house_not_found() {
  HouseList list;
  House *found;

  runningtest("test_find_exact_house_not_found");

  init_house_list(&list);
  append_house(&list, "Carrer de Roc Boronat", 138, 41.403981, 2.193255);

  found = find_exact_house(&list, "Carrer de Roc Boronat", 999);

  assert(found == NULL);

  free_house_list(&list);
  successtest();
}

void test_collect_valid_numbers_sorted() {
  HouseList list;
  int numbers[10];
  int count;

  runningtest("test_collect_valid_numbers_sorted");

  init_house_list(&list);
  append_house(&list, "Carrer de Roc Boronat", 140, 41.404050, 2.193400);
  append_house(&list, "Carrer de Roc Boronat", 136, 41.403900, 2.193100);
  append_house(&list, "Carrer de Roc Boronat", 138, 41.403981, 2.193255);

  count = collect_valid_numbers(&list, "Carrer de Roc Boronat", numbers, 10);

  assertEqualsInt(count, 3);
  assertEqualsInt(numbers[0], 136);
  assertEqualsInt(numbers[1], 138);
  assertEqualsInt(numbers[2], 140);

  free_house_list(&list);
  successtest();
}

void test_free_house_list_resets_list() {
  HouseList list;

  runningtest("test_free_house_list_resets_list");

  init_house_list(&list);
  append_house(&list, "Carrer de Roc Boronat", 138, 41.403981, 2.193255);

  free_house_list(&list);

  assert(list.head == NULL);
  assertEqualsInt(list.count, 0);

  successtest();
}

void test_init_place_list() {
  PlaceList list;

  runningtest("test_init_place_list");

  init_place_list(&list);

  assert(list.head == NULL);
  assertEqualsInt(list.count, 0);

  successtest();
}

void test_append_place_increases_count() {
  PlaceList list;

  runningtest("test_append_place_increases_count");

  init_place_list(&list);
  append_place(&list, "Area Tallers", 41.403782, 2.193446);

  assertEqualsInt(list.count, 1);

  free_place_list(&list);
  successtest();
}

void test_append_two_places_links_nodes() {
  PlaceList list;

  runningtest("test_append_two_places_links_nodes");

  init_place_list(&list);

  append_place(&list, "Area Tallers", 41.403782, 2.193446);
  append_place(&list, "UPF Campus del Poblenou", 41.404000, 2.194000);

  assertEqualsInt(list.count, 2);
  assertEqualsInt(list.head != NULL, 1);
  assertEqualsInt(list.head->next != NULL, 1);

  free_place_list(&list);
  successtest();
}

void test_find_exact_place_found() {
  PlaceList list;
  Place *found;

  runningtest("test_find_exact_place_found");

  init_place_list(&list);
  append_place(&list, "Area Tallers", 41.403782, 2.193446);

  found = find_exact_place(&list, "Area Tallers");

  assertEqualsInt(found != NULL, 1);

  free_place_list(&list);
  successtest();
}

void test_find_exact_place_case_insensitive() {
  PlaceList list;
  Place *found;

  runningtest("test_find_exact_place_case_insensitive");

  init_place_list(&list);
  append_place(&list, "Area Tallers", 41.403782, 2.193446);

  found = find_exact_place(&list, "area tallers");

  assertEqualsInt(found != NULL, 1);

  free_place_list(&list);
  successtest();
}

void test_find_exact_place_not_found() {
  PlaceList list;
  Place *found;

  runningtest("test_find_exact_place_not_found");

  init_place_list(&list);
  append_place(&list, "Area Tallers", 41.403782, 2.193446);

  found = find_exact_place(&list, "Unknown Place");

  assert(found == NULL);

  free_place_list(&list);
  successtest();
}

void test_free_place_list_resets_list() {
  PlaceList list;

  runningtest("test_free_place_list_resets_list");

  init_place_list(&list);
  append_place(&list, "Area Tallers", 41.403782, 2.193446);
  append_place(&list, "UPF Campus del Poblenou", 41.404000, 2.194000);

  free_place_list(&list);

  assert(list.head == NULL);
  assertEqualsInt(list.count, 0);

  successtest();
}

void test_init_street_list() {
  StreetList list;

  init_street_list(&list);

  assert(list.head == NULL);
}

void test_append_street_segment() {
  StreetList list;

  init_street_list(&list);

  append_street_segment(&list, "Test Street", "1", "2", 10.0, 20.0, 30.0, 40.0,
                        100.0);

  assert(list.head != NULL);
  assert(strcmp(list.head->name, "Test Street") == 0);
  assert(strcmp(list.head->id1, "1") == 0);
  assert(strcmp(list.head->id2, "2") == 0);

  free_street_list(&list);
}

void test_find_closest_street_segment() {
  StreetList list;
  StreetSegment *s;

  init_street_list(&list);

  append_street_segment(&list, "Street A", "1", "2", 10.0, 10.0, 20.0, 20.0,
                        100.0);

  s = find_closest_street_segment(&list, 15.0, 15.0);

  assert(s != NULL);
  assert(strcmp(s->name, "Street A") == 0);

  free_street_list(&list);
}

void test_init_intersection_map() {
  IntersectionMap map;

  runningtest("test_init_intersection_map");

  init_intersection_map(&map);

  assert(map.buckets[0] == NULL);

  successtest();
}

void test_build_intersection_map() {
  StreetList streets;
  IntersectionMap map;
  int result;

  runningtest("test_build_intersection_map");

  init_street_list(&streets);
  init_intersection_map(&map);

  append_street_segment(&streets, "Street A", "1", "2", 10.0, 10.0, 20.0, 20.0,
                        100.0);

  append_street_segment(&streets, "Street B", "2", "3", 20.0, 20.0, 30.0, 30.0,
                        100.0);

  result = build_intersection_map(&map, &streets);

  assertEqualsInt(result, 1);

  free_intersection_map(&map);
  free_street_list(&streets);

  successtest();
}

void test_bfs_route_finds_path() {
  StreetList streets;
  IntersectionMap map;
  Path path;
  StreetSegment *a;
  StreetSegment *c;
  int result;

  runningtest("test_bfs_route_finds_path");

  init_street_list(&streets);
  init_intersection_map(&map);

  append_street_segment(&streets, "Street A", "1", "2", 0.0, 0.0, 0.0, 1.0,
                        100.0);

  append_street_segment(&streets, "Street B", "2", "3", 0.0, 1.0, 0.0, 2.0,
                        100.0);

  append_street_segment(&streets, "Street C", "3", "4", 0.0, 2.0, 0.0, 3.0,
                        100.0);

  build_intersection_map(&map, &streets);

  a = streets.head;
  c = streets.head->next->next;

  result = bfs_route(&map, a, c, &path);

  assertEqualsInt(result, 1);
  assertEqualsInt(path.length, 3);
  assert(strcmp(path.segments[0]->name, "Street A") == 0);
  assert(strcmp(path.segments[1]->name, "Street B") == 0);
  assert(strcmp(path.segments[2]->name, "Street C") == 0);

  free_intersection_map(&map);
  free_street_list(&streets);

  successtest();
}

void sample_lib_test() {
  running("sample_lib_test");

  test_valid_map_name_xl1();
  test_valid_map_name_xs1();
  test_invalid_map_name();

  test_init_house_list();
  test_append_house_increases_count();
  test_append_two_houses_links_nodes();
  test_find_exact_house_found();
  test_find_exact_house_with_abbreviation();
  test_find_exact_house_not_found();
  test_collect_valid_numbers_sorted();
  test_free_house_list_resets_list();

  test_init_place_list();
  test_append_place_increases_count();
  test_append_two_places_links_nodes();
  test_find_exact_place_found();
  test_find_exact_place_case_insensitive();
  test_find_exact_place_not_found();
  test_free_place_list_resets_list();

  test_init_street_list();
  test_append_street_segment();
  test_find_closest_street_segment();
  test_init_intersection_map();
  test_bfs_route_finds_path();
  test_build_intersection_map();

  success();
}

int main() {
  sample_lib_test();
  return 0;
}