/*

  ECHOSERV.C
  ==========
  (c) Paul Griffiths, 1999
  Email: mail@paulgriffiths.net
  
  Simple TCP/IP echo server.

*/


#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */
#include <sysmodule/sysmodule.h>

#include "helper.h"           /*  our own helper functions  */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/*  Global constants  */

#define ECHO_PORT          (2002)
#define MAX_LINE           (1000)


int main(int argc, char *argv[]) {
    int       list_s;                /*  listening socket          */
    int       conn_s;                /*  connection socket         */
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    char      buffer[MAX_LINE];      /*  character buffer          */
    char     *endptr;                /*  for strtol()              */

	fprintf(stdout, "Starting ECHOServer.\n");
    /*  Get port number from the command line, and
        set to default port if no arguments were supplied  */

    if ( argc == 2 ) {
		port = strtol(argv[1], &endptr, 0);
		if ( *endptr ) {
			fprintf(stderr, "ECHOSERV: Invalid port number.\n");
			exit(EXIT_FAILURE);
		}
    }else if ( argc < 2 ) {
		port = ECHO_PORT;
    }else{
		fprintf(stderr, "ECHOSERV: Invalid arguments.\n");
		exit(EXIT_FAILURE);
    }

	printf("SysLoadModule(NET)=%d\n", SysLoadModule(SYSMODULE_NET));
    printf("net_init()=%d\n", net_initialize_network());

    /*  Create the listening socket  */

    if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		fprintf(stderr, "ECHOSERV: Error creating listening socket.\n");
		printf("errno: %d\n", errno);
		exit(EXIT_FAILURE);
    }


    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);


    /*  Bind our socket addresss to the 
	listening socket, and call listen()  */

    if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
		fprintf(stderr, "ECHOSERV: Error calling bind()\n");
		exit(EXIT_FAILURE);
    }

    if ( listen(list_s, LISTENQ) < 0 ) {
		fprintf(stderr, "ECHOSERV: Error calling listen()\n");
		exit(EXIT_FAILURE);
    }
    char* message = "Welcome to ECHOServer test.\nType exit and hit enter to close app, otherwise type anything you want and hit enter and I will replay it back to you free of charge. :)";
    /*  Enter an infinite loop to respond
        to client requests and echo input  */
	int run = 1;
    while ( run ) {

		/*  Wait for a connection, then accept() it  */

		if ( (conn_s = accept(list_s, NULL, NULL) ) < 0 ) {
			fprintf(stderr, "ECHOSERV: Error calling accept()\n");
			exit(EXIT_FAILURE);
		}

		Writeline(conn_s, message, strlen(message));

		/*  Retrieve an input line from the connected socket
			then simply write it back to the same socket.     */

		Readline(conn_s, buffer, MAX_LINE-1);
		if(strncmp("exit", buffer, 4) == 0)
			run = 0;
		Writeline(conn_s, buffer, strlen(buffer));


		/*  Close the connected socket  */

		if ( closesocket(conn_s) < 0 ) {
			fprintf(stderr, "ECHOSERV: Error calling closesocket()\n");
			exit(EXIT_FAILURE);
		}

    }
	net_finalize_network();
	SysUnloadModule(SYSMODULE_NET);
}

