#include "Definitions.h"

static int readerCount = 0;

int readFile()
{
    //char *info = malloc(sizeof(char) * 256);
    int send;

    sem_wait(read_sem);
    readerCount++;
    if(readerCount == 1)
        sem_wait(write_sem);
    sem_post(read_sem);

    //-----------------------------------------
    //This is the critical Section for reading
    //-----------------------------------------

     send = var;

    wait(read_sem);
    readerCount--;

    if(readerCount == 0)
        sem_post(write_sem);

    sem_post(read_sem);

    return send;
}

void writeToFile()
{
    sem_wait(write_sem);

    // -----------------------------
    // Critical section for writing
    // -----------------------------
    sleep(5);
    var++;

    sem_post(write_sem);
}



