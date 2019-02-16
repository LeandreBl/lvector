#include <criterion/criterion.h>

#include "lvector.h"

Test(lvector, create10)
{
  lvector(int) v;

  lvector_create(v, 10, NULL);
  cr_assert(v.arr != NULL);
  cr_assert(v.destr == NULL);
  cr_assert(v.len == 0);
  cr_assert(v.rsize == 10 * sizeof(int));
}

Test(lvector, create0)
{
  lvector(int) v;

  lvector_create(v, 0, NULL);
  //cr_assert(v.arr != NULL); depends on malloc implementation :/
  cr_assert(v.destr == NULL);
  cr_assert(v.len == 0);
  cr_assert(v.rsize == 0);
}

static void free_str(char **pstr)
{
  free(*pstr);
}

Test(lvector, destroy)
{
  lvector(char *) v;

  lvector_create(v, 20, free_str);
  for (size_t i = 0; i < 20; ++i) {
    lvector_push_back(v, strdup("test"));
  }
  lvector_destroy(v);
  cr_assert(v.destr == NULL);
  cr_assert(v.len == 0);
  cr_assert(v.rsize == 0);
  cr_assert(v.arr == NULL);
}

Test(lvector, type_size)
{
  lvector(int) v;
  cr_assert(lvector_type_size(v) == sizeof(int));
  lvector(long) v1;
  cr_assert(lvector_type_size(v1) == sizeof(long));
  lvector(char *) v2;
  cr_assert(lvector_type_size(v2) == sizeof(char *));
  lvector(short) v3;
  cr_assert(lvector_type_size(v3) == sizeof(short));
}

Test(lvector, capacity)
{
  lvector(int) v;

  lvector_create(v, 10, NULL);
  cr_assert(lvector_capacity(v) == 10);
  lvector_destroy(v);
  lvector_create(v, 0, NULL);
  cr_assert(lvector_capacity(v) == 0);
  lvector_destroy(v);
  lvector_create(v, 1000, NULL);
  cr_assert(lvector_capacity(v) == 1000);
  lvector_destroy(v);
}

Test(lvector, empty)
{
  lvector(int) v;

  lvector_create(v, 10, NULL);
  cr_assert(lvector_empty(v) == true);
  for (int i = 0; i < 10; ++i) {
    lvector_push_back(v, i);
  }
  cr_assert(lvector_empty(v) == false);
  for (int i = 0; i < 10; ++i) {
    lvector_pop_back(v);
  }
  cr_assert(lvector_empty(v) == true);
  lvector_destroy(v);
}

Test(lvector, reserve)
{
  lvector(int) v;

  lvector_create(v, 0, NULL);
  lvector_reserve(v, 10);
  cr_assert(v.arr != NULL);
  cr_assert(v.len == 0);
  cr_assert(v.rsize >= sizeof(int) * 10);
  lvector_reserve(v, 100);
  cr_assert(v.arr != NULL);
  cr_assert(v.len == 0);
  cr_assert(v.rsize >= sizeof(int) * 100);
  lvector_destroy(v);
}

Test(lvector, size)
{
  lvector(size_t) v;

  lvector_create(v, 0, NULL);
  cr_assert(lvector_size(v) == 0);
  for (size_t i = 0; i < 100; ++i) {
    cr_assert(lvector_size(v) == i);
    lvector_push_back(v, i);
  }
  cr_assert(lvector_size(v) == 100);
}

Test(lvector, shrink_to_fit)
{
  lvector(int) v;

  lvector_create(v, 100, NULL);
  for (int i = 0; i < 50; ++i) {
    lvector_push_back(v, i);
  }
  lvector_shrink_to_fit(v);
  cr_assert(v.rsize >= v.len);
  for (int i = 0; i < 50; ++i) {
    lvector_pop_back(v);
  }
  lvector_shrink_to_fit(v);
  cr_assert(v.rsize >= v.len);
  lvector_destroy(v);
}

Test(lvector, front)
{
  lvector(int) v;
  int *value;

  lvector_create(v, 10, NULL);
  lvector_push_back(v, 56);
  lvector_push_back(v, 10000);
  value = lvector_front(v);
  *value = 12;
  cr_assert(v.arr[0] == 12);
  *lvector_front(v) = 556;
  cr_assert(v.arr[0] == 556);
  lvector_destroy(v);
}

