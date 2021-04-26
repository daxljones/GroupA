#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include "passangerInformation.h"

// GROUP A
// RYAN ISENNOCK
// risenno@ostatemail.okstate.edu

int main(int argc, char *argv[])
{
    printf("\n          FINAL GROUP PROJECT");
    printf("\n          Team A");
    printf("\n          Spring 2021");
    printf("\n\nFinal Program: 04/30/2021");

    printf("\n\nGROUP MEMBERS:");
    printf("\nDax Jones");
    printf("\nAyrton Ledesma");
    printf("\nRyan Isennock\n");

    char input;
    int option = -1;

    do{
        printf("\n\n\tMENU\n");
        printf("\n1. Make a reservation.");
        printf("\n2. Inquiry about the ticket.");
        printf("\n3. Modify the reservation.");
        printf("\n4. Cancel the reservation.");
        printf("\n5. Exit the program\n");
        printf("\nOption: ");
        scanf("%s", &input);
        if(isdigit(input)){
            option = (int)input - 48;
            if(option < 1 || option > 5){
                printf("\nNot a valid option. Please try again!\n");
            }
            else {
                switch(option)
                {
                    case 1:
                        MakeReservation();
                        break;

                    case 2:
                        InquiryTicket();
                        break;

                    case 3:
                        ModifyReservation();
                        break;

                    case 4:
                        CancelReservation();
                        break;
                }
            }
        }
        else {
            printf("\nNot a valid option. Please try again!\n");
        }

    } while(option != 5);
    return 0;
}
/*
void MakeReservation(){
    char name[50];
    char DOB[50];
    char gender[10];
    int id = 0;
    int numTravelers = 0;

    int validInput = 0;
    char confirmation[10];

    char typeCheck;

    printf("\n\tMAKE RESERVATION");
    printf("\n\nName:\t\t\t");
    while ((getchar()) != '\n');
    fgets(&currentClient.name, 50, stdin);

    printf("DOB:\t\t\t");
    fgets(&currentClient.DOB, 50, stdin);

    printf("Gender:\t\t\t");
    fgets(&currentClient.gender, 10, stdin);

    do{
        printf("ID num:\t\t\t");
        if(scanf("%d", &id) == 1){
            currentClient.governmentIDNum = id;
        }
        else {
            printf("Not a valid ID Number. Please try again!\n");
        }
    } while(id == 0);

    do{
        printf("Number of travelers:\t");
        if(scanf("%d", &numTravelers) == 1){
            currentClient.numOfTravelers = numTravelers;
        }
        else {
            printf("Not a valid number of travelers. Please try again!\n");
        }
    } while(numTravelers == 0);

    do{
        printf("\n\nDo you want to make a reservation?(yes/no): ");
        scanf(" %s", confirmation);
        if(!strcmp(confirmation, "yes")){
            printf("\n\nYour reservation has been completed!\n");
            //Write into txt file
            break;
        }

        else if(!strcmp(confirmation, "no")){
            printf("\n\nYour reservation has been canceled!\n");
            break;
        }

        else {
            printf("\nNot a valid confirmation. Please enter yes or no\n");
        }

    } while(1);
}
*/

