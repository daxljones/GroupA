// GROUP A
// AYRTON LEDESMA
// ayrton.ledesma_fuentes@okstate.edu

#include "Definitions.h"


/*
int main(int argc, char *argv[])
{
    //struct client customer;
    //customer = passangerInformation();
    //MakingReservation(&customer);

    //InquiryTicket();
    MakeReservation();

    return 0;
}
*/


//Read
void MakeReservation(int clientSocket){
    struct client customer;
    customer = passangerInformation(clientSocket);
    char *confirmation;

    char message[256];


    do{
        memset(&message, '\0', sizeof(message));
        sprintf(message, "\n\nDo you want to make a reservation?(yes/no): ");
        sendMessage(message, clientSocket);
        confirmation = clientInput(clientSocket);

        if(!strcmp(confirmation, "yes")){
            memset(&message, '\0', sizeof(message));
            sprintf(message, "\n\nYour reservation is being processed...\n");
            sendMessage(message, clientSocket);

            
            enterQueue(customer.numOfTravelers, pthread_self());

            MakingReservation(&customer, clientSocket);
            memset(&message, '\0', sizeof(message));
            sprintf(message, "\n\nYour reservation has been completed!\n");
            sendMessage(message, clientSocket);

            break;
        }

        else if(!strcmp(confirmation, "no")){
            memset(&message, '\0', sizeof(message));
            sprintf(message, "\n\nYour reservation has been canceled!\n");
            sendMessage(message, clientSocket);

            break;
        }

        else {
            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nNot a valid confirmation. Please enter yes or no\n");
            sendMessage(message, clientSocket);
            free(confirmation);
        }

    } while(1);

    free(confirmation);
}

//Read
struct client passangerInformation(int clientSocket){
    struct client customer;
    char *confirmation;
    char message[256];


    sprintf(message, "Enter name: \t\t");
    sendMessage(message, clientSocket);
    //scanf (" %[^\n]%*c", customer.name);
    confirmation = clientInput(clientSocket);
    strcpy(customer.name, confirmation);
    free(confirmation);

    sprintf(message, "Enter DOB: \t\t");
    sendMessage(message, clientSocket);
    //scanf(" %[^\n]%*c", customer.DOB);
    confirmation = clientInput(clientSocket);
    strcpy(customer.DOB, confirmation);
    free(confirmation);


    sprintf(message, "Enter gender: \t\t");
    sendMessage(message, clientSocket);
    //scanf (" %[^\n]%*c", customer.gender);
    confirmation = clientInput(clientSocket);
    strcpy(customer.gender, confirmation);
    free(confirmation);


    sprintf(message, "Enter ID number: \t");
    sendMessage(message, clientSocket);
    //scanf(" %d", &customer.governmentIDNum);
    confirmation = clientInput(clientSocket);
    customer.governmentIDNum = atoi(confirmation);
    free(confirmation);

    sprintf(message, "Number of Travelers:\t");
    sendMessage(message, clientSocket);
    //scanf(" %d", &customer.numOfTravelers);
    confirmation = clientInput(clientSocket);
    customer.numOfTravelers = atoi(confirmation);
    free(confirmation);

    sprintf(message, "\nDate of travel (1 or 2):");
    sendMessage(message, clientSocket);
    //scanf(" %d", &customer.dateOfTravel);
    confirmation = clientInput(clientSocket);
    customer.dateOfTravel = atoi(confirmation);
    free(confirmation);

    //Adding server number and modifications
    customer.serverNumber = 0;
    strcpy(customer.modifications, "None");

    return customer;
}


//Reads ticket number
void InquiryTicket(int clientSocket){
    char stateInfo[1000];
    char line[1000];
    FILE *fptr;

    char message[256];


    char ticketNumber[10];
    char *name[500];
    char *DOB[10];
    char *gender[10];
    int idNumber;
    int numOfTravelers;
    char *input;

    //Adding server number and modifications
    char *modifications[100];
    int serverNumber;

    int match;
    int day;

    if ((fptr = fopen("Day1.txt", "r")) == NULL) {
        printf("Error! Could not open Day1.txt");
        exit(1);
    }

    do {
        match = 0;
        //fgets(stateInfo, 1000, fptr);
        memset(&message, '\0', sizeof(message));
        sprintf(message, "\n\tINQUIRY TICKET\n");
        sendMessage(message, clientSocket);

        memset(&message, '\0', sizeof(message));
        sprintf(message, "\nTicket Number: ");
        sendMessage(message, clientSocket);

        input = clientInput(clientSocket);

        while(fscanf(fptr, "%s\t%[^\n^\t]%*c\t%s\t%s\t%d\t%d\t%d\t%s\n", ticketNumber, name, DOB, gender, &idNumber, &numOfTravelers, &serverNumber, modifications) != EOF){
            if(!strcmp(ticketNumber, input)){
                //Add char function: Look for tickets chosen in Seats1_History
                day = 1;
                match = 1;
                break;
            }
        }

        if(match == 0){
            if ((fptr = fopen("Day2.txt", "r")) == NULL) {
                printf("Error! Could not open Day2.txt");
                exit(1);
            }

            //fgets(stateInfo, 1000, fptr);

            while(fscanf(fptr, "%s\t%[^\n^\t]%*c\t%s\t%s\t%d\t%d\t%d\t%s\n", ticketNumber, name, DOB, gender, &idNumber, &numOfTravelers, &serverNumber, modifications) != EOF){
                if(!strcmp(ticketNumber, input)){
                    //Add char function: Look for tickets chosen in Seats1_History
                    day = 2;
                    match = 1;
                    break;
                }
            }
        }

        if(match != 0){
            DisplayReservation(day, ticketNumber, clientSocket);
            break;
        }

        else {
            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nCould not find ticket. Please try again!");
            sendMessage(message, clientSocket);

            rewind(fptr);
        }
    } while(1);

    free(input);
    fclose(fptr);
}

