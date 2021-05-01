To run:

Server:
gcc -pthread -o start Dax_MasterServer.c
./start

***MUST REMOVE COMMENT LINES OF WAIT INCLUDE FOR LINUX SYSTEMS

Client:
gcc -o clientStart Dax_Client.c
./clientStart


The program will create N number of servers. Each Server will create M number of threads in a pool, 
and each will busy wait checking a queue for an entry. When a client connects to a server, it will be 
placed in a queue for a thread to pick up and handle. The server will send the client a menu, the client 
will respond and send the appropriate information to the client. The client has multiple actions to choose from.

The server will maintain customer information for 2 days with 3 files each. All customer interactions
Include interactions with the files. 

All threads are synchronized through semaphores.

Priority for making reservations is based on which client has the most ticket orders.