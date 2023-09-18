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
	{                                       \
		lvector_clear(vector);              \
		free((vector).arr);                 \
		memset(&vector, 0, sizeof(vector)); \
	}

#define lvector_type_size(vector) sizeof(*((vector).arr))

#define lvector_create(vector, reserved_size, destructor)                 \
	({                                                                    \
		int __lvector_create_ret = 0;                                     \
                                                                          \
		memset(&vector, 0, sizeof(vector));                               \
		(vector).rsize = reserved_size;                                   \
		(vector).arr = malloc(lvector_type_size(vector) * reserved_size); \
		if ((vector).arr == NULL)                                         \
		{                                                                 \
			memset(&(vector), 0, sizeof(vector));                         \
			__lvector_create_ret = -1;                                    \
		}                                                                 \
		else                                                              \
		{                                                                 \
			(vector).len = 0;                                             \
			(vector).destr = destructor;                                  \
		}                                                                 \
		__lvector_create_ret;                                             \
	})

#define lvector_size(vector) ((vector).len)

#define lvector_resize(vector, newsize)                                                                                             \
	({                                                                                                                              \
		int __lvector_resize_ret = 0;                                                                                               \
                                                                                                                                    \
		if (newsize == 0)                                                                                                           \
		{                                                                                                                           \
			lvector_clear(vector);                                                                                                  \
		}                                                                                                                           \
		else if (newsize != (vector).rsize)                                                                                         \
		{                                                                                                                           \
			for (size_t __lvector_resize_iterator = newsize; __lvector_resize_iterator < (vector).len; ++__lvector_resize_iterator) \
			{                                                                                                                       \
				if ((vector).destr != NULL)                                                                                         \
				{                                                                                                                   \
					(vector).destr(&(vector).arr[__lvector_resize_iterator]);                                                       \
				}                                                                                                                   \
			}                                                                                                                       \
			void *p =                                                                                                               \
				realloc((vector).arr,                                                                                               \
						newsize * lvector_type_size(vector));                                                                       \
			if (p != NULL)                                                                                                          \
			{                                                                                                                       \
				(vector).rsize = newsize;                                                                                           \
				(vector).arr = p;                                                                                                   \
				if (newsize < (vector).len)                                                                                         \
				{                                                                                                                   \
					(vector).len = newsize;                                                                                         \
				}                                                                                                                   \
			}                                                                                                                       \
			else                                                                                                                    \
			{                                                                                                                       \
				__lvector_resize_ret = -1;                                                                                          \
			}                                                                                                                       \
		}                                                                                                                           \
		__lvector_resize_ret;                                                                                                       \
	})

#define lvector_capacity(vector) ((vector).rsize)

#define lvector_empty(vector) ((vector).len == 0)

#define lvector_reserve(vector, reserve_size)              \
	({                                                     \
		int __lvector_reserve_ret = 0;                     \
                                                           \
		if (lvector_capacity(vector) < reserve_size)       \
		{                                                  \
			__lvector_reserve_ret = lvector_resize(        \
				vector,                                    \
				reserve_size * lvector_type_size(vector)); \
		}                                                  \
		__lvector_reserve_ret;                             \
	})

#define lvector_shrink_to_fit(vector) lvector_resize(vector, lvector_size(vector))

#define lvector_front(vector) (&(vector).arr[0])

#define lvector_back(vector) (&(vector).arr[(vector).len - 1])

#define lvector_data(vector) ((vector).arr)

