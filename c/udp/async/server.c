#include<pthread.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include<arpa/inet.h>

#define MAX_CONNECTIONS 100
#define MAX_QUEUE_SIZE 100

struct pool_entry_t {
	int addr;
	short port;
	char in_use;
	pthread_t thread;
	struct thread_queue_t* queue;
};

//circular, writes over self
struct thread_queue_t {
	pthread_mutex_t mut;
	pthread_cond_t cond;
	int fp, bp;
	int buffer[MAX_QUEUE_SIZE]; 
};

int init_entry(struct pool_entry_t* entry) {
	memset(entry, 0, sizeof(struct pool_entry_t));

	entry->in_use = 1;
	entry->queue = malloc(sizeof(struct thread_queue_t));
	entry->queue->fp = 0;
	entry->queue->bp = 0;

	if(pthread_mutex_init(&entry->queue->mut, 0) != 0) {
		fprintf(stderr, "Couldn't create queue mutex!\n");

		return -1;
	}


	if(pthread_cond_init(&entry->queue->cond, 0) != 0) {
		fprintf(stderr, "Couldn't create queue condition!\n");

		return -1;
	}

	return 0;
}

int deinit_entry(struct pool_entry_t* entry) {
	entry->in_use = 0;

	if(pthread_mutex_destroy(&entry->queue->mut) != 0) {
		fprintf(stderr, "Couldn't destroy queue mutex!\n");
		return -1;
	}


	if(pthread_cond_destroy(&entry->queue->cond) != 0) {
		fprintf(stderr, "Couldn't destroy queue condition!\n");
		return -1;
	}

	free(entry->queue);

	return 0;
}

int pop_from_queue(struct pool_entry_t* entry) {
	int val;
	val = entry->queue->buffer[entry->queue->fp++];

	if(entry->queue->fp > MAX_QUEUE_SIZE)
		entry->queue->fp = 0;

	return val;
}

int push_to_queue(struct pool_entry_t* entry, int value) {
	entry->queue->buffer[entry->queue->bp++] = value;

	if(entry->queue->bp > MAX_QUEUE_SIZE)
		entry->queue->bp = 0;

	return 0;
}

void* worker_thread(void* args) {
	//keep track of sum and print to screen
	struct pool_entry_t* e;
	e = (struct pool_entry_t*)(args + 0);

	char addr_str[INET_ADDRSTRLEN];
	short port;
	inet_ntop(AF_INET, (void*)(&e->addr), addr_str, INET_ADDRSTRLEN);
	port = ntohs(e->port);

	int data, num, sum;
	sum = 0;
	while(1) {
		pthread_cond_wait(&e->queue->cond, &e->queue->mut);
		data = pop_from_queue(e);
		num = htonl(data);
		sum += num;

		printf("woker thread %s:%hu recieved %d\n", addr_str, port, num);
		printf("woker thread %s:%hu sum %d\n", addr_str, port, sum);

	}
}

struct pool_entry_t* get_entry(struct pool_entry_t* pool, struct sockaddr_in* addr) {
	//yea ok lets hash
	//iterate through and compare
	//if no then create
	
	long long to_mod;
	to_mod = (*(int*)(&addr->sin_addr) << 8) | ntohs(addr->sin_port);

	struct pool_entry_t* e;
	int index, searched;


	searched = 0;
	index = (int)(to_mod + 1) % MAX_CONNECTIONS;

	while(1) {
		e = &(pool[index++]);	

		//loop around
		if(index > MAX_CONNECTIONS)
			index = 0;

		if(searched++ > MAX_CONNECTIONS - 1) { 
			e = 0;
			break;
		}

		//if not in use, it cant hold your entry
		if(!e->in_use) 
			continue;

		//if 
		if(e->addr == *(long*)(&addr->sin_addr) && e->port == (short)addr->sin_port)
			break;
	}

	if(e != 0) return e;


	index = (int)(to_mod + 1) % MAX_CONNECTIONS;
	while(pool[index].in_use) { index++; }

	e = &(pool[index]);
	printf("initializing entry @ %p\n", e);

	init_entry(e);
	e->addr = *(long *)(&addr->sin_addr);
	e->port = (short)addr->sin_port;

	pthread_create(&e->thread, 0, worker_thread, e);

	return e;
}




int main(int argc, char** argv) {
	int socket_fd;
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_fd < 0) {
		fprintf(stderr, "socket() failed!\n");
		return -1;
	}
	printf("created socket\n");

	struct sockaddr_in socket_info;
	memset(&socket_info, 0, sizeof(socket_info));

	socket_info.sin_family = AF_INET;
	socket_info.sin_addr.s_addr = htonl(INADDR_ANY);
	socket_info.sin_port = htons(9999);
	printf("configured info\n");

	if(bind(socket_fd, (struct sockaddr*)&socket_info, sizeof(socket_info)) != 0) {
		fprintf(stderr, "Couldn't bind socket");
		return -1;
	}

	printf("bound socket\n");

	struct pool_entry_t* pool;
	pool = malloc(sizeof(struct pool_entry_t) * MAX_CONNECTIONS);
	memset(pool, 0, sizeof(struct pool_entry_t) * MAX_CONNECTIONS);

	struct pool_entry_t* cur_entry;
	struct sockaddr_in incoming;
	socklen_t  incoming_size;

	while(1) {
		int raw_data;

		if(recvfrom(socket_fd, (void*)&raw_data, sizeof(raw_data), 0,
				(struct sockaddr *) &incoming, &incoming_size) < 0) {
			fprintf(stderr, "recvfrom() failed!");
			fprintf(stderr, "%s", strerror(errno));
			return -1;
		}

		cur_entry = get_entry(pool, &incoming);

		push_to_queue(cur_entry, raw_data);
		pthread_cond_signal(&cur_entry->queue->cond);
	}
}
