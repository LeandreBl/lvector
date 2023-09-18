#define _GNU_SOURCE
#include <stdio.h>
#include <criterion/criterion.h>

#include "lvector.h"

Test(lvector, create10)
{
	lvector(int) v;

	cr_assert(lvector_create(v, 10, NULL) == 0);
	cr_assert(v.arr != NULL);
	cr_assert(v.destr == NULL);
	cr_assert(v.len == 0);
	cr_assert(v.rsize == 10);
}

Test(lvector, create0)
{
	lvector(int) v;

	cr_assert(lvector_create(v, 0, NULL) == 0);
	/* cr_assert(v.arr != NULL); depends on malloc implementation :/ */
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

	cr_assert(lvector_create(v, 20, free_str) == 0);
	for (size_t i = 0; i < 20; ++i)
	{
		cr_assert(lvector_push_back(v, strdup("test")) == 0);
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

	cr_assert(lvector_create(v, 10, NULL) == 0);
	cr_assert(lvector_capacity(v) == 10);
	lvector_destroy(v);
	cr_assert(lvector_create(v, 0, NULL) == 0);
	cr_assert(lvector_capacity(v) == 0);
	lvector_destroy(v);
	cr_assert(lvector_create(v, 1000, NULL) == 0);
	cr_assert(lvector_capacity(v) == 1000);
	lvector_destroy(v);
}

Test(lvector, empty)
{
	lvector(int) v;

	cr_assert(lvector_create(v, 10, NULL) == 0);
	cr_assert(lvector_empty(v) == true);
	for (int i = 0; i < 10; ++i)
	{
		cr_assert(lvector_push_back(v, i) == 0);
	}
	cr_assert(lvector_empty(v) == false);
	for (int i = 0; i < 10; ++i)
	{
		lvector_pop_back(v);
	}
	cr_assert(lvector_empty(v) == true);
	lvector_destroy(v);
}

Test(lvector, reserve)
{
	lvector(int) v;

	cr_assert(lvector_create(v, 0, NULL) == 0);
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

	cr_assert(lvector_create(v, 0, NULL) == 0);
	cr_assert(lvector_size(v) == 0);
	for (size_t i = 0; i < 100; ++i)
	{
		cr_assert(lvector_size(v) == i);
		cr_assert(lvector_push_back(v, i) == 0);
	}
	cr_assert(lvector_size(v) == 100);
}

Test(lvector, shrink_to_fit)
{
	lvector(int) v;

	cr_assert(lvector_create(v, 100, NULL) == 0);
	for (int i = 0; i < 50; ++i)
	{
		cr_assert(lvector_push_back(v, i) == 0);
	}
	cr_assert(lvector_shrink_to_fit(v) == 0);
	cr_assert(v.rsize >= v.len);
	for (int i = 0; i < 50; ++i)
	{
		lvector_pop_back(v);
	}
	cr_assert(lvector_shrink_to_fit(v) == 0);
	cr_assert(v.rsize >= v.len);
	lvector_destroy(v);
}

Test(lvector, front)
{
	lvector(int) v;
	int *value;

	cr_assert(lvector_create(v, 10, NULL) == 0);
	cr_assert(lvector_push_back(v, 56) == 0);
	cr_assert(lvector_push_back(v, 10000) == 0);
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

	cr_assert(lvector_create(v, 10, NULL) == 0);
	cr_assert(lvector_push_back(v, 1) == 0);
	cr_assert(lvector_push_back(v, 2) == 0);
	cr_assert(lvector_push_back(v, 12) == 0);
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

	cr_assert(lvector_create(v, 10, NULL) == 0);
	for (int i = 0; i < 10; ++i)
		cr_assert(lvector_push_back(v, i) == 0);
	arr = lvector_data(v);
	cr_assert(memcmp(arr, v.arr, sizeof(int) * v.len) == 0);
	lvector_destroy(v);
}

static int string_copier(char **dest, char *const *src)
{
	*dest = strdup(*src);
	return (!(*dest));
}

Test(lvector, assign)
{
	lvector(char *) v;
	lvector(char *) dup = {0};

	cr_assert(lvector_create(v, 10, free_str) == 0);
	cr_assert(lvector_push_back(v, strdup("Hello")) == 0);
	cr_assert(lvector_push_back(v, strdup("Im")) == 0);
	cr_assert(lvector_push_back(v, strdup("Leandre")) == 0);
	cr_assert(lvector_assign(dup, v, string_copier) == 0);
	cr_assert(dup.len == v.len);
	cr_assert(dup.destr == v.destr);
	cr_assert(dup.rsize <= v.rsize);
	for (size_t i = 0; i < lvector_size(v); ++i)
	{
		cr_assert(&dup.arr[i] != &v.arr[i]);
		cr_assert(strcmp(dup.arr[i], v.arr[i]) == 0);
	}
	lvector_destroy(v);
	lvector_destroy(dup);
}

Test(lvector, push_back)
{
	lvector(int) v;

	cr_assert(lvector_create(v, 0, NULL) == 0);
	for (size_t i = 0; i < 100; ++i)
	{
		cr_assert(v.len == i);
		cr_assert(lvector_push_back(v, i) == 0);
		cr_assert(v.rsize >= i);
	}
	cr_assert(v.len == 100);
	for (size_t i = 0; i < lvector_size(v); ++i)
		cr_assert(v.arr[i] == (int)i);
	lvector_destroy(v);
}

Test(lvector, push_front)
{
	lvector(int) v;

	cr_assert(lvector_create(v, 0, NULL) == 0);
	for (size_t i = 0; i < 100; ++i)
	{
		cr_assert(v.len == i);
		cr_assert(lvector_push_front(v, i) == 0);
		cr_assert(v.rsize >= i);
	}
	cr_assert(v.len == 100);
	for (size_t i = 0; i < lvector_size(v); ++i)
		cr_assert(v.arr[i] == 99 - (int)i);
	lvector_destroy(v);
}

Test(lvector, pop_back)
{
	lvector(int) v;

	cr_assert(lvector_create(v, 0, NULL) == 0);
	for (size_t i = 0; i < 100; ++i)
		cr_assert(lvector_push_back(v, i) == 0);
	for (size_t i = 0; i < 20; ++i)
	{
		cr_assert(v.len == 100 - i);
		lvector_pop_back(v);
	}
	cr_assert(v.len == 80);
	for (size_t i = 0; i < lvector_size(v); ++i)
		cr_assert(v.arr[i] == (int)i);
	lvector_destroy(v);
}

Test(lvector, insert)
{
	lvector(int) v;

	cr_assert(lvector_create(v, 10, NULL) == 0);
	cr_assert(lvector_insert(v, 0, 34) == 0);
	cr_assert(v.arr[0] == 34);
	cr_assert(v.len == 1);
	cr_assert(lvector_insert(v, 1, 73) == 0);
	cr_assert(v.arr[0] == 34);
	cr_assert(v.arr[1] == 73);
	cr_assert(v.len == 2);
	cr_assert(lvector_insert(v, 0, 123) == 0);
	cr_assert(v.arr[0] == 123);
	cr_assert(v.arr[1] == 34);
	cr_assert(v.arr[2] == 73);
	cr_assert(v.len == 3);
	lvector_destroy(v);
}

Test(lvector, erase_item_str)
{
	lvector(char *) v;
	char *s;

	cr_assert(lvector_create(v, 10, free_str) == 0);
	for (int i = 0; i < 10; ++i)
	{
		s = strdup("salut");
		cr_assert(lvector_push_back(v, s) == 0);
	}
	lvector_erase_item(v, s);
	lvector_destroy(v);
}

Test(lvector, erase)
{
	lvector(int) v;

	cr_assert(lvector_create(v, 10, NULL) == 0);
	for (int i = 0; i < 10; ++i)
	{
		cr_assert(lvector_push_back(v, i) == 0);
	}
	/* 0 -> 9 */
	lvector_erase(v, 0);
	/* 1 -> 9 */
	cr_assert(v.len == 9);
	cr_assert(v.arr[0] == 1);
	lvector_erase(v, 8);
	/* 1 -> 8 */
	cr_assert(v.len == 8);
	cr_assert(v.arr[7] == 8);
	lvector_destroy(v);
}

Test(lvector, erase_item)
{
	lvector(int) v;

	cr_assert(lvector_create(v, 10, NULL) == 0);
	for (int i = 0; i < 10; ++i)
	{
		cr_assert(lvector_push_back(v, i) == 0);
	}
	lvector_erase_item(v, 5);
	cr_assert(v.len == 9);
	for (size_t i = 0; i < lvector_size(v); ++i)
		cr_assert(v.arr[i] != 5);
	lvector_erase_item(v, 0);
	cr_assert(v.len == 8);
	for (size_t i = 0; i < lvector_size(v); ++i)
		cr_assert(v.arr[i] != 0);
	lvector_erase_item(v, 9);
	cr_assert(v.len == 7);
	for (size_t i = 0; i < lvector_size(v); ++i)
		cr_assert(v.arr[i] != 9);
	lvector_destroy(v);
}
static size_t s_count = 0;

static void count_destr(int *var)
{
	cr_assert(var != NULL);
	++s_count;
}

Test(lvector, clear)
{
	lvector(int) v;

	cr_assert(lvector_create(v, 1000, count_destr) == 0);
	for (int i = 0; i < 1000; ++i)
		cr_assert(lvector_push_back(v, i) == 0);
	lvector_clear(v);
	cr_assert(v.len == 0);
	cr_assert(s_count == 1000);
	lvector_destroy(v);
}

static int dup_str(char **s, const char *str)
{
	*s = strdup(str);
	return (!(*s));
}

static int empty_str(char **s)
{
	*s = strdup("empty-str");
	return (!(*s));
}

Test(lvector, emplace_back)
{
	lvector(char *) v;

	cr_assert(lvector_create(v, 10, free_str) == 0);
	cr_assert(lvector_emplace_back(v, dup_str, "Salut") == 0);
	cr_assert(lvector_emplace_back(v, empty_str) == 0);
	cr_assert(v.arr != NULL);
	cr_assert(v.len == 2);
	cr_assert(v.rsize >= 2);
	cr_assert_str_eq(v.arr[0], "Salut");
	cr_assert_str_eq(v.arr[1], "empty-str");
	lvector_destroy(v);
}

Test(lvector, emplace)
{
	lvector(char *) v;

	cr_assert(lvector_create(v, 10, free_str) == 0);
	cr_assert(lvector_emplace(v, 0, dup_str, "Salut") == 0);
	cr_assert(lvector_emplace(v, 0, empty_str) == 0);
	cr_assert(v.arr != NULL);
	cr_assert(v.len == 2);
	cr_assert(v.rsize >= 2);
	cr_assert_str_eq(v.arr[0], "empty-str");
	cr_assert_str_eq(v.arr[1], "Salut");
	lvector_destroy(v);
}

Test(lvector, foreach)
{
	lvector(char *) v;
	size_t count = 0;

	cr_assert(lvector_create(v, 10, free_str) == 0);
	cr_assert(lvector_emplace_back(v, dup_str, "toto0") == 0);
	cr_assert(lvector_emplace_back(v, dup_str, "toto1") == 0);
	cr_assert(lvector_emplace_back(v, dup_str, "toto2") == 0);
	cr_assert(lvector_emplace_back(v, dup_str, "toto3") == 0);
	cr_assert(lvector_emplace_back(v, dup_str, "toto4") == 0);

	lvector_foreach(pstr, v)
	{
		cr_assert(strncmp(*pstr, "toto", 4) == 0);
		cr_assert((*pstr)[4] == '0' + (int)count);
		(*pstr)[0] = 'b';
		++count;
	}
	cr_assert(count == v.len);
	count = 0;
	lvector_foreach(pstr, v)
	{
		cr_assert(strncmp(*pstr, "boto", 4) == 0);
		cr_assert((*pstr)[4] == '0' + (int)count);
		++count;
	}
	cr_assert(count == v.len);
	lvector_destroy(v);
}

Test(lvector, lvector_foreach_backward)
{
	lvector(size_t) v = {0};
	size_t count = 0;

	for (size_t i = 0; i < 10; ++i)
	{
		cr_assert(lvector_push_back(v, i) == 0);
	}
	lvector_backward_foreach(val, v)
	{
		cr_assert(*val == 10 - count - 1);
		++count;
	}
	cr_assert(count == 10);
	lvector_destroy(v);
}

Test(lvector, lvector_for)
{
	lvector(size_t) v = {0};

	for (size_t i = 0; i < 10; ++i)
	{
		cr_assert(lvector_push_back(v, i) == 0);
	}
	lvector_for(i, v)
	{
		cr_assert(v.arr[i] == i);
	}
}

Test(lvector, lvector_spush_back)
{
	lvector(size_t) v = {0};

	for (size_t i = 0; i < 50; ++i)
	{
		cr_assert(lvector_spush_back(v, i) == 1);
	}
	for (size_t i = 0; i < 50; ++i)
	{
		cr_assert(lvector_spush_back(v, i) == 0);
	}
	cr_assert(v.len == 50);
	lvector_for(i, v)
	{
		cr_assert(v.arr[i] == i);
	}
	lvector_destroy(v);
}

Test(lvector, lvector_resize)
{
	lvector(size_t) v = {0};

	cr_assert(lvector_resize(v, 13) == 0);
	cr_assert(v.arr != NULL);
	cr_assert(v.len == 0);
	cr_assert(v.rsize >= 13);
	lvector_destroy(v);
}

Test(lvector, lvector_with_erase_if)
{
	lvector(size_t) v = {0};

	for (size_t i = 0; i < 1000; ++i)
	{
		cr_assert(lvector_push_back(v, i) == 0);
	}
	cr_assert(lvector_macro_erase_if(v, item, *item % 2 == 0) == 500);
	lvector_foreach(item, v)
	{
		cr_assert(*item % 2 != 0);
	}
	lvector_destroy(v);
}

static int is_odd(size_t *item, int base)
{
	return *item % base;
}

Test(lvector, lvector_erase_if)
{
	lvector(size_t) v = {0};

	for (size_t i = 0; i < 1000; ++i)
	{
		cr_assert(lvector_push_back(v, i) == 0);
	}
	cr_assert(lvector_erase_if(v, is_odd, 2) == 500);
	lvector_foreach(item, v)
	{
		cr_assert(*item % 2 != 0);
	}
	lvector_destroy(v);
}

static int is_the_right_addr(char **s, const char *p)
{
	return *s - p;
}

Test(lvector, lvector_find_if)
{
	lvector(char *) v = {0};
	char *to_find = NULL;
	size_t position = 0;

	cr_assert(lvector_create(v, 0, free_str) == 0);
	for (size_t i = 0; i < 1000; ++i) {
		cr_assert(lvector_emplace_back(v, asprintf, "test%zu", i) == 0);
	}
	cr_assert(asprintf(&to_find, "zbozbz") != -1);
	srand((unsigned int)(long)&v);
	position = rand() % lvector_size(v);
	cr_assert(lvector_insert(v, position, to_find) == 0);
	char **pfind = lvector_find_if(v, is_the_right_addr, to_find);
	cr_assert(*pfind == to_find);
	lvector_destroy(v);
}

Test(lvector, lvector_macro_find_if)
{
	lvector(char *) v = {0};
	char *to_find = NULL;
	size_t position = 0;

	cr_assert(lvector_create(v, 0, free_str) == 0);
	for (size_t i = 0; i < 1000; ++i) {
		cr_assert(lvector_emplace_back(v, asprintf, "test%zu", i) == 0);
	}
	cr_assert(asprintf(&to_find, "zbozbz") != -1);
	srand((unsigned int)(long)&v);
	position = rand() % lvector_size(v);
	cr_assert(lvector_insert(v, position, to_find) == 0);
	char **pfind = lvector_macro_find_if(v, item, *item == to_find);
	cr_assert(*pfind == to_find);
	lvector_destroy(v);
}
