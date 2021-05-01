//To run: gcc -pthread -o start Dax_MasterServer.c Dax_Queue.c
//        ./start


#include "Definitions.h" //needed for all user files
//#include <wait.h> //UNCOMMENT THIS INLCUDE IF USING LINUX

//prototypes for server use only
void server(int);
int connectionWithClient(int *, int *);
void * threadFunc(void *);
void * masterThread(void *);

#define BASEPORT 2220 //base port all server will work off of
#define NUM_OF_SERVERS 5 //num of servers
#define THREAD_NUM 5 //num of threads for each server

pthread_mutex_t lock[NUM_OF_SERVERS]; //mutex lock for each server

priorityPackage priorityList[THREAD_NUM * NUM_OF_SERVERS]; // list that reserves a spot for each possible thread for priority

//struct needed to send info to threads
struct threadPackage{
    struct queue *q;
    int serverNum;
};




//==========================================
//              Master Server
//==========================================

/*
    Main Server execution: Sets up every thing needed for server actions.
*/
int main()
{
    printf("Master Server starting...\n\n");


    readerCount = 0; // intializes reader count

    int i = 0;
    pid_t pid;

    //set up semaphores
    sem_unlink(WRITE);
    sem_unlink(READ);

    write_sem = sem_open(WRITE, IPC_CREAT, 0660, 1);
    read_sem = sem_open(READ, IPC_CREAT, 0660, 1);

    //intialize mutex locks
    for(i = 0; i < NUM_OF_SERVERS; i++)
    {
        if (pthread_mutex_init(&lock[i], NULL) != 0)
            printf("Mutex init failed!\n");
    }

    //intialize mutex lock for prioirty list
    if (pthread_mutex_init(&pq, NULL) != 0)
            printf("Mutex init failed!\n");

    //create thread to handle priority
    pthread_t mstrthr;
    pthread_create(&mstrthr, NULL, masterThread, NULL);

    //create servers
    for(i = 0; i < NUM_OF_SERVERS; i++)
    {
        pid = fork();

        if(pid == 0)
            server(i);
    }

    //wait until all children are done
    while(wait(NULL) != -1){;}

    //close semaphores
    sem_close(write_sem);
    sem_close(read_sem);

}




//==========================================
//              Servers
//==========================================

