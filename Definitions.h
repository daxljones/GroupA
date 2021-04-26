#ifndef DEF_H
#define DEF_H

#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include "Queue.h"

#define WRITE "/write"
#define READ "/read"

int readFile();
void writeToFile();

sem_t *write_sem;
sem_t *read_sem;

static int readerCount;


struct Customer{
    char fullName[50];
    char DOB[10];
    char gender;
    int GovID;
    char DOT[10];
    int numOfAdditionals;
    struct Customer *addInfo;
};



#endif