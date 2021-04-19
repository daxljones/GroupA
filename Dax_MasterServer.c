#include "Definitions.h"
#include "Queue.h"
#include <pthread.h>
//#include <wait.h>

void server(int);
int connectionWithClient(int *);
void * threadFunc(void *);

#define BASEPORT 2224
#define NUM_OF_SERVERS 1
#define THREAD_NUM 1

pthread_mutex_t lock;


//==========================================
//              Master Server
//==========================================

int main()
{
    printf("Master Server starting...\n\n");

    int i = 0;
    pid_t pid;

    if (pthread_mutex_init(&lock, NULL) != 0)
        printf("Mutex init failed!\n");

    for(i = 0; i < NUM_OF_SERVERS; i++)
    {
        pid = fork();

        if(pid == 0)
            server(i);
    }
    

    while(wait(NULL) != -1){;} 

}




//==========================================
//              Servers
//==========================================


void server(int portAdd)
{
    printf("Server %d starting...\n\n", portAdd + 1);

    //----------------------------
    //Create Thread Pool and Queue
    //----------------------------
    pthread_t *pool = malloc(sizeof(pthread_t) * THREAD_NUM);
    struct queue *q = createQueue();

    for(int i = 0; i < THREAD_NUM; i++)
    {
        pthread_create(&pool[i], NULL, threadFunc, q);
    }


    //-----------------------
    //Create Open Sockets
    //-----------------------
    int sockfd, ret;
    struct sockaddr_in serverAddr;

    int newSocket;
    struct sockaddr_in newAddr;

    socklen_t addr_size;

    sockfd =  socket(AF_INET, SOCK_STREAM, 0);
    close(sockfd);
    sockfd =  socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        printf("ERROR IN CONNECTION\n");
        exit(1);
    }
    printf("Server Socket is Created.\n");

    const int PORT = BASEPORT + portAdd;

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if(ret < 0)
    {
        printf("ERROR IN BINDING\n");
        exit(1);
    }
    printf("Bind to port %d\n", PORT);

    if(listen(sockfd, 10) == 0)
    {
        printf("Listening..\n");
    }
    else
    {
        printf(("Error in binding2\n"));
        exit(1);
    }

    //---------------------
    //Taking in Connections
    //---------------------
    printf("--Will now be accepting clients...\n\n");
    while(1)
    {
        newSocket = accept(sockfd, (struct sockaddr *)&newAddr, &addr_size);
        if(newSocket < 0)
        {
            printf("Error for new socket\n");
            exit(1);
        }
        
        printf("Connection Accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

        int *pclient = malloc(sizeof(int));
        *pclient = newSocket;

        pthread_mutex_lock(&lock);
        enqueue(q, pclient);
        pthread_mutex_unlock(&lock);
    }

    
    exit(0);
}





//==========================================
//              Thread Jobs
//==========================================


void * threadFunc(void *qp)
{
    printf("Thread Created!\n");

    struct queue *q;
    q = (struct queue *)qp;
    
    int *socket;
    int s;
    socket = &s;
    
    while(1)
    {
        pthread_mutex_lock(&lock);
        socket = dequeue(q);
        pthread_mutex_unlock(&lock);

        //printf("%d\n", *socket);
        if(*socket != -1){
            connectionWithClient(socket);
        }

        free(socket);
        
    }

    return NULL;
}


int connectionWithClient(int *s)
{
    int clientSocket = *s;
    //free(s);

    char menu[] = "Pick a number of choice:\n\n1. I'll say Hello\n2. I'll say Hey!\n3. I'll Exit\n"; //Menu Needs to be declared some where to send 
    char userChoice[256];
    int choice;
    char message[256];

    while(1)
    {
        //===================================================
        //This is the loop the menu sending/recieving will go
        //I've written a rough outline for how it should work
        //===================================================


        printf("Sending message...\n");
        
        // Send menu
        if(send(clientSocket, menu, strlen(menu), 0) == -1)
        {
            printf("\n\n[-]Something Failed sending message!\n\n");
        }
                    

        printf("Waiting to recieve...\n");
        if(recv(clientSocket, userChoice, 256, 0)  == -1) //recieve client response
        {
            printf("\n\n[-]Something Failed Recieving!\n\n");
            return -1;
        }
        

        printf("Message: %s\n", userChoice);
        choice = atoi(userChoice); //conversion of choice from string to int
        printf("now its: %d\n", choice);

        if(choice == 3)
        {
            break;
        }

        printf("Checking choice...\n");



        //=====================================================================================================
        //Instead of sending messages back- each case need to call a function to handle the selected menu task
        //=====================================================================================================
        memset(message, 0, sizeof(message));
        switch (choice)
        {
            case 1:
                strcpy(message, "Hello!\n");
                break;
            
            case 2:
                strcpy(message, "Hey!\n");
                break;
            
            default:
                strcpy(message, "Whatever you entered was wrong.\n");
                break;
        }


        if(send(clientSocket, message, strlen(message), 0)  == -1)
        {
            //printf("\n\n[-]Something Failed sending 1!\n\n");
            perror("Not working.");
            return -1;
        }
        printf("Sent my message!\n");
        
        if(recv(clientSocket, userChoice, 256, 0)  == -1) //recieve client response
        {
            printf("\n\n[-]Something Failed Recieving!\n\n");
            return -1;
        }

    }

    printf("Now leaving...\n\n");
    
    close(clientSocket);

    return 1;
}
