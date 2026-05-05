#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>


int main(int argc, char** argv) {
	int server_socket;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket == -1) {
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
	socket_info.sin_addr.s_addr = htonl(INADDR_ANY); //any local address
	socket_info.sin_port = htons(9999);
	printf("Configured socket_info...\n");
	
	//now bind the socket to the socket_info
	if(bind(server_socket, (struct sockaddr *)&socket_info, sizeof(socket_info)) != 0) {
		fprintf(stderr, "Couldn't bind socket!\n");
		return 1;
	}

	//sets up a listening queue that is 5 connections long
	if(listen(server_socket, 5) != 0) {
		fprintf(stderr, "Couldn't listen on socket!\n");
		return 1;
	}

	int connection_fd;
	struct sockaddr_in incoming_info;
	socklen_t incoming_info_l; //accept stores incoming_info len in a size_t thingy

	connection_fd = accept(server_socket, (struct sockaddr *)&incoming_info, &incoming_info_l);
	if(connection_fd < 0) {
		fprintf(stderr, "Couldn't accept connection!\n");
		return 1;
	}

	while(1) {
		int raw_data;

		if(recv(connection_fd, (void*)&raw_data, sizeof(raw_data), 0) < -1) {
			fprintf(stderr, "recv() failed!\n");
			return 1;
		}

		int number;
		number = ntohl(raw_data);

		printf("Received number: %d\n", number);
	}

	return 0;
};
