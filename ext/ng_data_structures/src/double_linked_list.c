#include <stddef.h> // required for NULL
#include <stdlib.h>
#include <time.h>
#include "..\inc\double_linked_list.h"

ng_double_list __ng_double_list_new(const size_t item_size, int(*compare_function)(const void* v1, const void* v2))
{
	ng_double_list double_list = malloc(sizeof(ng_t_double_list));
	if (!double_list)
		return NULL;
	double_list->item_size = item_size;
	double_list->capacity = 5;
	double_list->length = 0;
	double_list->compare_function = compare_function;
	double_list->head = (ng_head_double_list)calloc(double_list->capacity, sizeof(ng_t_node_double_list) * item_size);
	if (!double_list->head)
	{
		free(double_list);
		return NULL;
	}
	double_list->tail = (ng_head_double_list)calloc(double_list->capacity, sizeof(ng_t_node_double_list) * item_size);
	if (!double_list->tail)
	{
		free(double_list->head);
		free(double_list);
		return NULL;
	}
	return double_list;
}

int __ng_double_list_increase_capacity(ng_double_list double_list, size_t size_increase_amount)
{
	if (size_increase_amount > 0)
	{
		ng_head_double_list newHead = (ng_head_double_list)realloc(double_list->head, (double_list->capacity + size_increase_amount) * sizeof(ng_t_node_double_list) * double_list->item_size);
		if (!newHead)
		{
			return -1;
		}

		ng_head_double_list newTail = (ng_head_double_list)realloc(double_list->tail, (double_list->capacity + size_increase_amount) * sizeof(ng_t_node_double_list) * double_list->item_size);
		if (!newTail)
		{
			free(newHead);
			return -1;
		}

		double_list->head = newHead;
		double_list->tail = newTail;

		double_list->capacity += size_increase_amount;
	}
	return 0;
}

int __ng_double_list_add(ng_double_list double_list, void* value)
{
	ng_node_double_list new_node = malloc(sizeof(ng_t_node_double_list) * double_list->item_size);
	if (!new_node)
	{
		return -1;
	}
	new_node->data = value;

	ng_node_double_list tail_node = *double_list->tail;
	if (tail_node)
	{
		tail_node->next = new_node;
		new_node->prev = tail_node;
	}
	else
	{
		new_node->prev = NULL;
		*double_list->head = new_node;
	}
	new_node->next = NULL;
	*double_list->tail = new_node;
	++(double_list->length);
	return 0;
}

int __ng_double_list_remove_at(ng_double_list double_list, size_t index)
{
	if (index >= double_list->length)
	{
		return -1;
	}
	
	ng_node_double_list node;
	if (index == 0)
	{
		node = *double_list->head;
		*double_list->head = node->next;
		if (node)
		{
			node->next->prev = NULL;
		}
		node->next = NULL;
	}
	else if (index == double_list->length - 1)
	{
		node = *double_list->tail;
		*double_list->tail = node->prev;
		if (node)
		{
			node->prev->next = NULL;
		}
		node->prev = NULL;
	}
	else
	{
		if (index > (int)(double_list->length * 0.5f))
		{
			node = *double_list->tail;
			for (int i = index; i < double_list->length; ++i)
			{
				node = node->prev;
			}
		}
		else
		{
			node = *double_list->head;
			for (int i = 0; i < index; ++i)
			{
				node = node->next;
			}
		}
		node->prev->next = node->next;
		node->next->prev = node->prev;
		node->prev = NULL;
		node->next = NULL;
	}
	--(double_list->length);
	return 0;
}

int __ng_double_list_add_after(ng_double_list double_list, void* value_to_add, void* value_after)
{
	ng_node_double_list new_node = malloc(sizeof(ng_t_node_double_list) * double_list->item_size);
	if (!new_node)
	{
		return -1;
	}
	new_node->data = value_to_add;

	ng_node_double_list last_node = NULL;
	ng_node_double_list node = *double_list->tail;
	while (node)
	{
		if (double_list->compare_function(node->data, value_after) == 0)
		{
			if (last_node)
			{
				last_node->prev = new_node;
				new_node->prev = node;
				new_node->next = last_node;
				node->next = new_node;
			}
			else
			{
				node->next = new_node;
				new_node->prev = node;
				new_node->next = NULL;
				*double_list->tail = new_node;
			}
			++(double_list->length);
			return 0;
		}
		last_node = node;
		node = node->prev;
	}
	return -1;
}

int __ng_double_list_add_before(ng_double_list double_list, void* value_to_add, void* value_before)
{
	ng_node_double_list new_node = malloc(sizeof(ng_t_node_double_list) * double_list->item_size);
	if (!new_node)
	{
		return -1;
	}
	new_node->data = value_to_add;

	ng_node_double_list last_node = NULL;
	ng_node_double_list node = *double_list->head;
	while (node)
	{
		if (double_list->compare_function(node->data, value_before) == 0)
		{
			if (last_node)
			{
				last_node->next = new_node;
				new_node->prev = last_node;
				new_node->next = node;
				node->prev = new_node;
			}
			else
			{
				node->prev = new_node;
				new_node->prev = NULL;
				new_node->next = node;
				*double_list->head = new_node;
			}
			++(double_list->length);
			return 0;
		}
		last_node = node;
		node = node->next;
	}
	return -1;
}

ng_double_list __ng_double_list_shuffle(ng_double_list double_list)
{
	srand(time(NULL));
	return __ng_double_list_shuffle_pick_function(double_list, (size_t)(double_list->length * 0.5f), rand);
}

ng_double_list __ng_double_list_shuffle_pick_function(ng_double_list double_list, const size_t time_to_shuffle, int(*pick_function)())
{
	int max = double_list->length;
	ng_double_list shuffled_double_list = __ng_double_list_new(max, double_list->compare_function);
	if (!shuffled_double_list)
	{
		return NULL;
	}
	
	ng_node_double_list node = *double_list->head;
	while (node)
	{
		__ng_double_list_add(shuffled_double_list, node->data);
		node = node->next;
	}

	for (size_t i = 0; i < time_to_shuffle; ++i)
	{
		size_t random1 = pick_function() % max;
		size_t random2 = pick_function() % max;
		if (random1 != random2)
		{
			node = *shuffled_double_list->head;
			ng_node_double_list node1 = NULL;
			ng_node_double_list node2 = NULL;
			size_t j = 0;
			while (node)
			{
				if (j == random1)
				{
					node1 = node;
					if (node2)
					{
						break;
					}
				}
				if (j == random2)
				{
					node2 = node;
					if (node1)
					{
						break;
					}
				}
				++j;
				node = node->next;
			}
			void* data_tmp = node1->data;
			node1->data = node2->data;
			node2->data = data_tmp;
		}
	}
	return shuffled_double_list;
}

void* __ng_double_list_get_at(ng_double_list double_list, const size_t index)
{
	if (index >= double_list->length)
	{
		return NULL;
	}

	ng_node_double_list node = NULL;
	if (index > (size_t)(double_list->length * 0.5f))
	{
		node = *double_list->tail;
		size_t max = double_list->length - index - 1;
		for (size_t i = 0; i < max; ++i)
		{
			node = node->prev;
		}
	}
	else
	{
		node = *double_list->head;
		for (size_t i = 0; i < index; ++i)
		{
			node = node->next;
		}
	}
	return node->data;
}