#include <iostream>
#include <stdio.h>
#include <string.h>
#define messagescount 100 
#define clientscount 10

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

void printClients(){	
	for (int j=0; j<clientscount;j++){
		if( strcmp("", clients_list[j].name) !=0){
		std::cout<< "- "<< clients_list[j].id << "-"<< clients_list[j].name <<"status : "<< clients_list[j].status <<std::endl;}
	}
}

void changeStatus(int id, int status ){
	std::cout<<"Cambiare el status del cliente "<<id<< " a "<<status<< std::endl;
	clients_list[id].status = status;

}
void printMessages(){
	//despliegue de mensajes
	char username[100];
	std::cout<< "Quien eres? "<< std::endl;	
	std::cin >> username;
		
		
	for (int j=0; j<5;j++){
		
		if( strcmp(username, messages_list[j].sender) ==0){
			std::cout << messages_list[j].content << ": YOU" <<std::endl;
			}
		else {
			std::cout<< "-"<< messages_list[j].sender << "-"<< messages_list[j].content <<std::endl;
		}
	}


}

int main (){
	//registro de mensajes al chat esto debe venir de cada buffer
	//for (int i=0; i<2; i++){
	//	std::cout<< "Ingrese el nombre del remitente del mensaje "<< i<< std::endl; 
	//	messages_list[i].isprivate = false;		
	//	std::cin >> messages_list[i].sender;
	//	std::cout<< "Ingrese el contenido del mensaje "<< i<< std::endl;
	//	std::cin >> messages_list[i].content;
	//	std::cout<< "Ingrese el destinatario del mensaje "<< i<< std::endl;
	//	std::cin >> messages_list[i].receiver;
	//	}
	//printMessages();
	//registro de cliente al chat esto debe venir de cada buffer
	for (int i=0; i<2; i++){
		std::cout<< "Ingrese el nombre del cliente "<< i<< std::endl; 
		std::cin >>clients_list[i].name;		
		clients_list[i].status =1;
		clients_list[i].id = i;
		
		}
	printClients();
	std::cout<< "Ingrese el id del cliente al que le quiere cambiar el status"<<std::endl; 
	int user, newstatus;	
	std::cin >> user;
	std::cout<< "Ingrese el nuevo status"<<std::endl;
	std::cin >> newstatus;
	changeStatus(user, newstatus);
	printClients();
	return 0;
}