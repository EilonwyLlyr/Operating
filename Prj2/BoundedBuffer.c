#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#define MAX 8

sem_t bufsize;
pthread_mutex_t buflock;

void* Producer(){

}
void* Consumer(){

}
int main(int argc, char *argv[])
{
    char buffer[MAX];

    pthread_t p;
    pthread_t c;
    sem_init(&bufsize, 0, MAX);
    pthread_mutex_init(&buflock, NULL);

    pthread_create(&p, NULL, &Producer, NULL);
    pthread_create(&c, NULL, &Consumer, NULL);

    pthread_join(Consumer, NULL);
    pthread_join(Producer, NULL);

    printf("END\n");

    return 0;
}