/*
     void server(): Method ran by children to act as servers. Sets up TCP sockets
     and accepts clients. Creates a thread pool that will take connections.
*/
void server(int portAdd)
{
    printf("Server %d starting...\n\n", portAdd + 1);

    //----------------------------
    //Create Thread Pool and Queue
    //----------------------------

    //prepare thread package
    struct threadPackage *tp;
    struct threadPackage package;
    tp = &package;

    pthread_t *pool = malloc(sizeof(pthread_t) * THREAD_NUM); //declare thread array
    struct queue *q = createQueue(); // create queue

    tp->serverNum = portAdd; //set server num
    tp->q = q; // set queue reference

    // run all threads in pool
    for(int i = 0; i < THREAD_NUM; i++)
    {
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
        newSocket = accept(sockfd, (struct sockaddr *)&newAddr, &addr_size); //wait for clients
        if(newSocket < 0)
        {
            printf("Error for new socket\n");
            exit(1);
        }

        printf("Connection Accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

        int *pclient = malloc(sizeof(int)); // pass socket info to new variable in heap
        *pclient = newSocket;

        // add client to queue in controlled lock
        pthread_mutex_lock(&lock[portAdd]);
        enqueue(q, pclient);
        pthread_mutex_unlock(&lock[portAdd]);
    }


    exit(0);
}





//==========================================
//              Thread Jobs
//==========================================

/*
     void * threadFunc(void *package): Method ran by threads in a pool. Infinitely run checking for new things in queue.
     On client connection, continue on to handling client.

     Parameters:
        void *package: Package sent by Server holding needed info
*/
void * threadFunc(void *package)
{
    printf("Thread Created!\n");

    struct threadPackage *tp;
    tp = (struct threadPackage *)package; // Cast void * into threadPackage

    int *socket;
    int s;
    socket = &s;
    int *serverNumber;

    int val = tp->serverNum;
    serverNumber = &val;

    //printf("\nSERVER NO:%d", tp->serverNum);

    while(1)
    {
        //Attempt taking connection off queue in controlled access
        pthread_mutex_lock(&lock[tp->serverNum]);
        socket = dequeue(tp->q);
        pthread_mutex_unlock(&lock[tp->serverNum]);


        if(*socket != -1) //check if anything was pulled off
        {
            connectionWithClient(socket, serverNumber); // execute handling with clients
        }
        free(socket); //Free socket since it's not needed

    }

    return NULL;
}


int connectionWithClient(int *s, int *sN)
{
    int clientSocket = *s;
    int serverNumber = *sN;

    // intialize menu
    char menu[] = "\n\n\tMENU\n1. Make a reservation.\n2. Inquiry about the ticket.\n3. Modify the reservation.\n4. Cancel the reservation.\n5. Exit the program\n\nOption:\n";
    char *userChoice; //holds user response
    int choice; // conversion of user response
    char message[256]; // buffer to hold message

    while(1)
    {
        sendMessage(menu, clientSocket); // Send menu

        userChoice = clientInput(clientSocket); // get user input

        choice = atoi(userChoice); //conversion of user choice from string to int

        switch (choice) // dictates action based on user choice
        {
             case 1:
                MakeReservation(clientSocket, serverNumber); // execute making reservation
                break;

            case 2:
                InquiryTicket(clientSocket); //executing inquiry
                break;

            case 3:
                ModifyReservation(clientSocket); // execute modifying reservation
                break;

            case 4:
                CancelReservation(clientSocket); // cancel reservation
                break;
            case 5:
                sendMessage("\n\nGoodbye!\n", clientSocket); // say goodbye to client
                sendMessage("exit", clientSocket); // confirm with user about exit
                return 0;
            default:
                sendMessage("Please enter a valid option.\n", clientSocket); // tell client about wrong choice
                break;
        }

    }

    printf("Now leaving...\n\n");

    close(clientSocket); // close client socket

    return 1;
}




//=============================================
//            Server Communications
//=============================================

/*
    void sendMessage(char *, int): Used to accept string and send to client in uniform design

    Parameters:
        message: The message to be sent to client
        clientSocket: Socket to be sent to
*/
void sendMessage(char *message, int clientSocket)
{
    if(send(clientSocket, message, 256, 0) == -1) //send message
    {
        printf("\n\n[-]Something Failed sending message!\n\n");
    }
    printf("Sent: %s\n\n", message);
}

/*
    char * clientInput(int): Method to easily request client input and return response

    Parameters:
        clientSocket: Socket to be swork with
*/
char * clientInput(int clientSocket)
{
    char *clientResponse = malloc(sizeof(char) * 256); // prepare message to be returned

    sendMessage("input", clientSocket); // tell client servers is asking for input


    if(recv(clientSocket, clientResponse, 256, 0)  == -1) //recieve client response
    {
        printf("\n\n[-]Something Failed Recieving!\n\n");
        return NULL;
    }

    // Remove \n if it exist
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

/*
    void sendFile(char *, char *, int): Used to send client a file

    Parameters:
        contents: Contents of file to be sent
        name: name of file for client
        clientSocket: Socket to be used
*/
void sendFile(char *contents, char *name, int clientSocket)
{
    sendMessage("file", clientSocket); //tell client tp prepare for file
    char fileName[100];
    sprintf(fileName, "%sReciept.txt", name); // set up file name
    sendMessage(fileName, clientSocket); //send file name to client

    if(send(clientSocket, contents, 5000, 0) == -1) //send file to client
    {
        printf("\n\n[-]Something Failed sending message!\n\n");
    }
}


//===============================
//       Priority Methods
//===============================

/*
    void enterQueue(int, char *): Enters calling thread into an array based on num of
    passengers as priority and creates semaphore based on thread provided code.

    Parameters:
        numOfTickets: number of tickets thread has, used as priority
        code: thread provided code to use as semaphore code
*/
void enterQueue(int numOfTickets, char *code)
{
    printf("---MADE IT--");
    pthread_mutex_lock(&pq); //accessing list so don't allow anyone else

    for(int i = 0; i < (THREAD_NUM * NUM_OF_SERVERS); ++i) // go through array to find first available slot
    {
        if(priorityList[i].code == NULL) // check if slot is null
        {
            priorityList[i].code = code; // store info
            priorityList[i].priority = numOfTickets;
        }
    }
    //create semaphore based on code
    sem_t *createsemaphore;
    sem_unlink(code);
    createsemaphore = sem_open(WRITE, IPC_CREAT, 0660, 0);

    pthread_mutex_unlock(&pq);
}

/*
    void masterThread(void *): Over arching thread that checks list for threads waiting to
    make a reservation. Checks which thread has the highest priority and then opens semaphore
    based on its code. Ups the priority on all unchosen threads to prevent starvation.
*/
void * masterThread(void *p)
{
    int max = 0; // used to keep track of max priority
    int index = 0; // index of highest priority thread

    while(1)
    {
        max = 0; // reset variables
        index = 0;

        pthread_mutex_lock(&pq);


        for(int i = 0; i < (THREAD_NUM * NUM_OF_SERVERS); ++i) // check all slots in array
        {
            if(priorityList[i].code != NULL && priorityList[i].priority > max) //check if not null and is greater than max
            {
                index = i; // set index
                max = priorityList[i].priority; // set max priority
            }
        }

        if(max > 0) // check if there was anything in queue
        {
            //open semaphore based on code of highest priority
            sem_t *opensem;
            opensem = sem_open(priorityList[index].code, 0, 0660, 0);
            sem_post(opensem);

            //set that index to null
            priorityList[index].code = NULL;
            priorityList[index].priority = 0;

            for(int i = 0; i < (THREAD_NUM * NUM_OF_SERVERS); ++i) // go through and up each threads priority that didn't get picked
            {
                if(priorityList[i].code != NULL)
                {
                    priorityList[i].priority++;
                }
            }
        }

        pthread_mutex_unlock(&pq);
    }
}

