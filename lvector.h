#ifndef L_VECTOR_H_
#define L_VECTOR_H_

#include <stdlib.h>
#include <string.h>

#define max(a, b) ((a >= b) ? a : b)

#define lvector(type)          \
	struct                     \
	{                          \
		type *arr;             \
		union                  \
		{                      \
			size_t len;        \
			size_t size;       \
		};                     \
		size_t rsize;          \
		void (*destr)(type *); \
	}

#define lvector_destroy(vector)             \
	do                                      \
	{                                       \
		lvector_clear(vector);              \
		free((vector).arr);                 \
		memset(&vector, 0, sizeof(vector)); \
	} while (0)

#define lvector_type_size(vector) sizeof(*((vector).arr))

#define lvector_create(vector, size, destructor)                 \
	do                                                           \
	{                                                            \
		memset(&vector, 0, sizeof(vector));                      \
		(vector).rsize = size;                                   \
		(vector).arr = malloc(lvector_type_size(vector) * size); \
		if ((vector).arr == NULL)                                \
			memset(&(vector), 0, sizeof(vector));                \
		(vector).len = 0;                                        \
		(vector).destr = destructor;                             \
	} while (0)

#define lvector_size(vector) ((vector).len)

#define lvector_resize(vector, newsize)                           \
	do                                                            \
	{                                                             \
		if (newsize == 0)                                         \
			lvector_clear(vector);                                \
		else if (newsize != (vector).rsize)                       \
		{                                                         \
			for (size_t i__ = newsize; i__ < (vector).len; ++i__) \
				if ((vector).destr != NULL)                       \
					(vector).destr(&(vector).arr[i__]);           \
			void *p =                                             \
				realloc((vector).arr,                             \
						newsize * lvector_type_size(vector));     \
			if (p != NULL)                                        \
			{                                                     \
				(vector).rsize = newsize;                         \
				(vector).arr = p;                                 \
				if (newsize < (vector).len)                       \
					(vector).len = newsize;                       \
			}                                                     \
		}                                                         \
	} while (0)

#define lvector_capacity(vector) ((vector).rsize)

#define lvector_empty(vector) ((vector).len == 0)

#define lvector_reserve(vector, reserve_size)             \
	do                                                    \
	{                                                     \
		if (lvector_capacity(vector) < reserve_size)      \
			lvector_resize(                               \
				vector,                                   \
				reserve_size *lvector_type_size(vector)); \
	} while (0)

#define lvector_shrink_to_fit(vector)                 \
	do                                                \
	{                                                 \
		lvector_resize(vector, lvector_size(vector)); \
	} while (0)

#define lvector_front(vector) (&(vector).arr[0])

#define lvector_back(vector) (&(vector).arr[(vector).len - 1])

#define lvector_data(vector) ((vector).arr)

#define lvector_assign(vector_dest, vector_src, copier)          \
	do                                                           \
	{                                                            \
		lvector_create(vector_dest, lvector_size(vector_src),    \
					   (vector_src).destr);                      \
		if (copier != NULL)                                      \
		{                                                        \
			size_t i__ = 0;                                      \
			lvector_foreach(obj, vector_src)                     \
			{                                                    \
				if (!copier(&(vector_dest).arr[i__], obj))       \
					++i__;                                       \
			}                                                    \
		}                                                        \
		else                                                     \
		{                                                        \
			for (size_t i__ = 0; i__ < lvector_size(vector_src); \
				 ++i__)                                          \
				(vector_dest).arr[i__] =                         \
					(vector_src).arr[i__];                       \
		}                                                        \
		(vector_dest).len = lvector_size(vector_src);            \
	} while (0)

#define lvector_copy(vector_dest, vector_src, copier) \
	lvector_assign(vector_dest, vector_src, copier)

#define lvector_push_back(vector, new_item)                     \
	do                                                          \
	{                                                           \
		if ((vector).len == (vector).rsize)                     \
			lvector_resize(vector, max(1, (vector).rsize * 2)); \
		if ((vector).len < (vector).rsize)                      \
		{                                                       \
			(vector).arr[(vector).len] = new_item;              \
			++(vector).len;                                     \
		}                                                       \
	} while (0)

#define lvector_push_front(vector, new_item) \
	do                                       \
	{                                        \
		lvector_insert(vector, 0, new_item); \
	} while (0)

#define lvector_pop_back(vector)                               \
	do                                                         \
	{                                                          \
		if (lvector_size(vector) != 0)                         \
			lvector_erase(vector, (lvector_size(vector) - 1)); \
	} while (0)

#define lvector_insert(vector, position, new_item)                                      \
	do                                                                                  \
	{                                                                                   \
		if (position < lvector_size(vector))                                            \
		{                                                                               \
			if ((vector).len == (vector).rsize)                                         \
				lvector_resize(vector,                                                  \
							   max(1, (vector).rsize * 2));                             \
			if ((vector).len < (vector).rsize)                                          \
			{                                                                           \
				memmove(&(vector).arr[position + 1],                                    \
						&(vector).arr[position],                                        \
						(lvector_size(vector) - position) * lvector_type_size(vector)); \
				(vector).arr[position] = new_item;                                      \
				++(vector).len;                                                         \
			}                                                                           \
		}                                                                               \
		else if (position == lvector_size(vector))                                      \
			lvector_push_back(vector, new_item);                                        \
	} while (0)

