#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "dictionary.h"
#include "authcode.h"

typedef struct player_t {
	char ip_address[21];
	char authcode[14];
	float x_pos;
	float y_pos;
	struct sockaddr_in socket;
} player_t;
typedef player_t* player;

int cmp_player(const void* player1, const void* player2)
{
	return strcmp(((const player)player1)->ip_address, ((const player)player2)->ip_address) == 0;
}

int cmp_strings(const void* o1, const void* o2) {
	return strcmp((const char*)o1, (const char*)o2);
}

const float x_poses[] = { 64, 256, 128, 160, 96, 288 };
const float y_poses[] = { 64, 96, 128, 160 };

int send_broadcast_except(int socket, ng_set players, player exception, char* message, int message_size)
{
	const size_t current_set_size = players->hashmap_size;
	for (size_t i = 0; i < current_set_size; ++i)
	{
		ng_node_list current_node = *(players->nodes[i]->nodes);
		while (current_node)
		{
			ng_dictionary_pair node_value = (ng_dictionary_pair)(current_node->data);
			player p = malloc(sizeof(struct player_t));
			p = (player)node_value->value;
			if (strcmp(p->ip_address, exception->ip_address) != 0)
			{
				sendto(socket, message, message_size, 0, (struct sockaddr*)&(p->socket), ntohs(p->socket.sin_port));
			}
			current_node = current_node->next;
		}
	}
	return 1;
}

int send_broadcast(int socket, ng_set players, char* message, int message_size)
{
	const size_t current_set_size = players->hashmap_size;
	for (size_t i = 0; i < current_set_size; ++i)
	{
		ng_node_list current_node = *(players->nodes[i]->nodes);
		while (current_node)
		{
			ng_dictionary_pair node_value = (ng_dictionary_pair)(current_node->data);
			player p = malloc(sizeof(struct player_t));
			p = (player)node_value->value;
			sendto(socket, message, message_size, 0, (struct sockaddr*)&(p->socket), ntohs(p->socket.sin_port));
			current_node = current_node->next;
		}
	}
	return 1;
}

float get_random_val(const float values[])
{
	int index = rand() % (int)(sizeof(values) * 0.25f);
	//printf("Random value: %d %f\n", index, values[index]);
	return values[index];
}

int main(int argc, char** argv)
{
	srand(time(NULL));
#ifdef _WIN32
	// this part is only required on Windows: it initializes the Winsock2 dll
	WSADATA wsa_data;
	if (WSAStartup(0x0202, &wsa_data))
	{
		printf("unable to initialize winsock2 \n");
		return -1;
	}
#endif
	int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s < 0)
	{
		printf("unable to initialize the UDP socket \n");
		return -1;
	}
	printf("socket %d created \n", s);
	struct sockaddr_in sin;
	inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr); // this will create a big endian 32 bit address
	sin.sin_family = AF_INET;
	sin.sin_port = htons(9999); // converts 9999 to big endian
	if (bind(s, (struct sockaddr*)&sin, sizeof(sin)))
	{
		printf("unable to bind the UDP socket \n");
		return -1;
	}

	ng_set players = ng_dictionary_new(cmp_strings);

	for (;;)
	{
		char buffer[4096];
		struct sockaddr_in sender_in;
		int sender_in_size = sizeof(sender_in);
		int len = recvfrom(s, buffer, 4096, 0, (struct sockaddr*)&sender_in, &sender_in_size);
		if (len > 0)
		{
			buffer[len - 1] = '\0';
			char addr_as_string[21];
			inet_ntop(AF_INET, &sender_in.sin_addr, addr_as_string, 15);
			u_short port = ntohs(sender_in.sin_port);
			snprintf(addr_as_string + strlen(addr_as_string), 6, "%d", port);
			addr_as_string[strlen(addr_as_string)] = '\0';
			if (ng_dictionary_contains_key(players, addr_as_string, strlen(addr_as_string)) == 0)
			{
				player p = ng_dictionary_get(players, addr_as_string, strlen(addr_as_string));
				if (len == 9)
				{
					memcpy(&p->x_pos, buffer, 4);
					memcpy(&p->y_pos, buffer + 4, 4);
					//printf("%s: new position is (%f, %f)\n", addr_as_string, p->x_pos, p->y_pos);
					char old_player_message[29];
					memcpy(old_player_message, p->ip_address, 20);
					memcpy(old_player_message + 20, &p->x_pos, 4);
					memcpy(old_player_message + 24, &p->y_pos, 4);
					send_broadcast_except(s, players, p, old_player_message, 29);
				}
			}
			else
			{
				player new_player = malloc(sizeof(player_t));
				memcpy(new_player->ip_address, addr_as_string, strlen(addr_as_string));
				new_player->ip_address[strlen(addr_as_string)] = '\0';
				memcpy(new_player->authcode, __ng_authcode_generate(addr_as_string), 13);
				new_player->x_pos = get_random_val(x_poses);
				new_player->y_pos = get_random_val(y_poses);
				memcpy(&new_player->socket, &sender_in, sizeof(struct sockaddr_in));
				
				ng_dictionary_add(players, addr_as_string, strlen(addr_as_string), new_player);
				printf("New player added %s:%d. Auth: %s\n", new_player->ip_address, ntohs(sender_in.sin_port), new_player->authcode);

				char start_message[22];
				memcpy(start_message, new_player->authcode, 13);
				memcpy(start_message + 13, &new_player->x_pos, 4);
				memcpy(start_message + 17, &new_player->y_pos, 4);
				start_message[21] = '\0';

				sendto(s, start_message, 22, 0, (struct sockaddr*)&sender_in, ntohs(sender_in.sin_port));
				printf("Starting value: %f %f\n", new_player->x_pos, new_player->y_pos);

				char new_player_message[29];
				memcpy(new_player_message, addr_as_string, 20);
				memcpy(new_player_message + 20, &new_player->x_pos, 4);
				memcpy(new_player_message + 24, &new_player->y_pos, 4);
				new_player_message[28] = '\0';
				send_broadcast_except(s, players, new_player, new_player_message, 29);

				char old_player_message[29];
				const size_t current_set_size = players->hashmap_size;
				for (size_t i = 0; i < current_set_size; ++i)
				{
					ng_node_list current_node = *(players->nodes[i]->nodes);
					while (current_node)
					{
						ng_dictionary_pair node_value = (ng_dictionary_pair)(current_node->data);
						player p = malloc(sizeof(struct player_t));
						p = (player)node_value->value;
						if (strcmp(p->ip_address, new_player->ip_address) != 0)
						{
							memcpy(old_player_message, p->ip_address, 20);
							memcpy(old_player_message + 20, &p->x_pos, 4);
							memcpy(old_player_message + 24, &p->y_pos, 4);
							old_player_message[28] = '\0';
							sendto(s, old_player_message, 29, 0, (struct sockaddr*)&sender_in, ntohs(sender_in.sin_port));
						}
						current_node = current_node->next;
					}
				}
			}
		}
	}
	return 0;
}