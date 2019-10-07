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
    //gets the length of the file
    while(c != EOF){
      c = fgetc(fp1);
      counter++;
    }
    //sends pointer to the start of the file
    rewind(fp1);
    //creates a buffer the lenth of the file
    char readbuff[counter];
    int index = 0;
    c = fgetc(fp1);
    //sends the information from the file into the buffer
    while(c != EOF){
      readbuff[index] = c;
      c = fgetc(fp1);
      index++;
    }
    //closes the reading end
    close(p[0]);
    //writes into the pipe
    write(p[1], readbuff, counter);
    //closes the writing end
    close(p[1]);

    fclose(fp1);
    printf("child one done\n");
    exit(0);
}

void child_two(char destination[], int p[]){
    FILE *fp2;
    fp2 = fopen(destination, "w+");
    //creates a buffer that can only hold 1024 characters
    char writebuff[1024];
    
    if (fp2 == NULL) {
      printf("Program will now exit.\n");
      exit(EXIT_FAILURE);
    }
    //closes the writing end
    close(p[1]);
    //reads from the pipe and saves it into the writebuff
    read(p[0], &writebuff, sizeof(writebuff));
    //compresses the file similar to MyCompress
    compressFile(writebuff, fp2);
    //closes the reading end
    close(p[0]);
    fclose(fp2);
    printf("child two done\n");
    exit(0);
}

int main(int argc, char* argv[]){

    int status;
    //checks to see if argumenrts are passes
    if(argc == 1){
        printf("No arguments passed\n");
    }
    else if(argc == 2){
        printf("incuficent arguemnts\n");
    }
    else if(argc == 3){
      //creates two childs and a pipe
        pid_t pid;
        pid_t pid2;
        int p[2];
        if(pipe(p) < 0){
            exit(1);
        }
        //first child is created
        pid = fork();
        if(pid < 0){
            printf("Error: fork failed.\n");
        }
        else if(pid == 0){
          //sends the source file name and pipe 
            child_one(argv[1], p);
        }
        //second child is created
        pid2 = fork();
        if(pid2 < 0){
          printf("Error: fork failed.\n");
        }
        else if(pid2 == 0){
          //waits for child 1 to finish
          wait(&pid);
          //send the output file name and pipe
          child_two(argv[2], p);
        }
        //closes the pipe on the parents end.
        close(p[0]);
        close(p[1]);
        //waits for child 2 to finish
        waitpid(pid2, &status, WUNTRACED);
        printf("this is parent\n");
        }
    return 0;
}