//Writing
void MakingReservation(struct client *customer, int clientSocket){
    //Generate random ticket number, right now same ticket number for all of them

    char message[256];

    char *ticketPtr = randomTicketGeneration();
    char ticketNumber[6];
    strcpy(ticketNumber, ticketPtr);
    ticketNumber[strlen(ticketNumber) + 1] = '\0';

    //char ticketNumber[] = "OSU1234";

    //---------------------PUT WRITING SEMPHORE HERE-----------------------
    takeOut(pthread_self());

    int dayOfTravel = customer->dateOfTravel;
    if(dayOfTravel == 1){
        FILE *f = fopen("Day1.txt", "a");
        if (f == NULL)
        {
            printf("Error opening Day1.txt file!\n");
            exit(1);
        }

        fprintf(f, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\r\n", ticketNumber, customer->name, customer->DOB, customer->gender, customer->governmentIDNum, customer->numOfTravelers, customer->serverNumber, customer->modifications);
        //fprintf(f, "%s\t%s\t%s\t%s\t%d\t%d\r\n", ticketNumber, customer->name, customer->DOB, customer->gender, customer->governmentIDNum, customer->numOfTravelers);
        fclose(f);
    }

    else if(dayOfTravel == 2){
        FILE *f = fopen("Day2.txt", "a");
        if (f == NULL)
        {
            printf("Error opening Day2.txt file!\n");
            exit(1);
        }
        fprintf(f, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\r\n", ticketNumber, customer->name, customer->DOB, customer->gender, customer->governmentIDNum, customer->numOfTravelers, customer->serverNumber, customer->modifications);
        //fprintf(f, "%s\t%s\t%s\t%s\t%d\t%d\r\n", ticketNumber, customer->name, customer->DOB, customer->gender, customer->governmentIDNum, customer->numOfTravelers);
        fclose(f);
    }

    //Allow user to pick seats
    SeatsAvailable(customer->numOfTravelers, ticketNumber, customer->dateOfTravel, clientSocket);
    memset(&message, '\0', sizeof(message));
    sprintf(message, "\nYour ticket number is: %s", ticketNumber);
    sendMessage(message, clientSocket);

    //Display final reservation
    DisplayReservation(dayOfTravel, ticketNumber, clientSocket);
}


char* randomTicketGeneration(void)
{
    int randomizer = 0;

    srand((unsigned int)(time(NULL)));

    // Characters to generate random ticket
    char numbers[] = "0123456789";
    char letter[] = "abcdefghijklmnoqprstuvwyzx";
    char LETTER[] = "ABCDEFGHIJKLMNOQPRSTUYWVZX";
    char symbols[] = "!@#$^&*?";
    static char ticket[6];

    randomizer = rand() % 4;
    for (int i = 0; i < 5; i++) {

        if (randomizer == 1) {
            ticket[i] = numbers[rand() % 10];
            randomizer = rand() % 4;
        }
        else if (randomizer == 2) {
            ticket[i] = symbols[rand() % 8];
            randomizer = rand() % 4;
        }
        else if (randomizer == 3) {
            ticket[i] = LETTER[rand() % 26];
            randomizer = rand() % 4;
        }
        else {
            ticket[i] = letter[rand() % 26];
            randomizer = rand() % 4;
        }
    }
    char *randomTicket = ticket;
    return randomTicket;
}

//Write
void CancelReservation(int clientSocket){
    char stateInfo[1000];
    char line[1000];
    FILE *fptr, *fptr1, *fptr2, *fptr3;

    char ticketNumber[10];
    char *name[500];
    char *DOB[10];
    char *gender[10];
    int idNumber;
    int numOfTravelers;

    int day = 0;

    int match;

    char message[256];
    char *input;

    //Adding server number and modifications
    char *modifications[100];
    int serverNumber;

    //WRITING SEMAPHORE
    sem_wait(write_sem);


    if ((fptr = fopen("Day1.txt", "r")) == NULL) {
        printf("Error! Could not open Day1.txt");
        exit(1);
    }

    fptr1 = fopen("tmp.txt", "w");

    do {
        match = 0;
        memset(&message, '\0', sizeof(message));
        sprintf(message, "\nTicket Number: ");
        sendMessage(message, clientSocket);

        input = clientInput(clientSocket);

        if(input == "1"){
            break;
        }

        //Look for ticket number in Day 1
        while(fscanf(fptr, "%s\t%[^\n^\t]%*c\t%s\t%s\t%d\t%d\t%d\t%s\n", ticketNumber, name, DOB, gender, &idNumber, &numOfTravelers, &serverNumber, modifications) != EOF){
            if(!strcmp(ticketNumber, input)){
                match = 1;
                day = 1;
            }

            else {
                fprintf(fptr1, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\r\n", ticketNumber, name, DOB, gender, idNumber, numOfTravelers, serverNumber, modifications);
            }
        }

        if(match == 0){
            fptr3 = fopen("tmp1.txt", "w");
            //sprintf(message, "\nDEBUG: Attempting to look in Day2.txt");
            if ((fptr2 = fopen("Day2.txt", "r")) == NULL) {
                printf("Error! Could not open Day2.txt trying to cancel reservation");
                exit(1);
            }
            //sprintf(message, "\nDEBUG2: Successfully opened Day2.txt");
            while(fscanf(fptr2, "%s\t%[^\n^\t]%*c\t%s\t%s\t%d\t%d\t%d\t%s\n", ticketNumber, name, DOB, gender, &idNumber, &numOfTravelers, &serverNumber, modifications) != EOF){
                if(!strcmp(ticketNumber, input)){
                    //sprintf(message, "\nDEBUG3: Match found in Day 2, no copying line into tmp file");
                    match = 1;
                    day = 2;
                }

                else {
                    //sprintf(message, "\nDEBUG4: This line does not equal ticket, copying it into tmp.txt");
                    fprintf(fptr3, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\r\n", ticketNumber, name, DOB, gender, idNumber, numOfTravelers, serverNumber, modifications);
                }
            }
        }

        if(match == 1){
            //Give seats back
            //sprintf(message, "\nInput before sending to remove seats: %s", input);

            sem_post(write_sem);

            GiveSeatsBack(day, input);
            RemoveSeats(day, input);

            //sprintf(message, "\nDEBUG: Match found after copying files, Exiting now");
            break;
        }
        else {
            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nCouldn't find ticket number. Please try again! ");
            sendMessage(message, clientSocket);
        }

    } while(1);

    if(day == 1){
        fclose(fptr);
        fclose(fptr1);
        remove("Day1.txt");
        rename("tmp.txt", "Day1.txt");
    }

    else if(day == 2){
        fclose(fptr2);
        fclose(fptr3);
        remove("Day2.txt");
        rename("tmp1.txt", "Day2.txt");
    }
    memset(&message, '\0', sizeof(message));
    sprintf(message, "\n\nYour reservation has been canceled!");
    sendMessage(message, clientSocket);

    free(input);
}

//Write
void ModifyReservation(int clientSocket){
    int choice;
    int choice2;
    int numOfTravelersModified;
    int operation;

    FILE *fptr, *fptr2;
    char ticketNumber[10];
    char *name[500];
    char *DOB[10];
    char *gender[10];
    int idNumber;
    int numOfTravelers;
    char *input;
    int day = 0;

    int match;

    char message[256];
    char *userInput;

    int serverNumber;
    char *modifications[100];


    if ((fptr = fopen("Day1.txt", "r")) == NULL) {
        printf("Error! Could not open Day1.txt");
        exit(1);
    }

    memset(&message, '\0', sizeof(message));
    sprintf(message, "\n\n\tMODIFY RESERVATION\n");
    sendMessage(message, clientSocket);

    do {
        match = 0;
        memset(&message, '\0', sizeof(message));
        sprintf(message, "\nTicket Number: ");
        sendMessage(message, clientSocket);

        input = clientInput(clientSocket);

        //Look for ticket number in Day 1
        while(fscanf(fptr, "%s\t%[^\n^\t]%*c\t%s\t%s\t%d\t%d\t%d\t%s\n", ticketNumber, name, DOB, gender, &idNumber, &numOfTravelers, &serverNumber, modifications) != EOF){
            if(!strcmp(ticketNumber, input)){
                match = 1;
                day = 1;
                break;
            }
        }

        if(match == 0){
            if ((fptr2 = fopen("Day2.txt", "r")) == NULL) {
                printf("Error! Could not open Day2.txt trying to modify reservation");
                exit(1);
            }

            while(fscanf(fptr2, "%s\t%[^\n^\t]%*c\t%s\t%s\t%d\t%d\t%d\t%s\n", ticketNumber, name, DOB, gender, &idNumber, &numOfTravelers, &serverNumber, modifications) != EOF){
                if(!strcmp(ticketNumber, input)){
                    match = 1;
                    day = 2;
                    break;
                }
            }
        }

        if(match == 1){
            break;
        }

        else {
            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nCouldn't find ticket number. Please try again! ");
            sendMessage(message, clientSocket);

        }

    } while(1);

    memset(&message, '\0', sizeof(message));
    sprintf(message, "\n1. Change Seat\n2. Change Travel Day\n3. Change Number of Travelers\n\nEnter option number: ");
    sendMessage(message, clientSocket);

    //Add checker for correct input
    userInput = clientInput(clientSocket);
    choice = atoi(userInput);

    //choice = 1;
    switch(choice)
    {
        case 1:
            //Change Seats
            ChangeSeats(day, input, clientSocket);
            break;

        case 2:
            //Change Travel Day
            ChangeTravelDay(day, input, numOfTravelers, clientSocket);
            break;

        case 3:
            //Change number of travelers
            memset(&message, '\0', sizeof(message));
            sprintf(message, "\n\n1. Add Travelers\n2. Remove Travelers\n\nEnter option number: ");
            sendMessage(message, clientSocket);

            memset(userInput, '\0', sizeof(userInput));
            userInput = clientInput(clientSocket);
            choice2 = atoi(userInput);

            switch(choice2)
            {
                case 1:
                    memset(&message, '\0', sizeof(message));
                    sprintf(message, "\n\nPlease enter number of seats to add: ");
                    sendMessage(message, clientSocket);

                     memset(userInput, '\0', sizeof(userInput));
                    userInput = clientInput(clientSocket);
                   numOfTravelersModified= atoi(userInput);

                    operation = 1;
                    ChangeNumberTravelers(day, input, numOfTravelersModified, operation, clientSocket);
                    break;

                case 2:
                    memset(&message, '\0', sizeof(message));
                    sprintf(message, "\n\nPlease enter number of seats to remove: ");
                    sendMessage(message, clientSocket);

                    memset(userInput, '\0', sizeof(userInput));
                    userInput = clientInput(clientSocket);
                    numOfTravelersModified= atoi(userInput);
                    //Remove seats operation
                    operation = -1;
                    ChangeNumberTravelers(day, input, numOfTravelersModified, operation, clientSocket);
                    break;
            }
    }
}

//Write
void ChangeSeats(int day, char input[], int clientSocket){
    char ticketNumber[10];
    int seatsFound = 0;
    int seat;

    char message[256];


    if(day == 1){
        FILE *fptr1;
        //Open Seats_History2
        if ((fptr1 = fopen("Seats1_History.txt", "r")) == NULL) {
            printf("Error! Could not open Day1.txt");
            exit(1);
        }

        while(fscanf(fptr1, "%s\t%d\n", ticketNumber, &seat) != EOF){
            if(!strcmp(ticketNumber, input)){
                seatsFound++;
            }
        }

        memset(&message, '\0', sizeof(message));
        sprintf(message, "\nNumber of seats initially purchased: %d", seatsFound);
        sendMessage(message, clientSocket);

        //Removing original seats
        GiveSeatsBack(day, input);
        RemoveSeats(day, input);

        memset(&message, '\0', sizeof(message));
        sprintf(message, "\nPlease enter the new seat number for each passanger");
        sendMessage(message, clientSocket);


        //Allow user to pick new seats
        SeatsAvailable(seatsFound, ticketNumber, day, clientSocket);

        //Display final reservation
        DisplayReservation(day, ticketNumber, clientSocket);
    }

    else if(day == 2){
        FILE *fptr1;
        //Open Seats_History2
        if ((fptr1 = fopen("Seats2_History.txt", "r")) == NULL) {
            printf("Error! Could not open Day1.txt");
            exit(1);
        }

        while(fscanf(fptr1, "%s\t%d\n", ticketNumber, &seat) != EOF){
            if(!strcmp(ticketNumber, input)){
                seatsFound++;
            }
        }

        memset(&message, '\0', sizeof(message));
        sprintf(message, "\nNumber of seats initially purchased: %d", seatsFound);
        sendMessage(message, clientSocket);

        //Removing original seats
        GiveSeatsBack(day, input);
        RemoveSeats(day, input);

        memset(&message, '\0', sizeof(message));
        sprintf(message, "\nPlease enter the new seat number for each passanger");
        sendMessage(message, clientSocket);


        //Allow user to pick new seats
        SeatsAvailable(seatsFound, ticketNumber, day, clientSocket);

        //Display final reservation
        DisplayReservation(day, ticketNumber, clientSocket);
    }
    else {
        memset(&message, '\0', sizeof(message));
        sprintf(message, "Error! Not a valid travel day - Change Seats");
        sendMessage(message, clientSocket);

    }
}

//Write
void DisplayReservation(int day, char input[], int clientSocket){
    FILE *fptr, *fptr1;
    int seat;
    int match = 0;

    char ticketNumber[10];
    char *name[50];
    char *DOB[10];
    char *gender[10];
    int idNumber;
    int numOfTravelers;

    //Added server number and modifications
    char* modifications[100];
    int serverNumber;

    char message[256];
    char fileContents[5000];


    if(day == 1){
        if ((fptr = fopen("Day1.txt", "r")) == NULL) {
            printf("Error! Could not open Day1.txt - DisplayReservation");
            exit(1);
        }

        //Store all the information of the current customer
        while(fscanf(fptr, "%s\t%[^\n^\t]%*c\t%s\t%s\t%d\t%d\t%d\t%s\n", ticketNumber, name, DOB, gender, &idNumber, &numOfTravelers, &serverNumber, modifications) != EOF){
            if(!strcmp(ticketNumber, input)){
                match = 1;
                break;
            }
        }

        if(match == 1){
            if ((fptr1 = fopen("Seats1_History.txt", "r")) == NULL) {
                printf("Error! Could not open Seats1_History.txt - DisplayReservation");
                exit(1);
            }

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\n\tRESERVATION INFO\n");
            strcat(fileContents, message);
            sendMessage(message, clientSocket);

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nTicket Number: \t%s", &ticketNumber);
            strcat(fileContents, message);
            sendMessage(message, clientSocket);

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nName:\t\t%s", &name);
            strcat(fileContents, message);
            sendMessage(message, clientSocket);

            memset(&message, '\0', sizeof(message));
            printf("\n\n-------DOB: %s------\n\n", DOB);
            sprintf(message, "\nDOB:\t\t%s", &DOB);
            strcat(fileContents, message);
            sendMessage(message, clientSocket);

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nGender:\t\t%s", &gender);
            strcat(fileContents, message);
            sendMessage(message, clientSocket);

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nID Number:\t%d", idNumber);
            strcat(fileContents, message);
            sendMessage(message, clientSocket);

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nNum of Travels: %d", numOfTravelers);
            strcat(fileContents, message);
            sendMessage(message, clientSocket);

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nDay:\t\t%d", day);
            strcat(fileContents, message);
            sendMessage(message, clientSocket);

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nSeats:\t\t");
            strcat(fileContents, message);
            sendMessage(message, clientSocket);


            while(fscanf(fptr1, "%s\t%d\n", ticketNumber, &seat) != EOF){
                if(!strcmp(ticketNumber, input)){
                    memset(&message, '\0', sizeof(message));
                    sprintf(message, "%d ", seat);
                    strcat(fileContents, message);
                    sendMessage(message, clientSocket);

                }
            }

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nServer:\t\t%d", serverNumber);
            sendMessage(message, clientSocket);

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nModifications:\t%s", modifications);
            sendMessage(message, clientSocket);

            sendFile(fileContents, ticketNumber, clientSocket);
        }

        else {
            memset(&message, '\0', sizeof(message));
            sprintf(message, "Something went wrong with the reservation...");
            sendMessage(message, clientSocket);

        }

        fclose(fptr);
        fclose(fptr1);
    }

    else if(day == 2){
        if ((fptr = fopen("Day2.txt", "r")) == NULL) {
            printf("Error! Could not open Day2.txt - DisplayReservation");
            exit(1);
        }

        //Store all the information of the current customer
        while(fscanf(fptr, "%s\t%[^\n^\t]%*c\t%s\t%s\t%d\t%d\t%d\t%s\n", ticketNumber, name, DOB, gender, &idNumber, &numOfTravelers, &serverNumber, modifications)){
            if(!strcmp(ticketNumber, input)){
                match = 1;
                break;
            }
        }

        if(match == 1){
            if ((fptr1 = fopen("Seats2_History.txt", "r")) == NULL) {
                printf("Error! Could not open Seats2_History.txt - DisplayReservation");
                exit(1);
            }

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\n\tRESERVATION INFO\n");
            sendMessage(message, clientSocket);

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nTicket Number: \t%s", &ticketNumber);
            sendMessage(message, clientSocket);

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nName:\t\t%s", &name);
            sendMessage(message, clientSocket);

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nDOB:\t\t%s", &DOB);
            sendMessage(message, clientSocket);

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nGender:\t\t%s", &gender);
            sendMessage(message, clientSocket);

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nID Number:\t%d", idNumber);
            sendMessage(message, clientSocket);

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nNum of Travels: %d", numOfTravelers);
            sendMessage(message, clientSocket);

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nDay:\t\t%d", day);
            sendMessage(message, clientSocket);

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nSeats:\t\t");
            sendMessage(message, clientSocket);

            while(fscanf(fptr1, "%s\t%d\n", ticketNumber, &seat) != EOF){
                if(!strcmp(ticketNumber, input)){
                    memset(&message, '\0', sizeof(message));
                    sprintf(message, "%d ", seat);
                    sendMessage(message, clientSocket);

                }
            }

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nServer:\t\t%d", serverNumber);
            sendMessage(message, clientSocket);

            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nModifications:\t%s", modifications);
            sendMessage(message, clientSocket);

        }

        else {
            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nSomething went wrong with the reservation...");
            sendMessage(message, clientSocket);

        }

        fclose(fptr);
        fclose(fptr1);
    }

    else {
        memset(&message, '\0', sizeof(message));
        sprintf(message, "\nNot a valid travel day");
        sendMessage(message, clientSocket);

    }
}

//Write
void ChangeTravelDay(int day, char input[], int numTravelers, int clientSocket){
    //sprintf(message, "\nDebug: Calling from ChangeTravel day");
    //sprintf(message, "\nValues received: \nDay: %d TicketNumber: %s Travelers: %d", day, input, numTravelers);

    FILE *fptr1, *fptr2, *fptr3, *fptr4, *fptr5;
    int currentSeat;
    int availableSeats;
    int seat;

    char ticketNumber[10];
    char *name[500];
    char *DOB[10];
    char *gender[10];
    int idNumber;
    int numOfTravelers;

    //Added serverNumber and modifcations
    int serverNumber;
    char *modifications[100];

    char message[256];


    if(day == 1){
        availableSeats = 20;
        //Open available seats file for the other day
        if ((fptr2 = fopen("Seats2.txt", "r")) == NULL) {
            printf("\nError! Could not open Seats2.txt - ChangeTravelDay");
            exit(1);
        }

        //Check how many seats are still available
        while(fscanf(fptr2, "%d\n", &currentSeat) != EOF){
            //If seat is taken
            if(currentSeat == 0){
                availableSeats--;
            }
        }

        fclose(fptr2);

        //sprintf(message, "\nDebug available Seats: %d", availableSeats);

        if(availableSeats < numTravelers){
            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nSorry! There are not enough seats for the other day. Couldn't change days");
            sendMessage(message, clientSocket);
        }

        else {
            //Open file were reservation was originally made
            if ((fptr1 = fopen("Day1.txt", "r")) == NULL) {
                printf("\nError! Could not open Day1.txt - ChangeTravelDay");
                exit(1);
            }

            //while(fscanf(fptr1, "%s\t%[^\n^\t]%*c\t%s\t%s\t%d\t%d\t%d\t%s\n", ticketNumber, name, DOB, gender, &idNumber, &numOfTravelers, &serverNumber, modifications) != EOF){
            //Look for original reservation
            while(fscanf(fptr1, "%s\t%[^\n^\t]%*c\t%s\t%s\t%d\t%d\t%d\t%s\n", ticketNumber, name, DOB, gender, &idNumber, &numOfTravelers, &serverNumber, modifications) != EOF){
                //When ticket is found
                if(!strcmp(ticketNumber, input)){
                    //Open other day file
                    FILE *f = fopen("Day2.txt", "a");
                    if (f == NULL){
                        printf("Error opening Day2.txt file! - Change Travel day (day 1)\n");
                        exit(1);
                    }
                    //Print reservation into other file
                    fprintf(f, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\r\n", ticketNumber, name, DOB, gender, idNumber, numOfTravelers, serverNumber, modifications);
                    fclose(f);
                    break;
                }
            }

            //REMOVING RESERVATION FROM INITIAL FILE
            //Set pointer of day 1 to the beginning
            rewind(fptr1);

            //Open tmp file
            fptr3 = fopen("tmp6.txt", "w");

            //Scan for ticket number
            while(fscanf(fptr1, "%s\t%[^\n^\t]%*c\t%s\t%s\t%d\t%d\t%d\t%s\n", ticketNumber, name, DOB, gender, &idNumber, &numOfTravelers, &serverNumber, modifications) != EOF){
                //When ticket is found
                if(!strcmp(ticketNumber, input)){
                    //Do nothing
                }
                //Print the current information into tmp file
                else {
                    fprintf(fptr3, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\r\n", ticketNumber, name, DOB, gender, idNumber, numOfTravelers, serverNumber, modifications);
                }
            }

            fclose(fptr3);
            fclose(fptr1);
            remove("Day1.txt");
            rename("tmp6.txt", "Day1.txt");

            //REMOVE SEATS FROM ORIGINAL INITIAL SEATS FILE
            GiveSeatsBack(day, input);
            RemoveSeats(day, input);

            //Switching days
            day = 2;

            //ALLOW USER TO PICK NEW SEATS - IN DAY 2
            SeatsAvailable(numOfTravelers, ticketNumber, day, clientSocket);
            DisplayReservation(day, input, clientSocket);
        }
    }

    else if(day == 2){
        availableSeats = 20;
        //Open available seats file for the other day
        if ((fptr2 = fopen("Seats1.txt", "r")) == NULL) {
            printf("\nError! Could not open Seats1.txt - ChangeTravelDay");
            exit(1);
        }

        //Check how many seats are still available
        while(fscanf(fptr2, "%d\n", &currentSeat) != EOF){
            //If seat is taken
            if(currentSeat == 0){
                availableSeats--;
            }
        }

        fclose(fptr2);

        //sprintf(message, "\nDebug available Seats: %d", availableSeats);

        if(availableSeats < numTravelers){
            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nSorry! There are not enough seats for the other day. Couldn't change days");
            sendMessage(message, clientSocket);

        }

        else {
            //Open file were reservation was originally made
            if ((fptr1 = fopen("Day2.txt", "r")) == NULL) {
                printf("\nError! Could not open Day1.txt - ChangeTravelDay");
                exit(1);
            }

            //Look for original reservation
            while(fscanf(fptr1, "%s\t%[^\n^\t]%*c\t%s\t%s\t%d\t%d\t%d\t%s\n", ticketNumber, name, DOB, gender, &idNumber, &numOfTravelers, &serverNumber, modifications) != EOF){
                //When ticket is found
                if(!strcmp(ticketNumber, input)){
                    //Open other day file
                    FILE *f = fopen("Day1.txt", "a");
                    if (f == NULL){
                        printf("Error opening Day1.txt file! - Change Travel day (day 1)\n");
                        exit(1);
                    }
                    //Print reservation into other file
                    fprintf(f, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\r\n", ticketNumber, name, DOB, gender, idNumber, numOfTravelers, serverNumber, modifications);
                    fclose(f);
                    break;
                }
            }

            //REMOVING RESERVATION FROM INITIAL FILE
            //Set pointer of day 1 to the beginning
            rewind(fptr1);

            //Open tmp file
            fptr3 = fopen("tmp7.txt", "w");

            //Scan for ticket number
            while(fscanf(fptr1, "%s\t%[^\n^\t]%*c\t%s\t%s\t%d\t%d\t%d\t%s\n", ticketNumber, name, DOB, gender, &idNumber, &numOfTravelers, &serverNumber, modifications) != EOF){
                //When ticket is found
                if(!strcmp(ticketNumber, input)){
                    //Do nothing
                }
                //Print the current information into tmp file
                else {
                    fprintf(fptr3, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\r\n", ticketNumber, name, DOB, gender, idNumber, numOfTravelers, serverNumber, modifications);
                }
            }

            fclose(fptr3);
            fclose(fptr1);
            remove("Day2.txt");
            rename("tmp7.txt", "Day2.txt");

            //REMOVE SEATS FROM ORIGINAL INITIAL SEATS FILE
            GiveSeatsBack(day, input);
            RemoveSeats(day, input);

            //Switching days
            day = 1;

            //ALLOW USER TO PICK NEW SEATS - IN DAY 2
            SeatsAvailable(numOfTravelers, ticketNumber, day, clientSocket);
            DisplayReservation(day, input, clientSocket);
        }
    }
}

//Write
void ChangeNumberTravelers(int day,char input[], int numOfTravelersModified, int operation, int clientSocket){
    FILE *fptr1, *fptr2, *fptr3;

    char ticketNumber[10];
    char *name[500];
    char *DOB[10];
    char *gender[10];
    int idNumber;
    int numOfTravelers;

    int currentSeat;
    int availableSeats;
    int seat;
    int valid = 0;


    char message[256];

    //Added server number and modifications
    int serverNumber;
    char *modifications[100];


    if(day == 1){
        //If trying to add, make sure there are enough seats available
        if(operation == 1){
            availableSeats = 20;
            //Open available seats file
            if ((fptr1 = fopen("Seats1.txt", "r")) == NULL) {
                printf("\nError! Could not open Seats1.txt - ChangeTravelDay");
                exit(1);
            }

            //Check how many seats are still available
            while(fscanf(fptr1, "%d\n", &currentSeat) != EOF){
                //If seat is taken
                if(currentSeat == 0){
                    availableSeats--;
                }
            }
            fclose(fptr1);

            if(availableSeats < numOfTravelersModified){
                memset(&message, '\0', sizeof(message));
                sprintf(message, "There are not enough seats to complete your request. Please try again.");
                sendMessage(message, clientSocket);

            }

            else {
                //Modify reservation
                if ((fptr2 = fopen("Day1.txt", "r")) == NULL) {
                    printf("\nError! Could not open Day1.txt - Modify Reservation Add");
                    exit(1);
                }

                fptr3 = fopen("tmp8.txt", "w");

                while(fscanf(fptr2, "%s\t%[^\n^\t]%*c\t%s\t%s\t%d\t%d\t%d\t%s\n", ticketNumber, name, DOB, gender, &idNumber, &numOfTravelers, &serverNumber, modifications) != EOF){
                    if(!strcmp(ticketNumber, input)){
                        //Change number of travelers
                        fprintf(fptr3, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\r\n", ticketNumber, name, DOB, gender, idNumber, numOfTravelers + numOfTravelersModified, serverNumber, modifications);
                    }

                    else {
                        fprintf(fptr3, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\r\n", ticketNumber, name, DOB, gender, idNumber, numOfTravelers, serverNumber, modifications);
                    }
                }

                fclose(fptr3);
                fclose(fptr2);
                remove("Day1.txt");
                rename("tmp8.txt", "Day1.txt");

                //Allow user to pick new seats
                SeatsAvailable(numOfTravelersModified, input, day, clientSocket);
                DisplayReservation(day, input, clientSocket);
            }
        }

        else if(operation == -1){
            //Open original reservation
            if ((fptr1 = fopen("Day1.txt", "r")) == NULL) {
                printf("\nError! Could not open Day1.txt - Modify Reservation Remove");
                exit(1);
            }

            fptr2 = fopen("tmp9.txt", "w");

            while(fscanf(fptr1, "%s\t%[^\n^\t]%*c\t%s\t%s\t%d\t%d\t%d\t%s\n", ticketNumber, name, DOB, gender, &idNumber, &numOfTravelers, &serverNumber, modifications) != EOF){
                if(!strcmp(ticketNumber, input)){
                    if((numOfTravelers - numOfTravelersModified) > 0){
                        fprintf(fptr2, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\r\n", ticketNumber, name, DOB, gender, idNumber, numOfTravelers - numOfTravelersModified, serverNumber, modifications);
                        valid = 1;
                    }
                    else {
                        fprintf(fptr2, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\r\n", ticketNumber, name, DOB, gender, idNumber, numOfTravelers, serverNumber, modifications);
                        memset(&message, '\0', sizeof(message));
                        sprintf(message, "\nCouldn't modify reservation. Can't remove more seats than the ones that were originally purchased.");
                        sendMessage(message, clientSocket);
                    }
                }

                else {
                    fprintf(fptr2, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\r\n", ticketNumber, name, DOB, gender, idNumber, numOfTravelers, serverNumber, modifications);
                }
            }
            fclose(fptr2);
            fclose(fptr1);
            remove("Day1.txt");
            rename("tmp9.txt", "Day1.txt");

            if(valid == 1){
                //Remove as many seats as requested from Seats1_History
                GiveSeatsBack(day, input);
                RemoveSeats(day, input);

                //ALLOW USER TO PICK NEW SEATS
                SeatsAvailable(numOfTravelers - numOfTravelersModified, ticketNumber, day, clientSocket);
                DisplayReservation(day, input, clientSocket);
            }
        }

        else {
            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nNot valid operation");
            sendMessage(message, clientSocket);

        }
    }

    else if (day == 2){
        //If trying to add, make sure there are enough seats available
        if(operation == 1){
            availableSeats = 20;
            //Open available seats file
            if ((fptr1 = fopen("Seats2.txt", "r")) == NULL) {
                printf("\nError! Could not open Seats2.txt - ChangeTravelDay");
                exit(1);
            }

            //Check how many seats are still available
            while(fscanf(fptr1, "%d\n", &currentSeat) != EOF){
                //If seat is taken
                if(currentSeat == 0){
                    availableSeats--;
                }
            }
            fclose(fptr1);

            if(availableSeats < numOfTravelersModified){
                memset(&message, '\0', sizeof(message));
                sprintf(message, "There are not enough seats to complete your request. Please try again.");
                sendMessage(message, clientSocket);

            }

            else {
                //Modify reservation
                if ((fptr2 = fopen("Day2.txt", "r")) == NULL) {
                    printf("\nError! Could not open Day2.txt - Modify Reservation Add");
                    exit(1);
                }

                fptr3 = fopen("tmp8.txt", "w");

                while(fscanf(fptr2, "%s\t%[^\n^\t]%*c\t%s\t%s\t%d\t%d\t%d\t%s\n", ticketNumber, name, DOB, gender, &idNumber, &numOfTravelers, &serverNumber, modifications) != EOF){
                    if(!strcmp(ticketNumber, input)){
                        //Change number of travelers
                        fprintf(fptr3, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\r\n", ticketNumber, name, DOB, gender, idNumber, numOfTravelers + numOfTravelersModified, serverNumber, modifications);
                    }

                    else {
                        fprintf(fptr3, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\r\n", ticketNumber, name, DOB, gender, idNumber, numOfTravelers, serverNumber, modifications);
                    }
                }

                fclose(fptr3);
                fclose(fptr2);
                remove("Day2.txt");
                rename("tmp8.txt", "Day2.txt");

                //Allow user to pick new seats
                SeatsAvailable(numOfTravelersModified, input, day, clientSocket);
                DisplayReservation(day, input, clientSocket);
            }
        }

        else if(operation == -1){
            //Open original reservation
            if ((fptr1 = fopen("Day2.txt", "r")) == NULL) {
                printf("\nError! Could not open Day1.txt - Modify Reservation Remove");
                exit(1);
            }

            fptr2 = fopen("tmp9.txt", "w");

            while(fscanf(fptr1, "%s\t%[^\n^\t]%*c\t%s\t%s\t%d\t%d\t%d\t%s\n", ticketNumber, name, DOB, gender, &idNumber, &numOfTravelers, &serverNumber, modifications) != EOF){
                if(!strcmp(ticketNumber, input)){
                    if((numOfTravelers - numOfTravelersModified) > 0){
                        fprintf(fptr2, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\r\n", ticketNumber, name, DOB, gender, idNumber, numOfTravelers - numOfTravelersModified, serverNumber, modifications);
                        valid = 1;
                    }
                    else {
                        fprintf(fptr2, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\r\n", ticketNumber, name, DOB, gender, idNumber, numOfTravelers, serverNumber, modifications);
                        memset(&message, '\0', sizeof(message));
                        sprintf(message, "\nCouldn't modify reservation. Can't remove more seats than the ones that were originally purchased.");
                        sendMessage(message, clientSocket);

                    }
                }

                else {
                    fprintf(fptr2, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\r\n", ticketNumber, name, DOB, gender, idNumber, numOfTravelers, serverNumber, modifications);
                }
            }
            fclose(fptr2);
            fclose(fptr1);
            remove("Day2.txt");
            rename("tmp9.txt", "Day2.txt");

            if(valid == 1){
                //Remove as many seats as requested from Seats1_History
                GiveSeatsBack(day, input);
                RemoveSeats(day, input);

                //ALLOW USER TO PICK NEW SEATS
                SeatsAvailable(numOfTravelers - numOfTravelersModified, ticketNumber, day, clientSocket);
                DisplayReservation(day, input, clientSocket);
            }
        }

        else {
            memset(&message, '\0', sizeof(message));
            sprintf(message, "\nNot valid operation");
            sendMessage(message, clientSocket);

        }

    }
    else {
        printf("\nDebug: Not a valid day to travel - Change number of travelers");

    }
}

