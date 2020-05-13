#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <sys/un.h>	
#define SOCK_PATH "chat_socket"
#define BUFSIZE 1024
				
int main()
{
	int sockfd, fdmax, i;
    int t, len;
    struct sockaddr_un remote;
    printf("Welcome to chat room \n");
	fd_set master;
	fd_set read_fds;
    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
    }
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(sockfd, (struct sockaddr *)&remote, len) == -1) {
    perror("connect");
    exit(1);
    }
    printf("Connected.\n");
	    FD_ZERO(&master);
        FD_ZERO(&read_fds);
        FD_SET(0, &master);
        FD_SET(sockfd, &master);
	fdmax = sockfd;
	printf("1.) To send Messaage - @<username> <text> \n");
    printf("2.) To Broadcast - @All <text> \n");
    printf("3.) To exit - quit \n");
	while(1){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("select");
			exit(4);
		}
		
		for(i=0; i <= fdmax; i++ )
			if(FD_ISSET(i, &read_fds))
            {
                char bufsend[BUFSIZE];
                char bufrecv[BUFSIZE];
                int bytes;
                if (i == 0){
                    fgets(bufsend, BUFSIZE, stdin);
                    if (strcmp(bufsend , "quit\n") == 0) {
                        exit(0);
                    }else
                        send(sockfd, bufsend, strlen(bufsend), 0);
                }else {
                    bytes = recv(sockfd, bufrecv, BUFSIZE, 0);
                    bufrecv[bytes] = '\0';
                    printf("%s\n" , bufrecv);
                    fflush(stdout);
                }
            }
	}
	close(sockfd);
	return 0;
}