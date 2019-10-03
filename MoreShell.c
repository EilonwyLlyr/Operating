#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void child(char command[]){
    strtok(command, "\n");

    char *arg[5];
    char *p = strtok(command, " ");
    int count = 0;
    while(p != NULL){
        arg[count] = p;
        count++;
        p = strtok(NULL, " ");
    }
    arg[count] = NULL;
    execvp(arg[0], arg);
}

int main(int argc, char *argv[]){
    char command[1024];
    char q[1024] = "exit";
    int *status;

    pid_t pid;
    while (strcmp("exit\n", command) != 0){
        printf("enter:");

        fgets(command, 1024, stdin);
        if(strcmp("exit\n", command) != 0){
            pid = fork();
            if (pid == -1){
                printf("Error");
            }
            else if(pid == 0){
                child(command);
                exit(0);
            }
            else{
                waitpid(pid, status, WUNTRACED);
            }
        }
    }
}