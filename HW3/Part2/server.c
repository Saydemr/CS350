/*
	C socket server example, handles multiple clients using threads
*/

#include<stdio.h>
#include<string.h>	//strlen
#include<stdlib.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<pthread.h> //for threading , link with lpthread
#include <time.h>       /* time_t, struct tm, time, localtime */

#define MAX_CLIENTS 3
#define MAX_CONCURRENT 2
#define MAX_CONNECTION_DURATION 10.0
int client_count = 0;
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
int queued_connections = 0;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;



//the thread function
void *connection_handler(void *);
void *queue_handler(void *);

int main(int argc , char *argv[])
{
	int socket_desc , client_sock , c , *new_sock;
	struct sockaddr_in server , client;
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind() done.");

	//Listen
	listen(socket_desc , 3);
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	
	while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)))
	{

		puts("Connection in triage");

		if (client_count >= MAX_CLIENTS)	 
		{
			puts("Maximum number of concurrent connections reached. Sending SIG_TERM to client");
			write(client_sock, "SIG_TERM", 8);
			close(client_sock);
		}
		else
		{
			printf("Connection #%d accepted\n", client_count+1);
			write(client_sock, "SIG_ACK", 8);
			pthread_t sniffer_thread;
			new_sock = malloc(1);
			*new_sock = client_sock;

			if(pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
			{
				perror("Could not create thread");
				return 1;
			}
		}
	}
	
	if (client_sock < 0)
	{
		perror("Accept failed");
		return 1;
	}
	
	return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{

	pthread_mutex_lock(&client_mutex);
	client_count++;
	pthread_mutex_unlock(&client_mutex);

	while(client_count > MAX_CONCURRENT)
	{
		sleep(1);
	}

	//Get the socket descriptor
	int sock = *(int*)socket_desc;
	int read_size;
	char *message , client_message[2000];
	
	struct timespec start, finish;
	double elapsed = 0.0;
	clock_gettime(CLOCK_MONOTONIC, &start);

    
	//Receive a message from client
	while (elapsed < MAX_CONNECTION_DURATION && (read_size = recv(sock , client_message , 2000 , 0)) > 0)
	{
		clock_gettime(CLOCK_MONOTONIC, &finish);
		elapsed = (finish.tv_sec - start.tv_sec);
		elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
		
		//Send the message back to client
		int client_message_size = sizeof(client_message) / sizeof(client_message[0]);
		send(sock, client_message, client_message_size , 0);
		memset(client_message, 0, 2000);

		if(read_size == 0)
		{
			puts("Client disconnected");
			fflush(stdout);
			break;
		}
		else if(read_size == -1)
		{
			perror("recv failed");
			break;
		}
		else if (elapsed > MAX_CONNECTION_DURATION)
		{
			puts("Connection timed out");
			break;
		}
	}
	
	write(sock, "SIG_TERM", 8);
	pthread_mutex_lock(&client_mutex);
	client_count--;
	pthread_mutex_unlock(&client_mutex);
	//Free the socket pointer
	free(socket_desc);
	pthread_exit(NULL);
	
	return 0;
}
