#include <stddef.h> // required for NULL
#include <stdlib.h>
#include <stdio.h>
#include "..\inc\linked_list.h"

ng_list __ng_list_new(const size_t items_size, int(*compare_function)(const void* v1, const void* v2))
{
	ng_list list = malloc(sizeof(ng_t_list));
	if (!list) return NULL;
	list->elementSize = items_size;
	list->capacity = 5;
	list->length = 0;
	list->compare_function = compare_function;
	list->nodes = (ng_head_list)calloc(list->capacity, sizeof(ng_t_node_list) * items_size);
	if (!list->nodes)
	{
		free(list);
		return NULL;
	}
	return list;
}

int __ng_list_increase_capacity(ng_list list, const size_t size_increase_amount)
{
	if (size_increase_amount > 0)
	{
		list->capacity += size_increase_amount;
		ng_head_list newNodes = (ng_head_list)realloc(list->nodes, list->capacity * sizeof(ng_t_node_list) * list->elementSize);
		if (!newNodes)
		{
			return -1;
		}
		list->nodes = newNodes;
	}
	return 0;
}

int __ng_list_add(ng_list list, void* value)
{
	ng_node_list element = malloc(sizeof(ng_t_node_list) * list->elementSize);
	if (!element) return -1;
	element->data = value;
	ng_node_list tail = *(list->nodes);
	if (!tail)
	{
		*(list->nodes) = element;
	}
	else
	{
		while (tail->next)
		{
			tail = tail->next;
		}
		tail->next = element;
	}
	element->next = NULL;
	++list->length;
	if (list->length > list->capacity - 2)
		__ng_list_increase_capacity(list, list->capacity);
	return 0;
}

void* __ng_list_get_at(ng_list list, size_t index)
{
	if (index >= list->length)
	{
		return NULL;
	}

	ng_node_list currentNode = *(list->nodes);
	for (size_t i = 0; i < index; ++i)
	{
		currentNode = currentNode->next;
	}
	return currentNode->data;
}

void* __ng_list_get_last(ng_list list)
{
	return __ng_list_get_at(list, list->length - 1);
}

int __ng_list_remove_at(ng_list list, size_t index)
{
	if (index >= list->length)
	{
		return -1;
	}
	
	ng_node_list last_node = *list->nodes;
	if (index == 0)
	{
		*list->nodes = last_node->next;
		last_node->next = NULL;
	}
	else
	{
		ng_node_list current_node = last_node->next;
		for (size_t i = 1; i < index; ++i)
		{
			last_node = current_node;
			current_node = current_node->next;
		}
		last_node->next = current_node->next;
		current_node->next = NULL;
	}
	--(list->length);
	return 0;
}

int __ng_list_remove_first(ng_list list)
{
	return __ng_list_remove_at(list, 0);
}

int __ng_list_remove_last(ng_list list)
{
	return __ng_list_remove_at(list, list->length - 1);
}

int __ng_list_remove(ng_list list, void* value_to_remove)
{
	ng_node_list last_node = NULL;
	ng_node_list current_node = *list->nodes;
	while (current_node)
	{
		if (list->compare_function(value_to_remove, current_node->data) == 0)
		{
			if (last_node)
			{
				last_node->next = current_node->next;
			}
			else
			{
				*list->nodes = current_node->next;
			}
			current_node->next = NULL;
			--(list->length);
			return 0;
		}
		last_node = current_node;
		current_node = current_node->next;
	}
	return -1;
}

ng_list __ng_list_reverse(ng_list list) 
{
	ng_list reversed_list = __ng_list_new(list->elementSize, list->compare_function);
	for (int i = list->length - 1; i > -1 ; --i)
	{
		__ng_list_add(reversed_list, __ng_list_get_at(list, i));
	}
	return reversed_list;
}