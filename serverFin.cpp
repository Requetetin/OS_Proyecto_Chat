
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
        cout<< "- "<< clients_list[j].name <<" status : "<< clients_list[j].status <<endl;
        
    }
    } 
}

string printMessages(){
    string concatenate;
    concatenate = "["    
    for (int j=0; j<messagescount;j++){
        if( messages_list[j].from!= ""){
        cout<< "-("<< messages_list[j].delivered << "):"<< messages_list[j].from <<" : "<< messages_list[j].message <<endl;
        concatennate = concatenate + "["+ messages_list[j].message+ ","+ messages_list[j].from+"," +messages_list[j].delivered +"]"
    }
    } concatenate = concatenate+ "]"
    return concatenate
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
         
    char buffer[1025];
    fd_set readfds;  
         
    //a message 
    char *message = "Hola mundo \r\n";  
     
    //initialise all client_socket[] to 0 so not checked 
    for (i = 0; i < max_clients; i++)  
    {  
        client_socket[i] = 0;  
    }  
         
    //crea socket master para poder emparentar el resto
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)  
    {  
        perror("socket failed");  
        exit(EXIT_FAILURE);  
    }  
     
    
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
         
    //bind-ea el socket al puerto 88888888 
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)  
    {  
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    }  
    
    
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
            //agregar a lista si tiene file descriptor valido 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                 
            //max socket sirve para llevar inidce de cuantos sockets hay registrados 
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
             
            //definicion de ip del cliente
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
            read(new_socket, buffer, 1024);
            printf("Message connect: %s", buffer);
            json j_request;
            j_request = json::parse(buffer);
            cout<<"deberia guardar este usuario a la lista : "<<j_request["body"][1]<<endl;
            char response[1024];
            snprintf(response, sizeof(response), "{\"response\": \"INIT_CONEX\",\"code\": 200 }");
            cout<<  "ESTO RESPONDIO EL SERVIDOR: " <<response <<endl;
            send(new_socket, response, sizeof(response), 0);
            int ix = getNextClientIndex();
            clients_list[ix].name = j_request["body"][1];
            clients_list[ix].status = 1;
            printClients();
            
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
                         
                    //Libera el espacio del socket
                    close( sd );  
                    client_socket[i] = 0;  
                }  
                     
                else 
                {  
                    
                    //Lectura y manejo de solicitudes 
                    cout<<"ESta es la solicitud que esta recibiendo" <<buffer <<endl;
                    //Aqui se deben manejar las solicitudes.
                    json j_request;
                    j_request = json::parse(buffer);
                    //MAneja la respuesta de solicitar chats 
                    if (j_request["request"] == "GET_CHAT") {
                        if(j_request["body"] =="all"){
                            cout<<"Mostrar chat general" <<endl;
                        } else {
                            cout<<"Mostrar chat de "<<j_request["body"] <<endl;

                        }
                    char response[1024];
                    string messages = printMessages();
                    snprintf(response, sizeof(response), "{\"response\": \"GET_CHAT\",\"code\": \"200\", \"body\":  }");
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

                        //lo guarda con todos los mensajes
                        messages_list[next].message = j_request["body"][0];
                        messages_list[next].from = j_request["body"][1];
                        messages_list[next].delivered = j_request["body"][2];
                        messages_list[next].to = j_request["body"][3];
                        printMessages();
                        char response[1024];
                        snprintf(response, sizeof(response), "{\"response\": \"POST_CHAT\",\"code\": \"200\" }");
                        send(new_socket, response, sizeof(response), 0);
                        //al mismo tiempo deberia enviarle al resto de clientes NEW MESSAGE
                        


                    }
                    //manejar la solicitud de actualizar el estado de un cliente
                    if (j_request["request"] == "PUT_STATUS") {
                        cout<<"Cambiar estado a " << j_request["body"]<< endl;
                        //changeStatus(j_request["body"]);
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



                    buffer[valread] = '\0';  
                    send(sd , buffer , strlen(buffer) , 0 );  
                }  
            }  
        }  
    }  
         
    return 0;  
}  