#define lvector_erase(vector, position)                                                 \
	do                                                                                  \
	{                                                                                   \
		if (position < lvector_size(vector))                                            \
		{                                                                               \
			if ((vector).destr != NULL)                                                 \
				(vector).destr(&(vector).arr[position]);                                \
			memmove(&(vector).arr[position],                                            \
					&(vector).arr[position + 1],                                        \
					(lvector_size(vector) - position - 1) * lvector_type_size(vector)); \
			--(vector).len;                                                             \
		}                                                                               \
	} while (0)

#define lvector_erase_item(vector, item)                        \
	do                                                          \
	{                                                           \
		for (size_t i__ = 0; i__ < lvector_size(vector); ++i__) \
			if ((vector).arr[i__] == item)                      \
			{                                                   \
				lvector_erase(vector, i__);                     \
				break;                                          \
			}                                                   \
	} while (0)

#define lvector_erase_from_ptr(vector, ptr)                                  \
	do                                                                       \
	{                                                                        \
		if (ptr >= lvector_front(vector) && ptr <= lvector_back(vector))     \
		{                                                                    \
			if ((vector).destr != NULL)                                      \
				(vector).destr(ptr);                                         \
			memmove(ptr, ptr + 1,                                            \
					lvector_back(vector) - ptr + lvector_type_size(vector)); \
			--(vector).len;                                                  \
		}                                                                    \
	} while (0)

#define lvector_clear(vector)                                \
	do                                                       \
	{                                                        \
		if ((vector).destr != NULL)                          \
			for (size_t i__ = 0; i__ < lvector_size(vector); \
				 ++i__)                                      \
				((vector).destr(&(vector).arr[i__]));        \
		(vector).len = 0;                                    \
	} while (0)

#define lvector_emplace_back(vector, function, ...)             \
	do                                                          \
	{                                                           \
		if ((vector).len == (vector).rsize)                     \
			lvector_resize(vector, max(1, (vector).rsize * 2)); \
		if ((vector).len < (vector).rsize)                      \
		{                                                       \
			memset(&(vector).arr[lvector_size(vector)], 0,      \
				   sizeof(*(vector).arr));                      \
			if (!function(&(vector).arr[lvector_size(vector)],  \
						  ##__VA_ARGS__))                       \
				++(vector).len;                                 \
		}                                                       \
	} while (0)

#define lvector_emplace(vector, position, function, ...)                                \
	do                                                                                  \
	{                                                                                   \
		if (position < lvector_size(vector))                                            \
		{                                                                               \
			if ((vector).len == (vector).rsize)                                         \
				lvector_resize(vector,                                                  \
							   max(1, (vector).rsize * 2));                             \
			if ((vector).len < (vector).rsize)                                          \
			{                                                                           \
				memmove(&(vector).arr[position + 1],                                    \
						&(vector).arr[position],                                        \
						(lvector_size(vector) - position) * lvector_type_size(vector)); \
				memset(&(vector).arr[position], 0,                                      \
					   sizeof(*(vector).arr));                                          \
				if (!function(&(vector).arr[position],                                  \
							  ##__VA_ARGS__))                                           \
					++(vector).len;                                                     \
			}                                                                           \
		}                                                                               \
		else if (position == lvector_size(vector))                                      \
			lvector_emplace_back(vector, function, ##__VA_ARGS__);                      \
	} while (0)

#define lvector_spush_back(vector, new_item)                                           \
	do                                                                                 \
	{                                                                                  \
		if (lvector_size(vector) == 0)                                                 \
		{                                                                              \
			lvector_push_back(vector, new_item);                                       \
			break;                                                                     \
		}                                                                              \
		for (size_t i__ = 0; i__ < lvector_size(vector); ++i__)                        \
		{                                                                              \
			if ((vector).arr[i__] == new_item)                                         \
				break;                                                                 \
			else if (i__ == lvector_size(vector) - 1 && (vector).arr[i__] != new_item) \
			{                                                                          \
				lvector_push_back(vector, new_item);                                   \
				break;                                                                 \
			}                                                                          \
		}                                                                              \
	} while (0)

#define lvector_foreach(item_name, vector)                           \
	for (__typeof__((vector).arr) item_name = lvector_front(vector); \
		 item_name <= lvector_back(vector); ++item_name)

#define lvector_backward_foreach(item_name, vector)                 \
	for (__typeof__((vector).arr) item_name = lvector_back(vector); \
		 item_name >= lvector_front(vector); --item_name)

#define lvector_for(index_name, vector)                            \
	for (size_t index_name = 0; index_name < lvector_size(vector); \
		 ++index_name)

#define lvector_backward_for(index_name, vector)                         \
	for (ssize_t index_name = lvector_size(vector) - 1; index_name >= 0; \
		 --index_name)

#endif /* !L_VECTOR_H_ */
