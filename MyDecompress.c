
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void compressFile(FILE *fptr1, FILE *fptr2) {
  char saved, c;
  int counter = 0;

  c = fgetc(fptr1);

  while (c != EOF) {
    if (c == '\n') {
      putc('\n', fptr2);
    }

    else if (c == ' ') {
      putc(' ', fptr2);
    }

    else if (c == '-') {
      saved = '0';
      c = fgetc(fptr1);
      char i[32] = "";
      int loc = 0;
      while(c != '-'){
          i[loc] = c;
          loc++;
          c = fgetc(fptr1);
      }
      counter = atoi(i);
      while(counter > 0){
          putc(saved, fptr2);
          counter--;
      }
    }
    else if (c == '+'){
        saved = '1';
        c = fgetc(fptr1);
        char i[32] = "";
        int loc = 0;
        while(c != '+'){
            i[loc] = c;
            loc++;
            c = fgetc(fptr1);
        }
        counter = atoi(i);
        while(counter > 0){
            putc(saved, fptr2);
            counter--;
        }
    }

    else {
        putc(c, fptr2);
    }
    c = fgetc(fptr1);
  }

  printf("Contents has been decompressed to file\n");

}

int main(int argc, char* argv[]) {
    FILE *fptr1, *fptr2;
    if(argc == 1){
        printf("No arguments passed\n");
    }
    else if(argc == 2){
        printf("incuficent arguemnts\n");
    }
    else if(argc == 3){

    fptr1 = fopen(argv[1], "r");

    if (fptr1 == NULL) {
      printf("Program will now exit.\n");
      exit(EXIT_FAILURE);
    }

    fptr2 = fopen(argv[2], "w+");

    if (fptr2 == NULL) {
      fclose(fptr1);
      printf("Program will now exit.\n");
      exit(EXIT_FAILURE);
    }

    compressFile(fptr1, fptr2);

    fclose(fptr1);
    fclose(fptr2);
    }

    return 0;
}
