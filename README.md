# lvector
A fast templated vector C implementation using macros (#define, single header library)

`All those functions "works" as C++ std::vector class (but faster cause we're better) but in a C version`

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

/*
** all emplace and copy functions should return 0 on success
** if any other value, the new element will not be inserted
*/

/* pdest is a pointer onto the new created element (already in the array) */
static int str_allocator_nb(char **pdest, const char *src, size_t nb)
{
  if (asprintf(pdest, "[%zu] %s", nb, src) == -1)
    return -1;
  return 0;
}

/* simply strdup the given string */
static int duplicate_str(char **pdest, const char *str)
{
  *pdest = strdup(str);
  if (*pdest == NULL)
    return -1;
  return 0;
}

/* same as above but from the address of the item */
static int copy_str(char **pdest, char *const *psrc)
{
  *pdest = strdup(*psrc);
  if (*pdest == NULL)
    return -1;
  return 0;
}

int main(void)
{
  lvector(char *) v;
  lvector(char *) copy_v = { 0 }; /* Create an empty vector, without lvector_create call */

  /* create a vector of size 0, with 'free_str' function as destructor */
  lvector_create(v, 0, free_str);

  /*
  ** push the return of 'strdup' at the end of the vector
  ** not safe if strdup returns NULL, but it's an example, forgive me :(
  */
  lvector_push_back(v, strdup("General Kenobi ..."));
  /* call the 'duplicate_str' function on the new vector internal object */
  lvector_emplace_back(v, duplicate_str, "Is that a plane ?");
  /* same as above but with 'str_allocator_nb', but insert the object at index 0 */
  lvector_emplace(v, 0, str_allocator_nb, "Hello there !", 42);

  /*
  ** foreach example, gives the address of the stored item,
  ** avoid copying the content, and so you can modify it's content
  */
  lvector_foreach(str, v) {
    printf("%s\n", *str);
  }
  lvector_assign(copy_v, v, copy_str);
  lvector_destroy(v);

  printf("----------------------\n");
  /* lvector_for example, gives a usable index, named by the user */
  lvector_for(i, copy_v) {
    printf("%s\n", copy_v.arr[i]);
  }

  lvector_destroy(copy_v);
  return (0);
}
```
