//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux

//PRueba del ejemplo de geeks for geeks
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include "json.hpp"
#include <iomanip>
#include <iostream>
using json = nlohmann::json;

using namespace std;

#define messagescount 100 
#define clientscount 10
    
#define TRUE 1
#define FALSE 0
#define PORT 8888

struct client{
    string name;
    int status;
};

struct message {
    string delivered;
    string from;
    string to;
    string message;

};

client clients_list[clientscount];
message messages_list[messagescount];

void changeStatus(int id, int status ){
    cout<<"Cambiare el status del cliente "<<id<< " a "<<status<< endl;
    clients_list[id].status = status;

}

void printClients(){    
    for (int j=0; j<clientscount;j++){
        if( clients_list[j].name != ""){
        cout<< "- "<< clients_list[j].name <<" status : "<< clients_list[j].status <<endl;}
    }
}

void printMessages(){    
    for (int j=0; j<messagescount;j++){
        if( messages_list[j].from!= ""){
        cout<< "-("<< messages_list[j].delivered << "):"<< messages_list[j].from <<" : "<< messages_list[j].message <<endl;}
    }
}




//Sirve para saber cual es el siguiente espacio disponible para mensajes 
int getNextMessageIndex(){
    for (int j=0; j<messagescount;j++){
        if(messages_list[j].from ==""){
            return j;
        }
    }
    return 0;

}

//Sirve para saber cual es el siguiente espacio disponible para clientes 
int getNextClientIndex(){
    for (int j=0; j<clientscount;j++){
        if(clients_list[j].name == ""){
            return j;
        }
    }
    return 0;
}



    
int main(int argc , char *argv[])
{
    int opt = TRUE;
    int master_socket , addrlen , new_socket , client_socket[30] ,
        max_clients = 30 , activity, i , valread , sd;
    int max_sd;
    struct sockaddr_in address;
        
    char buffer[1025]; //data buffer of 1K
        
    //set of socket descriptors
    fd_set readfds;
        
    //a message
    char *message = "ECHO Daemon v1.0 \r\n";
    
    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }
        
    //create a master socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
        sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
        
    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    //printf("Listener on port %d \n", PORT);
        
    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
        
    //accept the incoming connection
    addrlen = sizeof(address);
    //puts("Waiting for connections ...");
        
    while(TRUE)
    {
        //clear the socket set
        FD_ZERO(&readfds);
    
        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;
            
        //add child sockets to set
        for ( i = 0 ; i < max_clients ; i++)
        {
            //socket descriptor
            sd = client_socket[i];
                
            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd , &readfds);
                
            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }
    
        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
    
        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }
            
        //If something happened on the master socket ,
        //then its an incoming connection
        if (FD_ISSET(master_socket, &readfds))
        {
            if ((new_socket = accept(master_socket,
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            
            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
        
            //send new connection greeting message
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )
            {
                perror("send");
            }
                
            //puts("Welcome message sent successfully");
                
            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++)
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    read(new_socket, buffer, 1024);
                    printf("Message connect: %s", buffer);
                    json j_request;
                    j_request = json::parse(buffer);
                    cout<<"deberia guardar este usuario a la lista : "<<j_request["body"][1]<<endl;
                    //printf("Adding to list of sockets as %d\n" , i);
                        
                    break;
                }
            }
        }
            
        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)
        {   
            //Lectura de get chat
            
            sd = client_socket[i];

                
            if (FD_ISSET( sd , &readfds))
            {   
                printf("leyendo input de socket ");
                //Check if it was for closing , and also read the
                //incoming message
                json j_request;
                if ((valread = read( sd , buffer, 1024)) == 0)
                {
                    read( sd , buffer, 1024);
                    j_request = json::parse(buffer);
                    //MAneja la respuesta de solicitar chats 
                    if (j_request["request"] == "GET_CHAT") 
                    {
                        if(j_request["body"] =="all"){
                            cout<<"Mostrar chat general" <<endl;
                        } else {
                            cout<<"Mostrar chat de "<<j_request["body"] <<endl;

                                }
                        char response[1024];
                        snprintf(response, sizeof(response), "{\"response\": \"GET_CHAT\",\"code\": \"200\", \"body\": \"\" }");
                        send(new_socket, response, sizeof(response), 0);

                    }
                }
                //Manejar la solicitud de postear un mensaje 
                if (j_request["request"] == "POST_CHAT") 
                {
                    cout<<"POSTEAR en chat general" << j_request["body"]<< endl;
                   // if(j_request["body"])
                    int next;
                    next= getNextMessageIndex();
                    string bodymessage = to_string(j_request["body"][0]);
                    char bodymessagechar[bodymessage.length()+1];

                    //lo guarda con todos los mensajes
                    messages_list[next].message = j_request["body"][0];
                    messages_list[next].from = j_request["body"][1];
                    messages_list[next].delivered = j_request["body"][2];
                    messages_list[next].to = j_request["body"][3];
                    printMessages();
                    char response[1024];
                    snprintf(response, sizeof(response), "{\"response\": \"POST_CHAT\",\"code\": \"200\" }");
                    send(new_socket, response, sizeof(response), 0);
                    
                
                }

            }

                    
                //Echo back the message that came in
                else
                {
                    //set the string terminating NULL byte on the end
                    //of the data read
                    buffer[valread] = '\0';
                    send(sd , buffer , strlen(buffer) , 0 );
                }
            }
        }
    
        
    return 0;
}
