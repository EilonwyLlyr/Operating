#include<stdio.h>

void myCompress(FILE *out, int &sameCharCount, char lastCharacterRead){
	if(sameCharCount >=16){
		if(lastCharacterRead == '0'){
			putc('-', out);
			fprintf(out, "%d",sameCharCount);
			putc('-',out);
		}else if(lastCharacterRead == '1'){
			putc('+', out);
			fprintf(out, "%d",sameCharCount);
			putc('+',out);
		}
	}
}

int main (int argc, char **argv) {
	FILE *fp;
	int size=0,
	numZeros = 0,
	numOnes = 0,totalcount;
	int data[100];
	int count = 0;
	printf("%s \n", argv[1]);
	printf("%s \n", argv[2]);
	fp = fopen(argv[1], "r");

	if(fp == NULL){
		printf("not able to open file");
	}

	FILE *out;
	out = fopen(argv[2] ,"w");
	char ch;
	int i =0;
	char lastCharacterRead=' ';
	char currentChar;
	int sameCharCount = 0;

	while ((ch = fgetc(fp)) != EOF) {
		currentChar = ch;
		printf("%c" ,currentChar);

		if(currentChar != lastCharacterRead ){
			if(sameCharCount >=16){
			myCompress(out,sameCharCount,lastCharacterRead);
			}

			else{
				for(int i=0; i<sameCharCount; i++){
					putc(lastCharacterRead,out);
				}
			}

			sameCharCount = 0;
		}

		sameCharCount++;
		lastCharacterRead = currentChar;   
	}

	if(sameCharCount !=0){
		if(sameCharCount >=16){
			myCompress(out,sameCharCount,lastCharacterRead);
		}

		else{
			for(int i=0; i<sameCharCount; i++){
				putc(lastCharacterRead,out);
			}
		}
	}

	fclose(out);
}
