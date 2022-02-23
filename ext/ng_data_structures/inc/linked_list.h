#pragma once
typedef struct ng_s_node_list
{
	void* data;
	struct ng_s_node_list* next;
} ng_t_node_list;

typedef ng_t_node_list* ng_node_list;
typedef ng_node_list* ng_head_list;

typedef struct ng_s_list
{
	ng_head_list nodes;
	size_t elementSize;
	size_t capacity;
	size_t length;
	int(*compare_function)(const void*, const void*);
} ng_t_list;

typedef ng_t_list* ng_list;

#define ng_list_new(item_type, compare_function) __ng_list_new(sizeof(item_type), compare_function)
#define ng_list_increase_capacity(list, size_increase_amount) __ng_list_increase_capacity((ng_list)list, size_increase_amount)
#define ng_list_add(list, value) __ng_list_add((ng_list) list, value)
#define ng_list_get_at(list, index) __ng_list_get_at((ng_list) list, index)
#define ng_list_reverse(list) __ng_list_reverse((ng_list) list)
#define ng_list_get_last(list) __ng_list_get_last((ng_list) list)
#define ng_list_remove_first(list) __ng_list_remove_first((ng_list) list)
#define ng_list_remove_at(list, index) __ng_list_remove_at((ng_list) list, index)
#define ng_list_remove_last(list) __ng_list_remove_last((ng_list) list)
#define ng_list_remove(list, value_to_remove) __ng_list_remove((ng_list) list, value_to_remove)


ng_list __ng_list_new(const size_t item_size, int(*compare_function)(const void* v1, const void* v2));
int __ng_list_increase_capacity(ng_list list, const size_t size_increase_amount);
int __ng_list_add(ng_list list, void* value);
void* __ng_list_get_at(ng_list list, size_t index);
ng_list __ng_list_reverse(ng_list list);
void* __ng_list_get_last(ng_list list);
int __ng_list_remove_first(ng_list list);
int __ng_list_remove_at(ng_list list, size_t index);
int __ng_list_remove_last(ng_list list);
int __ng_list_remove(ng_list list, void* value_to_remove);