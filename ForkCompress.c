#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void child(char fp1[], char fp2[]){
    //executes MyCompress and sends the arguments from the command line to it.
    execl("./MyCompress", "./MyCompress", fp1, fp2, NULL);
    printf("child finished\n");
    exit(1);
}

int main(int argc, char* argv[]){
    //checks to see if arguments are passes in the command line
    if(argc == 1){
        printf("No arguments passed\n");
    }
    else if(argc == 2){
        printf("incuficent arguemnts\n");
    }
    else if(argc == 3){
        //creates a pid and forks
        pid_t pid;
        pid = fork();
        //checks for erros
        if(pid < 0){
            printf("Error: fork failed.\n");
            exit(1);
        }
        else if(pid == 0){
            //child sends information into child function
            child(argv[1], argv[2]);
        }
        else{
            //parent waits for child
            wait(&pid);
            printf("This is Parent\n");
        }
    }
    

    return 0;

}
