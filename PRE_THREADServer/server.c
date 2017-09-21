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
int arrayEstados[10];
int colaSolicitudes[10];
int cantSolicitudes = 0;
int cabeza = 0;
int cantidadHilos=0;


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

    int miPos= *(int*)socket_desc;
    int miEstado;
    int numbytes;
    char client_request[2000];
    char *sizeChar;
    unsigned char *bitmap;

    while(1){
        miEstado=arrayEstados[miPos];
        if (miEstado!=-1)
        {
             //Lee Request
            if((numbytes = recv(miEstado, client_request , 2000 , 0)) > 0)
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
                    
                    sendData(bitmap, sizeChar, miEstado, 4);
                }

                else
                {
                    sendData(bitmap, sizeChar, miEstado, tipoArchivo);
                }

            }
            //Free the socket pointer;
            close(miEstado);
            arrayEstados[miPos] = -1;
        }
   

    }

}

void push(int pIdSocket){
    if(cantSolicitudes>=10){
        puts("No se pueden aceptar mas solicitudes, la cola esta llena");
    }
    else{
        colaSolicitudes[(cabeza+cantSolicitudes)%10]=pIdSocket;
        cantSolicitudes++;
    }
}

int pop(){
    int valor;
    if(cantSolicitudes<=0){
        //puts("No hay solicitudes pendientes");
        return -1;
    }
    else{
        valor=colaSolicitudes[cabeza];
        cabeza=(cabeza+1)%10;
        cantSolicitudes = cantSolicitudes-1;
        return valor;
    }
}

void *verificarHilo(void *socket_desc){
    int contadorVerificador = 0;
    int socketRecuperado;
    while(1){
        
        for(contadorVerificador;contadorVerificador<cantidadHilos;contadorVerificador++){
            if(arrayEstados[contadorVerificador]==-1){
                socketRecuperado=pop();
                if(socketRecuperado!=-1){
                    arrayEstados[contadorVerificador]=socketRecuperado;
                }
                
            }
        
        }
        contadorVerificador = 0;
        sleep(1);
    }
    
}

int main(int argc , char *argv[])
{
    int socket_desc , new_socket, c, numbytes, *new_sock, *pHilo;
    struct sockaddr_in server, client;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("No pudo crear el socket");
    }

    int contadorHilos=0;
    int contadorEstados=0;
    int tam=sizeof(pthread_t);

    int cant_pthread = (int) argv[1];

    for(contadorEstados;contadorEstados<10;contadorEstados++){
        arrayEstados[contadorEstados]=-1;
    }

    cantidadHilos = atoi(argv[1]);
    pthread_t hiloActual;

    for(contadorHilos;contadorHilos<cantidadHilos;contadorHilos++){
        pHilo=malloc(1);
        *pHilo=contadorHilos;
        
        pthread_create( &hiloActual , NULL ,  ejecutarHilo , (void*) pHilo);
        //*copiaPuntero=hiloActual;
        //copiaPuntero=copiaPuntero+tam;
        
        
    }

    pthread_t verificador;
    pthread_create( &verificador , NULL ,  verificarHilo , NULL);
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("No pudo crear el socket");
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 7002 ); //Puerto

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

        push(new_socket);
        
    }
    return 0;
}