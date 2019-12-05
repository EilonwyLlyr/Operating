#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>

char buffer[1024] = {0};
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *reverse(void *arg)
{
    pthread_mutex_lock(&lock);
    int newSocket = *((int*)arg);
    if(read(newSocket, buffer, 1024) <= 0){
        pthread_mutex_unlock(&lock);
        close(newSocket);
        pthread_exit(NULL);
    }
    char *buff = buffer;
    buff = strtok(buff, "\n");
    int start = 0;
    int end = strlen(buff);
    end--;
    char temp;
    while (start < end)
    {
        temp = buff[start];
        buff[start] = buff[end];
        buff[end] = temp;
        start++;
        end--;
    }

    send(newSocket, buff, strlen(buff), 0);
    strcpy(buffer, "");
    pthread_mutex_unlock(&lock);
    close(newSocket);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int server, client, valread;
    struct sockaddr_in6 address;
    struct sockaddr_storage storage;
    socklen_t addr_size;
    int opt = 1;


    if ((server = socket(AF_INET6, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if(setsockopt(server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin6_family = AF_INET6;
    address.sin6_addr = in6addr_any;
    address.sin6_port = htons(5000);

    if (bind(server, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server, 5) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    pthread_t tid[60];
    int i = 0;
    while(1){
        addr_size = sizeof(storage);
        client = accept(server, (struct sockaddr *) &storage, &addr_size);
        pthread_create(&tid[i], NULL, reverse, &client);
        if(i >= 50){
            i = 0;
            while(i < 50){
                pthread_join(tid[i++], NULL);
            }
            i = 0;
        }
    }

    return 0;
}