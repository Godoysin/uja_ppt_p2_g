/*******************************************************
Protocolos de Transporte
Grado en Ingenier�a Telem�tica
Dpto. Ingen�er�a de Telecomunicaci�n
Univerisdad de Ja�n

Fichero: servidor.c
Versi�n: 1.0
Fecha: 23/09/2012
Descripci�n:
	Servidor de eco sencillo TCP.

Autor: Juan Carlos Cuevas Mart�nez

*******************************************************/
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <Winsock2.h>

#include "protocol.h"




main()
{

	WORD wVersionRequested;
	WSADATA wsaData;
	SOCKET sockfd,nuevosockfd;
	struct sockaddr_in  local_addr,remote_addr;
	char buffer_out[1024],buffer_in[1024], cmd[10], usr[10], pas[10], sum[2048];
	char a[5];
	int i, j, flag;
	int err,tamanio;
	int fin=0, fin_conexion=0;
	int recibidos=0,enviados=0;
	int estado=0;

	/** INICIALIZACION DE BIBLIOTECA WINSOCK2 **
	 ** OJO!: SOLO WINDOWS                    **/
	wVersionRequested=MAKEWORD(1,1);
	err=WSAStartup(wVersionRequested,&wsaData);
	if(err!=0){
		return(-1);
	}
	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1){
		WSACleanup() ;
		return(-2);
	}
	/** FIN INICIALIZACION DE BIBLIOTECA WINSOCK2 **/


	sockfd=socket(AF_INET,SOCK_STREAM,0); //Creaci�n del socket- Crea un descriptor socket

	if(sockfd==INVALID_SOCKET)	{
		return(-3);
	}
	else {
		local_addr.sin_family		=AF_INET;			// Familia de protocolos de Internet
		local_addr.sin_port			=htons(TCP_SERVICE_PORT);	// Puerto del servidor
		local_addr.sin_addr.s_addr	=htonl(INADDR_ANY);	// Direccion IP del servidor Any cualquier disponible
													// Cambiar para que conincida con la del host
	}
	
	//Enlace el socket a la direccion local (IP y puerto)
	if(bind(sockfd,(struct sockaddr*)&local_addr,sizeof(local_addr))<0) //Bind- Asocia una direcci�n local con un socket
		return(-4);
	
	//Se prepara el socket para recibir conexiones y se establece el tama�o de cola de espera
	if(listen(sockfd,5)!=0) //Listen- crea una cola de espera para almacenar solicitudes de conexi�n
		return (-6);
	
	tamanio=sizeof(remote_addr);

	do
	{
		printf ("SERVIDOR> ESPERANDO NUEVA CONEXION DE TRANSPORTE\r\n"); //Llegamos aqu� si se ha creado bien la conexi�n 
		//con el cliente.
		
		nuevosockfd=accept(sockfd,(struct sockaddr*)&remote_addr,&tamanio); //Accept- Espera una solicitud de conexi�n

		if(nuevosockfd==INVALID_SOCKET) {
			
			return(-5);
		}

		printf ("SERVIDOR> CLIENTE CONECTADO\r\nSERVIDOR [IP CLIENTE]> %s\r\nSERVIDOR [CLIENTE PUERTO TCP]>%d\r\n",
					inet_ntoa(remote_addr.sin_addr),ntohs(remote_addr.sin_port));

		//Mensaje de Bienvenida
		sprintf_s (buffer_out, sizeof(buffer_out), "%s Bienvenindo al servidor de ECO%s",OK,CRLF);
		
		enviados=send(nuevosockfd,buffer_out,(int)strlen(buffer_out),0); //Send- env�a un mensaje
		//DONE Comprobar error de env�o
		if(enviados<=0)
		{
			if(enviados<0) //Caso que la operaci�n haya fallado =-1
			{
				fin_conexion = 1;
				printf ("SERVIDOR> Se ha producido un error en el env�o de datos\r\n");
			}
			else //Caso que la operaci�n se haya cerrado =0
			{
				fin_conexion = 1;
				printf ("SERVIDOR> Se ha cerrado la conexion de aplicacion\r\n");
			}
		}
		else //Si no hay errores en el env�o de datos, paso al estado correspondiente.
		{
			//Se reestablece el estado inicial
			estado = S_USER;
			fin_conexion = 0;
		}
		

		printf ("SERVIDOR> Esperando conexion de aplicacion\r\n");
		do
		{
			//Se espera un comando del cliente
			recibidos = recv(nuevosockfd,buffer_in,1023,0); //Recv- Recibir un mensaje
			//DONE Comprobar posible error de recepci�n
			if(recibidos<=0)
			{
				if(recibidos<0) //Caso de error
				{
					printf ("SERVIDOR> Se ha producido un error en la recepicon de datos\r\n");
					fin_conexion = 1;
					estado = S_QUIT;
				}
				else //Caso de cerrar conexion
				{
					printf ("SERVIDOR> Se ha cerrado la conexion de aplicacion\r\n");
					fin_conexion = 1;
					estado = S_QUIT;
				}
			}
			
			//Sin esta sentencia, en el caso de que recibidos sea negativo, se produce un error y se cuelga el servidor.
			if(recibidos>=0)
			{
				buffer_in[recibidos] = 0x00;
				printf ("SERVIDOR [bytes recibidos]> %d\r\nSERVIDOR [datos recibidos]>%s", recibidos, buffer_in);
			}
			
			switch (estado)
			{
				case S_USER:    /*****************************************/
					strncpy_s ( cmd, sizeof(cmd),buffer_in, 4);
					cmd[4]=0x00; //En C los arrays finalizan con el byte 0000 0000

					if ( strcmp(cmd,SC)==0 ) //Si el comando recibido es solicitud de conexion de aplicaci�n.
					{
						sscanf_s (buffer_in,"USER %s\r\n",usr,sizeof(usr));
						
						//No tiene sentido que te permita introducir un usuario incorrecto
						//y que no lo puedas modificar sin tener que reestablecer la conexi�n.
						// envia OK acepta todos los usuarios hasta que tenga la clave

						//Si el usuario es correcto, pasa a pedir la clave
						if(strcmp(usr,USER)==0){
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s", OK,CRLF);
						
							estado = S_PASS;
							printf ("SERVIDOR> Esperando clave\r\n");
						}
						else{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s Autenticacion erronea%s", ER,CRLF);

							estado = S_USER;
							printf ("SERVIDOR> Esperando usuario\r\n");
						}
					} 
					else
					if( strcmp(cmd,SD)==0 ) //Finalizacion de la conexion de aplicaci�n.
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Fin de la conexion%s", OK,CRLF);
						fin_conexion=1;
					}
					else
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Comando incorrecto%s",ER,CRLF);
					}
				break;

				case S_PASS: /******************************************************/

					
					strncpy_s ( cmd, sizeof(cmd), buffer_in, 4);
					cmd[4]=0x00; //En C los arrays finalizan con el byte 0000 0000

					if ( strcmp(cmd,PW)==0 ) //Si el comando recibido es password:
					{
						sscanf_s (buffer_in,"PASS %s\r\n",pas,sizeof(pas));

						if ( (strcmp(usr,USER)==0) && (strcmp(pas,PASSWORD)==0) ) //Si el usuario y la clave recibidos son correctos
						{
							//env�a aceptaci�n de la conexi�n de aplicaci�n, nombre de usuario y
							//la direccion IP desde donde se ha conectado.
							sprintf_s (buffer_out, sizeof(buffer_out), "%s %s IP(%s)%s", OK, usr, inet_ntoa(remote_addr.sin_addr),CRLF);
							estado = S_DATA;
							printf ("SERVIDOR> Esperando comando\r\n");
						}
						else
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s Autenticaci�n err�nea%s",ER,CRLF);
						}
					} else
					if ( strcmp(cmd,SD)==0 ) //Finalizaci�n de la conexi�n de aplicaci�n.
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Fin de la conexion%s", OK,CRLF);
						fin_conexion=1;
					}
					else
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Comando incorrecto%s",ER,CRLF);
					}
				break;

				case S_DATA: /***********************************************************/
					
					buffer_in[recibidos] = 0x00;
					sum[0]=0x00;
					
					strncpy_s(cmd,sizeof(cmd), buffer_in, 4);
					strncpy_s(sum,sizeof(sum), buffer_in, 19);
					cmd[4]=0;

					printf ("SERVIDOR [Comando]>%s\r\n",cmd);

					//Operacion de SUMA
					if(strcmp(cmd,SUM)==0)
					{
						flag = 0;
						printf("SERVIDOR> Esperando numeros\r\n");
						sscanf(sum, "SUM %d %d", &i, &j);
						if(strcmp(cmd,SUM)==0){
							if(i<=(-10000)){
								sprintf_s (buffer_out, sizeof(buffer_out), "%s Primer numero demasiado corto: %d %s",ER,i,CRLF);
								flag = 1;
							}
							if(i>=10000){
								sprintf_s (buffer_out, sizeof(buffer_out), "%s Primer numero demasiado largo: %d %s",ER,i,CRLF);
								flag = 1;
							}
							if(j<=(-10000)){
								sprintf_s (buffer_out, sizeof(buffer_out), "%s Segundo numero demasiado corto: %d %s",ER,j,CRLF);
								flag = 1;
							}
							if(j>=10000){
								sprintf_s (buffer_out, sizeof(buffer_out), "%s Segundo numero demasiado largo: %d %s",ER,j,CRLF);
								flag = 1;
							}
							if(flag==0){
								sprintf_s (buffer_out, sizeof(buffer_out), "%s Suma = %d %s",OK,i+j,CRLF);
							}
						}
					}
					
					else if ( strcmp(cmd,SD)==0 ) //Finalizacion de la conexion de aplicacion
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Fin de la conexion%s", OK,CRLF);
						fin_conexion=1;
					}
					else if (strcmp(cmd,SD2)==0) //Finalizacion de la conexion de aplicacion y finaliza servidor
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Finalizando servidor%s", OK,CRLF);
						fin_conexion=1;
						fin=1;
					}
					else
					//if(strcmp(a,SUM)!=0&&strcmp(cmd,SD)!=0&&strcmp(cmd,SD2)!=0)
					{
						sprintf_s (buffer_out, sizeof(buffer_out), "%s Comando incorrecto yo%s",ER,CRLF);
					}
					break;

				default:
					break;
					
			} // switch

			enviados=send(nuevosockfd,buffer_out,(int)strlen(buffer_out),0); //Send- Enviar un mensaje
			//DONE
			if(enviados<=0)
			{
				if(enviados<0) //Caso de error
				{
					printf("SERVIDOR> Hay un error en el env�o de datos\r\n");
					fin_conexion=1;
				}
				else //Caso de fin de conexion
				{
					printf("SERVIDOR> Se ha cerrado la conexion\r\n");
					fin_conexion=1;
				}
			}


		} while (!fin_conexion);
		printf ("SERVIDOR> CERRANDO CONEXION DE TRANSPORTE\r\n");
		shutdown(nuevosockfd,SD_SEND); //Shutdown- Deshabilita la recepci�n y/o el env�o de datos por el socket
		closesocket(nuevosockfd); //Close- cierra socket

	}while(!fin);

	printf ("SERVIDOR> CERRANDO SERVIDOR\r\n");

	return(0);
} 
