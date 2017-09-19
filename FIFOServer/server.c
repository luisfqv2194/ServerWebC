#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>



char webpage[] = 
"HTTP/1.1 200 OF\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>ShellWaveX</title>\r\n"
"<style>body {background-clor: #FFFF00} </style></head>\r\n"
"<body><center><h1>Hello world!</h1><br>\r\n";


void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_len = sizeof(client_addr);
    int fd_server, fd_client;
    char buf[2048];
    int fdimg;
    int on = 1;


    fd_server = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_server < 0)
    {
        error("socket");
    }

    setsockopt(fd_server, SOL_SOCKET, SO_REUSEADOR, &on, sizeof(int));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons (8080);

    if (bind(fd_server, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1 )
    {
        close(fd_server);
        error ("bind");
        
    }

    if (listen(fd_server, 10) == -1)
    {
        close(fd_server);
        error ("listen");
    }

    while(1)
    {
        fd_client = accept(fd_server, (struct sockaddr *) &client_addr, &sin_len);

        if (fd_client == -1)
        {
            perror("Client connection failed...\n");
            continue;
        }

        printf("Got client connection...\n");

        if (!fork())
        {
            /* child process */
            close (fd_server);
            memset(buf,0,2048);
            read(fd_client,buf,2047);

            printf("%s\n",buf);

            if (!strcmp(buf, "GET /favicon.ico",16))
            {
                fdimg = open("favicon.ico", O_RDONY);
                sendfile(fd_client, fdimg, NULL, 4000);
                close (fdimg);
            }
            else if (!strcmp(buf, "GET /docktest.jpg",16))
            {
                fdimg = open("docktest.jpg", O_RDONY);
                sendfile(fd_client, fdimg, NULL, 6000);
                close (fdimg);
            }
            else
                write(fd_client , webpage, sizeof(webpage) - 1);


            close(fd_client);
            printf("Closing...\n");
            exit(0);
        }
        /*parent process */
        close (fd_client);

    }


    return 0;
}