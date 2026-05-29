#ifndef NORMALIZE_H
#define NORMALIZE_H

#include "types.h"

void trim_newline(char *s);
void normalize_street(char *dst, const char *src);
void normalize_place(char *dst, const char *src);
int levenshtein(const char *s1, const char *s2);

#endif