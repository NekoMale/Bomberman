#pragma once
#include "set.h"

typedef struct ng_s_dictionary_pair
{
	size_t key_length;
	const void* key;
	void* value;
} ng_t_dictionary_pair;

typedef ng_t_dictionary_pair* ng_dictionary_pair;

#define ng_dictionary_new(compare_function) __ng_dictionary_new(compare_function)
#define ng_dictionary_new_hashed(hashmap_size, hash_function, compare_function) __ng_dictionary_new_hashed(hashmap_size, hash_function, compare_function)
#define ng_dictionary_increase_hashmap_size(dictionary, size_increase_amount) __ng_dictionary_increase_hashmap_size((ng_set)dictionary, size_increase_amount)
#define ng_dictionary_add(dictionary, key, key_length, value) __ng_dictionary_add((ng_set)dictionary, key, key_length, value)
#define ng_dictionary_contains_key(dictionary, key, key_length) __ng_dictionary_contains_key((ng_set)dictionary, key, key_length)
#define ng_dictionary_get(dictionary, key, key_length) __ng_dictionary_get((ng_set)dictionary, key, key_length)
#define ng_dictionary_remove(dictionary, key, key_length) __ng_dictionary_remove((ng_set)dictionary, key, key_length)

ng_set __ng_dictionary_new(int(*compare_function)(const void* v1, const void* v2));
ng_set __ng_dictionary_new_hashed(const size_t hashmap_size,
	size_t(*hash_function)(const void* key, const size_t key_length),
	int(*compare_function)(const void* v1, const void* v2));
int __ng_dictionary_increase_hashmap_size(ng_set dictionary, const size_t size_increase_amount);
int __ng_dictionary_add(ng_set dictionary, const void* key, const size_t key_length, void* value);
int __ng_dictionary_contains_key(ng_set dictionary, const void* key, const size_t key_length);
void* __ng_dictionary_get(ng_set dictionary, const void* key, const size_t key_length);
int __ng_dictionary_remove(ng_set dictionary, const void* key, const size_t key_length);