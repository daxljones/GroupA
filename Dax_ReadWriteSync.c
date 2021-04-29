#include "Definitions.h"


void readFile()
{
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


}

void writeToFile()
{
    sem_wait(write_sem);

    // -----------------------------
    // Critical section for writing
    // -----------------------------

    sem_post(write_sem);
}



