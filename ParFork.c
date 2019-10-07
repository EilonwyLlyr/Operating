#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// the output file where the forks will write into
FILE * fp2;

//a way to pass both the file and file name to the fork
struct Files{
    FILE * fp;
    char* file;
};

void *Combine(void * fps){

    struct Files *newfps = (struct Files*) fps;
    //asigns the file and file name
    FILE *chunk = newfps->fp;
    char *name = newfps->file;
    int number = name[6];
    
    //the same as MyCompress
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
}

int main(int argc, char *argv[]) {
    FILE * fp1;
    //checks to see if arguments are passed in the command line
    if(argc == 1){
        printf("No arguments passed\n");
    }
    else if(argc == 2){
        printf("Insufficient argument\n");
    }
    else if(argc == 3){
      //two files, one file opens the source, and the second is to write to the destination
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
        //gets the size of the file
        fseek(fp1, 0L, SEEK_END);
        long int res = ftell(fp1);
        rewind(fp1);
        res = res / 10;

        //creates 10 temporary file names that are the chuncks
        char files[10][260];
        for(int i =0; i < 10; i++){
            sprintf(files[i], "chunck%d", i);
        }
        //creates 10 temporary files to write into that are the chuncks
        FILE * fpmul[10];
        for(int i = 0; i < 10; i++){
            fpmul[i] = fopen(files[i], "w");
        }
        //chunks the source file into the 10 temporary files
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
        //closes the source file and the temporary files.
        fclose(fp1);
        for(int i = 0; i < 10; i++){
            fclose(fpmul[i]);
        }
        //creates an array of struct Files that were defined to open
        //the temporary files again as read mode
        struct Files fs[10];
        for(int i = 0; i < 10; i++){
            fs[i].fp = fopen(files[i], "r");
            fs[i].file = files[i];
        }
	
	pid_t pids[10];

	//creates the 10 forks and sends the struct File acordingly
	for(int i = 0; i < 10; i++) {
		pids[i] = fork();

		if (pids[i] == 0){
			Combine(&fs[i]);
			exit(1);
		}
	}
        //closes the temporary files and deletes them
        for(int i = 0; i < 10; i++){
            fclose(fs[i].fp);
            remove(files[i]);
        }
    }
    //errors: only the last chunck seems to get compressed, even though 
    //all the other chuncks are separated as they should be
	return 0;
}
