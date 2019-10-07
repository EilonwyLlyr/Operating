#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void child(char* arg[]){
    execvp(arg[0], arg);
}

int main(int argc, char *argv[]){
    char command[1024];
    char q[1024] = "exit";
    int *status;

    pid_t pid;
    //the start of the shell
    while (strcmp("exit\n", command) != 0){
        printf("enter:");
        //gets the input from the user and places it in command
        fgets(command, 1024, stdin);
        //fgets keeps the \n from the user input
        if(strcmp("exit\n", command) != 0){
            //removes the \n  from the input
            char *cmd = strtok(command, "\n");
            char *arg[2];
            //sets up the inputs from the execvp
            arg[0] = cmd;
            arg[1] = NULL;
            
            pid = fork();
            if (pid == -1){
                printf("Error");
            }
            else if(pid == 0){
                //child sends the command and executes it.
                child(arg);
                exit(0);
            }
            else{
                //waits for child
                waitpid(pid, status, WUNTRACED);
            }
        }
    }
    return 0;
}
