#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

int sock, exit_flag;
char* user;

void trim_string(char* arr, int length) {
	int i;
	for (i=0; i<length; i++){
		if(arr[i] == '\n') {
			arr[i] = '\0';
			break;
		}
	}
}

void* inputs() {

}

void* outputs() {
	char buffer[1024] = {};
	time_t hour;
	char message[1024];
	char* recipient = "all";
	
	while (1) {
		printf("Enter a message: ");
		fgets(buffer, 1024, stdin);
		trim_string(buffer, 1024);
		if (strcmp(buffer, "exit") == 0) {
			exit_flag = 1;
			char exit_connect[1024] = "{request: END_CONEX}";
			send(sock, exit_connect, sizeof(buffer), 0);
			break;
		} else {
			hour = time(NULL);
			struct tm *ptm = localtime(&hour);
			snprintf(message, sizeof(message), "{request: POST_CHAT, body: [%s, %s, delivered at: \"%02d:%02d\", to: \"%s\"]}", buffer, user, ptm->tm_hour, ptm->tm_min, recipient);
			send(sock, message, sizeof(message), 0);
		}
	}
}

int main (int argc, char* argv[]) {
	//Get the input values
	user = argv[1];
	char* ip = argv[2];
	char* ports = argv[3];
	int port = strtol(ports, NULL, 0);
	
	struct sockaddr_in serv_addr;
	exit_flag = 0;
	
	//Initial message
	char init_connect[1024];
	time_t curr_time = time(NULL);
	struct tm *ptm = localtime(&curr_time);
	snprintf(init_connect, sizeof(init_connect), "{request: INIT_CONEX, body: [\"%s\", \"%02d-%02d-%d %02d:%02d:%02d\"]}", user, ptm->tm_mday, ptm->tm_mon + 1, ptm->tm_year + 1900, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	
	
	char buffer[1024] = { 0 };
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		printf("\n Socket creation error\n");
		return -1;
	}
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	
	if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
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
	pthread_t send_thread, rec_thread;
	send(sock, init_connect, strlen(init_connect), 0);
	pthread_create(&send_thread, NULL, inputs, NULL);
	pthread_join(send_thread, NULL);
	pthread_create(&rec_thread, NULL, outputs, NULL);
	pthread_join(rec_thread, NULL);
	while(1){
		if (exit_flag) {
			printf("Bye\n");
			break;
		}
	}
	close(sock);
}

