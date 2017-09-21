#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<signal.h>

#define BACKLOG 10  


FILE *f;

int splitFileName (char fileName[],int numbytes){

    int tipo = -1;

    char* fileNameCopy = malloc(numbytes * sizeof(char));

    strcpy(fileNameCopy,fileName);
    // Returns first token 
    char *token = strtok(fileNameCopy, ".");
   
    // Keep printing tokens while one of the
    // delimiters present in str[].
    token = strtok(NULL, ".");
    if (token!=NULL)
    {
        printf("El token luego del split es: %s\n", token);
        if (strcmp(token,"txt") == 0)
        {
            tipo = 1;
        }
        else if (strcmp(token,"html") == 0)
        {
            tipo = 2;
        }
        else if (strcmp(token,"png") == 0)
        {
            tipo = 3;
        }
        else if (strcmp(token,"jpg") == 0)
        {
            tipo = 4;
        }
        
    }
    free(fileNameCopy);
    return tipo;
}

char * getFileName(char buf[], int numbytes, char fileNameBuf[]){
    char *fileName = malloc(numbytes*sizeof(char));
    int i = 0;
    for (i = 0; i < numbytes; ++i) {
        if (buf[i+5]!=' ') { 
            fileName[i] = buf[i+5];
        }
    }

    strcpy(fileNameBuf, fileName);

    free(fileName);
    return fileNameBuf;   
}

void sendData(unsigned char* bitmap, char* sizeChar, int new_socket,int tipoArchivo){
    //Llegar al EOF
    fseek(f,0,SEEK_END);
    int size = ftell(f);
    //Volver al inicio

    fseek(f,0,SEEK_SET);
    bitmap = (unsigned char*)malloc(size);
    fread(bitmap,size,1,f);

    sizeChar = (char*)malloc(16);
    snprintf(sizeChar, 16, "%d", size);

    write(new_socket, "HTTP/1.1 200 OK\n", 16);
    write(new_socket, "Content-length: ", 16);
    write(new_socket, sizeChar,16);
    write(new_socket, "\n",1);
    if (tipoArchivo == 1 || tipoArchivo == 2)
    {
        write(new_socket, "Content-Type: text/plain\n\n", 26);
    }
    else if (tipoArchivo == 3)
    {
        write(new_socket, "Content-Type: image/png\n\n", 25);
    }
    else
    {
        write(new_socket, "Content-Type: image/jpeg\n\n", 26);
    }
    
    write(new_socket,bitmap,size);
    fclose(f);

}

void *ejecutarHilo(void *socket_desc){

    int sock = *(int*)socket_desc;

    int numbytes;
    char client_request[2000];
    char *sizeChar;
    unsigned char *bitmap;
    //Lee Request
    if((numbytes = recv(sock, client_request , 2000 , 0)) > 0)
    {

        client_request[numbytes] = '\0';
        char *fileName = (char*)malloc(numbytes);

        fileName = getFileName(client_request,numbytes,fileName);
                
        int tipoArchivo = splitFileName(fileName,numbytes);

        char path[2000] = {0};

        strcat(path, "files/");
        strcat(path, fileName);

        //abrir archivo
        f=fopen(path,"r"); //concatenar despues
        if(f==NULL)
        {
            puts("Archivo no encontrado");
            f=fopen("404.jpg","r");
                    
            sendData(bitmap, sizeChar, sock, 4);
        }

        else
        {
            sendData(bitmap, sizeChar, sock, tipoArchivo);
        }

    }
    //Free the socket pointer
    free(socket_desc);
    close(sock);

}

int main(int argc , char *argv[])
{
    int socket_desc , new_socket, c, numbytes, *new_sock;
    struct sockaddr_in server, client;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("No pudo crear el socket");
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 7003 ); //Puerto

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("Enlace fallido");
        return 1;
    }
    puts("Socket enlazado");
    //Listen
    listen(socket_desc , BACKLOG);

    //Accept and incoming connection
    puts("Esperando por conexiones...");
    c = sizeof(struct sockaddr_in);

    while(1){
        new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);

        if (new_socket<0)
        {
            perror("Aceptación fallida");
            return 1;
        }
        

        puts("Conexión aceptada");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;
         
        if( pthread_create( &sniffer_thread , NULL ,  ejecutarHilo , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
        
        
        
        
        
    }
    return 0;
}
