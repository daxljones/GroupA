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

#define PORT 2224

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

    while(1)
    {
        while(1)
        {
            memset(re, '\0', sizeof(re));
            //printf("\n[-]Waiting to recive next thing...\n");
            if(recv(clientSocket, re, 256, 0)  == -1) //recieve client response
            {
                printf("\n\n[-]Something Failed Recieving!\n\n");
                exit(0);
            }

            fflush(stdout);
            printf("%s", re);

            if(strcmp(re, "input") == 0) //Server needs user input
            {
                break;
            }

            
        }

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