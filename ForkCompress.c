#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void child(char fp1[], char fp2[]){

    execl("./MyCompress", "./MyCompress", fp1, fp2, NULL);
    printf("child finished\n");

}

int main(int argc, char* argv[]){

    if(argc == 1){
        printf("No arguments passed\n");
    }
    else if(argc == 2){
        printf("incuficent arguemnts\n");
    }
    else if(argc == 3){

        pid_t pid;
        pid = fork();
        if(pid < 0){
            printf("Error: fork failed.\n");
        }
        else if(pid == 0){
            child(argv[1], argv[2]);
        }
        else{
            wait(&pid);
            printf("This is Parent\n");
        }
    }
    

    return 0;

}