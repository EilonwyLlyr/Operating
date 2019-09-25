
#include <stdio.h>
#include <stdlib.h>

void compressFile(FILE *fp1, FILE *fp2) {
  char saved, c;
  int counter = 1;

  c = fgetc(fp1);

  while (c != EOF) {
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
    c = fgetc(fp1);
  }
  printf("Contents has been compressed to file\n");
}

int main(int argc, char* argv[]) {
    FILE *fp1, *fp2;
    if(argc == 1){
        printf("No arguments passed\n");
    }
    else if(argc == 2){
        printf("incuficent arguemnts\n");
    }
    else if(argc == 3){

    fp1 = fopen(argv[1], "r");

    if (fp1 == NULL) {
      printf("Program will now exit.\n");
      exit(EXIT_FAILURE);
    }

    fp2 = fopen(argv[2], "w+");

    if (fp2 == NULL) {
      fclose(fp1);
      printf("Program will now exit.\n");
      exit(EXIT_FAILURE);
    }

    compressFile(fp1, fp2);

    fclose(fp1);
    fclose(fp2);
    }

    return 0;
}
