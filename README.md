# lvector
A fast templated vector C implementation using macros (#define)

`All those functions "works" as C++ std::vector class`

```C
/* a bunch of little functions, just to show multiple usages */
static void free_str(char **pstr)
{
  free(*pstr);
}

static void str_allocator_nb(char **pdest, const char *src, size_t nb)
{
  asprintf(pdest, "[%zu] %s", nb, src);
}

static void duplicate_str(char **pdest, const char *str)
{
  *pdest = strdup(str);
}

static void copy_str(char **pdest, char **psrc)
{
  *pdest = strdup(*psrc);
}

int main(void)
{
  lvector(char *) v;
  lvector(char *) duplicate = { 0 }; /* Create an empty vector, without lvector_create call */

  lvector_create(v, 0, free_str);

  lvector_push_back(v, strdup("General Kenobi ..."));
  lvector_emplace_back(v, duplicate_str, "Who the f*** is Thomas Rue");
  lvector_emplace(v, 0, str_allocator_nb, "Hello there !", 42);

  for (size_t i = 0; i < v.len; ++i) {
    printf("%s\n", v.arr[i]);
  }

  lvector_assign(duplicate, v, copy_str);
  lvector_destroy(v);

  printf("----------------------\n");
  for (size_t i = 0; i < duplicate.len; ++i) {
    printf("%s\n", duplicate.arr[i]);
  }

  lvector_destroy(duplicate);
  return (0);
}
```