#define lvector_assign(vector_dest, vector_src, copier)                                                          \
	({                                                                                                           \
		int __lvector_assign_ret = 0;                                                                            \
                                                                                                                 \
		__lvector_assign_ret = lvector_create(vector_dest, lvector_size(vector_src),                             \
											  (vector_src).destr);                                               \
		if (__lvector_assign_ret == 0)                                                                           \
		{                                                                                                        \
			if (copier != NULL)                                                                                  \
			{                                                                                                    \
				size_t __lvector_assign_iterator = 0;                                                            \
				lvector_foreach(obj, vector_src)                                                                 \
				{                                                                                                \
					__lvector_assign_ret = copier(&(vector_dest).arr[__lvector_assign_iterator], obj);           \
					if (__lvector_assign_ret != 0)                                                               \
					{                                                                                            \
						__lvector_assign_ret = -1;                                                               \
						break;                                                                                   \
					}                                                                                            \
					++__lvector_assign_iterator;                                                                 \
				}                                                                                                \
			}                                                                                                    \
			else                                                                                                 \
			{                                                                                                    \
				for (size_t __lvector_assign_iterator = 0; __lvector_assign_iterator < lvector_size(vector_src); \
					 ++__lvector_assign_iterator)                                                                \
				{                                                                                                \
					(vector_dest).arr[__lvector_assign_iterator] =                                               \
						(vector_src).arr[__lvector_assign_iterator];                                             \
				}                                                                                                \
			}                                                                                                    \
			if (__lvector_assign_ret == 0)                                                                       \
			{                                                                                                    \
				(vector_dest).len = lvector_size(vector_src);                                                    \
			}                                                                                                    \
		}                                                                                                        \
		__lvector_assign_ret;                                                                                    \
	})

#define lvector_copy(vector_dest, vector_src, copier) \
	lvector_assign(vector_dest, vector_src, copier)

#define lvector_push_back(vector, new_item)                                               \
	({                                                                                    \
		int __lvector_push_back_ret = 0;                                                  \
                                                                                          \
		if ((vector).len == (vector).rsize)                                               \
		{                                                                                 \
			__lvector_push_back_ret = lvector_resize(vector, max(1, (vector).rsize * 2)); \
		}                                                                                 \
		if (__lvector_push_back_ret == 0 && (vector).len < (vector).rsize)                \
		{                                                                                 \
			(vector).arr[(vector).len] = new_item;                                        \
			++(vector).len;                                                               \
		}                                                                                 \
		__lvector_push_back_ret;                                                          \
	})

#define lvector_push_front(vector, new_item) lvector_insert(vector, 0, new_item)

#define lvector_pop_back(vector)                               \
	{                                                          \
		if (lvector_size(vector) != 0)                         \
			lvector_erase(vector, (lvector_size(vector) - 1)); \
	}

#define lvector_insert(vector, position, new_item)                                      \
	({                                                                                  \
		int __lvector_insert_ret = 0;                                                   \
                                                                                        \
		if (position < lvector_size(vector))                                            \
		{                                                                               \
			if ((vector).len == (vector).rsize)                                         \
			{                                                                           \
				__lvector_insert_ret = lvector_resize(vector,                           \
													  max(1, (vector).rsize * 2));      \
			}                                                                           \
			if (__lvector_insert_ret == 0 && (vector).len < (vector).rsize)             \
			{                                                                           \
				memmove(&(vector).arr[position + 1],                                    \
						&(vector).arr[position],                                        \
						(lvector_size(vector) - position) * lvector_type_size(vector)); \
				(vector).arr[position] = new_item;                                      \
				++(vector).len;                                                         \
			}                                                                           \
		}                                                                               \
		else if (position == lvector_size(vector))                                      \
		{                                                                               \
			__lvector_insert_ret = lvector_push_back(vector, new_item);                 \
		}                                                                               \
		__lvector_insert_ret;                                                           \
	})

#define lvector_erase(vector, position)                                                 \
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
	}

#define lvector_erase_item(vector, item)                                                                                                      \
	{                                                                                                                                         \
		for (size_t __lvector_erase_item_iterator = 0; __lvector_erase_item_iterator < lvector_size(vector); ++__lvector_erase_item_iterator) \
			if ((vector).arr[__lvector_erase_item_iterator] == item)                                                                          \
			{                                                                                                                                 \
				lvector_erase(vector, __lvector_erase_item_iterator);                                                                         \
				break;                                                                                                                        \
			}                                                                                                                                 \
	}

#define lvector_erase_from_ptr(vector, ptr)                                  \
	{                                                                        \
		if (ptr >= lvector_front(vector) && ptr <= lvector_back(vector))     \
		{                                                                    \
			if ((vector).destr != NULL)                                      \
				(vector).destr(ptr);                                         \
			memmove(ptr, ptr + 1,                                            \
					lvector_back(vector) - ptr + lvector_type_size(vector)); \
			--(vector).len;                                                  \
		}                                                                    \
	}

