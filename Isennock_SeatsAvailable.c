#include "Definitions.h"
// GROUP A
// RYAN ISENNOCK
// risenno@ostatemail.okstate.edu


void SeatsAvailable(int seatsPurchased, char ticketNumber[], int dayOfTravel, int clientSocket){
    int count = 0;
    int seatPicked;

    char message[256];
    char *confirmation;

    do {
        //Print Seats
        DisplaySeats(dayOfTravel, clientSocket);

        //Choosing seat
        //printf("\nPick seat for passanger #%d: ", count + 1);
        sprintf(message, "\nPick seat for passanger #%d: \n", count + 1);
        sendMessage(message, clientSocket);

        //scanf("%d", &seatPicked);
        confirmation = clientInput(clientSocket);
        seatPicked = atoi(confirmation);
        free(confirmation);

        memset(&message, '\0', sizeof(message));
        sprintf(message, "\nYou chose %d\n", seatPicked);
        sendMessage(message, clientSocket);

        ChooseSeat(seatPicked, dayOfTravel);
        TrackSeatChosen(ticketNumber, seatPicked, dayOfTravel, clientSocket);
        count++;

        //Repeat until all seats have been selected
    } while(count < seatsPurchased);
    //Printing seats after all seats have been chosen for visual purposes
    DisplaySeats(dayOfTravel, clientSocket);
}

void TrackSeatChosen(char ticketNumber[], int seatPicked, int dayOfTravel, int clientSocket){
    if(dayOfTravel == 1){
        FILE *f = fopen("Seats1_History.txt", "a");
        if (f == NULL)
        {
            printf("Error opening SeatsHistory1.txt file!\n");
            exit(1);
        }

        fprintf(f, "%s\t%d\r\n", ticketNumber, seatPicked);
        fclose(f);
    }

    else if(dayOfTravel == 2){
        FILE *f = fopen("Seats2_History.txt", "a");
        if (f == NULL)
        {
            printf("Error opening SeatsHistory2.txt file!\n");
            exit(1);
        }

        fprintf(f, "%s\t%d\r\n", ticketNumber, seatPicked);
        fclose(f);
    }
    else {
        //printf("Invalid day");
        char message[256];
        sprintf(message, "Invalid day\n");
        sendMessage(message, clientSocket);
        exit(1);
    }
}

void ChooseSeat(int seatPicked, int dayOfTravel){
    FILE *fptr1, *fptr2;
    int currentSeat;

    int seatTaken = 0;

    if(dayOfTravel == 1){
        fptr1 = fopen("tmp.txt", "w");
        if((fptr2 = fopen("Seats1.txt", "r")) == NULL) {
            printf("Error! Could not open Seats1.txt");
            exit(1);
        }
        while(fscanf(fptr2, "%d\n", &currentSeat) != EOF){
            if(currentSeat == seatPicked){
                fprintf(fptr1, "%d\r\n", seatTaken);
            }
            else {
                fprintf(fptr1, "%d\r\n", currentSeat);
            }
        }
        fclose(fptr1);
        fclose(fptr2);
        remove("Seats1.txt");
        rename("tmp.txt", "Seats1.txt");
    }

    else if(dayOfTravel == 2){
        fptr1 = fopen("tmp.txt", "w");
        if((fptr2 = fopen("Seats2.txt", "r")) == NULL) {
            printf("Error! Could not open Seats2.txt");
            exit(1);
        }
        while(fscanf(fptr2, "%d\n", &currentSeat) != EOF){
            if(currentSeat == seatPicked){
                fprintf(fptr1, "%d\r\n", seatTaken);
            }
            else {
                fprintf(fptr1, "%d\r\n", currentSeat);
            }
        }
        fclose(fptr1);
        fclose(fptr2);
        remove("Seats2.txt");
        rename("tmp.txt", "Seats2.txt");
    }
}

