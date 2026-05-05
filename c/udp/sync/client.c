#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>

int main(int argc, char** argv) {
	int client_socket;

	if((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "Couldn't create socket!\n");
		return 1;
	}

	struct sockaddr_in remote_server_info;
	memset(&remote_server_info, 0, sizeof(remote_server_info));

	remote_server_info.sin_family = AF_INET;
	remote_server_info.sin_addr.s_addr = inet_addr("127.0.0.1");
	remote_server_info.sin_port = htons(9999);

	while(1) {
		int number, data;
		
		number = rand();
		data = htonl(number);

		if(sendto(client_socket, (void *)&data, sizeof(data), 0,
			(struct sockaddr *)&remote_server_info, sizeof(remote_server_info)) < 0 ) {
			fprintf(stderr, "Couldn't send packet to server!\n");

			return 1;
		}

		printf("sent %d\n", number);
		sleep(3);
	}
}
