#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h> 
#include<pthread.h> //for threading , link with lpthread

char *IPDir;
char *serverPort;
char *files;
void *ejecutarHilo(void *);

int main(int argc , char *argv[])
{
    char pIP[100];
    char *copiaMalloc;
    pthread_t hiloActual;
	int valorHilo;
    
    IPDir=argv[1];
    
    serverPort=argv[2];
    

	puts("IP");
	puts(IPDir);
	puts("Puerto");
	puts(serverPort);
    files = argv[3];

    char* token = strtok(files,",");
    //Desde aqui se debe ir sacando el nombre de los archivos.
    while(token!=NULL){
        
        copiaMalloc=(char*)malloc(1);
        strncpy(copiaMalloc,token,strlen(token));
        puts("Nombre del archivo a utilizar");
        puts(copiaMalloc);
        valorHilo=pthread_create( &hiloActual , NULL ,  ejecutarHilo , (void*) copiaMalloc);
        printf("%d",valorHilo);
        fflush(stdout);
        sleep(20);
        token = strtok(NULL,",");
        
    }
    
}
    
void *ejecutarHilo(void *fileName){
	puts("Ejecutando Hilo con\n ");
	
    int socket_desc,valor,tamanoreciv;
    struct sockaddr_in server;
    char *message, server_reply[5000000],buf[50],pfileName[30],*ptrserver_reply,*parametroFile;
    char *salvar="Recuperada";
    parametroFile=(char*)fileName;
    puts(parametroFile);
    
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    valor = atoi(serverPort);

    server.sin_addr.s_addr = inet_addr(IPDir);
    server.sin_family = AF_INET;
    server.sin_port = htons( valor );
    
    //Connect to remote server
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("connect error");
        return 1;
    }

    puts("Connected\n");
    
    //Send some data
    //se debe enviar el mensaje con la estructura del browser
    message = "GET /";
    snprintf(buf, sizeof buf, "%s%s%s", message, parametroFile," ");
    puts(buf);
    
    if( send(socket_desc , buf , sizeof(buf) , 0) < 0)
    {
        puts("Send failed");
        return 1;
    }
    puts("Data Send\n");
    sleep(5);
    tamanoreciv = recv(socket_desc, server_reply , 5000000 , 0);
    if( tamanoreciv < 0)
    {
        puts("recv failed");
    }
    ptrserver_reply = &server_reply[75];
    
    //concatenar el nombre de la imagen solicitada
    snprintf(pfileName, sizeof pfileName, "%s", parametroFile);
    
    FILE *f;
    f = fopen(pfileName,"w");
    fwrite(ptrserver_reply,tamanoreciv-75,1,f);
    fclose(f);
    
}
