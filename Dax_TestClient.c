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

#define PORT 2225

void readFile(int);

int main()
{
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

    char input[256];
    char re[256];
    int recieved = 0;

    while(1)
    {
        while(1)
        {
            if((recieved = recv(clientSocket, re, sizeof(re), 0))  > 0) //recieve client response
            {
                if(strcmp(re, "input") == 0 || strcmp(re, "exit") == 0 || strcmp(re, "file") == 0) //Server needs user input
                {
                    break;
                }

                fflush(stdout);
                printf("%s", re);
            }
        }

        if(strcmp(re, "exit") == 0)
        {
            printf("\n\n[+] Client Closing.\n\n");
            return 0;
        }
        else if(strcmp(re, "file") == 0)
        {
            readFile(clientSocket);
            continue;

        }

        printf("\n");

        fgets(input, sizeof(input), stdin);

        printf("\ngoing to send.\n");
        
        if(send(clientSocket, input, strlen(input) + 1, 0) == -1)
        {
            printf("\n\n[-]Something Failed sending message!\n\n");
        }

        printf("\nSending...\n");

    }

    return 0;
}


void readFile(int clientSocket)
{   
    char fileName[256];

    if(recv(clientSocket, fileName, sizeof(fileName), 0)  == -1) //recieve client response
    {
        printf("\n\n[-]Something Failed Recieving!\n\n");
        exit(0);
    }


    char fileBuffer[5000];


    if(recv(clientSocket, fileBuffer, sizeof(fileBuffer), 0)  == -1) //recieve client response
    {
        printf("\n\n[-]Something Failed Recieving!\n\n");
        exit(0);
    }

    

    FILE *reciept;

    reciept = fopen(fileName, "w+");

    fwrite(fileBuffer, sizeof(char), strlen(fileBuffer) + 1, reciept);
    fclose(reciept);
}