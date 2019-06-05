# lvector
A fast templated vector C implementation using macros (#define)

`All those functions "works" as C++ std::vector class (but faster cause we're better)`

```C
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lvector.h"

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

static void copy_str(char **pdest, char *const *psrc)
{
  *pdest = strdup(*psrc);
}

int main(void)
{
  lvector(char *) v;
  lvector(char *) duplicate = { 0 }; /* Create an empty vector, without lvector_create call */

  /* create a vector of size 0, with 'free_str' function as destructor */
  lvector_create(v, 0, free_str);

  /* push the return of 'strdup' at the end of the vector */
  lvector_push_back(v, strdup("General Kenobi ..."));
  /* call the 'duplicate_str' function on the new vector internal object */
  lvector_emplace_back(v, duplicate_str, "Who the f*** is Thomas Rue ?!");
  /* same as above but with 'str_allocator_nb', but insert the object at index 0 */
  lvector_emplace(v, 0, str_allocator_nb, "Hello there !", 42);

  /* foreach example, gives the address of the inside item */
  lvector_foreach(str, v) {
    printf("%s\n", *str);
  }
  lvector_assign(duplicate, v, copy_str);
  lvector_destroy(v);

  printf("----------------------\n");
  /* lvector_for example, gives a usable index, named by the user */
  lvector_for(i, duplicate) {
    printf("%s\n", duplicate.arr[i]);
  }

  lvector_destroy(duplicate);
  return (0);
}
```
