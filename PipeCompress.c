#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void compressFile(char readbuff[], FILE *fp2) {
  char saved, c;
  int counter = 1;

  for(int i = 0; i < strlen(readbuff); i++){
    c = readbuff[i];
    if (saved == '\n') {
      putc('\n', fp2);
    }

    if (saved == ' ') {
      putc(' ', fp2);
    }

    if (saved == c && (saved == '0' || saved == '1')) {
      counter++;
    }

    else {
      if (counter >= 16) {
        if (saved == '0') {
          putc('-',fp2);
          fprintf(fp2,"%d",counter);
          putc('-',fp2);
        }
        else if (saved == '1') {
          putc('+',fp2);
          fprintf(fp2,"%d",counter);
          putc('+',fp2);
        }
      }
      else {
        while (counter > 0) {
            if (saved == '0') {
              putc('0',fp2);
            }
            else if (saved == '1') {
              putc('1',fp2);
            }
            counter--;
        }
      }
      counter = 1;
    }
    saved = c;
  }
  printf("Contents has been compressed to file\n");
}


void child_one(char source[], int p[]){
    FILE *fp1;
    fp1 = fopen(source, "r");
    int counter = 1;
    if (fp1 == NULL) {
      printf("Program will now exit.\n");
      exit(EXIT_FAILURE);
    }

    char c;
    c = fgetc(fp1);
    while(c != EOF){
      c = fgetc(fp1);
      counter++;
    }
    rewind(fp1);
    char readbuff[counter];
    int index = 0;
    c = fgetc(fp1);
    while(c != EOF){
      readbuff[index] = c;
      c = fgetc(fp1);
      index++;
    }

    close(p[0]);

    write(p[1], readbuff, counter);

    close(p[1]);

    fclose(fp1);
    printf("child one done\n");
    exit(0);
}

void child_two(char destination[], int p[]){
    FILE *fp2;
    fp2 = fopen(destination, "w+");

    char writebuff[1024];
    
    if (fp2 == NULL) {
      printf("Program will now exit.\n");
      exit(EXIT_FAILURE);
    }
    
    close(p[1]);

    read(p[0], &writebuff, sizeof(writebuff));

    compressFile(writebuff, fp2);
    close(p[0]);
    fclose(fp2);
    printf("child two done\n");
    exit(0);
}

int main(int argc, char* argv[]){

    int status;
    
    if(argc == 1){
        printf("No arguments passed\n");
    }
    else if(argc == 2){
        printf("incuficent arguemnts\n");
    }
    else if(argc == 3){

        pid_t pid;
        pid_t pid2;
        int p[2];
        if(pipe(p) < 0){
            exit(1);
        }

        pid = fork();
        if(pid < 0){
            printf("Error: fork failed.\n");
        }
        else if(pid == 0){
            child_one(argv[1], p);
        }
        
        pid2 = fork();
        if(pid2 < 0){
          printf("Error: fork failed.\n");
        }
        else if(pid2 == 0){
          wait(&pid);
          child_two(argv[2], p);
        }

        close(p[0]);
        close(p[1]);

        waitpid(pid2, &status, WUNTRACED);
        printf("this is parent\n");
        }
    return 0;
}