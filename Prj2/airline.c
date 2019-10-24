#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

sem_t P2B;
sem_t B2P;
sem_t P2S;
sem_t S2P;
sem_t P2F;
sem_t F2P;

void *passengerProcess(void *pas)
{
    int p = *(int *)pas;
    printf("Passenger #%d just arrived at the terminal.\n", p);
    sem_post(&P2B);
    printf("Passenger #%d is waiting at baggage processing for a handler.\n", p);
    sem_wait(&B2P);
    sem_post(&P2S);
    printf("Passenger #%d is waiting to be screened by a screener.\n", p);
    sem_wait(&S2P);
    sem_post(&P2F);
    printf("Passenger #%d is waiting to board the plane by an attendent.\n", p);
    sem_wait(&F2P);
    printf("Passenger #%d has been seated and relaxes.\n", p);
    pthread_exit(NULL);
}
void *baggerProcess(void *bag)
{
    for (;;)
    {
        sem_wait(&P2B);
        sem_post(&B2P);
        
    }
    pthread_exit(NULL);
}
void *securityProcess(void *sec)
{
    for (;;)
    {
        sem_wait(&P2S);
        sem_post(&S2P);
    }
    pthread_exit(NULL);
}
void *attendentProcess(void *att)
{
    for (;;)
    {
        sem_wait(&P2F);
        sem_post(&F2P);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc == 5)
    {

        int p = atoi(argv[1]);
        int b = atoi(argv[2]);
        int s = atoi(argv[3]);
        int a = atoi(argv[4]);

        sem_init(&P2B, 0, 0);
        sem_init(&B2P, 0, 0);
        sem_init(&P2S, 0, 0);
        sem_init(&S2P, 0, 0);
        sem_init(&P2F, 0, 0);
        sem_init(&F2P, 0, 0);

        pthread_t passengers[p];
        pthread_t baggers[b];
        pthread_t security[s];
        pthread_t attendent[a];

        int ps[p];

        for (int i = 0; i < a; i++)
        {
            pthread_create(&attendent[i], NULL, &attendentProcess, NULL);
        }
        for (int i = 0; i < s; i++)
        {
            pthread_create(&security[i], NULL, &securityProcess, NULL);
        }
        for (int i = 0; i < b; i++)
        {
            pthread_create(&baggers[i], NULL, &baggerProcess, NULL);
        }
        for (int i = 0; i < p; i++)
        {
            ps[i] = i;
            pthread_create(&passengers[i], NULL, &passengerProcess, &ps[i]);
        }

        for (int i = 0; i < p; i++)
        {
            pthread_join(passengers[i], NULL);
        }

    }
    else
    {
        printf("wrong number of arguments, needs 4, inputed: %d\n", argc - 1);
    }
    return 0;
}