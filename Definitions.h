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
#include <sys/wait.h>
#include "Queue.h"


#define WRITE "/write"
#define READ "/read"

sem_t *write_sem;
sem_t *read_sem;

struct client{
    char name[50];
    char DOB[50];
    char gender[10];
    int dateOfTravel;
    int governmentIDNum;
    int numOfTravelers;
    char modifications[100];
    int serverNumber;
};

void sendMessage(char *, int);
char * clientInput(int);
void sendFile(char *, char *, int);
void InquiryTicket(int);
void MakingReservation(struct client *, int);
void ModifyReservation(int);
void MakeReservation(int);
struct client passangerInformation(int);
void SeatsAvailable(int, char *, int, int);
char* randomTicketGeneration(void);
void CancelReservation(int);
void GiveSeatsBack(int, char *);
void RemoveSeats(int, char *);
void TrackSeatChosen(char *, int, int, int);
void ChooseSeat(int, int);
void DisplaySeats(int, int);
void DisplayReservation(int, char *, int);
void ChangeNumberTravelers(int,char *, int, int, int);
void ChangeTravelDay(int, char *, int, int);
void ChangeSeats(int, char *, int);

static int readerCount;


#include "Dax_Queue.c"
#include "Dax_ReadWriteSync.c"
#include "readingInformation.c"
#include "RetrievingSeats.c"
#include "SeatsAvailable.c"


#endif
