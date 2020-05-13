#include <stdio.h>  
#include <string.h>   //strlen  
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>   //close    
#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros  
#include <sys/un.h>
#define SOCK_PATH "chat_socket"  
     
int main()   
{   
    
    int master_socket , len , new_socket , client_socket[30] ,  
          max_clients = 10 , activity, i , valread , sd;   
    int max_sd,t;     
    struct sockaddr_un local,remote;
    char buffer[1025];   
    fd_set readfds;   
    char message[100];
    strcpy(message,"Your username is");
    for (i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    }   
         
    //creating socket 
    if( (master_socket = socket(AF_UNIX , SOCK_STREAM , 0)) < 0)   
    {   
        perror("socket failed");   
        exit(EXIT_FAILURE);   
    }   
     
    local.sun_family = AF_UNIX;
     strcpy(local.sun_path, SOCK_PATH);
     unlink(local.sun_path);
     len = strlen(local.sun_path) + sizeof(local.sun_family);
     if (bind(master_socket, (struct sockaddr *)&local, len) == -1) {
     perror("bind");
     exit(1);
     }  
    printf("Welcome to chat server \n");
    printf("Listener active \n");   
           
    if (listen(master_socket, 3) < 0)   
    {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    }     
    char temp[10]; 
    int currpos;
    char temp2[1024];
    while(1)   
    {   
          
        FD_ZERO(&readfds);    
        FD_SET(master_socket, &readfds);   
        max_sd = master_socket;     
        for ( i = 0 ; i < max_clients ; i++)   
        {   
            sd = client_socket[i];    
            if(sd > 0)   
                FD_SET( sd , &readfds);     
            if(sd > max_sd)   
                max_sd = sd;   
        }   
  
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
        if ((activity < 0) && (errno!=EINTR))   
        {   
            printf("select error");   
        }    
        if (FD_ISSET(master_socket, &readfds))   
        {   
            if ((new_socket = accept(master_socket, (struct sockaddr *)&remote, &t))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
             
            printf("New connection , socket fd is %d\n" , new_socket);

            currpos=-2;
            for (i = 0; i < max_clients; i++)   
            {    
                if( client_socket[i] == 0 )   
                {   
                    currpos=i;
                    client_socket[i] = new_socket;           
                    break;   
                }   
            }
            strcpy(message,"Your username is");
            sprintf(temp,"%d",currpos+1);
            strcat(message," usr");
            strcat(message,temp);
              
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )   
            {   
                perror("send");   
            }   
                 
            puts("Username assigned successfully");   
                
        }   
             
        for (i = 0; i < max_clients; i++)   
        {   
            sd = client_socket[i];   
                 
            if (FD_ISSET( sd , &readfds))   
            {   
                 
                if ((valread = read( sd , buffer, 1024)) == 0)   
                {   
                      
                    printf("Client disconnected \n");   
                    close( sd );   
                    client_socket[i] = 0;   
                }     
                else 
                {   
                    buffer[valread] = '\0';
                    if(buffer[0]=='@')
                    {
                        if(buffer[1]=='u' && buffer[2]=='s' && buffer[3]=='r')
                        {
                            int sockk=buffer[4]-'0';
                            sockk--;
                            char sender=i+'1';
                            strcpy(temp2,"Usr  sent:");
                            temp2[3]=sender;
                            strcat(temp2,buffer+5);

                        int flag=0;
                        for (int k=0; k< max_clients; k++)
                        {
                            if(k==sockk && client_socket[k]!=0 && sockk!=i)
                            {
                                send(client_socket[k] , temp2 , strlen(temp2) , 0 );
                                flag=1;
                            }    
                        }
                        if(flag==0 && sockk!=i)
                        {
                            send(client_socket[i] , "No such user exist" , 18 , 0 );
                        }
                    }
                    else{
                        if(buffer[1]=='A' && buffer[2]=='l' && buffer[3]=='l')
                        {
                            char sender=i+'1';
                            strcpy(temp2,"Usr  sent:");
                            temp2[3]=sender;
                            strcat(temp2,buffer+5);

                        for (int k=0; k< max_clients; k++)
                    {
                        if(client_socket[k]!=sd)
                        send(client_socket[k] , temp2 , strlen(temp2) , 0 );    
                    }   

                        }
                        else{
                            send(client_socket[i] , "Wrong Command" ,13  , 0 );

                        }
                        
                     

                    }

                    }
                    else{
                        send(client_socket[i] , "Wrong Command" ,13  , 0 );

                    }

                      
                }   
            }   
        }   
    }   
         
    return 0;   
}   