void DisplaySeats(int dayOfTravel, int clientSocket){
    char stateInfo[1000];
    char line[1000];
    FILE *fptr;
    int currentSeat;
    int rows = 5;
    int columns = 4;
    int rowCounter = 0;
    int columnCounter = 0;

    char message[256];

    sem_wait(read_sem);
    readerCount++;
    if(readerCount == 1)
        sem_wait(write_sem);

    sem_post(read_sem);

    //printf("\n\t\tAVAILBLE SEATS    DAY: %d\n\n", dayOfTravel);
    sprintf(message, "\n\t\tAVAILBLE SEATS    DAY: %d\n\n", dayOfTravel);
    sendMessage(message, clientSocket);

    //read from day 1
    if(dayOfTravel == 1){
        if ((fptr = fopen("Seats1.txt", "r")) == NULL) {
            printf("Error! Could not open Seats1.txt");
            exit(1);
        }

        memset(&message, '\0', sizeof(message));
        sprintf(message, "\n\t");
        sendMessage(message, clientSocket);

        while(fscanf(fptr, "%d\n", &currentSeat) != EOF)
        {
            if(rowCounter == rows){
                //printf("\n\t");
                memset(&message, '\0', sizeof(message));
                sprintf(message, "\n\t");
                sendMessage(message, clientSocket);
                rowCounter = 0;
                columnCounter++;
            }
            if(columnCounter == columns/2){
                memset(&message, '\0', sizeof(message));
                sprintf(message, "\n\t");
                sendMessage(message, clientSocket);
                columnCounter = 0;
            }

            if(currentSeat == 0){
                //printf("-\t");
                memset(&message, '\0', sizeof(message));
                sprintf(message, "-\t");
                sendMessage(message, clientSocket);
            }
            else {
                //printf("%d\t", currentSeat);
                memset(&message, '\0', sizeof(message));
                sprintf(message, "%d\t", currentSeat);
                sendMessage(message, clientSocket);
            }
            rowCounter++;
        }
        //printf("\n");
        memset(&message, '\0', sizeof(message));
        sprintf(message, "\n");
        sendMessage(message, clientSocket);
        fclose(fptr);

        wait(read_sem);
        readerCount--;

        if(readerCount == 0)
            sem_post(write_sem);

        sem_post(read_sem);
    }

    //Read from day 2
    else if(dayOfTravel == 2){
        if ((fptr = fopen("Seats2.txt", "r")) == NULL) {
            printf("Error! Could not open Seats2.txt");
            exit(1);
        }

        //printf("\n\t");
        memset(&message, '\0', sizeof(message));
        sprintf(message, "\n\t");
        sendMessage(message, clientSocket);
        while(fscanf(fptr, "%d\n", &currentSeat) != EOF)
        {
            if(rowCounter == rows){
                //printf("\n\t");
                memset(&message, '\0', sizeof(message));
                sprintf(message, "\n\t");
                sendMessage(message, clientSocket);
                rowCounter = 0;
                columnCounter++;
            }
            if(columnCounter == columns/2){
                //printf("\n\t");
                memset(&message, '\0', sizeof(message));
                sprintf(message, "\n\t");
                sendMessage(message, clientSocket);
                columnCounter = 0;
            }

            if(currentSeat == 0){
                //printf("-\t");
                memset(&message, '\0', sizeof(message));
                sprintf(message, "-\t");
                sendMessage(message, clientSocket);
            }
            else {
                //printf("%d\t", currentSeat);
                memset(&message, '\0', sizeof(message));
                sprintf(message, "%d\t", currentSeat);
                sendMessage(message, clientSocket);
            }
            rowCounter++;
        }
        //printf("\n");
        memset(&message, '\0', sizeof(message));
        sprintf(message, "\n");
        sendMessage(message, clientSocket);
        fclose(fptr);

        wait(read_sem);
        readerCount--;

        if(readerCount == 0)
            sem_post(write_sem);

        sem_post(read_sem);
    }
    else {
        //printf("Invalid day");
        memset(&message, '\0', sizeof(message));
        sprintf(message, "Invalid Day\n");
        sendMessage(message, clientSocket);
        exit(1);
    }
}
