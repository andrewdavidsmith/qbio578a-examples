/* Just a wrapper for a dynamic array to push things into
 *
 * Copyright (C) 2024 Andrew D. Smith
 *
 * Authors: Andrew D. Smith
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include "dynamic_array.h"

#include <stdlib.h>  // for calloc and free
#include <limits.h>  // for INT_MAX

#ifdef max
#undef max
#endif
#define max(a,b) ((a) > (b) ? (a) : (b))


struct dynamic_array {
  int size;
  int capacity;
  int *array;
};


int da_size(const dynamic_array *da) {
  return da->size;
}


dynamic_array *da_init(void) {
  dynamic_array *da = (dynamic_array*)calloc(1, sizeof(dynamic_array));
  da->size = 0;
  da->capacity = 0;
  da->array = NULL;
  return da;
}


void da_free(dynamic_array *da) {
  if (da->array != NULL)
    free(da->array);
  free(da);
}


dynamic_array *da_push(dynamic_array *da, const int val) {
  if (da->size == da->capacity) {
    da->capacity = max(1, 2*da->capacity);
    da->array = (int*)realloc(da->array, da->capacity*sizeof(int));
  }
  da->array[da->size] = val;
  da->size++;
  return da;
}


int da_element_at(const dynamic_array *da, const int position) {
  if (da->size <= position) return -INT_MAX;
  return da->array[position];
}
