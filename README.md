# ServerWebC

# Table of Contents
1. [Example](#example)
2. [Example2](#example2)
3. [Third Example](#third-example)
4. [Análisis de resultados del programa](#análisis-de-resultados-del-programa)

## Example
## Example2
## Third Example

Se trata de 4 servidores implementados cada uno de diferente manera:

* __FIFO__: La idea es que el servidor esté oyendo en uno de los puertos y vaya atendiendo las peticiones conforme van llegando. Existe un solo proceso secuencial, ninguna acción se ejecuta en paralelo. De manera que el servidor irá atendiendo las peticiones una por una.
* __FORK:__ Este servidor recibirá las solicitudes y por cada nueva solicitud creará un nuevo proceso que la atienda. Esta claro que el servidor solo atiende solicitudes y los que se encargan de responder y de mantener la comunicación con los clientes son los nuevos procesos creados. Los procesos hijos deben morir cada vez que haya terminado la transferencia del archivo, al igual que la comunicación o socket debe ser cerrado. El proceso padre no debe morir, ni es necesario que lleve control de sus hijos.
* __THREAD:__ Este servidor recibirá las solicitudes y por cada nueva solicitud creará un nuevo hilo que la atienda. Esta claro que el servidor solo atiende solicitudes y los que se encargan de responder y de mantener la comunicación con los clientes son los hilos creados. Los hilos deben morir cada vez que haya terminado la transferencia del archivos, al igual que la comunicación o socket debe ser cerrado. El proceso padre no debe morir, ni es necesario que lleve control de sus hilos. Sin embargo, el proceso padre debe seguir recibiendo solicitudes, no debe quedarse esperando a que ningún hilo termine.
* __PRE-THREAD:__ Este servidor recibirá las solicitudes y cada nueva solicitud la distribuirá entre los n hilos que ya debe de tener creados. La cantidad de hilos corresponden a un parámetro que el servidor debe pedir al ejecutarlo. Está claro que el servidor solo atiende solicitudes y los que se encargan de responder y de mantener la comunicación con los clientes son los hilos. Los hilos no deben morir cada vez que haya terminado la transferencia del archivos, pero la comunicación o socket debe ser cerrado. El proceso padre no debe morir, y necesita tener información del estado de cada uno de sus hilos, con el fin de saber a cuál de ellos le puede asignar una nueva solicitud. El proceso padre debe seguir recibiendo solicitudes, no debe quedarse esperando a que ningún hilo termine  
* __CLIENTE:__ el cliente debe permitir que el usuario introduzca un nombre de un archivo y después solicitarlo al servidor. Con la información que obtiene del servidor debe salvar en el disco de la computadora donde corre el cliente el archivo que solicitó. Es claro que se pueden levantar varias instancias del cliente. Pero además, el usuario puede introducir el nombre de varios archivos separados por comas. Si este es el caso la aplicación debe generar un hilo por cada archivo. Y será el hilo el que se debe comunicar con el servidor por medio del Socket.



## Análisis de resultados del programa ##

### ¿Qué sirve? ###

#### Browser (Chrome, FireFox o Safari) ####

| Servers  | txt      |    html       | png  | jpg |
| :------: | :------: | :-----------: | :--: |:--: |
| FIFO | X   | X  | X | X |
| FORK | X   | X  | X | X |
| PRE_THREAD | X   | X  | X | X |
| THREAD | X   | X  | X | X |

#### Cliente ####

| Servers  | txt      |    html       | png  | jpg |
| :------: | :------: | :-----------: | :--: |:--: |
| FIFO | X   | X  |  | X* |
| FORK | X   | X  |  | X* |
| PRE_THREAD | X   | X  |  | X* |
| THREAD | X   | X  |  | X* |

### ¿Qué no sirve? ###
#### Cliente ####

| Servers  | txt      |    html       | png  | jpg |
| :------: | :------: | :-----------: | :--: |:--: |
| FIFO |    |   | X | X* |
| FORK |    |   | X | X* |
| PRE_THREAD |    |   | X | X* |
| THREAD |    |   | X | X* |

**NOTA: El "*" significa que dicho tipo de archivo falla o funciona dependiendo de su tamaño**

## Casos de Prueba ##

### Servidor Fifo ###


### Servidor Fork ###


### Servidor Pre-Thread ###


### Servidor Thread ###


### Cliente ###

## Lecciones Aprendidas  ##

### Servidor Fifo ###
Fue el servidor más sencillo y no presento mucha dificultad.

### Servidor Fork ###
Se presentarón algunos problemas, debido a como funciona el fork, sin embargo se logro implementar de forma correcta.

### Servidor Pre-Thread ###
Fue el más complicado de todos debido a que se tuvo que optar por manejar cada hilo dentro de un arreglo, y esto aumento la complejidad del servidor.

### Servidor Thread ###
Este servidor resulto muy similar al Fork y no fue tan compleja su implementación, a pesar que se tuvo que investigar como la librería pthread.h trabaja los hilos.

### Cliente ###
El cliente presenta un problema a la hora de guardar archivos .png y los archivos .jpg grandes no se graban en el disco de forma completa, por todo lo demás el cliente funciona correctamente. 

## Compilación ##
### Servidor FIFO ###
Acceder, mediante la consola de linux, a la carpeta FIFOserver y digitar el siguiente código de compilación:

>gcc -o server server.c

Y para correr basta con escribir:

>./server

**NOTA: El servidor corre en el puerto 7000.**

### Servidor Fork ###
Acceder, mediante la consola de linux, a la carpeta FORKserver y digitar el siguiente código de compilación:

>gcc -o server server.c

Y para correr basta con escribir:

>./server

**NOTA: El servidor corre en el puerto 7001.**

### Servidor Pre-Thread ###
Acceder, mediante la consola de linux, a la carpeta THREADserver y digitar el siguiente código de compilación:

>gcc -o server server.c -pthread

Y para correr basta con escribir:

>./server [cantidad_de_hilos]

**NOTA: El servidor corre en el puerto 7002.**

### Servidor Thread ###
Acceder, mediante la consola de linux, a la carpeta THREADserver y digitar el siguiente código de compilación:

>gcc -o server server.c -pthread

Y para correr basta con escribir:

>./server

**NOTA: El servidor corre en el puerto 7003.**

### Cliente ###
Acceder, mediante la consola de linux, a la carpeta Client y digitar el siguiente código de compilación:

>gcc -o client client.c -pthread

Y para correr basta con escribir:

>./client [IP] [PUERTO] [nombre_archivos]

Ejemplo 1:

>./client 127.0.0.1 7001 ejemplo.txt

Ejemplo 2:

>./client 127.0.0.1 7004 ejemplo.txt,ejemplo2.txt

**[nombre_archivos] = Nombre de los archivos que se desean utilizar, estos deben ir separados por comas**

## Conclusiones ##
