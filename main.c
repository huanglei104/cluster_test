#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>


void* server_listen(void * args) {
	int fd = (int)(long int)args;
	int ret = -1;
	while(1) {
		ret = accept(fd, NULL, NULL);
		printf("a client connect\n");
	}

	return NULL;
}

int main(int argc, char *argv[]) {

	int port_num = 0, start_port = 0, i = 0;
	int server_fd = -1, ret = -1;
	struct sockaddr_in addr;

	if(argc != 3) {
		printf("usage: ./main port_count start_port\n");
		exit(-1);
	}
	port_num = atoi(argv[1]);
	start_port = atoi(argv[2]);
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = PF_INET;
	addr.sin_addr.s_addr = inet_addr("192.168.31.166");
	server_fd = socket(PF_INET, SOCK_STREAM, 0);
	if(server_fd == -1) {
		perror("socket server");
		exit(-1);
	}

	for(; i < port_num; i++) {

		pthread_t thread;
		addr.sin_port = htonl(start_port + i);
		ret = bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
		if(ret == -1) {
			perror("server socket bind");
			exit(-1);
		}
		ret = listen(server_fd, 5);
		if(ret == -1) {
			perror("server socket listen");
			exit(-1);
		}
		pthread_create(&thread, NULL, server_listen, (void*)(long int)server_fd);

	}
	pthread_exit(0);

	return 0;
}
