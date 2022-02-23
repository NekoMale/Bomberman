#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include <Windows.h>
#include "udp_client.h"
#include "bomberman.h"
#include "linked_list.h"

static struct sockaddr_in sin;
static char authcode[14];
static int s;

int __ng_client_start()
{
#ifdef _WIN32
	// this part is only required on Windows: it initializes the Winsock2 dll
	WSADATA wsa_data;
	if (WSAStartup(0x0202, &wsa_data))
	{
		printf("unable to initialize winsock2\n");
		return -1;
	}
#endif

	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s < 0)
	{
		printf("unable to initialize the UDP socket\n");
		return -1;
	}

#ifdef _WIN32
	unsigned long nb_mode = 1;
	ioctlsocket(s, FIONBIO, &nb_mode);
#else
	int flags = fcntl(s, F_GETFL, 0);
	if (flags < 0)
		return flags;
	flags |= O_NONBLOCK;
	fcntl(s, F_SETFL, flags);
#endif

	printf("Socket %d created\n", s);
	
	inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr); // this will create a big entian 32 bit address
	sin.sin_family = AF_INET;
	sin.sin_port = htons(9999); // converts 9999 to big endian

	int timeout = 10000;

	if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(unsigned int)))
	{
		printf("unable to set socket option for receive timeout");
		return -1;
	}

	int sent_bytes = sendto(s, "a", 1, 0, (struct sockaddr*)&sin, sizeof(sin));
	return 0;
}

int __ng_client_update(ng_list players, bomberman_t* local_player)
{
	//printf("Client update\n");
	char messagefrom[4096];
	struct sockaddr_in sender_in;
	int sender_in_size = sizeof(sender_in);
	int len = recvfrom(s, messagefrom, 4096, 0, (struct sockaddr*)&sender_in, &sender_in_size);
	messagefrom[4095] = '\0';
	//printf("%d\n", len);
	if (len == 22)
	{
		//inet_ntop(AF_INET, &sender_in.sin_addr, addr_as_string, 16);
		memcpy(authcode, messagefrom, 14);
		float x, y;
		memcpy(&x, messagefrom + 13, 4);
		memcpy(&y, messagefrom + 17, 4);
		local_player->movable.x = x;
		local_player->movable.y = y;

		//printf("Starting package received (%f, %f) \n", local_player.movable.x, local_player.movable.y);
		//int sent_bytes = sendto(s, authcode, 14, 0, (struct sockaddr*)&sender_in, sizeof(sender_in));
	}
	else if (len == 29)
	{
		char other_player_ip_address[21];
		float other_player_x, other_player_y;
		memcpy(other_player_ip_address, messagefrom, 20);
		other_player_ip_address[20] = '\0';
		memcpy(&other_player_x, messagefrom + 20, 4);
		memcpy(&other_player_y, messagefrom + 24, 4);

		int found = -1;
		for (int i = 0; i < players->length; ++i)
		{
			bomberman other_player = (bomberman)(players->nodes[i]->data);
			if (strcmp(other_player->ip_address, other_player_ip_address) == 0)
			{
				other_player->movable.x = other_player_x;
				other_player->movable.y = other_player_y;
				found = 0;
				//printf("Old player position received %s (%f, %f)\n", other_player.ip_address, other_player.movable.x, other_player.movable.y);
				break;
			}
		}
		if (found < 0)
		{
			bomberman other_player = malloc(sizeof(bomberman_t));
			memcpy(other_player->ip_address, other_player_ip_address, 20);
			other_player->ip_address[20] = '\0';
			other_player->movable.x = other_player_x;
			other_player->movable.y = other_player_y;
			other_player->movable.width = 32;
			other_player->movable.height =32;
			other_player->movable.speed = 48;

			ng_list_add(players, other_player);
			bomberman prova = (bomberman)ng_list_get_at(players, 0);
			printf("New player position received %s (%f, %f)\n", prova->ip_address, prova->movable.x, prova->movable.y);
		}

		//printf("Package received %s (%f, %f)\n", other_player_ip_address, other_player_x, other_player_y);
	}
	//printf("Client update end\n");
	return 0;
}

int __ng_client_send_position(float x, float y)
{
	char message[9];
	memcpy(message, &x, sizeof(float));
	memcpy(message + 4, &y, sizeof(float));
	message[8] = '\0';
	int sent_bytes = sendto(s, message, 9, 0, (struct sockaddr*)&sin, sizeof(sin));
	return 0;
}