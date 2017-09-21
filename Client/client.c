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
int tipoArchivo;
void *ejecutarHilo(void *);

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
        tipoArchivo = splitFileName(copiaMalloc,sizeof(char));
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
    tamanoreciv = read(socket_desc, server_reply , 50000000);
    if( tamanoreciv < 0)
    {
        puts("recv failed");
    }

    if (tipoArchivo == 1 || tipoArchivo == 2 || tipoArchivo == 4)
    {
        ptrserver_reply = &server_reply[75];
        //concatenar el nombre de la imagen solicitada
    snprintf(pfileName, sizeof pfileName, "%s", parametroFile);
    
    FILE *f;
    f = fopen(pfileName,"w");
    fwrite(ptrserver_reply,tamanoreciv-75,1,f);
    fclose(f);
    f = fopen("file.jpg","w");
    fwrite(ptrserver_reply,tamanoreciv-75,1,f);
    fclose(f);
    }
    else
    {
        ptrserver_reply = &server_reply[74];
        //concatenar el nombre de la imagen solicitada
    snprintf(pfileName, sizeof pfileName, "%s", parametroFile);
    
    FILE *f;
    f = fopen(pfileName,"w");
    fwrite(ptrserver_reply,tamanoreciv-74,1,f);
    fclose(f);
    f = fopen("file.jpg","w");
    fwrite(ptrserver_reply,tamanoreciv-74,1,f);
    fclose(f);
    }
    
    
    
}
