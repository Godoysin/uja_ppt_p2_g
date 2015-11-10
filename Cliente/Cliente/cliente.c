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
#include <windows.h>

#include "protocol.h"


int main(int *argc, char *argv[])
{
	SOCKET sockfd;
	struct sockaddr_in server_in;
	char buffer_in[1024], buffer_out[1024], input[1024], sender[1024], mensaje[1024];
	char receiver[1024], buffer[1024];
	int recibidos=0,enviados=0;
	int estado=S_WAIT;
	char option;
	int fecha;
	char fech[1024];

	boolean bucle;

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

    char ipdest[16];
	char default_ip[16]="127.0.0.1";
	unsigned long ipdestl;
	struct in_addr address;
	
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

		
			//printf("CLIENTE> Introduzca la IP destino (pulsar enter para IP por defecto): ");
			//gets(ipdest);

			printf("Introduzca la direccion IP o el dominio destino: ");
			gets(ipdest);
			ipdestl=inet_addr(ipdest);
			if(ipdestl==INADDR_NONE){
				printf("entrar");
				//La dirección introducida por teclado no es correcta o
				//corresponde con un dominio.
				struct hostent *host;
				host=gethostbyname(ipdest);
				if(host!=NULL){
					memcpy(&address,host->h_addr_list[0],4);
					printf("\nDireccion %s\n",inet_ntoa(address));
					strcpy(ipdest,inet_ntoa(address));
				}
			}

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
						sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",HE,CRLF);
						break;

					case S_MAIL:
						printf("CLIENTE> Introduzca su direccion de correo electronico:%s",CRLF);
						gets(input);
						sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",MA,input,CRLF);
						strcpy(sender,input);
						break;

					case S_RCPT:
						printf("CLIENTE> Introduzca el correo electronico del destinatario:%s",CRLF);
						gets(input);
						sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",RE,input,CRLF);
						strcpy(receiver,input);
						break;

					case S_DATA:
						sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",DA,CRLF);
						break;

						//TODO enviar las siguientes cabeceras
						//Fecha origen
					case S_SEND: //Mail queued for delivery -> Mail en cola para entrega
						//Fecha
						fecha = getTimeZone();
						sprintf(fech,"%d",fecha);
						sprintf(buffer,"date: %s%s",fech,CRLF);
						//Para
						sprintf(buffer,"To: %s%s",receiver,CRLF);
						strcpy(buffer_out,buffer);
						//De:
						sprintf(buffer,"From: %s%s",sender,CRLF);
						strcat(buffer_out,buffer);
						//Asunto
						printf("Introduce el asunto del correo electronico%s",CRLF);
						gets(input);
						sprintf(buffer,"Subject: %s%s%s",input,CRLF,CRLF);
						strcat(buffer_out,buffer);
						//Mensaje
						printf("CLIENTE> Introduzca el contenido de su correo electronico%s",CRLF);
						printf("CLIENTE> Para terminar el mensaje introduzaca solo un punto%s",CRLF);
						do{
							gets(input);
							strcpy(buffer,input);
							strcat(buffer,CRLF);
							strcat(buffer_out,buffer);
						}while(strcmp(input,PNT) == 1);	//Si input=. sale.
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
						continue;

					case S_QUIT:
						sprintf(buffer_out,"%s %s",QU,CRLF);
						break;
				
					}
					//Envio
					if(estado!=S_WAIT){
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
						}//Control de errores
						//if(strncmp(buffer_in,ER2,1)==0){
							//printf("Se ha producido un error");
							//estado = S_QUIT;
						//}
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

int getTimeZone(){
   TIME_ZONE_INFORMATION tziOld;
   DWORD dwRet;
   dwRet = GetTimeZoneInformation(&tziOld);
   
   if(dwRet == TIME_ZONE_ID_STANDARD || dwRet == TIME_ZONE_ID_UNKNOWN)    
		tziOld.StandardBias/60;
   else if( dwRet == TIME_ZONE_ID_DAYLIGHT )
		return tziOld.DaylightBias/60;
   else{
      printf("GTZI failed (%d)\n", GetLastError());
      return 0;
   }
}