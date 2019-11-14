#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int server = 0, valread;
    struct sockaddr_in6 address;
    char message[1024] = {0};
    char buffer[1024] = {0};
    printf("Enter a message: ");
    fgets(message, 1024, stdin);
    if((server = socket(AF_INET6, SOCK_STREAM, 0)) < 0){
        perror("Socket error");
        return -1;
    }

    address.sin6_family = AF_INET6;
    address.sin6_port = htons(5000);

    if(inet_pton(AF_INET6, "::1", &address.sin6_addr) <=0){
        perror("Invalid address/ address not supported");
        return -1;
    }
    if(connect(server, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("connection failed");
        return -1;
    }

    send(server, message, strlen(message), 0);
    valread = read(server, buffer, 1024);
    printf("From Server: %s\n", buffer);
    return 0;

}