#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

FILE * fp2;
int controller = 0;

struct Files{
    FILE * fp;
    char* file;
};

void *Combine(void * fps){
    struct Files *newfps = (struct Files*) fps; 
    FILE *chunk = newfps->fp;
    char *name = newfps->file;
    int number = name[6];
    char c = fgetc(chunk);
    int counter = 1;
    char saved;
    while(c != EOF){
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
        c = fgetc(chunk);
    }
    controller++;
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    FILE * fp1;

    if(argc == 1){
        printf("No arguments passed\n");
    }
    else if(argc == 2){
        printf("Insufficient argument\n");
    }
    else if(argc == 3){
        fp1 = fopen(argv[1], "r");
        if(fp1 == NULL){
            printf("Error Opening file\n");
            exit(EXIT_FAILURE);
        }
        fp2 = fopen(argv[2], "w+");
        if(fp2 == NULL){
            printf("Error creating file\n");
            exit(EXIT_FAILURE);
        }

        fseek(fp1, 0L, SEEK_END);
        long int res = ftell(fp1);
        rewind(fp1);
        res = res / 10;

        char files[10][260];
        for(int i =0; i < 10; i++){
            sprintf(files[i], "chunck%d", i);
        }
        FILE * fpmul[10];
        for(int i = 0; i < 10; i++){
            fpmul[i] = fopen(files[i], "w");
        }

        char c = fgetc(fp1);
        long int counter = 0;
        for(int i = 0; i < 10; i++){
            while((c != EOF) && (counter <= res)){
                putc(c, fpmul[i]);
                c = fgetc(fp1);
                counter++;
            }
            counter = 0;
        }

        fclose(fp1);
        for(int i = 0; i < 10; i++){
            fclose(fpmul[i]);
        }
        struct Files fs[10];
        for(int i = 0; i < 10; i++){
            fs[i].fp = fopen(files[i], "r");
            fs[i].file = files[i];
        }
        

        pthread_t thread_id[10];
	    for(int i = 0; i < 10; i++)
	        pthread_create(&thread_id[i], NULL, &Combine, &fs[i]);

	    for(int i = 0; i < 10; i++)
	        pthread_join(thread_id[i], NULL);


        for(int i = 0; i < 10; i++){
            fclose(fs[i].fp);
            remove(files[i]);
        }
    }
	return 0;
}