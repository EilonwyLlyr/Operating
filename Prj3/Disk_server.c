#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#define BLOCKSIZE 128;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int sp;
int track;
int sector;
int blocks;
FILE *fptr = NULL;

void *reverse(void *arg)
{
    char buffer[1024] = {0};
    pthread_mutex_lock(&lock);
    int newSocket = *((int *)arg);
    int copy = dup2(newSocket, 1);
    if (read(newSocket, buffer, 1024) <= 0)
    {
        pthread_mutex_unlock(&lock);
        close(newSocket);
        pthread_exit(NULL);
    }
    char *buff = buffer;
    if (strcmp(buff, "I\n") == 0)
    {
        char tracks[8];
        char sectors[8];
        sprintf(tracks, "%d", track);
        sprintf(sectors, "%d", sector);
        char message[1024] = {0};
        strcat(message, "Tracks: ");
        strcat(message, tracks);
        strcat(message, ", Sectors: ");
        strcat(message, sectors);
        write(copy, message, 1024);
        pthread_mutex_unlock(&lock);
        close(newSocket);
        pthread_exit(NULL);
    }
    char *token = strtok(buff, " ");
    printf("%s\n", token);
    char *elem[10];
    int i = 0;
    while (i < 5)
    {
        elem[i] = token;
        i++;
        if (i < 4)
            token = strtok(NULL, " ");
        if (i == 4)
            token = strtok(NULL, "\n");
    }

    if (strcmp(elem[0], "R") == 0)
    {
        int tnum = atoi(elem[1]);
        if (tnum < track)
        {
            int snum = atoi(elem[2]);
            if (snum < sector)
            {
                char mess[128] = {0};
                int bnum = (tnum * track + snum) * 128;
                fptr = fopen("disk.txt", "r");
                int index = 0;
                while (index < bnum)
                {
                    fgetc(fptr);
                    index++;
                }
                for (int i = 0; i < 128; i++)
                {
                    mess[i] = getc(fptr);
                }
                write(copy, mess, 128);
                fclose(fptr);
                strcpy(buffer, "");
                pthread_mutex_unlock(&lock);
                close(newSocket);
                pthread_exit(NULL);
            }
            else
            {
                write(copy, "Invalid command", 16);
                strcpy(buffer, "");
                pthread_mutex_unlock(&lock);
                close(newSocket);
                pthread_exit(NULL);
            }
        }
        else
        {
            write(copy, "Invalid command", 16);
            strcpy(buffer, "");
            pthread_mutex_unlock(&lock);
            close(newSocket);
            pthread_exit(NULL);
        }
    }
    else if (strcmp(elem[0], "W") == 0)
    {
        int tnum = atoi(elem[1]);
        if (tnum < track)
        {
            int snum = atoi(elem[2]);
            if (snum < sector)
            {
                int length = atoi(elem[3]);
                if (length < 128 && strlen(elem[4]) < 128)
                {
                    int bnum = (tnum * track + snum) * 128;
                    fptr = fopen("disk.txt", "r+");
                    int index = 0;
                    while (index < bnum)
                    {
                        fgetc(fptr);
                        index++;
                    }
                    fputs(elem[4], fptr);

                    write(copy, "Message done", 128);
                    fclose(fptr);
                    strcpy(buffer, "");
                    pthread_mutex_unlock(&lock);
                    close(newSocket);
                    pthread_exit(NULL);
                }
                else
                {
                    write(copy, "Invalid command", 16);
                    strcpy(buffer, "");
                    pthread_mutex_unlock(&lock);
                    close(newSocket);
                    pthread_exit(NULL);
                }
            }
            else
            {
                write(copy, "Invalid command", 16);
                strcpy(buffer, "");
                pthread_mutex_unlock(&lock);
                close(newSocket);
                pthread_exit(NULL);
            }
        }
        else
        {
            write(copy, "Invalid command", 16);
            strcpy(buffer, "");
            pthread_mutex_unlock(&lock);
            close(newSocket);
            pthread_exit(NULL);
        }
    }
    else
    {
        write(copy, "Invalid command", 16);
        strcpy(buffer, "");
        pthread_mutex_unlock(&lock);
        close(newSocket);
        pthread_exit(NULL);
    }
}

int main(int argc, char *argv[])
{
    int server, client, valread;
    struct sockaddr_in6 address;
    struct sockaddr_storage storage;
    socklen_t addr_size;
    int opt = 1;
    if (argc == 4)
    {
        sp = atoi(argv[1]);
        track = atoi(argv[2]);
        sector = atoi(argv[3]);
        blocks = track * sector;
        fptr = fopen("disk.txt", "w");
        int filesize = blocks * BLOCKSIZE;
        for (int i = 0; i < filesize; i++)
        {
            fprintf(fptr, "%d", 0);
        }
        fclose(fptr);
        if ((server = socket(AF_INET6, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }
        if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        {
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
        while (1)
        {

            addr_size = sizeof(storage);
            client = accept(server, (struct sockaddr *)&storage, &addr_size);
            pthread_create(&tid[i], NULL, reverse, &client);
            i++;
            if (i >= 50)
            {
                i = 0;
                while (i < 50)
                {
                    pthread_join(tid[i++], NULL);
                }
                i = 0;
            }
        }
    }
    else
    {
        printf("Insuficiant arguments, need 3 parameters\n");
    }

    return 0;
}