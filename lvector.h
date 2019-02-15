#ifndef _L_VECTOR_H_
# define _L_VECTOR_H_

# include <stdlib.h>

#define LVECT_REALLOC_SIZE (5);

#define lvector(type)     \
struct                    \
{                         \
  type *arr;              \
  size_t len;             \
  size_t rsize;           \
  void (* destr)(type *); \
}                         \

#define lvect_destroy(vector) do {            \
  lvect_clear(vector);                        \
  free((vector).arr);                         \
  memset(&vector, 0, sizeof(vector));         \
} while (0)                                   \

#define lvect_type_size(vector) sizeof(*((vector).arr))

#define lvect_create(vector, size, destructor) do { \
  memset(&vector, 0, sizeof(vector));               \
  (vector).rsize = lvect_type_size(vector) * size;  \
  (vector).arr = malloc((vector).rsize);            \
  if ((vector).arr == NULL)                         \
    memset(&(vector), 0, sizeof(vector));           \
  (vector).len = 0;                                 \
  (vector).destr = destructor;                      \
} while (0)                                         \

#define lvect_size(vector) ((vector).len)

#define lvect_resize(vector, newsize) do {                                          \
  if (newsize != (vector).rsize) {                                                  \
    for (size_t i = newsize; i < (vector).len; ++i)                                 \
      if ((vector).destr != NULL)                                                   \
        (vector).destr(&(vector).arr[i]);                                           \
    (vector).rsize = newsize;                                                       \
    if (newsize < (vector).len)                                                     \
      (vector).len = newsize;                                                       \
    (vector).arr = realloc((vector).arr, (vector).rsize * lvect_type_size(vector)); \
    if ((vector).arr == NULL)                                                       \
      memset(&(vector), 0, sizeof(vector));                                         \
  }                                                                                 \
} while (0)                                                                         \

#define lvect_capacity(vector) ((vector).rsize / lvect_type_size(vector))

#define lvect_empty(vector) ((vector).len == 0)

#define lvect_reserve(vector, reserve_size) do {  \
  if (lvect_max_size(vector) < reserve_size)      \
    lvect_resize(vector, reserve_size);           \
} while (0)                                       \

#define lvect_shrink_to_fit(vector) do {    \
  lvect_resize(vector, lvect_size(vector)); \
} while (0)                                 \

#define lvect_front(vector) (&(vector).arr[0])

#define lvect_back(vector) ((vector.len != 0) ? (&(vector).arr[(vector).len - 1]) : (&(vector).arr[0]))

#define lvect_data(vector) ((vector).arr)

#define lvect_assign(vector, other_vector) ((vector) = (other_vector))

#define lvect_push_back(vector, new_item) do {                 \
  if ((vector).len == (vector).rsize)                          \
    lvect_resize(vector, (vector).rsize + LVECT_REALLOC_SIZE); \
  (vector).arr[(vector).len] = new_item;                       \
  ++(vector).len;                                              \
} while (0)                                                    \

#define lvect_pop_back(vector) do {                 \
  if (lvect_size(vector) != 0)                      \
    lvect_erase(vector, (lvect_size(vector) - 1));  \
} while (0)                                         \

#define lvect_insert(vector, position, new_item) do {                                                                         \
  if (position < lvect_size(vector)) {                                                                                        \
    memmove(&(vector).arr[position + 1], &(vector).arr[position], (lvect_size(vector) - position) * lvect_type_size(vector)); \
    (vector).arr[position] = new_item;                                                                                        \
    ++(vector).len;                                                                                                           \
  }                                                                                                                           \
  else if (position == lvect_size(vector))                                                                                    \
    lvect_push_back(vector, new_item);                                                                                        \
} while (0)                                                                                                                   \

#define lvect_erase(vector, position) do {                                                                                        \
  if (position < lvect_size(vector)) {                                                                                            \
    if ((vector).destr != NULL)                                                                                                   \
      (vector).destr(&(vector).arr[position]);                                                                                    \
    memmove(&(vector).arr[position], &(vector).arr[position + 1], (lvect_size(vector) - position - 1) * lvect_type_size(vector)); \
    --(vector).len;                                                                                                               \
  }                                                                                                                               \
} while (0)                                                                                                                       \

#define lvect_erase_item(vector, item) do {       \
  for (size_t i = 0; i < lvect_size(vector); ++i) \
    if ((vector).arr[i] == item) {                \
      lvect_erase(vector, i);                     \
      break;                                      \
    }                                             \
} while (0)                                       \

#define lvect_clear(vector) do {                    \
  if ((vector).destr != NULL)                       \
    for (size_t i = 0; i < lvect_size(vector); ++i) \
      ((vector).destr(&(vector).arr[i]));           \
  (vector).len = 0;                                 \
} while (0)                                         \

#endif /* !_L_VECTOR_H_ */