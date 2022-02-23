#include <stdlib.h>
#include "..\inc\dictionary.h"

ng_set __ng_dictionary_new(int(*compare_function)(const void* v1, const void* v2))
{
	return __ng_dictionary_new_hashed(10, djb33x_hash, compare_function);
}

ng_set __ng_dictionary_new_hashed(
	const size_t hashmap_size, 
	size_t(*hash_function)(const void* key, const size_t key_length), 
	int(*compare_function)(const void* v1, const void* v2))
{
	ng_set dictionary = (ng_set)malloc(sizeof(ng_t_set));
	if (!dictionary)
	{
		return NULL;
	}
	dictionary->hashmap_size = hashmap_size;
	dictionary->hash_function = hash_function;
	dictionary->nodes = (ng_list*)malloc(sizeof(ng_t_list) * hashmap_size);
	if (!dictionary->nodes)
	{
		free(dictionary);
		return NULL;
	}
	for (int i = 0; i < hashmap_size; ++i)
	{
		dictionary->nodes[i] = ng_list_new(ng_t_dictionary_pair, compare_function);
		if (!dictionary->nodes) {
			free(dictionary->nodes);
			dictionary->nodes = NULL;
			free(dictionary);
			return NULL;
		}
	}
	return dictionary;
}

int __ng_dictionary_increase_hashmap_size(ng_set dictionary, const size_t size_increase_amount)
{
	if (size_increase_amount > 0)
	{
		ng_set larger_set = __ng_dictionary_new_hashed(dictionary->hashmap_size + size_increase_amount, dictionary->hash_function, (*dictionary->nodes)->compare_function);
		if (!larger_set)
		{
			return -1;
		}

		const size_t current_set_size = dictionary->hashmap_size;
		for (size_t i = 0; i < current_set_size; ++i)
		{
			ng_node_list current_node = *(dictionary->nodes[i]->nodes);
			while (current_node)
			{
				ng_dictionary_pair node_value = (ng_dictionary_pair)(current_node->data);
				__ng_dictionary_add(larger_set, node_value->key, node_value->key_length, node_value->value);
				current_node = current_node->next;
			}
		}
		*dictionary = *larger_set;
	}
	return 0;
}

int __ng_dictionary_add(ng_set dictionary, const void* key, const size_t key_length, void* value)
{
	size_t hash = dictionary->hash_function(key, key_length);
	size_t index = hash % dictionary->hashmap_size;

	ng_node_list new_element = (ng_node_list)malloc(sizeof(ng_t_node_list));
	if (!new_element)
	{
		return -1;
	}

	ng_dictionary_pair set_element = (ng_dictionary_pair)malloc(sizeof(ng_t_dictionary_pair));
	if (!set_element)
	{
		return -1;
	}
	set_element->key_length = key_length;
	set_element->key = key;
	set_element->value = value;

	new_element->data = set_element;
	new_element->next = NULL;

	size_t node_length = 1;
	ng_node_list head = *dictionary->nodes[index]->nodes;
	if (!head)
	{
		*(dictionary->nodes[index]->nodes) = new_element;
	}
	else
	{
		ng_node_list tail;
		do
		{
			if ((*dictionary->nodes)->compare_function(((ng_dictionary_pair)(head->data))->key, key) == 0)
			{
				free(new_element);
				return -1;
			}
			tail = head;
			head = head->next;
			++node_length;
		} while (head);
		tail->next = new_element;
	}
	if (node_length > (size_t)(dictionary->hashmap_size * 0.5f))
	{
		size_t increase_size = dictionary->hashmap_size;
		if (increase_size < 4) increase_size = 4;
		__ng_dictionary_increase_hashmap_size(dictionary, increase_size);
	}
	return 0;
}

int __ng_dictionary_contains_key(ng_set dictionary, const void* key, const size_t key_length)
{
	size_t hash = dictionary->hash_function(key, key_length);
	size_t index = hash % dictionary->hashmap_size;

	ng_node_list head = *dictionary->nodes[index]->nodes;
	while (head)
	{
		if ((*dictionary->nodes)->compare_function(((ng_dictionary_pair)(head->data))->key, key) == 0)
		{
			return 0;
		}
		head = head->next;
	}
	return -1;
}

void* __ng_dictionary_get(ng_set dictionary, const void* key_to_retrieve, const size_t key_length)
{
	size_t hash = dictionary->hash_function(key_to_retrieve, key_length);
	size_t index = hash % dictionary->hashmap_size;

	ng_node_list head = *dictionary->nodes[index]->nodes;
	while (head)
	{
		if ((*dictionary->nodes)->compare_function(((ng_dictionary_pair)(head->data))->key, key_to_retrieve) == 0)
		{
			return ((ng_dictionary_pair)(head->data))->value;
		}
		head = head->next;
	}
	return NULL;
}

int __ng_dictionary_remove(ng_set dictionary, const void* key, const size_t key_length)
{
	size_t hash = dictionary->hash_function(key, key_length);
	size_t index = hash % dictionary->hashmap_size;

	ng_node_list last_node = NULL;
	ng_node_list node = *dictionary->nodes[index]->nodes;
	while (node)
	{
		if ((*dictionary->nodes)->compare_function(((ng_dictionary_pair)(node->data))->key, key) == 0)
		{
			if (last_node)
			{
				last_node->next = node->next;
			}
			else
			{
				*dictionary->nodes[index]->nodes = node->next;
			}
			node->data = NULL;
			return 0;
		}
		last_node = node;
		node = node->next;
	}
	return -1;
}