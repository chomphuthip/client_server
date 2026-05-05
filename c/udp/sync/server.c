#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>


int main(int argc, char** argv) {
	int server_socket;

	server_socket = socket(AF_INET, SOCK_DGRAM, 0);
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
	printf("Socket bound...\n");


	struct sockaddr_in incoming_info;
	socklen_t incoming_info_l; //accept stores incoming_info len in a size_t thingy

	while(1) {
		int raw_data;

		if(recvfrom(server_socket, (void*)&raw_data, sizeof(raw_data), 0,
					(struct sockaddr *)&incoming_info, &incoming_info_l) < 0) {
			fprintf(stderr, "recv() failed!\n");
			return 1;
		}

		int number;
		number = ntohl(raw_data);

		char addr_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(incoming_info.sin_addr), addr_str, INET_ADDRSTRLEN);

		printf("Received number: %d from %s\n", number, addr_str);
		memset(&addr_str, 0, INET_ADDRSTRLEN);
	}

	return 0;
};
