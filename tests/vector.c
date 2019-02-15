#include <criterion/criterion.h>

#include "lvector.h"

Test(lvector, create10)
{
  lvector(int) v;

  lvect_create(v, 10, NULL);
  cr_assert(v.arr != NULL);
  cr_assert(v.destr == NULL);
  cr_assert(v.len == 0);
  cr_assert(v.rsize == 10 * sizeof(int));
}

Test(lvector, create0)
{
  lvector(int) v;

  lvect_create(v, 0, NULL);
  //cr_assert(v.arr != NULL); depends on malloc implementation :/
  cr_assert(v.destr == NULL);
  cr_assert(v.len == 0);
  cr_assert(v.rsize == 0);
}

Test(lvector, type_size)
{
  lvector(int) v;
  cr_assert(lvect_type_size(v) == sizeof(int));
  lvector(long) v1;
  cr_assert(lvect_type_size(v1) == sizeof(long));
  lvector(char *) v2;
  cr_assert(lvect_type_size(v2) == sizeof(char *));
  lvector(short) v3;
  cr_assert(lvect_type_size(v3) == sizeof(short));
}

Test(lvector, capacity)
{
  lvector(int) v;

  lvect_create(v, 10, NULL);
  cr_assert(lvect_capacity(v) == 10);
  lvect_destroy(v);
  lvect_create(v, 0, NULL);
  cr_assert(lvect_capacity(v) == 0);
  lvect_destroy(v);
  lvect_create(v, 1000, NULL);
  cr_assert(lvect_capacity(v) == 1000);
  lvect_destroy(v);
}

Test(lvector, empty)
{
  lvector(int) v;

  lvect_create(v, 0, NULL);
  cr_assert(lvect_empty(v) == true);
  lvect_destroy(v);
}

static void free_str(char **pstr)
{
  free(*pstr);
}

Test(lvector, destroy)
{
  lvector(char *) v;

  lvect_create(v, 20, free_str);
  lvect_destroy(v);
  cr_assert(v.destr == NULL);
  cr_assert(v.len == 0);
  cr_assert(v.rsize == 0);
  cr_assert(v.arr == NULL);
}

