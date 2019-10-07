#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void child(char *arg[]){
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
            strtok(command, "\n");
            //creates a pointer array that can take up to 10 strings.
            char * arg[10];
            char *p = strtok(command, " ");
            int count = 0;
            //parses all the input by spaces and sends it to the pointer array
            while(p != NULL){
                arg[count] = p;
                count++;
                p = strtok(NULL, " ");
            }
            //places NULL at the end 
            arg[count] = NULL;
        

            pid = fork();
            if (pid == -1){
                printf("Error");
            }
            else if(pid == 0){
                //child sends the pointer array and exevutes the command
                child(arg);
                exit(0);
            }
            else{
                waitpid(pid, status, WUNTRACED);
            }
        }
    }
    return 0;
}
