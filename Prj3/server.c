#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>

struct details
{
    int c;
    char *b;
};

void *reverse(struct details *info)
{
    int c = (*info).c;
    char *buff = (*info).b;
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

    send(c, buff, strlen(buff), 0);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int server;
    int client[30];
    int new_client;
    int valread;
    struct sockaddr_in6 address;
    int addrlen = sizeof(address);
    int opt = 1;
    char buffer[1024] = {0};
    int sd, max_sd, activity;
    fd_set readfds;

    for (int i = 0; i < 30; i++)
    {
        client[i] = 0;
    }

    if ((server = socket(AF_INET6, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt failed");
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

    if (listen(server, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    for (;;)
    {
        FD_ZERO(&readfds);

        FD_SET(server, &readfds);
        max_sd = server;
        for (int i = 0; i < 30; i++)
        {
            sd = client[i];
            if (sd > 0)
                FD_SET(sd, &readfds);

            if (sd > max_sd)
                max_sd = sd;
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            printf("select error");
        }

        if (FD_ISSET(server, &readfds))
        {
            if ((new_client = accept(server, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
            {
                perror("accept failed");
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < 30; i++)
            {
                if (client[i] == 0)
                {
                    client[i] = new_client;
                    break;
                }
            }
        }

        for (int i = 0; i < 30; i++)
        {
            sd = client[i];
            if (FD_ISSET(sd, &readfds))
            {

                pthread_t c;
                struct details info;
                valread = read(sd, buffer, 1024);
                info.c = client[i];
                info.b = buffer;
                pthread_create(&c, NULL, (void *)reverse, &info);
                pthread_join(c, NULL);
                strcpy(buffer, "");
                close(sd);
                client[i] = 0;
            }
        }
    }
    return 0;
}