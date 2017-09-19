/*

http://beej.us/guide/bgnet/output/html/multipage/index.html

http://www.theinsanetechie.in/2014/02/a-simple-http-client-and-server-in-c.html

cd mnt/hgfs/ServerWebC/FORKServer

gcc -std=gnu99 -o server server.c

** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h> //Linux
//#include <sys/uio.h> //macOS
#include <sys/stat.h>
#include <netinet/in.h>
#include <fcntl.h> 
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include"time.h"
#include"dirent.h"

#define PORT "7001"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold

#define MAXDATASIZE 2048

//Variables para visualizar en browser
char headers[] = "HTTP/1.0 200 OK\r\n"
                 "Server: CPi\r\n\0";
char contentTypeText[] = "Content-type: text/plain\r\n\r\n\0";
char contentTypeJPG[] = "Content-type: image/jpeg\r\n\r\n\0";
char contentTypePNG[] = "Content-type: image/png\r\n\r\n\0";
char buffer[2048];
char html[] = "<html><head><title>Servidor Fork</title>"
              "</head><body>\0"; 
char html2[] = "</body></html>\r\n\0";



char * getFileNameFromBrowser(char buf[], int numbytes, char fileNameBuf[]){
    char *fileName = malloc(numbytes*sizeof(char));

    for (int i = 0; i < numbytes; ++i) {
        if (buf[i+5]!=' ') { 
            fileName[i] = buf[i+5];
        }
    }

    strcpy(fileNameBuf, fileName);

    printf("fileNameBuf tiene:\n%s\n", fileNameBuf);
   // printf("Los contendios de fileName son:\n%s\n", fileName);
    free(fileName);
    return fileNameBuf;
    
}

void * sendDataToBrowser(char headers[], char contentTypeText[], char html[],
    char datatoBrowser[], char data[], char html2[], int  child, int data_size) {

    //Concateno dentro del buffer data los headers del HTML y el buffer datatoBrowser
    snprintf(data, data_size, "%s %s %s %s %s", headers, 
        contentTypeText, html,datatoBrowser,html2);

    if (send( child, data, strlen(data), 0) == -1)
        perror("Couldn't send data");
}


int openFile(char fileName[], int  child, int tipo) {
    char path[2000] = {0};
    char *data;
    int data_size, fd;

    // Creo la ruta para buscar el archivo files/etc.
    strcat(path, "files/");
    strcat(path, fileName);

    // Abro el archivo
    fd = open(path, O_RDONLY);

    //Si no se abre el archivo
    if(fd == -1) 
    {
        printf("Error abriendo el archivo o no existe\n");

        if (tipo == 1) //Si es para el browser
        {
         
            // Cantidad de espacio necesario para desplegar el buffer,
            // son 42 por los 4 simbolos terminales y 38 del mensaje.
            data_size = (42 + strlen(headers) + strlen(contentTypeText)
                + strlen(html) + strlen(html2)) * sizeof(char);;

            // Reservo el espacio suficiente para almacenar el buffer 
            // junto con los headers del HTML mostrar en el Browser.    
            data = malloc(data_size);

            // Envio la información al Browser.
            sendDataToBrowser(headers, contentTypeText, html, 
                "No existe el archivo en el server FORK", data, html2, 
                child, data_size);
                        
            close(child);
            free(data);
            free(fileName);
            exit(0);

        }

        else
        {
            /*Codigo de cliente TODO */
        }

        
    }

    return fd;
}


void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



int main(void)
{
    int sockfd,  child, numbytes;  // listen on sock_fd, new connection on  child
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char buf[MAXDATASIZE];
    char s[INET6_ADDRSTRLEN];
    int rv;

    
  
    //snprintf(data, sizeof data, "%s %s", headers, html);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
         child = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if ( child == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
            if ((numbytes = recv( child, buf, MAXDATASIZE-1, 0)) == -1){
                perror("Server wasn't able to receive the data");
                exit(1);
            }

            buf[numbytes] = '\0';
            char *fileName = malloc(numbytes*sizeof(char));
            int fd, res, size;
            char path[2000] = {0};
            // argumento de fstat
            struct stat stat_buf;
            off_t offset = 0;
            char *data;
            int data_size = 0;
            if (numbytes > 5)
            {
                // Verifico si el request viene del Browser
                if (buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T' && buf[3] == ' ' && buf[4] == '/')
                {
                    
                    // Guardo el nombre del archivo en fileName.
                    //fileName = getFileNameFromBrowser(buf,numbytes,fileName);
                    for (int i = 0; i < numbytes; ++i) {
                        if (buf[i+5]!=' ') { 
                            fileName[i] = buf[i+5];
                        }
                    }

                    fd = openFile(fileName, child, 1);
                    free(fileName);

                    // Obtiene el tamaño del archivo
                    fstat(fd, &stat_buf);
                    printf("Tam del archivo %ld\n", stat_buf.st_size);

                    // Creo el buffer donde se almacena los datos del archivo.
                    char *datatoBrowser = malloc(stat_buf.st_size*sizeof(char));
                    
                    // Cargo los contenidos del archivo en el buffer datatoBrowser
                    read(fd, datatoBrowser, stat_buf.st_size);

                    // Cantidad de espacio necesario para desplegar el buffer.
                    data_size = (4 + strlen(headers) + strlen(contentTypeText)
                        + strlen(html) + stat_buf.st_size + strlen(html2))
                        * sizeof(char);

                    // Reservo el espacio suficiente para almacenar el buffer 
                    // junto con los headers del HTML mostrar en el Browser. 
                    data = malloc(data_size);

                    //printf("%s y el size es de: %ld\n", datatoBrowser, stat_buf.st_size);

                    // Envio la información al Browser.
                    sendDataToBrowser(headers, contentTypeText, html,
                        datatoBrowser, data, html2, child, data_size);
                        
                    // Cierro el hijo, libero memoria y termino el proceso en exito.
                    free(datatoBrowser);
                    free(data);
                    close(fd);
                    close(child);
                    exit(0);

                }

                else {

                    printf("%s\n", buf);

                    snprintf(data, sizeof data, "%s",buf);

                    if (send(child, data, strlen(data), 0) == -1)
                        perror("send");
                    close( child);
                    exit(0);
                }

            }

            else {

                printf("%s\n", buf);

                snprintf(data, sizeof data, "%s",buf);

                if (send( child, data, strlen(data), 0) == -1)
                    perror("send");
                close( child);
                exit(0);

            }

            
            
        }
        close( child);  // parent doesn't need this
    }

    return 0;
}
 
