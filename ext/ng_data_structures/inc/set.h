#pragma once
#include "linked_list.h"

typedef struct ng_s_key_set
{
	size_t key_length;
	const void* key;
} ng_t_key_set;

typedef ng_t_key_set* ng_key_set;

typedef struct ng_s_set
{
	ng_list* nodes;
	size_t hashmap_size;
	size_t(*hash_function)(const void*, size_t);
} ng_t_set;

typedef ng_t_set* ng_set;

#define ng_set_new(compare_function) __ng_set_new(compare_function)
#define ng_set_new_hashed(hashmap_size, custom_hash_function, compare_function) __ng_set_new_hashed(hashmap_size, custom_hash_function, compare_function)
#define ng_set_increase_hashmap_size(set, size_increase_amount) __ng_set_increase_hashmap_size((ng_set) set, size_increase_amount)
#define ng_set_add(set, value, key_length) __ng_set_add((ng_set) set, value, key_length)
#define ng_set_exists(set, value, key_length) __ng_set_exists((ng_set) set, value, key_length)
#define ng_set_remove(set, value, key_length) __ng_set_remove((ng_set) set, value, key_length)

size_t djb33x_hash(const void* key, const size_t key_length);
ng_set __ng_set_new(int(*compare_function)(const void* v1, const void* v2));
ng_set __ng_set_new_hashed(const size_t hashmap_size, size_t(*hash_function)(const void* key, size_t key_length), int(*compare_function)(const void* v1, const void* v2));
int __ng_set_increase_hashmap_size(ng_set set, const size_t size_increase_amount);
int __ng_set_add(ng_set set, const void* value, const size_t key_length);
int __ng_set_exists(ng_set set, void* value, const size_t key_length);
int __ng_set_remove(ng_set set, void* value, const size_t key_length);