#define lvector_erase_if(vector, condition_function, ...)                                                 \
	({                                                                                                    \
		size_t __lvector_erase_if_original_count = lvector_size(vector);                                  \
                                                                                                          \
		for (size_t __lvector_erase_if_iterator = 0; __lvector_erase_if_iterator < lvector_size(vector);) \
		{                                                                                                 \
			if (condition_function(&(vector).arr[__lvector_erase_if_iterator], ##__VA_ARGS__) == 0)       \
			{                                                                                             \
				lvector_erase(vector, __lvector_erase_if_iterator);                                       \
			}                                                                                             \
			else                                                                                          \
			{                                                                                             \
				++__lvector_erase_if_iterator;                                                            \
			}                                                                                             \
		}                                                                                                 \
		(__lvector_erase_if_original_count - lvector_size(vector));                                       \
	})

#define lvector_macro_erase_if(vector, item_name, condition)                                                          \
	({                                                                                                                \
		size_t __lvector_macro_erase_if_original_count = lvector_size(vector);                                        \
                                                                                                                      \
		for (size_t __lvector_macro_erase_if_iterator = 0; __lvector_macro_erase_if_iterator < lvector_size(vector);) \
		{                                                                                                             \
			const __typeof__((*vector.arr)) *item_name = &v.arr[__lvector_macro_erase_if_iterator];                   \
			if (condition)                                                                                            \
			{                                                                                                         \
				lvector_erase(vector, __lvector_macro_erase_if_iterator);                                             \
			}                                                                                                         \
			else                                                                                                      \
			{                                                                                                         \
				++__lvector_macro_erase_if_iterator;                                                                  \
			}                                                                                                         \
		}                                                                                                             \
		(__lvector_macro_erase_if_original_count - lvector_size(vector));                                             \
	})

#define lvector_find_if(vector, condition_function, ...)                                                                             \
	({                                                                                                                               \
		__typeof__(*(vector.arr)) *__lvector_find_if_item_found = NULL;                                                              \
                                                                                                                                     \
		for (size_t __lvector_find_if_iterator = 0; __lvector_find_if_iterator < lvector_size(vector); ++__lvector_find_if_iterator) \
		{                                                                                                                            \
			if (condition_function(&(vector).arr[__lvector_find_if_iterator], ##__VA_ARGS__) == 0)                                   \
			{                                                                                                                        \
				__lvector_find_if_item_found = &(vector).arr[__lvector_find_if_iterator];                                            \
				break;                                                                                                               \
			}                                                                                                                        \
		}                                                                                                                            \
		__lvector_find_if_item_found;                                                                                                \
	})

#define lvector_macro_find_if(vector, item_name, condition)                                                                                            \
	({                                                                                                                                                 \
		__typeof__(*(vector.arr)) *__lvector_macro_find_if_item_found = NULL;                                                                          \
                                                                                                                                                       \
		for (size_t __lvector_macro_find_if_iterator = 0; __lvector_macro_find_if_iterator < lvector_size(vector); ++__lvector_macro_find_if_iterator) \
		{                                                                                                                                              \
			__typeof__(*(vector.arr)) *item_name = &(vector).arr[__lvector_macro_find_if_iterator];                                                    \
			;                                                                                                                                          \
			if (condition)                                                                                                                             \
			{                                                                                                                                          \
				__lvector_macro_find_if_item_found = item_name;                                                                                        \
				break;                                                                                                                                 \
			}                                                                                                                                          \
		}                                                                                                                                              \
		__lvector_macro_find_if_item_found;                                                                                                            \
	})

#define lvector_clear(vector)             \
	{                                     \
		if ((vector).destr != NULL)       \
		{                                 \
			lvector_foreach(item, vector) \
			{                             \
				(vector).destr(item);     \
			}                             \
		}                                 \
		(vector).len = 0;                 \
	}

#define lvector_emplace_back(vector, function, ...)                                          \
	({                                                                                       \
		int __lvector_emplace_back_ret = 0;                                                  \
                                                                                             \
		if ((vector).len == (vector).rsize)                                                  \
		{                                                                                    \
			__lvector_emplace_back_ret = lvector_resize(vector, max(1, (vector).rsize * 2)); \
		}                                                                                    \
		if (__lvector_emplace_back_ret == 0 && (vector).len < (vector).rsize)                \
		{                                                                                    \
			memset(&(vector).arr[lvector_size(vector)], 0,                                   \
				   sizeof(*(vector).arr));                                                   \
			__lvector_emplace_back_ret = function(&(vector).arr[lvector_size(vector)],       \
												  ##__VA_ARGS__);                            \
			if (__lvector_emplace_back_ret >= 0)                                             \
			{                                                                                \
				++(vector).len;                                                              \
			}                                                                                \
		}                                                                                    \
		(__lvector_emplace_back_ret < 0 ? -1 : 0);                                           \
	})

#define lvector_emplace(vector, position, function, ...)                                   \
	({                                                                                     \
		int __lvector_emplace_ret = 0;                                                     \
                                                                                           \
		if (position < lvector_size(vector))                                               \
		{                                                                                  \
			if ((vector).len == (vector).rsize)                                            \
			{                                                                              \
				__lvector_emplace_ret = lvector_resize(vector,                             \
													   max(1, (vector).rsize * 2));        \
			}                                                                              \
			if (__lvector_emplace_ret == 0 && (vector).len < (vector).rsize)               \
			{                                                                              \
				memmove(&(vector).arr[position + 1],                                       \
						&(vector).arr[position],                                           \
						(lvector_size(vector) - position) * lvector_type_size(vector));    \
				memset(&(vector).arr[position], 0,                                         \
					   sizeof(*(vector).arr));                                             \
				__lvector_emplace_ret = function(&(vector).arr[position],                  \
												 ##__VA_ARGS__);                           \
				if (__lvector_emplace_ret >= 0)                                            \
				{                                                                          \
					++(vector).len;                                                        \
				}                                                                          \
			}                                                                              \
		}                                                                                  \
		else if (position == lvector_size(vector))                                         \
		{                                                                                  \
			__lvector_emplace_ret = lvector_emplace_back(vector, function, ##__VA_ARGS__); \
		}                                                                                  \
		(__lvector_emplace_ret < 0 ? -1 : 0);                                              \
	})

#define lvector_spush_back(vector, new_item)                                                                                                      \
	({                                                                                                                                            \
		int __lvector_spush_back_ret = 0;                                                                                                         \
                                                                                                                                                  \
		if (lvector_size(vector) == 0)                                                                                                            \
		{                                                                                                                                         \
			__lvector_spush_back_ret = lvector_push_back(vector, new_item);                                                                       \
			if (__lvector_spush_back_ret == 0)                                                                                                    \
			{                                                                                                                                     \
				__lvector_spush_back_ret = 1;                                                                                                     \
			}                                                                                                                                     \
		}                                                                                                                                         \
		else                                                                                                                                      \
		{                                                                                                                                         \
			for (size_t __lvector_spush_back_iterator = 0; __lvector_spush_back_iterator < lvector_size(vector); ++__lvector_spush_back_iterator) \
			{                                                                                                                                     \
				if ((vector).arr[__lvector_spush_back_iterator] == new_item)                                                                      \
				{                                                                                                                                 \
					__lvector_spush_back_ret = 0;                                                                                                 \
					break;                                                                                                                        \
				}                                                                                                                                 \
				else if (__lvector_spush_back_iterator == lvector_size(vector) - 1 && (vector).arr[__lvector_spush_back_iterator] != new_item)    \
				{                                                                                                                                 \
					__lvector_spush_back_ret = lvector_push_back(vector, new_item);                                                               \
					if (__lvector_spush_back_ret == -1)                                                                                           \
					{                                                                                                                             \
						break;                                                                                                                    \
					}                                                                                                                             \
					__lvector_spush_back_ret = 1;                                                                                                 \
					break;                                                                                                                        \
				}                                                                                                                                 \
			}                                                                                                                                     \
		}                                                                                                                                         \
		__lvector_spush_back_ret;                                                                                                                 \
	})

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
