#include <stddef.h> // required for NULL
#include <stdlib.h>
#include "..\inc\set.h"
#include "..\inc\linked_list.h"

size_t djb33x_hash(const void* key, const size_t key_length)
{
	size_t hash = 5381;
	const char* c_key = key;
	for (size_t i = 0; i < key_length; i++)
	{
		hash = ((hash << 5) + hash) ^ c_key[i];
	}
	return hash;
}

ng_set __ng_set_new(int(*compare_function)(const void* v1, const void* v2)) {
	return __ng_set_new_hashed(10, djb33x_hash, compare_function);
}

ng_set __ng_set_new_hashed(const size_t hashmap_size, size_t(*hash_function)(const void* key, size_t key_length), int(*compare_function)(const void* v1, const void* v2)) 
{
	ng_set set = (ng_set)malloc(sizeof(ng_t_set));
	if (!set)
	{
		return NULL;
	}
	set->hashmap_size = hashmap_size;
	set->hash_function = hash_function;
	set->nodes = (ng_list*)malloc(sizeof(ng_t_list) * hashmap_size);
	if (!set->nodes)
	{
		free(set);
		return NULL;
	}
	for (int i = 0; i < hashmap_size; ++i)
	{
		set->nodes[i] = ng_list_new(ng_key_set, compare_function);
		if (!set->nodes) {
			free(set->nodes);
			set->nodes = NULL;
			free(set);
			return NULL;
		}
	}
	return set;
}

int __ng_set_add(ng_set set, const void* value, const size_t key_length)
{
	size_t hash = set->hash_function(value, key_length);
	size_t index = hash % set->hashmap_size;

	ng_node_list new_element = (ng_node_list)malloc(sizeof(ng_t_node_list));
	if (!new_element)
	{
		return -1;
	}

	ng_key_set set_element = (ng_key_set)malloc(sizeof(ng_t_key_set));
	if (!set_element)
	{
		return -1;
	}

	set_element->key_length = key_length;
	set_element->key = value;

	new_element->data = set_element;
	new_element->next = NULL;


	size_t node_length = 1;
	ng_node_list head = *set->nodes[index]->nodes;
	if (!head)
	{
		*(set->nodes[index]->nodes) = new_element;
	}
	else
	{
		ng_node_list tail;
		do
		{
			if ((*set->nodes)->compare_function(((ng_key_set)(head->data))->key, value) == 0)
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
	if (node_length > (size_t)(set->hashmap_size * 0.5f))
	{
		size_t increase_size = set->hashmap_size;
		if (increase_size < 4) increase_size = 4;
		__ng_set_increase_hashmap_size(set, increase_size);
	}
	return 0;
}

int __ng_set_exists(ng_set set, void* value, const size_t key_length)
{
	size_t hash = set->hash_function(value, key_length);
	size_t index = hash % set->hashmap_size;

	ng_node_list head = *set->nodes[index]->nodes;
	while (head)
	{
		if ((*set->nodes)->compare_function(((ng_key_set)(head->data))->key, value) == 0)
		{
			return 0;
		}
		head = head->next;
	}
	return -1;
}

int __ng_set_remove(ng_set set, void* value, const size_t key_length)
{
	size_t hash = set->hash_function(value, key_length);
	size_t index = hash % set->hashmap_size;

	ng_node_list lastNode = NULL;
	ng_node_list node = *set->nodes[index]->nodes;
	while (node)
	{
		if ((*set->nodes)->compare_function(((ng_key_set)(node->data))->key, value) == 0)
		{
			if (lastNode)
			{
				lastNode->next = node->next;
			}
			else {
				*set->nodes[index]->nodes = node->next;
			}
			node->data = NULL;
			return 0;
		}
		lastNode = node;
		node = node->next;
	}
	return -1;
}

int __ng_set_increase_hashmap_size(ng_set set, const size_t size_increase_amount)
{
	if (size_increase_amount > 0)
	{
		ng_set larger_set = __ng_set_new_hashed(set->hashmap_size + size_increase_amount, set->hash_function, (*set->nodes)->compare_function);
		if (!larger_set)
		{
			return -1;
		}

		const size_t current_set_size = set->hashmap_size;
		for (size_t i = 0; i < current_set_size; ++i)
		{
			ng_node_list current_node = *(set->nodes[i]->nodes);
			while (current_node)
			{
				ng_key_set node_value = (ng_key_set)(current_node->data);
				__ng_set_add(larger_set, node_value->key, node_value->key_length);
				current_node = current_node->next;
			}
		}
		*set = *larger_set;
	}
	return 0;
}
