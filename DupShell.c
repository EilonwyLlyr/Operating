#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

void child1(char *arg[]){
    execvp(arg[0], arg);
}

void child1pipe(char * arg[], int p[]){
    dup2(p[1], STDOUT_FILENO);
    close(p[0]);
    execvp(arg[0], arg);
    close(p[1]);
}
void child2(char *arg[], int p[]){
    dup2(p[0], STDIN_FILENO);
    close(p[1]);
    execvp(arg[0], arg);
    close(p[0]);
}


int main(int argc, char *argv[]){
    char command[1024];
    char q[1024] = "exit";
    int *status;
    pid_t pid;
    pid_t pid2;

    while (strcmp("exit\n", command) != 0){
        printf("enter:");

        fgets(command, 1024, stdin);
        int pipefds[2];
        if(pipe(pipefds) == -1){
            perror("pipe failure");
            exit(EXIT_FAILURE);
        }

        if(strcmp("exit\n", command) != 0){
            strtok(command, "\n");
            char * arg[10];
            char * argafter[10];
            char *p = strtok(command, " ");
            int count = 0;
            int found = 0;

            while(p != NULL){
                if(strcmp(p, "|") == 0){
                    arg[count] = NULL;
                    found = 1;
                    count = 0;
                    p = strtok(NULL, " ");
                }
                if(found == 0){
                    arg[count] = p;
                    count++;
                    p = strtok(NULL, " ");
                }
                else if (found == 1){
                    argafter[count] = p;
                    count++;
                    p = strtok(NULL, " ");
                }
            }
            if(found == 0){
                arg[count] = NULL;
            }
            else{
                argafter[count] = NULL;
            }

            pid = fork();
            if (pid == -1){
                printf("Error");
            }
            else if(pid == 0){
                if(found == 1){
                    child1pipe(arg, pipefds);
                }
                else{
                    child1(arg);
                }
                exit(EXIT_SUCCESS);
            }
            else{
                if(found == 1){
                    pid2 = fork();
                    if (pid2 == -1){
                        printf("Error");
                    }
                    else if(pid2 == 0){
                        waitpid(pid, status, WUNTRACED);
                        child2(argafter, pipefds);
                        exit(EXIT_SUCCESS);
                    }
                    else{
                        close(pipefds[0]);
                        close(pipefds[1]);
                        fflush(stdout);
                        waitpid(pid2, status, WUNTRACED);
                    }
                }
                else{
                    close(pipefds[0]);
                    close(pipefds[1]);
                    fflush(stdout);
                    waitpid(pid, status, WUNTRACED);
                }
            }
        }
    }
}