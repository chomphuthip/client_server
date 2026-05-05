#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/socket.h>
#include<stdio.h>
#include<arpa/inet.h>
#include<pthread.h>


struct worker_args {
	struct sockaddr_in addr;
	int fd;
};

void* worker_thread(void* args) {
	struct sockaddr_in addr;
	int fd, num, sum, recvd;

	memcpy(&addr, &(((struct worker_args*)args)->addr), sizeof(struct sockaddr_in));
	fd = ((struct worker_args*)args)->fd;
	free(args);

	printf("worker_thread(): fd = %d\n", fd);

	char addr_str[INET_ADDRSTRLEN];
	short port;
	inet_ntop(AF_INET, &addr.sin_addr, addr_str, INET_ADDRSTRLEN);
	port = ntohs(addr.sin_port);

	sum = 0;
	while(1) {
		if((recvd = recv(fd, &num, sizeof(num), 0)) <= 0) {
			if(recvd == 0)
				printf("Connection to %s:%hu closed.", addr_str, port);
			else
				fprintf(stderr, "Connection error to %s:%hu.", addr_str, port);

			fprintf(stderr, "%s\n", strerror(errno));

			return 0x0;
		}

		num = ntohl(num);
		sum = sum + num;

		printf("worker thread for %s:%hu recieved = %d\n", addr_str, port, num);
		printf("worker thread for %s:%hu sum = %d\n", addr_str, port, sum);
	}
}

//create thread on accept
int main(int argc, char** argv)  {
	int socket_fd;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		fprintf(stderr, "Couldn't create socket!\n");
		return -1;
	}

	struct sockaddr_in socket_addr;
	memset(&socket_addr, 0, sizeof(socket_addr));

	socket_addr.sin_family = AF_INET;
	socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	socket_addr.sin_port = htons(9999);

	if(bind(socket_fd, (struct sockaddr*)&socket_addr, sizeof(socket_addr)) < 0) {
		fprintf(stderr, "Couldn't bind socket!\n");
		return -1;
	}

	if(listen(socket_fd, 5) < 0) {
		fprintf(stderr, "Couldn't listen on socket\n");
		fprintf(stderr, "%s\n", strerror(errno));
		return -1;
	}

	struct sockaddr_in cur_connection_addr;
	struct worker_args* args;
	int cur_connection_fd;
	pthread_t cur_thread;
	socklen_t addr_ln;

	while(1) {
		if((cur_connection_fd = accept(socket_fd, (struct sockaddr*)&cur_connection_addr, &addr_ln)) < 0) {
			fprintf(stderr, "Couldn't accept connection!\n");
			return -1;
		}

		args = malloc(sizeof(struct worker_args));

		memcpy(&(args->addr), &cur_connection_addr, sizeof(cur_connection_addr));
		args->fd = cur_connection_fd;

		printf("main(): cur_connection_fd = %d\n", cur_connection_fd);

		pthread_create(&cur_thread, 0, worker_thread, args);
	}
}
