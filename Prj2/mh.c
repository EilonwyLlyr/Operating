#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>

sem_t mother;
sem_t father;

void *MotherProcess()
{
    sem_wait(&mother);
    printf("Father is going to sleep and waking up Mother to take care of the children.\n");
    for (int i = 0; i < 12; i++)
    {
        printf("child #%d is worken up by Mother.\n", i);
    }
    for (int i = 0; i < 12; i++)
    {
        printf("child #%d is being fed breakfast by Mother.\n", i);
        usleep(100);
    }
    for (int i = 0; i < 12; i++)
    {
        printf("child #%d is sent to school by Mother.\n", i);
        usleep(100);
    }
    for (int i = 0; i < 12; i++)
    {
        printf("child #%d is given dinner by Mother.\n", i);
        usleep(100);
    }
    for (int i = 0; i < 12; i++)
    {
        printf("child #%d is given a bath by Mother.\n", i);
        sem_post(&father);
        usleep(100);
    }
    pthread_exit(NULL);
}

void *FatherProcess(void *input)
{
    for(int i = 0; i < 12; i++){
        sem_wait(&father);
        printf("child #%d is being read a book by Father.\n", i);
        printf("child #%d is tucked into bed by Father.\n", i);
    }

    sem_post(&mother);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{

    if (argc == 2)
    {
        sem_init(&mother, 0, 1);
        sem_init(&father, 0, 0);
        pthread_t m;
        pthread_t f;
        int input = atoi(argv[1]);
        for (int i = 0; i < input; i++)
        {
            printf("This is day #%d of a day in the life of Mother Hubbard.\n", i);
            pthread_create(&m, NULL, &MotherProcess, NULL);
            pthread_create(&f, NULL, &FatherProcess, NULL);

            pthread_join(m, NULL);
            pthread_join(f, NULL);
            printf("End of day #%d\n", i);
        }
    }
    else
    {
        printf("%s", "wrong number of arguments");
    }

    return 0;
}