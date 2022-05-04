#include <iostream>
#include <stdio.h>
#include <string.h>


struct client{
	int id;
	char name[25];
};

struct message {
	bool isprivate; 
	char sender[100];
	char receiver[100];
	char content[1024];
};
client clients_list[10];
message messages_list[100];

void printClients(){	
	for (int j=0; j<5;j++){
		
		std::cout<< "- "<< clients_list[j].id << "-"<< clients_list[j].name <<std::endl;
	}


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
	for (int i=0; i<2; i++){
		std::cout<< "Ingrese el nombre del remitente del mensaje "<< i<< std::endl; 
		messages_list[i].isprivate = false;		
		std::cin >> messages_list[i].sender;
		std::cout<< "Ingrese el contenido del mensaje "<< i<< std::endl;
		std::cin >> messages_list[i].content;
		std::cout<< "Ingrese el destinatario del mensaje "<< i<< std::endl;
		std::cin >> messages_list[i].receiver;
		}
	printMessages();
	return 0;
}

