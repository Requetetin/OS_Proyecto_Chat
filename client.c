#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#define PORT 8080

int main (int argc, char const* argv[]) {
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char* hello = "Hello from client";
	char init_connect[1024];
	char* user = "Martin";
	time_t curr_time = time(NULL);
	snprintf(init_connect, sizeof(init_connect), "{request: INIT_CONEX, body: [connect_time: %s, user_id: %s]}", asctime(gmtime(&curr_time)), user);
	char buffer[1024] = { 0 };
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error\n");
		return -1;
	}
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		printf("\nInvalid address / Address not supported\n");
		return -1;
	}
	
	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("\nConnection Failed\n");
		return -1;
	}
	/* //Implementation by geeks for geeks, single message
	send(sock, hello, strlen(hello), 0);
	printf("Hello message sent\n");
	valread = read(sock, buffer, 1024);
	printf("%s\n", buffer);
	return 0;
	*/
	// Implementation by medium. Multiple messages
	send(sock, init_connect, strlen(init_connect), 0);
	while (1) {
		printf("Enter a message: ");
		fgets(buffer, 100, stdin);
		send(sock, buffer, strlen(hello), 0);
	}
}
