/*******************************************************
Protocolos de Transporte
Grado en Ingeniería Telemática
Dpto. Ingeníería de Telecomunicación
Univerisdad de Jaén

Fichero: cliente.c
Versión: 1.0
Fecha: 23/09/2012
Descripción:
	Cliente de eco sencillo TCP.

Autor: Juan Carlos Cuevas Martínez

*******************************************************/
#include <stdio.h>
#include <winsock.h>
#include <time.h>
#include <conio.h>

#include "protocol.h"


int main(int *argc, char *argv[])
{
	SOCKET sockfd;
	struct sockaddr_in server_in;
	char buffer_in[1024], buffer_out[1024], input[1024];
	int recibidos=0,enviados=0;
	int estado=S_WAIT;
	char option;

	boolean bucle;

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

    char ipdest[16];
	char default_ip[16]="127.0.0.1";
	
	//Inicialización Windows sockets
	wVersionRequested=MAKEWORD(1,1);
	err=WSAStartup(wVersionRequested,&wsaData);
	if(err!=0)
		return(0);

	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1)
	{
		WSACleanup();
		return(0);
	}
	//Fin: Inicialización Windows sockets

	do{ 
		sockfd=socket(AF_INET,SOCK_STREAM,0); //socket- Crea un descriptor socket

		if(sockfd==INVALID_SOCKET)
		{
			printf("CLIENTE> ERROR AL CREAR SOCKET\r\n");
			exit(-1);
		}
		else
		{
			printf("CLIENTE> SOCKET CREADO CORRECTAMENTE\r\n");

		
			printf("CLIENTE> Introduzca la IP destino (pulsar enter para IP por defecto): ");
			gets(ipdest);

			if(strcmp(ipdest,"")==0) //Si no introduces una IP, coge una por defecto
				strcpy(ipdest,default_ip);


			server_in.sin_family=AF_INET;
			server_in.sin_port=htons(TCP_SERVICE_PORT);
			server_in.sin_addr.s_addr=inet_addr(ipdest);
			
			estado = S_WAIT;
		
			//Establece la conexion de transporte.
			if(connect(sockfd,(struct sockaddr*)&server_in,sizeof(server_in))==0) //Connect- Inicia conexión con conector remoto
			//Si se conecta, te dice que se ha establecido conexión, si no, no.
			
			//El cliente empieza aquí, hay que implementar el saludo al servido (los comandos) y deben ser transparentes al usuario (ni los siente ni los padece)
			//lo que si hay que hacer es pedirle su email, destinatario y "algo más" y el mensaje que va a enviar.
			//Para ello en principio solo habría que modificar la máquina de estados.
			//Descargar el software de Ilias
			{
				printf("CLIENTE> CONEXION ESTABLECIDA CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);
			
		
				//Inicio de la máquina de estados.
				do{
					switch(estado)
					{
					case S_WAIT:
						//Se recibe el mensaje de bienvenida.
						break;

					case S_HELO:
						//Establece la conexion de aplicacion 
						printf("%s%s",HE,CRLF);
						sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",HE,CRLF);
						break;

					case S_MAIL:
						printf("%s%s",MA,CRLF);
						printf("CLIENTE> Introduzca su direccion de correo electronico:%s",CRLF);
						gets(input);
						sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",MA,input,CRLF);
						break;

					case S_RCPT:
						printf("%s%s",RE,CRLF);
						printf("CLIENTE> Introduzca el correo electronico del destinatario:%s",CRLF);
						gets(input);
						sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",RE,input,CRLF);
						break;

					case S_DATA:
						printf("%s%s",DA,CRLF);
						sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",DA,CRLF);
						break;

						//TODO enviar las siguientes cabeceras
						//Fecha origen
						//Asunto
						//Destinatario
						//Remitente
						//strcat
					case S_SEND: //Mail queued for delivery -> Mail en cola para entrega
						printf("SEND%s",CRLF);
						printf("CLIENTE> Introduzca el contenido de su correo electronico%s",CRLF);
						printf("CLIENTE> Escriba y pulse Enter para saltar de línea%s",CRLF);
						printf("CLIENTE> Para terminar el mensaje pulse Enter%s",CRLF);
						do{
							gets(input);
							if(strcmp(input,"") == 0){
								sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",PNT,CRLF);
							}
							else{
								sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",input,CRLF);
							}
						}while(strcmp(input,"") == 1);	//Si input=null sale.
						break;

					case S_CHOO:
						do{
							printf("CLIENTE> Elija enviar un nuevo correo electronico o salir de la aplicacion%s",CRLF);
							printf("CLIENTE> Nuevo Correo C, Salir S%s",CRLF);
							gets(input);
							if(strcmp(input,"C") == 0 || strcmp(input,"c") == 0){//Nuevo correo
								estado = S_MAIL;
								bucle = TRUE;
							}
							else if(strcmp(input,"S") == 0 || strcmp(input,"s") == 0){//Salir
								estado = S_QUIT;
								bucle = TRUE;
							}
							else{//Caso de error
								printf("%s%s",ER,CRLF);
								bucle = FALSE;
							}
						}while(bucle == FALSE);
						break;
				
					}
					//Envio
					if(estado!=S_WAIT && estado!=S_CHOO){
						//Ejercicio: Comprobar el estado de envio
						enviados=send(sockfd,buffer_out,(int)strlen(buffer_out),0); //Send- envía un mensaje
						if(enviados<=0)
						{
							if(enviados<0){
								printf("CLIENTE> Error en la recepcion de datos%s",CRLF);
								estado=S_QUIT;
							}
							else{
								printf("CLIENTE> Conexion con el servidor cerrada%s",CRLF);
								estado=S_QUIT;
							}
						}
					}

					//Recibo
					recibidos=recv(sockfd,buffer_in,512,0); //Recv- Recibir un mensaje
					if(recibidos<=0){
						DWORD error=GetLastError();
						if(recibidos<0){
							printf("CLIENTE> Error %d en la recepcion de datos\r\n",error);
							estado=S_QUIT;
						}
						else{
							printf("CLIENTE> Conexion con el servidor cerrada\r\n");
							estado=S_QUIT;
						}
					}
					else{
						buffer_in[recibidos]=0x00;
						printf(buffer_in);
						//TODO Hay que tocar cosas.
						//De primeras envía un 220 a modo de OK.
						//De segundas envía un 250 a modo de OK.
						//Al final cojo el 2 como codigo de aceptacion.
						if(strncmp(buffer_in,OK,1)==0){
							//estado!=S_DATA &&
							estado++;
						}
						if(strncmp(buffer_in,OK2,1)==0 && estado == S_DATA){
							estado++;
						}
					}

				}while(estado != S_QUIT);
				
	
		
			}
			else
			{
				printf("CLIENTE> ERROR AL CONECTAR CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);
			}		
			//Fin de la conexion de transporte
			closesocket(sockfd);  //Close- cierra socket
			
		}	
		printf("-----------------------\r\n\r\nCLIENTE> Volver a conectar (S/N)\r\n");
		option=_getche();

	}while(option!='n' && option!='N');

	
	
	return(0);

}
//Escribes quit y no cierra.