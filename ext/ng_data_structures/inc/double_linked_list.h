#pragma once
typedef struct ng_s_node_double_list
{
	void* data;
	struct ng_s_node_double_list* prev;
	struct ng_s_node_double_list* next;
} ng_t_node_double_list;

typedef  ng_t_node_double_list* ng_node_double_list;
typedef ng_node_double_list* ng_head_double_list;

typedef struct ng_s_double_list
{
	ng_head_double_list head;
	ng_head_double_list tail;
	size_t item_size;
	size_t capacity;
	size_t length;
	int(*compare_function)(const void*, const void*);
} ng_t_double_list;

typedef ng_t_double_list* ng_double_list;

#define ng_double_list_new(item_type, compare_function) __ng_double_list_new(sizeof(item_type), compare_function)
#define ng_double_list_increase_capacity(double_list, size_increase_amount) __ng_double_list_increase_capacity((ng_double_list)double_list, size_increase_amount)
#define ng_double_list_add(double_list, value) __ng_double_list_add((ng_double_list)double_list, value)
#define ng_double_list_remove_at(double_list, index) __ng_double_list_remove_at((ng_double_list)double_list, index)
#define ng_double_list_add_after(double_list, value_to_add, value_after) __ng_double_list_add_after((ng_double_list)double_list, value_to_add, value_after)
#define ng_double_list_add_before(double_list, value_to_add, value_before) __ng_double_list_add_before((ng_double_list)double_list, value_to_add, value_before)
#define ng_double_list_shuffle(double_list) __ng_double_list_shuffle((ng_double_list)double_list)
#define ng_double_list_shuffle_pick_function(double_list, time_to_shuffle, pick_function) __ng_double_list_shuffle_pick_function((ng_double_list)double_list, time_to_shuffle, pick_function)
#define ng_double_list_get_at(double_list, index) __ng_double_list_get_at((ng_double_list)double_list, index)

ng_double_list __ng_double_list_new(const size_t item_size, int(*compare_function)(const void* v1, const void* v2));
int __ng_double_list_increase_capacity(ng_double_list double_list, size_t size_increase_amount);
int __ng_double_list_add(ng_double_list doubleList, void* value);
int __ng_double_list_remove_at(ng_double_list double_list, size_t index);
int __ng_double_list_add_after(ng_double_list double_list, void* value_to_add, void* value_after);
int __ng_double_list_add_before(ng_double_list double_list, void* value_to_add, void* value_before);
ng_double_list __ng_double_list_shuffle(ng_double_list double_list);
ng_double_list __ng_double_list_shuffle_pick_function(ng_double_list double_list, const size_t time_to_shuffle, int(*pick_function)());
void* __ng_double_list_get_at(ng_double_list double_list, const size_t index);
