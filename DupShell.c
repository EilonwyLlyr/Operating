#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

void child1(char *arg[]){
    //simple execvp
    execvp(arg[0], arg);
}

void child1pipe(char * arg[], int p[]){
    //gets the output from execvp and sends it thorugh the pipe using dup2
    dup2(p[1], STDOUT_FILENO);
    close(p[0]);
    execvp(arg[0], arg);
    close(p[1]);
}
void child2(char *arg[], int p[]){
    //recives the output from the read end of the pipe and does the execvp command
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
        //creates a pipe to send from before | to after
        int pipefds[2];
        if(pipe(pipefds) == -1){
            perror("pipe failure");
            exit(EXIT_FAILURE);
        }

        if(strcmp("exit\n", command) != 0){
            strtok(command, "\n");
            //creates two pointer arrays, one for the inputs before | and one after
            char * arg[10];
            char * argafter[10];
            char *p = strtok(command, " ");
            int count = 0;
            //boolean to check it there was a | within the input
            int found = 0;

            while(p != NULL){
                //checks to see if | was found. makes the last in the firt array
                //makes the bool 1 and resets the counter to 0
                //skips to the next input seperated by space
                if(strcmp(p, "|") == 0){
                    arg[count] = NULL;
                    found = 1;
                    count = 0;
                    p = strtok(NULL, " ");
                }
                //gets placed in the first array
                if(found == 0){
                    arg[count] = p;
                    count++;
                    p = strtok(NULL, " ");
                }
                //gets placed in the second array
                else if (found == 1){
                    argafter[count] = p;
                    count++;
                    p = strtok(NULL, " ");
                }
            }
            //if | was not found makes the last in the first array NULL
            if(found == 0){
                arg[count] = NULL;
            }
            //if | was found makes the last in the second array NULL
            else{
                argafter[count] = NULL;
            }

            //creates first child
            pid = fork();
            if (pid == -1){
                printf("Error");
            }
            else if(pid == 0){
                //determins if there will be a need to send information through the pipe
                if(found == 1){
                    //sends the first array and pipe to child
                    child1pipe(arg, pipefds);
                }
                else{
                    //closes the pipe and sends first array to child
                    close(pipefds[0]);
                    close(pipefds[1]);
                    child1(arg);
                }
                exit(EXIT_SUCCESS);
            }
            else{
                //if there is a need to send information, creates a second child
                if(found == 1){
                    pid2 = fork();
                    if (pid2 == -1){
                        printf("Error");
                    }
                    else if(pid2 == 0){
                        //waits for child 1 and sends second array to child 2
                        waitpid(pid, status, WUNTRACED);
                        child2(argafter, pipefds);
                        exit(EXIT_SUCCESS);
                    }
                    else{
                        //closes the pipe in the parent and waits for child 2
                        close(pipefds[0]);
                        close(pipefds[1]);
                        fflush(stdout);
                        waitpid(pid2, status, WUNTRACED);
                    }
                }
                //if there is no need to send information closes the pipe and waits for child 1
                else{
                    close(pipefds[0]);
                    close(pipefds[1]);
                    fflush(stdout);
                    waitpid(pid, status, WUNTRACED);
                }
            }
        }
    }
    return 0;
}
