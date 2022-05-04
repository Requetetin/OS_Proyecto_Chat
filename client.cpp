#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <string>
#include <iostream>
#include <atomic>

#include "json.hpp"
#include <iomanip>
using json = nlohmann::json;

using namespace std;

int sock, end_flag;
char* user;
pthread_t send_thread, rec_thread;
string recipient;
char response[1024];

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
	json j_response;
	while (end_flag) {
		recv(sock, response, 1024, 0);
		j_response = json::parse(response);
		if (j_response["response"] == "NEW_MESSAGE") {
			printf("%s [%s]: %s", to_string(j_response["body"][2]).c_str(), to_string(j_response["body"][1]).c_str(), to_string(j_response["body"][0]).c_str());
		} 
	}
}

void* outputs(void* args) {
	char buffer[1024] = {};
	time_t hour;
	char message[1024];
	cin.clear();
	while (1) {
		printf("Enter a message: ");
		fgets(buffer, 1024, stdin);
		trim_string(buffer, 1024);
		if (strcmp(buffer, "back") == 0) {
			end_flag = 0;
			pthread_cancel(rec_thread);
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
	end_flag = 1;
	pthread_create(&send_thread, NULL, inputs, NULL);
	pthread_create(&rec_thread, NULL, outputs, NULL);
	pthread_join(send_thread, NULL);
}

void requestUsers() {
	char request_user[1024];
	snprintf(request_user, sizeof(request_user), "{\"request\": \"GET_USER\", \"body\": \"%s\"}", recipient.c_str());
	send(sock, request_user, sizeof(request_user), 0);
	read(sock, response, 1024);
	json j_response = json::parse(response);
	printf("%s\n", to_string(j_response["body"]).c_str());
}

int main (int argc, char* argv[]) {
	end_flag = 0;
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
			recipient =  "all";
			requestUsers();
		} else if (selector_menu == 5) {
			printf("Con que usuario se desea comunicar? ");
			scanf("%s", recipient.c_str());
			requestUsers();
		} else if (selector_menu == 6) {
			printf("************************************\n");
			printf("Instrucciones\n");
			printf("Presione los numeros para acceder a la funcion deseada\n");
			printf("Si se encuentra en un chat escriba \"back\" para regresar al menu\n\n");
			printf("************************************\n");
			printf("Codigos de error\n");
			printf("101: Usuario ya registrado\n");
			printf("102: Usuario no conectado\n");
			printf("103: No hay usuarios conectados\n");
			printf("104: El estatus no se pudo modificar\n");
			printf("105: Error inesperado del server\n");
		} else if (selector_menu == 7) {
			char exit_connect[1024] = "{\"request\": \"END_CONEX\"}";
			send(sock, exit_connect, sizeof(buffer), 0);
			break;
		} 
	}
	
	close(sock);
}

