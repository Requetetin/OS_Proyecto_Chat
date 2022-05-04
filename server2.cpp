// Server side C/C++ program to demonstrate Socket
// programming
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

#define messagescount 100 
#define clientscount 10
#include "json.hpp"
#include <iomanip>
using json = nlohmann::json;

//using namespace std;

struct client{
    int id;
    char name[25];
    int status;
};

struct message {
    bool isprivate; 
    char sender[100];
    char receiver[100];
    char content[1024];

};

client clients_list[clientscount];
message messages_list[messagescount];

void changeStatus(int id, int status ){
    std::cout<<"Cambiare el status del cliente "<<id<< " a "<<status<< std::endl;
    clients_list[id].status = status;

}

void printClients(){    
    for (int j=0; j<clientscount;j++){
        if( strcmp("", clients_list[j].name) !=0){
        std::cout<< "- "<< clients_list[j].id << "-"<< clients_list[j].name <<"status : "<< clients_list[j].status <<std::endl;}
    }
}


int main(int argc, char const* argv[])
{
	setbuf(stdout, NULL);
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    char* hello = "Hello from server";
 
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0))
        == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
 
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE); 
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
 
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    /*//Implementacion de geeks for geeks. Un solo mensaje
    if ((new_socket
         = accept(server_fd, (struct sockaddr*)&address,
                  (socklen_t*)&addrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    return 0;*/
     //Implementacion de medium. Multiple messages
    //printf("Hello message sent\n");
	while(new_socket = accept(server_fd, (struct sockaddr*)NULL, NULL)) {
		read(new_socket, buffer, 1024);
		printf("Message connect: %s", buffer);
		json j_request;
        while (read(new_socket, buffer, 1024) > 0) {
			printf("Message received: %s\n", buffer);
            j_request = json::parse(buffer);
            if (j_request["request"] == "GET_CHAT") {
                if(j_request["body"] =="all"){
                    std::cout<<"Mostrar chat general" <<std::endl;
                } else {
                    std::cout<<"Mostrar chat de "<<j_request["body"] <<std::endl;

                }
                


            }
		}
		exit(0);
	}
}
