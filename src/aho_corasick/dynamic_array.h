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

#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

typedef struct dynamic_array dynamic_array;

dynamic_array *da_init(void);
void da_free(dynamic_array *);
int da_size(const dynamic_array *);

// this dynamic array can only grow
dynamic_array *da_push(dynamic_array *, const int val);

// if the position is invalid, -MAX_INT will be returned
int da_element_at(const dynamic_array *, const int position);

#endif
