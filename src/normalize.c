#include "normalize.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

void trim_newline(char *s) {
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
    if (s[i] == '-' || s[i] == '_' || s[i] == ',' || s[i] == '.') {
      s[i] = ' ';
    }

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

void normalize_street(char *dst, const char *src) {
  char temp[MAX_INPUT];
  to_lowercase(temp, src);
  expand_abbreviations(temp);
  normalize_spaces(temp);
  strcpy(dst, temp);
}

void normalize_place(char *dst, const char *src) {
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

int levenshtein(const char *s1, const char *s2) {
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
