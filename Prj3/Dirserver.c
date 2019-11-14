#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>

struct details
{
    int c;
    char *b;
};

void child1(char * arg[], int c){
    dup(c);
    execvp(arg[0], arg);
}

void child1pipe(char* arg[], int p[]){
    dup2(p[1], STDOUT_FILENO);
    close(p[0]);
    execvp(arg[0], arg);
    close(p[1]);
}

void child2(char* arg[], int p[], int c){
    dup2(p[0], STDIN_FILENO);
    close(p[1]);
    execvp(arg[0], arg);
    close(p[0]);
}

void execute_cmd(struct details info)
{
    int pipefds[2];
    pid_t pid;
    pid_t pid2;
    int *status;
    int c = info.c;
    char *buff = info.b;
    buff = strtok(buff, "\n");

    char *arg[10];
    char *argafter[10];
    char *p = strtok(buff, " ");
    int count = 0;
    int found = 0;
    if (pipe(pipefds) == -1)
    {
        perror("Pipe Failure");
        exit(EXIT_FAILURE);
    }

    while (p != NULL)
    {
        if (strcmp(p, "|") == 0)
        {
            arg[count] = NULL;
            found = 1;
            count = 0;
            p = strtok(NULL, " ");
        }
        if (found == 0)
        {
            arg[count] = p;
            count++;
            p = strtok(NULL, " ");
        }
        else if (found == 1)
        {
            argafter[count] = p;
            count++;
            p = strtok(NULL, " ");
        }
    }
    if (found == 0)
    {
        arg[count] = NULL;
    }
    else
    {
        argafter[count] = NULL;
    }
    pid = fork();
    if (pid == -1)
    {
        printf("error");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        if (found == 1)
        {
            child1pipe(arg, pipefds);
        }
        else
        {
            close(pipefds[0]);
            close(pipefds[1]);
            child1(arg, c);
        }
        exit(EXIT_SUCCESS);
    }
    else
    {
        if (found == 1)
        {
            pid2 = fork();
            if (pid2 == -1)
            {
                printf("pipe Error");
                exit(EXIT_FAILURE);
            }
            else if (pid2 == 0)
            {
                waitpid(pid, status, WUNTRACED);
                child2(argafter, pipefds, c);
                exit(EXIT_SUCCESS);
            }
            else
            {
                close(pipefds[0]);
                close(pipefds[1]);
                fflush(stdout);
                waitpid(pid2, status, WUNTRACED);
            }
        }
        else
        {
            close(pipefds[0]);
            close(pipefds[1]);
            fflush(stdout);
            waitpid(pid, status, WUNTRACED);
        }
    }

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

    for(int i = 0; i < 30; i++){
        client[i] = 0;
    }

    if ((server = socket(AF_INET6, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
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

                struct details info;
                valread = read(sd, buffer, 1024);
                info.c = sd;
                info.b = buffer;
                execute_cmd(info);
                strcpy(buffer, "");
                close(sd);
                client[i] = 0;
            }
        }
    }
    return 0;
}