#include <math.h>
#include "Definitions.h"

// GROUP A
// RYAN ISENNOCK
// risenno@ostatemail.okstate.edu

/*
int main()
{
    int day = 1;
    char input[10];

    printf("\nTicket Number: ");
    scanf(" %s", input);

    //Make sure GiveSeatsBack goes before Remove Seats
    GiveSeatsBack(day, input);
    RemoveSeats(day, input);

    return 0;
}
*/


void RemoveSeats(int day, char input[]){
    char ticketNumber[10];

    sem_wait(write_sem);

    if(day == 1){
        FILE *intialfptr, *newfptr;
        int seat;
        //Open both files
        if ((intialfptr = fopen("Seats1_History.txt", "r")) == NULL) {
            printf("Error! Could not open Seats1_History.txt");
            exit(1);
        }

        //Generates an empty file every time
        newfptr = fopen("tmp2.txt", "w");

        while(fscanf(intialfptr, "%s\t%d\n", ticketNumber, &seat) != EOF){
            if(!strcmp(ticketNumber, input)){
            }

            else {
                fprintf(newfptr, "%s\t%d\r\n", ticketNumber, seat);
            }
        }
        fclose(intialfptr);
        fclose(newfptr);

        //Switch names
        remove("Seats1_History.txt");
        rename("tmp2.txt", "Seats1_History.txt");

        //Closing semaphore
        sem_post(write_sem);
    }

    else if(day == 2){
        FILE *intialfptr, *newfptr;
        int seat;
        //Open both files
        if ((intialfptr = fopen("Seats2_History.txt", "r")) == NULL) {
            printf("Error! Could not open Seats2_History.txt");
            exit(1);
        }

        //Generates an empty file every time
        newfptr = fopen("tmp2.txt", "w");

        while(fscanf(intialfptr, "%s\t%d\n", ticketNumber, &seat) != EOF){
            if(!strcmp(ticketNumber, input)){
                //Do nothing - not copying line into new file
            }

            else {
                //Copy line into new file
                fprintf(newfptr, "%s\t%d\r\n", ticketNumber, seat);
            }
        }
        fclose(intialfptr);
        fclose(newfptr);

        //Switch names
        remove("Seats2_History.txt");
        rename("tmp2.txt", "Seats2_History.txt");

        //Closing semaphore
        sem_post(write_sem);
    }
    else {
        printf("Remove seats: Not valid day for travel");
        exit(1);
    }
}

void GiveSeatsBack(int day, char input[]){
    char ticketNumber[10];

    sem_wait(write_sem);

    if(day == 1){
        FILE *fptr1, *fptr2, *fptr3;
        int seat;
        int count = 0;
        int seatState;

        //Open file where seats with ticket numbers are stored
        if((fptr1 = fopen("Seats1_History.txt", "r")) == NULL) {
            printf("Error! Could not open Seats1_History.txt");
            exit(1);
        }

        //Scanning ticket number and seats
        while(fscanf(fptr1, "%s\t%d\n", ticketNumber, &seat) != EOF){
            //printf("\nDebug: Scanning new line of SeatsHistory");

            if((fptr2 = fopen("Seats1.txt", "r")) == NULL) {
                printf("Error Give Seats Back 1! Could not open Seats1.txt");
                exit(1);
            }

            fptr3 = fopen("tmp3.txt", "w");
            count = 0;
            while(fscanf(fptr2, "%d\n", &seatState) != EOF){
                //Generates an empty file every time

                if(!strcmp(ticketNumber, input)){
                    count++;
                    if(seat == count){
                        //printf("\nDebug: PRINTING count");
                        fprintf(fptr3, "%d\r\n", count);
                    }
                    else{
                        //printf("\nDebug: Copying state of matching check");
                        fprintf(fptr3, "%d\r\n", seatState);
                    }
                }
                else {
                    //printf("\nDebug: Copying state");
                    fprintf(fptr3, "%d\r\n", seatState);
                }
            }
            //printf("\nDebug: Before rewind and fclose");
            rewind(fptr2);
            fclose(fptr3);
            fclose(fptr2);
            //Switch names
            remove("Seats1.txt");
            rename("tmp3.txt", "Seats1.txt");
        }
        fclose(fptr1);

        //Closing semaphore
        sem_post(write_sem);
    }

    else if(day == 2){
        FILE *fptr1, *fptr2, *fptr3;
        int seat;
        int count = 0;
        int seatState;

        if((fptr1 = fopen("Seats2_History.txt", "r")) == NULL) {
            printf("Error! Could not open Seats1_History.txt");
            exit(1);
        }

        while(fscanf(fptr1, "%s\t%d\n", ticketNumber, &seat) != EOF){

            if((fptr2 = fopen("Seats2.txt", "r")) == NULL) {
                printf("Error Give Seats Back 1! Could not open Seats1.txt");
                exit(1);
            }

            fptr3 = fopen("tmp3.txt", "w");
            count = 0;
            while(fscanf(fptr2, "%d\n", &seatState) != EOF){

                if(!strcmp(ticketNumber, input)){
                    count++;
                    if(seat == count){
                        fprintf(fptr3, "%d\r\n", count);
                    }
                    else{
                        fprintf(fptr3, "%d\r\n", seatState);
                    }
                }
                else {
                    fprintf(fptr3, "%d\r\n", seatState);
                }
            }

            rewind(fptr2);
            fclose(fptr3);
            fclose(fptr2);

            remove("Seats2.txt");
            rename("tmp3.txt", "Seats2.txt");
        }
        fclose(fptr1);

        //Closing semaphore
        sem_post(write_sem);
    }

    else {
        printf("\nGiveSeatsBack: Not valid travel day");
    }
}
