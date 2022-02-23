#pragma once
#include "bomberman.h"
#include "linked_list.h"

#define ng_client_start() __ng_client_start()
#define ng_client_update(players, local_player) __ng_client_update(players, local_player)
#define ng_client_send_position(x, y) __ng_client_send_position(x, y)

int __ng_client_start();
int __ng_client_update(ng_list players, bomberman_t* local_player);
int __ng_client_send_position(float x, float y);