//To run: gcc -pthread -o start Dax_MasterServer.c Dax_Queue.c
//        ./start


#include "Definitions.h"
#include <pthread.h>
//#include <wait.h>

void server(int);
int connectionWithClient(int *);
void * threadFunc(void *);

#define BASEPORT 2224
#define NUM_OF_SERVERS 1
#define THREAD_NUM 1


pthread_mutex_t lock[NUM_OF_SERVERS];

priorityPackage priorityList[THREAD_NUM * NUM_OF_SERVERS];

struct threadPackage{
    struct queue *q;
    int serverNum;
};




//==========================================
//              Master Server
//==========================================

int main()
{
    printf("Master Server starting...\n\n");

    
    readerCount = 0;

    int i = 0;
    pid_t pid;

    //set up semaphores
    sem_unlink(WRITE);
    sem_unlink(READ);

    write_sem = sem_open(WRITE, IPC_CREAT, 0660, 1);
    read_sem = sem_open(READ, IPC_CREAT, 0660, 1);

    for(i = 0; i < NUM_OF_SERVERS; i++)
    {
        if (pthread_mutex_init(&lock[i], NULL) != 0)
            printf("Mutex init failed!\n");
    }

    if (pthread_mutex_init(&pq, NULL) != 0)
            printf("Mutex init failed!\n");


    for(i = 0; i < NUM_OF_SERVERS; i++)
    {
        pid = fork();

        if(pid == 0)
            server(i);
    }
    

    while(wait(NULL) != -1){;} 

    sem_close(write_sem);
    sem_close(read_sem);

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
    struct threadPackage *tp;
    struct threadPackage package;
    tp = &package;    

    pthread_t *pool = malloc(sizeof(pthread_t) * THREAD_NUM);
    struct queue *q = createQueue();

    tp->serverNum = portAdd;
    tp->q = q;

    for(int i = 0; i < THREAD_NUM; i++)
    {
        pthread_attr_t attr;
        struct sched_param param;
        pthread_attr_init (&attr);
        pthread_attr_getschedparam (&attr, &param);
        pthread_attr_setschedparam (&attr, &param);

        pthread_create(&pool[i], NULL, threadFunc, tp);
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

        pthread_mutex_lock(&lock[portAdd]);
        enqueue(q, pclient);
        pthread_mutex_unlock(&lock[portAdd]);
    }

    
    exit(0);
}





//==========================================
//              Thread Jobs
//==========================================


void * threadFunc(void *package)
{
    printf("Thread Created!\n");

    struct threadPackage *tp;
    tp = (struct threadPackage *)package;
    
    int *socket;
    int s;
    socket = &s;


    
    while(1)
    {
        pthread_mutex_lock(&lock[tp->serverNum]);
        socket = dequeue(tp->q);
        pthread_mutex_unlock(&lock[tp->serverNum]);

        //printf("%d\n", *socket);
        if(*socket != -1)
        {
            connectionWithClient(socket);
        }
        free(socket);
        
    }

    return NULL;
}


int connectionWithClient(int *s)
{
    int clientSocket = *s;

    sendMessage("Sending File...\n", clientSocket);
    sendFile("It works!!\n:)", "hello", clientSocket);

    char menu[] = "\n\n\tMENU\n1. Make a reservation.\n2. Inquiry about the ticket.\n3. Modify the reservation.\n4. Cancel the reservation.\n5. Exit the program"; //Menu Needs to be declared some where to send 
    char *userChoice;
    int choice;
    char message[256];
    int num = 0;

    while(1)
    {   
        // Send menu
        sendMessage(menu, clientSocket);
        sendMessage("\n\nOption:\n", clientSocket);

        userChoice = clientInput(clientSocket);
        
        choice = atoi(userChoice); //conversion of choice from string to int

        printf("\n\nuserchoice: %d\n\n", choice);

       
        memset(message, 0, sizeof(message));
        switch (choice)
        {
             case 1:
                MakeReservation(clientSocket);
                break;

            case 2:
                InquiryTicket(clientSocket);
                break;

            case 3:
                ModifyReservation(clientSocket);
                break;

            case 4:
                CancelReservation(clientSocket);
                break;
            case 5:
                sendMessage("exit", clientSocket);
                sendMessage("\n\nGoodbye!\n", clientSocket);
                return 0;
            default:
                sendMessage("Please enter a valid option.\n", clientSocket);
                break;
        }

    }

    printf("Now leaving...\n\n");
    
    close(clientSocket);

    return 1;
}




//=============================================
//            Server Communications
//=============================================

void sendMessage(char *message, int clientSocket)
{
    if(send(clientSocket, message, 256, 0) == -1)
    {
        printf("\n\n[-]Something Failed sending message!\n\n");
    }
    printf("Sent: %s\n\n", message);
}

char * clientInput(int clientSocket)
{
    char *clientResponse = malloc(sizeof(char) * 256);

    //sleep(2);

    sendMessage("input", clientSocket);
    

    if(recv(clientSocket, clientResponse, 256, 0)  == -1) //recieve client response
    {
        printf("\n\n[-]Something Failed Recieving!\n\n");
        return -1;
    }

    char c;

    for(int i = 0; i < 256; ++i)
    {
        c = clientResponse[i];
        if(c == '\n')
        {
            clientResponse[i] = '\0';
            break;
        }
        else if(c == '\0')
        {
            break;
        }
    }

    printf("Recv: %s\n\n", clientResponse);

    return clientResponse;
}

void sendFile(char *contents, char *name, int clientSocket)
{
    sendMessage("file", clientSocket);
    char fileName[100];
    sprintf(fileName, "%sReciept.txt", name);
    printf("file name: %s\n\n", fileName);
    sendMessage(fileName, clientSocket);
    
    if(send(clientSocket, contents, 5000, 0) == -1)
    {
        printf("\n\n[-]Something Failed sending message!\n\n");
    }
}


//===============================
//       Priority Methods
//===============================


void enterQueue(int numOfTickets, pthread_t thread)
{
    int *priority;
    struct sched_param *param;

    pthread_mutex_lock(&pq);
    
    for(int i = 0; i < (THREAD_NUM * NUM_OF_SERVERS); ++i)
    {
        if(priorityList[i].tid == thread || priorityList[i].priority == 0)
        {
            priorityList[i].tid = thread;
            priorityList[i].priority = numOfTickets;

            pthread_getschedparam(thread, priority, param);
            pthread_setschedparam(thread, priorityList[i].priority, param);
        }
    }
    
    pthread_mutex_unlock(&pq);
}

void takeOut(pthread_t thread)
{
    pthread_mutex_lock(&pq);
    
    for(int i = 0; i < (THREAD_NUM * NUM_OF_SERVERS); ++i)
    {
        if(priorityList[i].priority != 0 && priorityList[i].tid == thread)
        {
            priorityList[i].priority = 0;
        }
    }

    for(int i = 0; i < (THREAD_NUM * NUM_OF_SERVERS); ++i)
    {
        int *priority;
        struct sched_param *param;

        if(priorityList[i].priority != 0)
        {
            priorityList[i].priority++;
            pthread_getschedparam(thread, priority, param);
            priorityList[i].priority = (*priority) + 1;
            pthread_setschedparam(thread, priorityList[i].priority, param);
        }
    }
    
    pthread_mutex_unlock(&pq);
}

