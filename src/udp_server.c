#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <stdio.h>

int start_server()
{
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
	for (;;)
	{
		char buffer[4096];
		struct sockaddr_in sender_in;
		int sender_in_size = sizeof(sender_in);
		int len = recvfrom(s, buffer, 4096, 0, (struct sockaddr*)&sender_in, &sender_in_size);
		if (len > 0)
		{
			char addr_as_string[64];
			inet_ntop(AF_INET, &sender_in.sin_addr, addr_as_string, 64);
			printf("received %d bytes from %s:%d\n", len, addr_as_string, ntohs(sender_in.sin_port));
		}
	}
	return 0;
}