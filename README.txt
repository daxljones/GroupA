To run:

Server:
gcc -pthread -o start Dax_MasterServer.c Dax_Queue.c
./start

Client:
gcc -o clientStart Dax_TestClient.c
./clientStart


The program so far will create N number of servers. Each Server will create M number of threads in a pool, 
and each will busy wait checking a queue for an entry. When a client connects to a server, it will be 
placed in a queue for a thread to pick up and handle. The server will send the client a menu, the client 
will respond and send the appropriate information to the client.