#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <fcntl.h> 
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>

#define NUM 100
#define SALIR "SALIR"

#define BACKLOG 10  


FILE *f;

struct peticion{
	int socket_cliente;
	char * peticion;
};

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
    fclose(f);
    write(new_socket,bitmap,size);
    

}

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

void *enviar(void *data){
	struct peticion * p = (struct peticion *) data;
	// file descriptor y return code
	int fd, rc=1, size;
	// arguento de fstat
	struct stat stat_buf;
	// file offset
	off_t offset = 0; 
	// Abre el archivo a enviar
	fd = open(p->peticion, O_RDONLY);

	//Si hay un erro abriendolo
	if(fd == -1){
		printf("* ERROR ABRIENDO EL ARCHIVO\n");
		return;
	}

	// Obtiene el tamaño del archivo a enviar
	fstat(fd, &stat_buf);
	printf("* TAMAÑO DEL ARCHIVO %ld\n", stat_buf.st_size);

	//Enviamos el tam al cliente para que sepa el tam del archivo que va recibir
	size = htonl(stat_buf.st_size);
	write(p->socket_cliente, &size, sizeof(size));

	offset = 0;
	// Se envia el archivo
	rc = sendfile(p->socket_cliente, fd, &offset, stat_buf.st_size);
	close(fd);
	if(rc == -1){
		printf("* ERROR EN SENDFILE()\n");
		return;
	}
	if (rc != stat_buf.st_size){
		printf("* ERROR TRANSFERENCIA DE DATOS INCOMPLETA\n");
		return;
	}
	printf("* LA PETICION %s SE REALIZO EXITOSAMENTE\n", p->peticion);
}

void FIFO(struct peticion * p){
	enviar(p);
	free(p);
}

void FORK(struct peticion * p){
	// Manejo en modo fork
}

void THREAD(struct peticion * p){
	// Manejo en modo thread
}

void P_THREAD(struct peticion * p){
	// Manejo en modo p_thread
}



void controlador(int socket_cliente, char peticion[], int num, int tipo){
	// Le corta el \n o \r al final de la peticion
	peticion[num] = '\0';
	if(peticion[strlen(peticion)-1] == '\n')
    	peticion[strlen(peticion)-1] = '\0';
    if(peticion[strlen(peticion)-1] == '\r')
    	peticion[strlen(peticion)-1] = '\0';

    // Si la peticion es salir, retorna un 0
    if(strncmp(SALIR,peticion,NUM)==0)
    	return;

	printf("* ENTRO UNA PETICION DEL ARCHIVO: %s \n", peticion);

	// Estructura a usar para hacer las peticiones
	struct peticion * p = (struct peticion *)malloc(sizeof(struct peticion));
	p->socket_cliente = socket_cliente;
	p->peticion = peticion;

	switch(tipo){
		case 1:
			printf("* SE ATENDERA EN MODO FIFO\n");
			FIFO(p);
			break;
		case 2:
			printf("* SE ATENDERA EN MODO FORK\n");
			FORK(p);
			break;
		case 3:
			printf("* SE ATENDERA EN MODO THREAD\n");
			THREAD(p);
			break;
		case 4:
			printf("* SE ATENDERA EN MODO P_THREAD\n");
			P_THREAD(p);
			break;
		default:
			printf("Tipo no admitido");
			break;
	}

	p = NULL;
}



// Parámetros: puerto, tipo de servidor gcc -g servidor.c -o servidor
int main(int argc, char **argv)
{
	// Variables a utilizar
	int socket_servidor, socket_cliente, longitud_cliente, 
    puerto, tipo, numbytes, estado;

		char peticion[2000];
        char client_request[2000];
    
		
        char *sizeChar;
		tipo = 1;
		puerto = 7000;

		// Estructuras que guardan la info del servidor y del cliente
		struct sockaddr_in servidor, cliente;
        unsigned char *bitmap;

 		// Configuración del servidor
 		//Prepare the sockaddr_in structure
        servidor.sin_family = AF_INET;
        servidor.sin_addr.s_addr = INADDR_ANY;
        servidor.sin_port = htons( 7000 ); //Puerto

		// Definición del socket
		printf("* CREANDO SOCKET\n");
		if ((socket_servidor=socket(AF_INET,SOCK_STREAM,0) )<0){
 			perror("* ERROR DE APERTURA DE SOCKET");
			exit(-1);
		}
		printf("* CREACION DE SOCKET EXITOSA\n");

		// Avisa al sistema que creó un socket
		printf("* INICIANDO BINDING\n");
		if(bind(socket_servidor,(struct sockaddr*)&servidor, sizeof(struct sockaddr))==-1) {
			printf("* ERROR EN BIND\n");
			exit(-1);
		}
		printf("* BINDING EXITOSO\n");

		// Establece el socket en modo escucha
		printf("* INICIANDO LISTENING\n");
		if(listen(socket_servidor,BACKLOG) == -1) {
			printf("* ERROR EN LISTEN()\n");
			exit(-1);
		}
		printf("* LISTENING EXITOSO\n");

		// Ciclo para manejar las peticiones
		printf("* EL SERVIDOR COMENZARA A RECIBIR PETICIONES\n");
		printf("\n");
        longitud_cliente= sizeof(struct sockaddr_in);

		while(1){
			
			socket_cliente = accept(socket_servidor, (struct sockaddr *)&cliente, (socklen_t*)&longitud_cliente);

        	if (socket_cliente<0)
        	{
            	perror("Aceptación fallida");
            	return 1;
        	}
 			
 			if((numbytes = recv(socket_cliente, client_request , 2000 , 0)) > 0)
            {

            	printf("%s\n", client_request);

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
                    
                    sendData(bitmap, sizeChar, socket_cliente, 4);
                }

                else
                {
                    sendData(bitmap, sizeChar, socket_cliente, tipoArchivo);
                }


            } 



            // Cierra la peticion
 			close(socket_cliente);
		}
		close(socket_servidor);
		exit(0);

}