#include<stdlib.h>
#include<poll.h>
#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>

// bo bo bo - jey one

#define MAX_CONNECTIONS 100

//stole idea for unordered array from https://beej.us/guide/bgnet/examples/pollserver.c
int pfd_arr_rm(struct pollfd *pfd_arr, int i, int* len_ptr) {
	if(*len_ptr < 0) return 0;
	//int back_of_arr = *len_ptr; pfd_arr[i] = pollfd_arr[back_of_arr]
	//pollfd_arr[i] = pollfd_arr[*(len_ptr--)];
	
	pfd_arr[i] = pfd_arr[*len_ptr];
	(*len_ptr)--;
	printf("current array length: %d\n", *len_ptr);
	return 1;
}

int pfd_arr_add(struct pollfd *pfd_arr, int fd, int* len_ptr) {
	printf("adding new file descriptor\n");
	if(*len_ptr > MAX_CONNECTIONS - 1) return 0;

	pfd_arr[*len_ptr].fd = fd;
	pfd_arr[*len_ptr].events = POLLIN;

	(*len_ptr)++;
	printf("current array length: %d\n", *len_ptr);

	return 1;
}

int handle_incoming(struct pollfd *pfd_arr, int l_fd, int* len_ptr) {
	struct sockaddr_in incoming_addr;
	socklen_t incoming_addr_len;
	int new_fd;

}


int main(int argc, char** argv) {
	int listening_fd;

	listening_fd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in listening_addr;
	memset(&listening_addr, 0, sizeof(listening_addr));

	listening_addr.sin_family = AF_INET;
	listening_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	listening_addr.sin_port = htons(9999);

	if(bind(listening_fd, (struct sockaddr *)&listening_addr, sizeof(listening_addr))) {
		perror("couldn't bind");
		return -1;
	}

	if(listen(listening_fd, 5) < 0) {
		perror("couldn't listen");
		return -1;
	}

	//use helper functions to interract, but allow looping over members
	struct pollfd *pollfd_arr;
	pollfd_arr = calloc(sizeof(struct pollfd), MAX_CONNECTIONS);

	int pollfd_arr_len, num_ready, new_fd, bytes_read, data, num;
	char incoming_addr_ip_str[INET_ADDRSTRLEN];
	struct sockaddr_in incoming_addr;
	socklen_t incoming_addr_len;

	//uh should pass sooooooooo im not checking it lol
	pfd_arr_add(pollfd_arr, listening_fd, &pollfd_arr_len);

	while(1) {
		num_ready = poll(pollfd_arr, pollfd_arr_len, 100);

		if(num_ready <= 0) continue;

		for(int i = 0; i < pollfd_arr_len; i++) {
			if(pollfd_arr[i].revents & POLLHUP) {
				getpeername(pollfd_arr[i].fd,
						(struct sockaddr *) &incoming_addr,
						&incoming_addr_len);

				inet_ntop(AF_INET, &(incoming_addr.sin_addr),
						incoming_addr_ip_str,
						INET_ADDRSTRLEN);

				printf("%s:%hu disconnected!\n",
						incoming_addr_ip_str,
						ntohs(incoming_addr.sin_port));

				pfd_arr_rm(pollfd_arr, i, &pollfd_arr_len);
			}
			if(!(pollfd_arr[i].revents & POLLIN)) continue;

			if(pollfd_arr[i].fd == listening_fd) {
				new_fd = accept(listening_fd,
					       (struct sockaddr *) &incoming_addr,
					       &incoming_addr_len
					 );

				pfd_arr_add(pollfd_arr, new_fd, &pollfd_arr_len);
			} else {
				recvfrom(pollfd_arr[i].fd, &data, sizeof(data), 0,
						(struct sockaddr*) &incoming_addr,
						&incoming_addr_len);

				inet_ntop(AF_INET, &(incoming_addr.sin_addr),
						incoming_addr_ip_str,
						INET_ADDRSTRLEN);

				num = ntohl(data);

				printf("recieved %d from %s:%hu\n",
						num, incoming_addr_ip_str,
						ntohs(incoming_addr.sin_port));
			}
		}
	}
}
