
#include <stdio.h>
#include <stdlib.h>

void compressFile(FILE *fptr1, FILE *fptr2) {
  char saved, c;
  int counter = 1;

  c = fgetc(fptr1);

  while (c != EOF) {
    if (saved == '\n') {
      putc('\n', fptr2);
    }

    if (saved == ' ') {
      putc(' ', fptr2);
    }

    if (saved == c && (saved == '0' || saved == '1')) {
      counter++;
    }

    else {
      if (counter >= 16) {
        if (saved == '0') {
          putc('-',fptr2);
          fprintf(fptr2,"%d",counter);
          putc('-',fptr2);
        }
        else if (saved == '1') {
          putc('+',fptr2);
          fprintf(fptr2,"%d",counter);
          putc('+',fptr2);
        }
      }
      else {
        while (counter > 0) {
            if (saved == '0') {
              putc('0',fptr2);
            }
            else if (saved == '1') {
              putc('1',fptr2);
            }
            counter--;
        }
      }
      counter = 1;
    }
    saved = c;
    c = fgetc(fptr1);
  }

  printf("Contents has been compressed to file\n");

}

int main(int argc, char* argv[]) {
    FILE *fptr1, *fptr2;
    char input[20], output[20];

    printf("Enter name of file to compress:\n");
    //gets(input);

    fptr1 = fopen("source", "r");

    if (fptr1 == NULL) {
      printf("Program will now exit.\n");
      exit(EXIT_FAILURE);
    }

    printf("Enter name of target file:\n");
    //gets(output);

    fptr2 = fopen("destination", "w");

    if (fptr2 == NULL) {
      fclose(fptr1);
      printf("Program will now exit.\n");
      exit(EXIT_FAILURE);
    }

    compressFile(fptr1, fptr2);

    fclose(fptr1);
    fclose(fptr2);

    return 0;
}
