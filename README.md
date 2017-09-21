# ServerWebC

### Miembros del equipo ###

Luis Quirós - 201237264

Ivan Calvo - 


# Índice
1. [¿Qué es este proyecto?](#qué-es-este-proyecto)
2. [Análisis de resultados del programa](#análisis-de-resultados-del-programa)
3. [Casos de Prueba](#casos-de-prueba)
4. [Lecciones Aprendidas](#lecciones-aprendidas)
5. [Compilación](#compilación)
6. [Investigación](#investigación)
7. [Conclusiones](#conclusiones)  

## ¿Qué es este proyecto?
Se trata de 4 servidores implementados cada uno de diferente manera:

* __FIFO__: La idea es que el servidor esté oyendo en uno de los puertos y vaya atendiendo las peticiones conforme van llegando. Existe un solo proceso secuencial, ninguna acción se ejecuta en paralelo. De manera que el servidor irá atendiendo las peticiones una por una.
* __FORK:__ Este servidor recibirá las solicitudes y por cada nueva solicitud creará un nuevo proceso que la atienda. Esta claro que el servidor solo atiende solicitudes y los que se encargan de responder y de mantener la comunicación con los clientes son los nuevos procesos creados. Los procesos hijos deben morir cada vez que haya terminado la transferencia del archivo, al igual que la comunicación o socket debe ser cerrado. El proceso padre no debe morir, ni es necesario que lleve control de sus hijos.
* __THREAD:__ Este servidor recibirá las solicitudes y por cada nueva solicitud creará un nuevo hilo que la atienda. Esta claro que el servidor solo atiende solicitudes y los que se encargan de responder y de mantener la comunicación con los clientes son los hilos creados. Los hilos deben morir cada vez que haya terminado la transferencia del archivos, al igual que la comunicación o socket debe ser cerrado. El proceso padre no debe morir, ni es necesario que lleve control de sus hilos. Sin embargo, el proceso padre debe seguir recibiendo solicitudes, no debe quedarse esperando a que ningún hilo termine.
* __PRE-THREAD:__ Este servidor recibirá las solicitudes y cada nueva solicitud la distribuirá entre los n hilos que ya debe de tener creados. La cantidad de hilos corresponden a un parámetro que el servidor debe pedir al ejecutarlo. Está claro que el servidor solo atiende solicitudes y los que se encargan de responder y de mantener la comunicación con los clientes son los hilos. Los hilos no deben morir cada vez que haya terminado la transferencia del archivos, pero la comunicación o socket debe ser cerrado. El proceso padre no debe morir, y necesita tener información del estado de cada uno de sus hilos, con el fin de saber a cuál de ellos le puede asignar una nueva solicitud. El proceso padre debe seguir recibiendo solicitudes, no debe quedarse esperando a que ningún hilo termine  
* __CLIENTE:__ el cliente debe permitir que el usuario introduzca un nombre de un archivo y después solicitarlo al servidor. Con la información que obtiene del servidor debe salvar en el disco de la computadora donde corre el cliente el archivo que solicitó. Es claro que se pueden levantar varias instancias del cliente. Pero además, el usuario puede introducir el nombre de varios archivos separados por comas. Si este es el caso la aplicación debe generar un hilo por cada archivo. Y será el hilo el que se debe comunicar con el servidor por medio del Socket.



## Análisis de resultados del programa ##

### Servidor Fifo ###
| Característica  | Estado      |    Descripción       |
| :------ | :------: | :----------- |
| Levantamiento del Servidor | A    |Se logra crear el servidor el cuál espera por peticiones   |
| Atención de las peticiones Cliente | A   |Recibe exitosamente las peticiones del cliente   |
| Atención de las peticiones del Browser |  A  |Recibe exitosamente las peticiones del Browser   |
| Manejo de Excepciones de request inválidos |  A  |El servidor es capaz de presentarle al cliente una imagen default al solicitarle una imagen que no existe   |
| Respuesta del Server |  A  |Despliega la imagen solicitada de forma correcta en el browser   |

### Servidor Fork ###
| Característica  | Estado      |    Descripción       |
| :------ | :------: | :----------- |
| Levantamiento del Servidor | A    |Se logra crear el servidor el cuál espera por peticiones   |
| Atención de las peticiones Cliente | A   |Recibe exitosamente las peticiones del cliente   |
| Atención de las peticiones del Browser |  A  |Recibe exitosamente las peticiones del Browser   |
| Manejo de Excepciones de request inválidos |  A  |El servidor es capaz de presentarle al cliente una imagen default al solicitarle una imagen que no existe   |
| Respuesta del Server |  A  |Despliega la imagen solicitada de forma correcta en el browser   |

### Servidor Pre-Thread ###
| Característica  | Estado      |    Descripción       |
| :------ | :------: | :----------- |
| Levantamiento del Servidor | A    |Se logra crear el servidor el cuál espera por peticiones   |
| Atención de las peticiones Cliente | A   |Recibe exitosamente las peticiones del cliente   |
| Atención de las peticiones del Browser |  A  |Recibe exitosamente las peticiones del Browser   |
| Manejo de Excepciones de request inválidos |  A  |El servidor es capaz de presentarle al cliente una imagen default al solicitarle una imagen que no existe   |
| Respuesta del Server |  A  |Despliega la imagen solicitada de forma correcta en el browser   |


### Servidor Thread ###
| Característica  | Estado      |    Descripción       |
| :------ | :------: | :----------- |
| Levantamiento del Servidor | A    |Se logra crear el servidor el cuál espera por peticiones   |
| Atención de las peticiones Cliente | A   |Recibe exitosamente las peticiones del cliente   |
| Atención de las peticiones del Browser |  A  |Recibe exitosamente las peticiones del Browser   |
| Manejo de Excepciones de request inválidos |  A  |El servidor es capaz de presentarle al cliente una imagen default al solicitarle una imagen que no existe   |
| Respuesta del Server |  A  |Despliega la imagen solicitada de forma correcta en el browser   |


### Cliente ###
| Característica  | Estado      |    Descripción       |
| :------ | :------: | :----------- |
| Levantamiento del Cliente | A    |Se logra crear el cliente el cuál envía peticiones   |
| Envio de un request | A   |Se envia correctamente el request  |
| Múltiples envíos de archivos | A   | Los archivos son enviados con éxito   |
| Envio de un request | A   |Recibe exitosamente las peticiones del cliente |
| Capacidad de salvar en disco la respuesta |  C  | Presenta problemas con archivos .png y .jpg grandes   |


## Casos de Prueba ##

### Servidor Fifo ###

#### TC-Fifo Cliente ####
| Inputs  | Resultados Esperados      |    Resultados Obtenidos       |
| :------ | :------: | :----------- |
| ./cliente 127.0.0.1 7000 html1.html | Archivo html1.html es guardado en la carpeta del Cliente    | Archivo html1.html es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7000 texto1.txt | Archivo texto1.txt es guardado en la carpeta del Cliente    | Archivo texto1.txt es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7000 img1.jpg | Archivo img1.jpg es guardado en la carpeta del Cliente    | No todo el archivo img1.jpg es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7000 img4.jpg | Archivo img4.jpg es guardado en la carpeta del Cliente    | Archivo img4.jpg es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7000 img1.png | Archivo img1.png es guardado en la carpeta del Cliente    | No todo el archivo img1.png es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7000 img122.jpg | Archivo img122.jpg es guardado pero con una imagen de "File Not Found" en la carpeta del Cliente    | Archivo img122.jpg es guardado pero con una imagen de "File Not Found" en la carpeta del Cliente |
| ./cliente 127.0.0.1 7000 html1.html,texto1.txt | Archivos html1.html y texto1.txt son guardados en la carpeta del Cliente    | Archivos html1.html y texto1.txt son guardados en la carpeta del Cliente |

#### TC-Fifo Browser ####
| Inputs  | Resultados Esperados      |    Resultados Obtenidos       |
| :------ | :------: | :----------- |
| localhost:7000/html1.html | Archivo html1.html es mostrado en el Browser    | Archivo html1.html es mostrado en el Browser   |
| localhost:7000/texto1.txt | Archivo texto1.txt es mostrado en el Browser    | Archivo texto1.txt es mostrado en el Browser   |
| localhost:7000/img1.jpg | Archivo img1.jpg es mostrado en el Browser    | Archivo img1.jpg es mostrado en el Browser   |
| localhost:7000/img4.jpg | Archivo img4.jpg es mostrado en el Browser    | Archivo img4.jpg es mostrado en el Browser   |
| localhost:7000/img1.png | Archivo img1.png es mostrado en el Browser    | Archivo img1.png es mostrado en el Browser |
| localhost:7000/img122.jpg | Se muestra una imagen de "File Not Found"  | Se muestra una imagen de "File Not Found" |


### Servidor Fork ###
#### TC-Fork Cliente ####
| Inputs  | Resultados Esperados      |    Resultados Obtenidos       |
| :------ | :------: | :----------- |
| ./cliente 127.0.0.1 7001 html1.html | Archivo html1.html es guardado en la carpeta del Cliente    | Archivo html1.html es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7001 texto1.txt | Archivo texto1.txt es guardado en la carpeta del Cliente    | Archivo texto1.txt es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7001 img1.jpg | Archivo img1.jpg es guardado en la carpeta del Cliente    | No todo el archivo img1.jpg es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7001 img4.jpg | Archivo img4.jpg es guardado en la carpeta del Cliente    | Archivo img4.jpg es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7001 img1.png | Archivo img1.png es guardado en la carpeta del Cliente    | No todo el archivo img1.png es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7001 img122.jpg | Archivo img122.jpg es guardado pero con una imagen de "File Not Found" en la carpeta del Cliente    | Archivo img122.jpg es guardado pero con una imagen de "File Not Found" en la carpeta del Cliente |
| ./cliente 127.0.0.1 7001 html1.html,texto1.txt | Archivos html1.html y texto1.txt son guardados en la carpeta del Cliente    | Archivos html1.html y texto1.txt son guardados en la carpeta del Cliente |

#### TC-Fork Browser ####
| Inputs  | Resultados Esperados      |    Resultados Obtenidos       |
| :------ | :------: | :----------- |
| localhost:7001/html1.html | Archivo html1.html es mostrado en el Browser    | Archivo html1.html es mostrado en el Browser   |
| localhost:7001/texto1.txt | Archivo texto1.txt es mostrado en el Browser    | Archivo texto1.txt es mostrado en el Browser   |
| localhost:7001/img1.jpg | Archivo img1.jpg es mostrado en el Browser    | Archivo img1.jpg es mostrado en el Browser   |
| localhost:7001/img4.jpg | Archivo img4.jpg es mostrado en el Browser    | Archivo img4.jpg es mostrado en el Browser   |
| localhost:7001/img1.png | Archivo img1.png es mostrado en el Browser    | Archivo img1.png es mostrado en el Browser |
| localhost:7001/img122.jpg | Se muestra una imagen de "File Not Found"  | Se muestra una imagen de "File Not Found" |


### Servidor Pre-Thread ###
#### TC-Pre_Thread Cliente ####
| Inputs  | Resultados Esperados      |    Resultados Obtenidos       |
| :------ | :------: | :----------- |
| ./cliente 127.0.0.1 7002 html1.html | Archivo html1.html es guardado en la carpeta del Cliente    | Archivo html1.html es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7002 texto1.txt | Archivo texto1.txt es guardado en la carpeta del Cliente    | Archivo texto1.txt es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7002 img1.jpg | Archivo img1.jpg es guardado en la carpeta del Cliente    | No todo el archivo img1.jpg es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7002 img4.jpg | Archivo img4.jpg es guardado en la carpeta del Cliente    | Archivo img4.jpg es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7002 img1.png | Archivo img1.png es guardado en la carpeta del Cliente    | No todo el archivo img1.png es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7002 img122.jpg | Archivo img122.jpg es guardado pero con una imagen de "File Not Found" en la carpeta del Cliente    | Archivo img122.jpg es guardado pero con una imagen de "File Not Found" en la carpeta del Cliente |
| ./cliente 127.0.0.1 7002 html1.html,texto1.txt | Archivos html1.html y texto1.txt son guardados en la carpeta del Cliente    | Archivos html1.html y texto1.txt son guardados en la carpeta del Cliente |

#### TC-Pre_Thread Browser ####
| Inputs  | Resultados Esperados      |    Resultados Obtenidos       |
| :------ | :------: | :----------- |
| localhost:7002/html1.html | Archivo html1.html es mostrado en el Browser    | Archivo html1.html es mostrado en el Browser   |
| localhost:7002/texto1.txt | Archivo texto1.txt es mostrado en el Browser    | Archivo texto1.txt es mostrado en el Browser   |
| localhost:7002/img1.jpg | Archivo img1.jpg es mostrado en el Browser    | Archivo img1.jpg es mostrado en el Browser   |
| localhost:7002/img4.jpg | Archivo img4.jpg es mostrado en el Browser    | Archivo img4.jpg es mostrado en el Browser   |
| localhost:7002/img1.png | Archivo img1.png es mostrado en el Browser    | Archivo img1.png es mostrado en el Browser |
| localhost:7002/img122.jpg | Se muestra una imagen de "File Not Found"  | Se muestra una imagen de "File Not Found" |


### Servidor Thread ###
#### TC-Thread Cliente ####
| Inputs  | Resultados Esperados      |    Resultados Obtenidos       |
| :------ | :------: | :----------- |
| ./cliente 127.0.0.1 7003 html1.html | Archivo html1.html es guardado en la carpeta del Cliente    | Archivo html1.html es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7003 texto1.txt | Archivo texto1.txt es guardado en la carpeta del Cliente    | Archivo texto1.txt es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7003 img1.jpg | Archivo img1.jpg es guardado en la carpeta del Cliente    | No todo el archivo img1.jpg es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7003 img4.jpg | Archivo img4.jpg es guardado en la carpeta del Cliente    | Archivo img4.jpg es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7003 img1.png | Archivo img1.png es guardado en la carpeta del Cliente    | No todo el archivo img1.png es guardado en la carpeta del Cliente   |
| ./cliente 127.0.0.1 7003 img122.jpg | Archivo img122.jpg es guardado pero con una imagen de "File Not Found" en la carpeta del Cliente    | Archivo img122.jpg es guardado pero con una imagen de "File Not Found" en la carpeta del Cliente |
| ./cliente 127.0.0.1 7003 html1.html,texto1.txt | Archivos html1.html y texto1.txt son guardados en la carpeta del Cliente    | Archivos html1.html y texto1.txt son guardados en la carpeta del Cliente |

#### TC-Thread Browser ####
| Inputs  | Resultados Esperados      |    Resultados Obtenidos       |
| :------ | :------: | :----------- |
| localhost:7003/html1.html | Archivo html1.html es mostrado en el Browser    | Archivo html1.html es mostrado en el Browser   |
| localhost:7003/texto1.txt | Archivo texto1.txt es mostrado en el Browser    | Archivo texto1.txt es mostrado en el Browser   |
| localhost:7003/img1.jpg | Archivo img1.jpg es mostrado en el Browser    | Archivo img1.jpg es mostrado en el Browser   |
| localhost:7003/img4.jpg | Archivo img4.jpg es mostrado en el Browser    | Archivo img4.jpg es mostrado en el Browser   |
| localhost:7003/img1.png | Archivo img1.png es mostrado en el Browser    | Archivo img1.png es mostrado en el Browser |
| localhost:7003/img122.jpg | Se muestra una imagen de "File Not Found"  | Se muestra una imagen de "File Not Found" |



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


## Investigación ##
## Conclusiones ##
