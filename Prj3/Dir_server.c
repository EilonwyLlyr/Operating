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
#include <pthread.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
char buffer[1024] = {0};

void child1(char * arg[], int c){
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

void *execute_cmd(void * info)
{
    pthread_mutex_lock(&lock);
    int newSocket = *((int*)info);
    dup2(newSocket, 1);
    if(read(newSocket, buffer, 1024) <= 0){
        pthread_mutex_unlock(&lock);
        close(newSocket);
        pthread_exit(NULL);
    }
    int pipefds[2];
    pid_t pid;
    pid_t pid2;
    int *status;
    char *buff = buffer;
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
            child1(arg, newSocket);
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
                child2(argafter, pipefds, newSocket);
                exit(EXIT_SUCCESS);
            }
            else
            {
                close(pipefds[0]);
                close(pipefds[1]);
                fflush(stdout);
                waitpid(pid2, status, WUNTRACED);
                pthread_mutex_unlock(&lock);
                close(newSocket);
                pthread_exit(NULL);
            }
        }
        else
        {
            close(pipefds[0]);
            close(pipefds[1]);
            fflush(stdout);
            waitpid(pid, status, WUNTRACED);
            pthread_mutex_unlock(&lock);
            close(newSocket);
            pthread_exit(NULL);
        }
    }

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
        pthread_create(&tid[i], NULL, execute_cmd, &client);
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