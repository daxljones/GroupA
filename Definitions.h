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
#include <time.h>
#include "Queue.h"


#define WRITE "/write"
#define READ "/read"

int readFile();
void writeToFile();
void InquiryTicket();
void MakingReservation();
struct client passangerInformation();
void MakeReservation();
char* randomTicketGeneration(void);
void CancelReservation();
void GiveSeatsBack(int, char *);
void RemoveSeats(int, char *);
void TrackSeatChosen(char *, int, int);
void SeatsAvailable(int, char *, int);
void ChooseSeat(int, int);
void DisplaySeats(int);
void DisplayReservation(int, char *);
void ChangeNumberTravelers(int,char *, int, int);
void ChangeTravelDay(int, char *, int);
void ChangeTravelDay(int, char *, int);
void ChangeSeats(int, char *);

sem_t *write_sem;
sem_t *read_sem;

static int readerCount;


struct client{
    char name[50];
    char DOB[50];
    char gender[10];
    int dateOfTravel;
    int governmentIDNum;
    int numOfTravelers;
};

#include "Dax_Queue.c"
#include "Dax_ReadWriteSync.c"
#include "readingInformation.c"
#include "RetrievingSeats.c"
#include "SeatsAvailable.c"


#endif