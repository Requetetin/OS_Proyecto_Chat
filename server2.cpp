// Server side C/C++ program to demonstrate Socket
// programming
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#define PORT 8080

#define messagescount 100 
#define clientscount 10
#include "json.hpp"
#include <iomanip>
using json = nlohmann::json;

using namespace std;

struct client{
    int id;
    string name;
    int status;
    struct sockaddr_in address;
};

struct message {
    char delivered[10] ;
    char from[100];
    char to[100];
    char message[1024];

};

client clients_list[clientscount];
message messages_list[messagescount];

void changeStatus(int id, int status ){
    cout<<"Cambiare el status del cliente "<<id<< " a "<<status<< endl;
    clients_list[id].status = status;

}

void printClients(){    
    for (int j=0; j<clientscount;j++){
        if( strcmp("", clients_list[j].name) !=0){
        cout<< "- "<< clients_list[j].id << "-"<< clients_list[j].name <<"status : "<< clients_list[j].status <<endl;}
    }
}

void printMessages(){    
    for (int j=0; j<messagescount;j++){
        if( strcmp("", messages_list[j].from) !=0){
        cout<< "-("<< messages_list[j].delivered << "):"<< messages_list[j].from <<" : "<< messages_list[j].message <<endl;}
    }
}




//Sirve para saber cual es el siguiente espacio disponible para mensajes 
int getNextMessageIndex(){
    for (int j=0; j<messagescount;j++){
        if(strcmp(messages_list[j].from, "")==0){
            return j;
        }
    }
    return 0;

}

//Sirve para saber cual es el siguiente espacio disponible para clientes 
int getNextClientIndex(){
    for (int j=0; j<clientscount;j++){
        if(strcmp(clients_list[j].name, "")==0){
            return j;
        }
    }
    return 0;
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

    //registro de nuevo usuario
	while(new_socket = accept(server_fd, (struct sockaddr*)NULL, NULL)) {
		read(new_socket, buffer, 1024);
		printf("Message connect: %s", buffer);
		json j_request;
        j_request = json::parse(buffer);
        cout<<"deberia guardar este usuario a la lista : "<<j_request["body"][1]<<endl;
        ostringstream ss;
        ss<<j_request["body"][1];
        string strOut =ss.str();
        clients_list[getNextClientIndex()].name = strOut.c_str();

        while (read(new_socket, buffer, 1024) > 0) {
			printf("Message received: %s\n", buffer);
            j_request = json::parse(buffer);
            //MAneja la respuesta de solicitar chats 
            if (j_request["request"] == "GET_CHAT") {
                if(j_request["body"] =="all"){
                    cout<<"Mostrar chat general" <<endl;
                } else {
                    cout<<"Mostrar chat de "<<j_request["body"] <<endl;

                }
            char response[1024];
            snprintf(response, sizeof(response), "{\"response\": \"GET_CHAT\",\"code\": \"200\", \"body\": \"\" }");
            send(new_socket, response, sizeof(response), 0);

            }

            //Manejar la solicitud de postear un mensaje 
            if (j_request["request"] == "POST_CHAT") {
                cout<<"POSTEAR en chat general" << j_request["body"]<< endl;
               // if(j_request["body"])
                int next;
                next= getNextMessageIndex();
                string bodymessage = to_string(j_request["body"][0]);
                char bodymessagechar[bodymessage.length()+1];

                cout<<"cuerpo del mensaje : "<< j_request["body"][0]<<endl;
                cout<<"remitente mensaje : "<< j_request["body"][1]<<endl;
                cout<<"fecha del mensaje : "<< j_request["body"][2]<<endl;
                cout<<"destinatario del mensaje : "<< j_request["body"][3]<<endl;
                //messages_list[next].message = strcpy(bodymessagechar, bodymessage.c_str());
                //messages_list[next].from = j_request["body"][1];
                //messages_list[next].delivered = j_request["body"][2];
                //messages_list[next].to = j_request["body"][3];

                char response[1024];
                snprintf(response, sizeof(response), "{\"response\": \"POST_CHAT\",\"code\": \"200\" }");
                send(new_socket, response, sizeof(response), 0);
                //al mismo tiempo de beri enviarle al resto de clientes NEW MESSAGE


            }
            //manejar la solicitud de actualizar el estado de un cliente
            if (j_request["request"] == "PUT_STATUS") {
                cout<<"Cambiar estado a " << j_request["body"]<< endl;
                
                char response[1024];
                snprintf(response, sizeof(response), "{\"response\": \"PUT_STATUS\",\"code\": \"200\" }");
                send(new_socket, response, sizeof(response), 0);
            }


            //manejar solicitud de estado de usuarios
            if (j_request["request"] == "GET_USER") {
                cout<<"Mostrar usuario " << j_request["body"]<< endl;
                
                char response[1024];
                snprintf(response, sizeof(response), "{\"response\": \"GET_USER\",\"code\": \"200\" , \"body\": \"\" }");
                send(new_socket, response, sizeof(response), 0);
            }

		}
		exit(0);
	}
}
