#include "Definitions.h"

//Prototype functions


/*
int main(int argc, char *argv[])
{
    //ChooseSeat(14);
    SeatsAvailable(4, "OSU1234", 1);
    return 0;
}
*/

void SeatsAvailable(int seatsPurchased, char ticketNumber[], int dayOfTravel){
    int count = 0;
    int seatPicked;

    do {
        //Print Seats
        DisplaySeats(dayOfTravel);

        //Choosing seat
        printf("\nPick seat for passanger #%d: ", count + 1);
        scanf("%d", &seatPicked);
        printf("\nYou chose %d\n", seatPicked);

        ChooseSeat(seatPicked, dayOfTravel);
        TrackSeatChosen(ticketNumber, seatPicked, dayOfTravel);
        count++;

        //Repeat until all seats have been selected
    } while(count < seatsPurchased);
    //Printing seats after all seats have been chosen for visual purposes
    DisplaySeats(dayOfTravel);
}

void TrackSeatChosen(char ticketNumber[], int seatPicked, int dayOfTravel){
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
        printf("Invalid day");
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

void DisplaySeats(int dayOfTravel){
    char stateInfo[1000];
    char line[1000];
    FILE *fptr;
    int currentSeat;
    int rows = 5;
    int columns = 4;
    int rowCounter = 0;
    int columnCounter = 0;

    printf("\n\t\tAVAILBLE SEATS    DAY: %d\n\n", dayOfTravel);
    //read from day 1
    if(dayOfTravel == 1){
        if ((fptr = fopen("Seats1.txt", "r")) == NULL) {
            printf("Error! Could not open Seats1.txt");
            exit(1);
        }

        printf("\n\t");
        while(fscanf(fptr, "%d\n", &currentSeat) != EOF)
        {
            if(rowCounter == rows){
                printf("\n\t");
                rowCounter = 0;
                columnCounter++;
            }
            if(columnCounter == columns/2){
                printf("\n\t");
                columnCounter = 0;
            }

            if(currentSeat == 0){
                printf("-\t");
            }
            else {
                printf("%d\t", currentSeat);
            }
            rowCounter++;
        }
        printf("\n");
        fclose(fptr);
    }

    //Read from day 2
    else if(dayOfTravel == 2){
        if ((fptr = fopen("Seats2.txt", "r")) == NULL) {
            printf("Error! Could not open Seats2.txt");
            exit(1);
        }

        printf("\n\t");
        while(fscanf(fptr, "%d\n", &currentSeat) != EOF)
        {
            if(rowCounter == rows){
                printf("\n\t");
                rowCounter = 0;
                columnCounter++;
            }
            if(columnCounter == columns/2){
                printf("\n\t");
                columnCounter = 0;
            }

            if(currentSeat == 0){
                printf("-\t");
            }
            else {
                printf("%d\t", currentSeat);
            }
            rowCounter++;
        }
        printf("\n");
        fclose(fptr);
    }
    else {
        printf("Invalid day");
        exit(1);
    }
}
