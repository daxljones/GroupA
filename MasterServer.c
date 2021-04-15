#include "Definitions.h"
#include "Queue.h"
#include <pthread.h>

void server(int);
int connectionWithClient(int *);
void * threadFunc(void *);

#define BASEPORT 2223
#define NUM_OF_SERVERS 1
#define THREAD_NUM 5

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

        /*pthread_t t;
        int *pclient = malloc(sizeof(int));
        *pclient = newSocket;
        pthread_create(&t, NULL, connectionWithClient, pclient);*/
    }

    
    exit(0);
}





//==========================================
//              Thread Jobs
//==========================================


void * threadFunc(void *qp)
{
    printf("Thread Created!\n");

    struct queue *q = (struct queue *)qp;
    

    int *socket;
    int s = -1;
    socket = &s;

    
    while(1)
    {
        pthread_mutex_lock(&lock);
        socket = dequeue(q);
        pthread_mutex_unlock(&lock);

        if(*socket != -1)
            connectionWithClient(socket);
        
    }

    return NULL;
}


int connectionWithClient(int *s)
{
    int clientSocket = *s;
    free(s);

    char buffer[256];

    while(1)
    {
        memset(buffer, 0, sizeof(buffer));
        if(recv(clientSocket, buffer, 256, 0)  == -1)
        {
            printf("\n\nBalls\n\n");
            return -1;
        }
        
        fflush(stdout);

        printf("Message: %s", buffer);

        if(strcmp(buffer, "exit\n") == 0)
        {
            break;
        }

        char balls[256];
        strcpy(balls, "I got your message!");

        send(clientSocket, balls, strlen(balls), 0);
    }

    close(clientSocket);

    return 1;
}
