#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>

int index_file = -1;
char* file_buffer = NULL;


void send_file(int socket) {
    
    int ret = -1;
    char head_buffer[1024] = {0};
    sprintf(head_buffer, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n", strlen(file_buffer));
    ret = write(socket, head_buffer, strlen(head_buffer));
    if(ret != strlen(head_buffer)) {
        printf("send header error");
        exit(-1);
    }
    ret = write(socket, file_buffer, strlen(file_buffer));
    if(ret != strlen(file_buffer)) {
        printf("send content error");
        exit(-1);
    }
}

void read_file() {

    int file_size = -1, ret = -1;
    struct stat st;
    memset(&st, 0, sizeof(st));
    ret = fstat(index_file, &st);
    if(ret != 0) {
        perror("fstat index file");
        exit(-1);
    }
    file_size = st.st_size;
    file_buffer = malloc(file_size);
    ret = read(index_file, file_buffer, file_size);
    if(ret != file_size) {
        printf("read error");
        exit(-1);
    }
}

void* server_listen(void * args) {
    int fd = (int)(long int)args;
    int total = 0;
    int socket = -1;
    while(1) {
        socket = accept(fd, NULL, NULL);
        printf("thread %ld receive a client, totla %d\n", pthread_self(), ++total);
        send_file(socket);
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
    addr.sin_addr.s_addr = inet_addr("10.0.2.15");

    index_file = open("index.html", O_RDONLY);
    if(index_file == -1) {
        perror("open index file");
        exit(-1);
    }
    read_file();

    for(; i < port_num; i++) {

        pthread_t thread;
        server_fd = socket(PF_INET, SOCK_STREAM, 0);
        if(server_fd == -1) {
            perror("socket server");
            exit(-1);
        }
        addr.sin_port = htons(start_port + i);
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
