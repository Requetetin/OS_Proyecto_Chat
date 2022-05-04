
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> 
#include <arpa/inet.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> 
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

//creo structs para guardar mensajer y clientes
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
         
    char buffer[1025];  //data buffer of 1K 
         
    //set of socket descriptors 
    fd_set readfds;  
         
    //a message 
    char *message = "Hola mundo \r\n";  
     
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
    
         
    //try to specify maximum of 3 pending connections for the master socket 
    if (listen(master_socket, 3) < 0)  
    {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  
         

    addrlen = sizeof(address);  
    puts(" NOW ONLINE ...");  
         
    while(TRUE)  
    {   
        FD_ZERO(&readfds);  
        FD_SET(master_socket, &readfds);  
        max_sd = master_socket;  
             
        //emparentar los sockets a master
        for ( i = 0 ; i < max_clients ; i++)  
        {  
            sd = client_socket[i];     
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                 
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  
     
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
       
        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");  
        }  
             
        //Master socket recibe las conecciones nuevas
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
            read(new_socket, buffer, 1024);
            printf("Message connect: %s", buffer);
            json j_request;
            j_request = json::parse(buffer);
            cout<<"deberia guardar este usuario a la lista : "<<j_request["body"][1]<<endl;
            
            //opcional
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )  
            {  
                perror("send");  
            }  
                 
              
                 
             //Lo agrega hasta el final
            for (i = 0; i < max_clients; i++)  
            {  
                
                if( client_socket[i] == 0 )  
                {  
                    client_socket[i] = new_socket;  
                    //printf("Adding to list of sockets as %d\n" , i);  
                         
                    break;  
                }  
            }  
        }  
             
        //Sino se agrega un nuevo socket en modo listener es de modo cliente
        for (i = 0; i < max_clients; i++)  
        {  
            sd = client_socket[i];  
                 
            if (FD_ISSET( sd , &readfds))  
            {  
                //Manejador de 
                if ((valread = read( sd , buffer, 1024)) == 0)  
                {  
                    //En caso de desconecciones 
                    getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addrlen);  
                    printf("Cliente se desconecto del chat, ip %s , port %d \n" , 
                          inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                         
                    //Libera el esacio del socket
                    close( sd );  
                    client_socket[i] = 0;  
                }  
                     
                else 
                {  
                    
                    //of the data read 
                    cout<<"ESta es la solicitud que esta recibiendo" <<buffer <<endl;
                    //Aqui se deben manejar las solicitudes.



                    buffer[valread] = '\0';  
                    send(sd , buffer , strlen(buffer) , 0 );  
                }  
            }  
        }  
    }  
         
    return 0;  
}  