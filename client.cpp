#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <string>

using namespace std;

int sock;
char* user;
pthread_t send_thread, rec_thread;
string recipient;

void trim_string(char* arr, int length) {
	int i;
	for (i=0; i<length; i++){
		if(arr[i] == '\n') {
			arr[i] = '\0';
			break;
		}
	}
}

void* inputs(void* args) {
	pthread_exit(NULL);
}

void* outputs(void* args) {
	char buffer[1024] = {};
	time_t hour;
	char message[1024];
	
	while (1) {
		printf("Enter a message: ");
		fgets(buffer, 1024, stdin);
		trim_string(buffer, 1024);
		if (strcmp(buffer, "back") == 0) {
			break;
		} else {
			hour = time(NULL);
			struct tm *ptm = localtime(&hour);
			snprintf(message, sizeof(message), "{\"request\": \"POST_CHAT\", \"body\": [\"%s\", \"%s\", \"%02d:%02d\", \"%s\"]}", buffer, user, ptm->tm_hour, ptm->tm_min, recipient.c_str());
			send(sock, message, sizeof(message), 0);
		}
	}
	pthread_exit(NULL);
}

void startThreads() {
	pthread_create(&send_thread, NULL, inputs, NULL);
	pthread_join(send_thread, NULL);
	pthread_create(&rec_thread, NULL, outputs, NULL);
	pthread_join(rec_thread, NULL);
}

int main (int argc, char* argv[]) {
	setbuf(stdout, NULL);
	//Get the input values
	user = argv[1];
	char* ip = argv[2];
	char* ports = argv[3];
	int port = atoi(ports);
	
	struct sockaddr_in serv_addr;
	
	//Initial message
	char init_connect[1024];
	time_t curr_time = time(NULL);
	struct tm *ptm = localtime(&curr_time);
	snprintf(init_connect, sizeof(init_connect), "{\"request\": \"INIT_CONEX\", \"body\": [\"%02d-%02d-%d %02d:%02d:%02d\", \"%s\"]}", ptm->tm_mday, ptm->tm_mon + 1, ptm->tm_year + 1900, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, user);
	
	
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
	
	
	send(sock, init_connect, strlen(init_connect), 0);
	//Menu
	int selector_menu, new_state;
	char state_change[1024];
	while(1) {
		printf("***********************************\n");
		printf("//Seleccione la opcion del menu\n");
		printf("1. Abrir chat general\n");
		printf("2. Abrir chat privado\n");
		printf("3. Cambiar de status\n");
		printf("4. Listar usuarios\n");
		printf("5. Mostrar usuario especifico\n");
		printf("6. Ayuda\n");
		printf("7. Salir\n");
		scanf("%d", &selector_menu);
		
		
		if (selector_menu == 1) {
			recipient = "all";
			startThreads();
		} else if (selector_menu == 2) {
			printf("Con que usuario se desea comunicar? ");
			scanf("%s", recipient.c_str());
			startThreads();
		} else if (selector_menu == 3) {
			printf("Ingrese el numero del nuevo estado: ");
			scanf("%d", &new_state);
			snprintf(state_change, sizeof(state_change), "{\"request\": \"PUT_STATUS\", \"body\": \"%d\"}", new_state);
			send(sock, state_change, sizeof(state_change), 0);
		} else if (selector_menu == 4) {
			
		} else if (selector_menu == 5) {
		
		} else if (selector_menu == 6) {
		
		} else if (selector_menu == 7) {
			char exit_connect[1024] = "{\"request\": \"END_CONEX\"}";
			send(sock, exit_connect, sizeof(buffer), 0);
			break;
		} 
	}
	
	close(sock);
}

