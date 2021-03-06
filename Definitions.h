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
#include <pthread.h>
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

typedef struct priorityPackage
{
    char *code;
    int priority;
}priorityPackage;

void sendMessage(char *, int);
char * clientInput(int);
void sendFile(char *, char *, int);
void enterQueue(int, char *);
void takeOut(pthread_t);
void InquiryTicket(int);
void MakingReservation(struct client *, int, char *);
void ModifyReservation(int);
void MakeReservation(int, int);
struct client passangerInformation(int, int);
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
void UpdateModificationText(int, char *, char *);

static int readerCount;
pthread_mutex_t pq;


#include "Dax_Queue.c"
#include "Dax_ReadWriteSync.c"
#include "Ayrton_ReadingInformation.c"
#include "Isennock_RetrievingSeats.c"
#include "Isennock_SeatsAvailable.c"


#endif
