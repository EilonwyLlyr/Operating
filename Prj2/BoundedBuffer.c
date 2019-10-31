#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#define MAX 8

sem_t bufsize;
pthread_mutex_t buflock;

void* Producer(){
    item next_produced;
    while (true) {
        /* produce an item in next produced */
        while(((in + 1) % MAX) == out)
            ; /* do nothing */
        buffer[in] = next_produced;
        in = (in + 1) % MAX;
    }
}
void* Consumer(){
    item next_consumed;
    while (true) {
            while (in == out)
                    ; /* do nothing */
            next_consumed = buffer[out];
            out = (out + 1) % MAX;

            /* consume the item in next consumed */
    }
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
