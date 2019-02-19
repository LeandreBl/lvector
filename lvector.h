#ifndef _L_VECTOR_H_
# define _L_VECTOR_H_

# include <stdlib.h>

#define max(a, b) ((a >= b) ? a : b)

#define lvector(type)     \
struct                    \
{                         \
  type *arr;              \
  size_t len;             \
  size_t rsize;           \
  void (* destr)(type *); \
}

#define lvector_destroy(vector) do {  \
  lvector_clear(vector);              \
  free((vector).arr);                 \
  memset(&vector, 0, sizeof(vector)); \
} while (0)

#define lvector_type_size(vector) sizeof(*((vector).arr))

#define lvector_create(vector, size, destructor) do {       \
  memset(&vector, 0, sizeof(vector));                       \
  (vector).rsize = size;                                    \
  (vector).arr = malloc(lvector_type_size(vector) * size);  \
  if ((vector).arr == NULL)                                 \
    memset(&(vector), 0, sizeof(vector));                   \
  (vector).len = 0;                                         \
  (vector).destr = destructor;                              \
} while (0)

#define lvector_size(vector) ((vector).len)

#define lvector_resize(vector, newsize) do {                                          \
  if (newsize != (vector).rsize) {                                                    \
    for (size_t i = newsize; i < (vector).len; ++i)                                   \
      if ((vector).destr != NULL)                                                     \
        (vector).destr(&(vector).arr[i]);                                             \
    (vector).rsize = newsize;                                                         \
    if (newsize < (vector).len)                                                       \
      (vector).len = newsize;                                                         \
    (vector).arr = realloc((vector).arr, (vector).rsize * lvector_type_size(vector)); \
    if ((vector).arr == NULL)                                                         \
      memset(&(vector), 0, sizeof(vector));                                           \
  }                                                                                   \
} while (0)

#define lvector_capacity(vector) ((vector).rsize)

#define lvector_empty(vector) ((vector).len == 0)

#define lvector_reserve(vector, reserve_size) do {                    \
  if (lvector_capacity(vector) < reserve_size)                        \
    lvector_resize(vector, reserve_size * lvector_type_size(vector)); \
} while (0)

#define lvector_shrink_to_fit(vector) do {      \
  lvector_resize(vector, lvector_size(vector)); \
} while (0)

#define lvector_front(vector) (&(vector).arr[0])

#define lvector_back(vector) ((vector.len != 0) ? (&(vector).arr[(vector).len - 1]) : (&(vector).arr[0]))

#define lvector_data(vector) ((vector).arr)

#define lvector_assign(vector_dest, vector_src, copier) do {                  \
  lvector_destroy(vector_dest);                                               \
  lvector_create(vector_dest, lvector_size(vector_src), (vector_src).destr);  \
  if (copier != NULL)                                                         \
    for (size_t i = 0; i < lvector_size(vector_src); ++i)                     \
      copier(&(vector_dest).arr[i], &(vector_src).arr[i]);                    \
  else                                                                        \
    for (size_t i = 0; i < lvector_size(vector_src); ++i)                     \
      (vector_dest).arr[i] = (vector_src).arr[i];                             \
  (vector_dest).len = lvector_size(vector_src);                               \
} while (0)

#define lvector_push_back(vector, new_item) do {                    \
  if ((vector).len == (vector).rsize)                               \
    lvector_resize(vector, max(1, (vector).rsize * 2));             \
  (vector).arr[(vector).len] = new_item;                            \
  ++(vector).len;                                                   \
} while (0)

#define lvector_pop_back(vector) do {                   \
  if (lvector_size(vector) != 0)                        \
    lvector_erase(vector, (lvector_size(vector) - 1));  \
} while (0)

#define lvector_insert(vector, position, new_item) do {                                                                           \
  if (position < lvector_size(vector)) {                                                                                          \
    memmove(&(vector).arr[position + 1], &(vector).arr[position], (lvector_size(vector) - position) * lvector_type_size(vector)); \
    (vector).arr[position] = new_item;                                                                                            \
    ++(vector).len;                                                                                                               \
  }                                                                                                                               \
  else if (position == lvector_size(vector))                                                                                      \
    lvector_push_back(vector, new_item);                                                                                          \
} while (0)

#define lvector_erase(vector, position) do {                                                                                          \
  if (position < lvector_size(vector)) {                                                                                              \
    if ((vector).destr != NULL)                                                                                                       \
      (vector).destr(&(vector).arr[position]);                                                                                        \
    memmove(&(vector).arr[position], &(vector).arr[position + 1], (lvector_size(vector) - position - 1) * lvector_type_size(vector)); \
    --(vector).len;                                                                                                                   \
  }                                                                                                                                   \
} while (0)

#define lvector_erase_item(vector, item) do {       \
  for (size_t i = 0; i < lvector_size(vector); ++i) \
    if ((vector).arr[i] == item) {                  \
      lvector_erase(vector, i);                     \
      break;                                        \
    }                                               \
} while (0)

#define lvector_clear(vector) do {                    \
  if ((vector).destr != NULL)                         \
    for (size_t i = 0; i < lvector_size(vector); ++i) \
      ((vector).destr(&(vector).arr[i]));             \
  (vector).len = 0;                                   \
} while (0)

#define lvector_emplace_back(vector, function, ...) do {            \
  if ((vector).len == (vector).rsize)                               \
    lvector_resize(vector, max(1, (vector).rsize * 2));             \
  function(&(vector).arr[lvector_size(vector)], ##__VA_ARGS__);     \
  ++(vector).len;                                                   \
} while (0)

#define lvector_emplace(vector, position, function, ...) do {                                                                     \
  if (position < lvector_size(vector)) {                                                                                          \
    memmove(&(vector).arr[position + 1], &(vector).arr[position], (lvector_size(vector) - position) * lvector_type_size(vector)); \
    function(&(vector).arr[position], ##__VA_ARGS__);                                                                 \
    ++(vector).len;                                                                                                               \
  }                                                                                                                               \
  else if (position == lvector_size(vector))                                                                                      \
    lvector_emplace_back(vector, function, ##__VA_ARGS__);                                                                        \
} while (0)

#endif /* !_L_VECTOR_H_ */
