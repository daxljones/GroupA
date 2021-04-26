#include "Definitions.h"



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

    sem_post(write_sem);
}



