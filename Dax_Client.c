//to run: gcc -o clientStart Dax_TestClient.c
//        ./clientStart

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 2220 //port to connect to server

void readFile(int);

int main()
{
    //Set up socket connections
    int clientSocket, ret;
    struct sockaddr_in serverAddr;


    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    close(clientSocket);
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocket < 0)
    {
        printf("ERROR IN CONNECTION");
        exit(1);
    }
    printf("Socket Created!\n");

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ret = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if(ret < 0)
    {
        printf("ERROR IN CONNECTION2");
        exit(1);
    }
    printf("CONNECTED TO SERVER\n\n");

    char input[256]; // holds user input
    char re[256]; // holds recieved messages
    int recieved = 0; //holds message size


    while(1)
    {
        while(1) // loop to recieve until told otherwise instructions
        {
            if((recieved = recv(clientSocket, re, sizeof(re), 0))  > 0) //recieve server message
            {
                if(strcmp(re, "input") == 0 || strcmp(re, "exit") == 0 || strcmp(re, "file") == 0) // if server sends special signal break
                {
                    break;
                }

                fflush(stdout);
                printf("%s", re); // print message
            }
        }

        if(strcmp(re, "exit") == 0) // if keyword is exit, leave
        {
            printf("\n\n[+] Client Closing.\n\n");
            return 0;
        }
        else if(strcmp(re, "file") == 0) // if keyword is file, prepare to accept file
        {
            readFile(clientSocket);
            continue;

        }

        printf("\n");

        fgets(input, sizeof(input), stdin); // user to enter input

        
        if(send(clientSocket, input, strlen(input) + 1, 0) == -1) // send to server
        {
            printf("\n\n[-]Something Failed sending message!\n\n");
        }

        printf("\nSending...\n");

    }

    return 0;
}


/*
    void readFile(int): special case in order to accept file and save it

    Parameters:
        clientSocket: socket to use
*/
void readFile(int clientSocket)
{   
    char fileName[256]; // stores file name

    if(recv(clientSocket, fileName, sizeof(fileName), 0)  == -1) //recieve file name
    {
        printf("\n\n[-]Something Failed Recieving!\n\n");
        exit(0);
    }


    char fileBuffer[5000]; // stores file contents


    if(recv(clientSocket, fileBuffer, sizeof(fileBuffer), 0)  == -1) //recieve file contents
    {
        printf("\n\n[-]Something Failed Recieving!\n\n");
        exit(0);
    }

    

    FILE *reciept; // file pointer

    reciept = fopen(fileName, "w+"); // creat new file based on file name from server

    fwrite(fileBuffer, sizeof(char), strlen(fileBuffer) + 1, reciept); // write contents to file
    fclose(reciept); // close file
}