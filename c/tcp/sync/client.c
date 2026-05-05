#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<stdlib.h>

int main(int argc, char** argv) {
	int client_socket;

	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(client_socket == -1) {
		fprintf(stderr, "Couldn't create socket!\n");
		return 1;
	} else {
		printf("Created socket...\n");
	}

	//since socket_info is a stack variable, and you 
	//	can't controlw what was already on the stack,
	//	you need to zeroize it to avoid uninitialized 
	//	memory vulns (or atleast its good practice to)
	struct sockaddr_in socket_info;

	memset(&socket_info, 0, sizeof(socket_info));

	socket_info.sin_family = AF_INET;
	socket_info.sin_addr.s_addr = inet_addr("127.0.0.1");
	socket_info.sin_port = htons(9999);
	printf("Configured socket_info...\n");

	
	if(connect(client_socket, (struct sockaddr *)&socket_info, sizeof(socket_info)) < 0) {
		fprintf(stderr, "Couldn't connect to server!\n");
		return 1;
	}

	while(1) {
		int number, data;
		number = rand();
		data = htonl(number);

		if(send(client_socket, (void*)&data, sizeof(data), 0) < -1) {
			fprintf(stderr, "send() failed!\n");
			return 1;
		}

		printf("sent %d\n", number);
		sleep(3);
	}

	return 0;
};