Test(lvector, back)
{
  lvector(int) v;
  int *value;

  lvector_create(v, 10, NULL);
  lvector_push_back(v, 1);
  lvector_push_back(v, 2);
  lvector_push_back(v, 12);
  value = lvector_back(v);
  *value = 3;
  cr_assert(v.arr[v.len - 1] == 3);
  *lvector_back(v) = 10000;
  cr_assert(v.arr[v.len - 1] == 10000);
  lvector_destroy(v);
}

Test(lvector, data)
{
  lvector(int) v;
  int *arr;

  lvector_create(v, 10, NULL);
  for (int i = 0; i < 10; ++i)
    lvector_push_back(v, i);
  arr = lvector_data(v);
  cr_assert(memcmp(arr, v.arr, sizeof(int) * v.len) == 0);
  lvector_destroy(v);
}

static void string_copier(char **dest, char **src)
{
  *dest = strdup(*src);
}

Test(lvector, assign)
{
  lvector(char *) v;
  lvector(char *) dup = { 0 };

  lvector_create(v, 10, free_str);
  lvector_push_back(v, strdup("Hello"));
  lvector_push_back(v, strdup("Im"));
  lvector_push_back(v, strdup("Leandre"));
  lvector_assign(dup, v, string_copier);
  cr_assert(dup.len == v.len);
  cr_assert(dup.destr == v.destr);
  cr_assert(dup.rsize <= v.rsize);
  for (size_t i = 0; i < lvector_size(v); ++i) {
    cr_assert(&dup.arr[i] != &v.arr[i]);
    cr_assert(strcmp(dup.arr[i], v.arr[i]) == 0);
  }
  lvector_destroy(v);
  lvector_destroy(dup);
}

Test(lvector, push_back)
{
  lvector(int) v;

  lvector_create(v, 0, NULL);
  for (size_t i = 0; i < 100; ++i) {
    cr_assert(v.len == i);
    lvector_push_back(v, i);
    cr_assert(v.rsize >= i);
  }
  cr_assert(v.len == 100);
  for (size_t i = 0; i < lvector_size(v); ++i)
    cr_assert(v.arr[i] == (int)i);
  lvector_destroy(v);
}

Test(lvector, pop_back)
{
  lvector(int) v;

  lvector_create(v, 0, NULL);
  for (size_t i = 0; i < 100; ++i)
    lvector_push_back(v, i);
  for (size_t i = 0; i < 20; ++i) {
    cr_assert(v.len == 100 - i);
    lvector_pop_back(v);
  }
  cr_assert(v.len == 80);
  for (size_t i = 0; i < lvector_size(v); ++i)
    cr_assert(v.arr[i] == (int)i);
  lvector_destroy(v);
}

static void dup_str(char **s, const char *str)
{
  *s = strdup(str);
}

static void empty_str(char **s)
{
  *s = strdup("empty-str");
}

Test(lvector, emplace_back)
{
  lvector(char *) v;

  lvector_create(v, 10, free_str);
  lvector_emplace_back(v, dup_str, "Salut");
  lvector_emplace_back(v, empty_str);
  cr_assert(v.arr != NULL);
  cr_assert(v.len == 2);
  cr_assert(v.rsize >= 2);
  cr_assert(strcmp(v.arr[0], "Salut") == 0);
  cr_assert(strcmp(v.arr[1], "empty-str") == 0);
  lvector_destroy(v);
}

Test(lvector, emplace)
{
  lvector(char *) v;

  lvector_create(v, 10, free_str);
  lvector_emplace(v, 0, dup_str, "Salut");
  lvector_emplace(v, 0, empty_str);
  cr_assert(v.arr != NULL);
  cr_assert(v.len == 2);
  cr_assert(v.rsize >= 2);
  cr_assert(strcmp(v.arr[0], "empty-str") == 0);
  cr_assert(strcmp(v.arr[1], "Salut") == 0);
  lvector_destroy(v);
}
