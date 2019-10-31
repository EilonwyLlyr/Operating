#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#define MAX 8

sem_t bufsize, full;
pthread_mutex_t buflock;

int count = 0;
void *Producer(char *buf)
{
    srand(time(0));
    int in = 0;
    pthread_mutex_lock(&buflock);
    for (int i = 0; i < MAX; i++)
    {
        sem_wait(&bufsize);
        buf[in] = 'A' + (rand() % 26);
        printf("placed %c in the buffer\n", buf[in]);
        in = (in + 1) % MAX;
        count++;
        if (count == MAX)
            printf("buffer is full\n");
        sem_post(&full);
    }
    pthread_mutex_unlock(&buflock);

    for (int i = 0; i < 20; i++)
    {
        sem_wait(&bufsize);
        buf[in] = 'A' + (rand() % 26);
        printf("produced %c in the buffer\n", buf[in]);
        in = (in + 1) % MAX;
        count++;
        if (count == MAX)
            printf("buffer is full\n");
        sem_post(&full);
    }

    pthread_exit(NULL);
}
void *Consumer(char *buf)
{
    int out = 0;
    if (count == 0)
        printf("buffer is empty\n");
    pthread_mutex_lock(&buflock);
    for (int i = 0; i < MAX; i++)
    {
        sem_wait(&full);
        printf("consumed %c from the buffer\n", buf[out]);
        out = (out + 1) % MAX;
        count--;
        if (count == 0)
            printf("buffer is empty\n");
        sem_post(&bufsize);
    }
    pthread_mutex_unlock(&buflock);

    for (int i = 0; i < 20; i++)
    {
        sem_wait(&full);
        printf("consumed %c from the buffer\n", buf[out]);
        out = (out + 1) % MAX;
        count--;
        if(count == 0)
            printf("buffer is empty\n");
        sem_post(&bufsize);
    }

    pthread_exit(NULL);
}
int main(int argc, char *argv[])
{
    char buffer[MAX] = "";

    pthread_t p;
    pthread_t c;
    sem_init(&bufsize, 0, MAX);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&buflock, NULL);

    pthread_create(&p, NULL, (void *)Producer, &buffer);
    usleep(100);
    pthread_create(&c, NULL, (void *)Consumer, &buffer);

    pthread_join(c, NULL);
    pthread_join(p, NULL);

    printf("END\n");

    return 0;
}
