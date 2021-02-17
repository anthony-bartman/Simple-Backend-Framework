/**
 *  Anthony Bartman
 *  1/28/21
 *  Networking II
 *  Dr. Rothe
 * 
 *  Desc: This file will run a simple http server that will only be able to return an indicated
 *  file from the HTTP GET request. If it exists on the server side, it will return that file with
 *  a HTTP 200 request. Otherwise, it will return an HTTP 404 error code with an error.html file.
 * 
 *  ** Code is based off of Dr. Rothe's tcpechoserver.c file **
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void httpResponse(int connection, int echoed, int bytes_read, char *buffer);

// Max message to echo
#define MAX_MESSAGE 1000

/* server main routine */

int main(int argc, char **argv)
{

    // locals
    unsigned short port = 80; // default port
    int sock;                 // socket descriptor

    // Was help requested?  Print usage statement
    if (argc > 1 && ((!strcmp(argv[1], "-?")) || (!strcmp(argv[1], "-h"))))
    {
        printf("\nUsage: tcpechoserver [-p port] port is the requested \
 port that the server monitors.  If no port is provided, the server \
 listens on port 22222.\n\n");
        exit(0);
    }

    // get the port from ARGV
    if (argc > 1 && !strcmp(argv[1], "-p"))
    {
        if (sscanf(argv[2], "%hu", &port) != 1)
        {
            perror("Error parsing port option");
            exit(0);
        }
    }

    // ready to go
    printf("tcp echo server configuring on port: %d\n", port);

    // for TCP, we want IP protocol domain (PF_INET)
    // and TCP transport type (SOCK_STREAM)
    // no alternate protocol - 0, since we have already specified IP

    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error on socket creation");
        exit(1);
    }

    // lose the pesky "Address already in use" error message
    int yes = 1;

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        perror("setsockopt");
        exit(1);
    }

    // establish address - this is the server and will
    // only be listening on the specified port
    struct sockaddr_in sock_address;

    // address family is AF_INET
    // our IP address is INADDR_ANY (any of our IP addresses)
    // the port number is per default or option above

    sock_address.sin_family = AF_INET;
    sock_address.sin_addr.s_addr = htonl(INADDR_ANY);
    sock_address.sin_port = htons(port);

    // we must now bind the socket descriptor to the address info
    if (bind(sock, (struct sockaddr *)&sock_address, sizeof(sock_address)) < 0)
    {
        perror("Problem binding");
        exit(-1);
    }

    // extra step to TCP - listen on the port for a connection
    // willing to queue 5 connection requests
    if (listen(sock, 15) < 0)
    {
        perror("Error calling listen()");
        exit(-1);
    }

    // go into forever loop and echo whatever message is received
    // to console and back to source
    char *buffer = calloc(MAX_MESSAGE, sizeof(char));
    int bytes_read;
    int echoed;
    int connection;

    while (1)
    {
        // hang in accept and wait for connection
        printf("====Waiting====\n");
        int connection = accept(sock, NULL, NULL);

        // Split Connections into multiple processes
        if (connection < 0)
        {
            perror("ERROR on accept");
            exit(-1);
        }
        int pid = fork();
        if (pid < 0)
            perror("ERROR on fork");
        if (pid == 0)
        {
            close(sock);
            httpResponse(connection, echoed, bytes_read, buffer);
            close(connection);
            exit(0);
        }
        else
        {
            close(connection);
        }
    } // end of outer loop

    // will never get here
    return (0);
}


// Base code for this method provided by Dr. Rothe
void httpResponse(int connection, int echoed, int bytes_read, char *buffer)
{
    buffer[0] = '\0'; // guarantee a null here to break out on a
                      // disconnect
    // read message
    bytes_read = read(connection, buffer, MAX_MESSAGE - 1);
    if (bytes_read == 0)
    { // socket closed
        printf("====Client Disconnected====\n");
        close(connection);
        // break; // break the inner while loop
    }

    // make sure buffer has null terminator
    buffer[bytes_read] = '\0';
    // see if client wants us to disconnect
    if (strncmp(buffer, "quit", 4) == 0)
    {
        printf("====Server Disconnecting====\n");
        close(connection);
    }

    /*
    *  HTTP REQUEST STRING TOKENIZATION
    */
    char *cmdVerb;
    char *fileName;
    char delim1[] = "\r\n";
    char delim2[] = " ";
    char *ptr = strtok(buffer, delim1);
    int i = 0;
    // Iterate through entire HTTP request
    while (ptr != NULL)
    {
        if (i == 0)
        {
            char *tempHeader = ptr;
            char *ptr2 = strtok(tempHeader, delim2);
            // Iterate through only header
            while (ptr2 != NULL)
            {
                if (i == 0)
                {
                    // Grabs HTTP Verb 'GET'
                    cmdVerb = ptr2;
                }
                else if (i == 1)
                {
                    // Grabs filename requested from browser
                    fileName = ptr2; 
                }
                ptr2 = strtok(NULL, delim2);
                i++;
            }
        }
        ptr = strtok(NULL, delim1);
        i++;
    }

    /**
     *  HTTP HEADER and BODY RESPONSE LOGIC
     */
    char header_200[] = "HTTP/1.1 200 OK\r\n\r\n";
    char header_404[] = "HTTP/1.1 404 Not Found\r\n\r\n";
    char *body = calloc(MAX_MESSAGE, sizeof(char));
    char *response;
    FILE *filedata;
    // Checking to see what file user requested
    if (strcmp(fileName, "/") == 0)
    {
        filedata = fopen("index.html", "rb");
    }
    else
    {
        fileName = fileName + 1;  // move pointer ahead by 1 to get rid of the backslash
        filedata = fopen(fileName, "rb");
    }

    // PREPARING HTTP Response header and body
    if (filedata != NULL) //If File exist on server side
    {
        //Allocate enough memory for 
        response = calloc(MAX_MESSAGE + strlen(header_200), sizeof(char));

        fread(body, 1, MAX_MESSAGE, filedata);
        memcpy(response, header_200, strlen(header_200));
        memcpy(response + strlen(header_200), body, MAX_MESSAGE + strlen(header_200));
    }
    else //Otherwise return error.html
    {
        response = calloc(MAX_MESSAGE + strlen(header_404), sizeof(char));
        filedata = fopen("error.html", "rb");

        fread(body, 1, MAX_MESSAGE, filedata);
        memcpy(response, header_404, strlen(header_404));
        memcpy(response + strlen(header_404), body, MAX_MESSAGE + strlen(header_404));
    }


    // HTTP Send formulated response back to client
    if ((echoed = write(connection, response, MAX_MESSAGE + strlen(header_404))) < 0)
    {
        perror("Error sending echo");
        exit(-1);
    }
    else
    {
        printf("Bytes echoed: %d\n", echoed);
        free(response);
        free(body);
    }

    close(connection);
    exit(0);
}