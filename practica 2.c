Autor: Laura Moreno Chicharro

*******************************************************/
#include <stdio.h>
#include <winsock.h>
#include <time.h>
#include <conio.h>


#define TCP_SERVER_PORT	25

//Estados
#define S_HELO 0
#define S_MAILFROM 1
#define S_RCPTTO 2
#define S_DATA 3
#define S_EMAIL 4
#define S_NUEVOMAIL 5
#define S_QUIT 6
#define S_EXIT 7



int main(int *argc, char *argv[])
{
	SOCKET sockfd;
	struct sockaddr_in server_in;
	char buffer_in[2048], buffer_out[2048];
	char host[1024];
	char correo[1024];
	char rcpt[1024];
	char mensaje[1024];
	char buffer[1024];
	char buffer2[2048];
	int recibidos=0,enviados=0;
	int estado=S_HELO;
	int continuar=1;
	char opcion[1024];
	char Si[]="si";
	char punto[]=".";
	char Quit[]="quit";


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

	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1){
		WSACleanup();
		return(0);
	}
	//Fin: Inicialización Windows sockets

	do{
	sockfd=socket(PF_INET,SOCK_STREAM,0);

	if(sockfd==INVALID_SOCKET){
		printf("\nERROR AL CREAR SOCKET");
	}
	else{
		printf("\nSOCKET CREADO CORRECTAMENTE");

		
		printf("Introduce la IP destino (enter para IP por defecto)");
		gets(ipdest);
		if(strcmp(ipdest,"")==0)
			strcpy(ipdest,default_ip);


		server_in.sin_family=AF_INET;
		server_in.sin_port=htons(TCP_SERVER_PORT);
		server_in.sin_addr.s_addr=inet_addr(ipdest);
		
		// establece la conexion de transporte
		if(connect(sockfd,(struct sockaddr*)&server_in,sizeof(server_in))==0)
		{
			printf("\nCONEXION ESTABLECIDA CON %s:%d",ipdest,TCP_SERVER_PORT);
			
			//recepcion de datos inicuales
			recibidos=recv(sockfd,buffer_in,512,0);
			buffer_in[recibidos]=0x00;
			printf("\n%s",buffer_in);


			//Inicio de la máquina de estados
			do{
				switch(estado)
				{
				case S_HELO:
					// mandando el mensaje HELO
					printf("Nombre del Host\n");
					gets(host);
					if(strcmp(Quit,host)==0){
						closesocket(sockfd);
						_getch();
						exit(-1);
					}
					sprintf(buffer_out, "HELO %s\r\n",host);
					//a continuacion el debe responderme con un mensaje de bienvenida
					break;


				case S_MAILFROM:
					// envio del mail from
					printf("Remitente\n");
					gets(correo);
					if(strcmp(Quit,correo)==0){
						closesocket(sockfd);
						_getch();
						exit(-1);
					}
					sprintf(buffer_out,"MAIL FROM: %s\r\n",correo);
					break;
				
				
				case S_RCPTTO:
					//envio del rcpt to
					printf("Destinatario\n");
					gets(rcpt);
					if(strcmp(Quit,rcpt)==0){
						closesocket(sockfd);
						_getch();
						exit(-1);
					}
					sprintf(buffer_out, "RCPT TO: %s\r\n",rcpt);
					break;


				case S_DATA:
					//envio del data
					sprintf (buffer_out, "DATA\r\n");
					break;

				case S_EMAIL:
					//envio del e-mail

					//destinariaro
					printf("Para:");
					gets(mensaje);
					sprintf(buffer_out,"To: %s\r\n",mensaje);

					//remitente
					printf("De:");
					gets(mensaje);
					sprintf(buffer,"From: %s\r\n",mensaje);
					strcat(buffer_out,buffer);

					//asunto
					printf("Asunto:");
					gets(mensaje);
					sprintf(buffer,"Subject: %s\r\n",mensaje);
					strcat(buffer_out,buffer);

					//mensaje
					printf("Mensaje:");
					sprintf(buffer," ");
					do{
						gets(mensaje);
						strcat(buffer,mensaje);
						strcat(buffer,"\n");
					}while(strncmp(punto,mensaje,1)!=0);
					strcat(buffer_out,"Mensaje: ");
					strcat(buffer_out,buffer);
					strcat(buffer_out,"\r\n");
					strcat(buffer_out,".\r\n");
					printf(buffer_out);
					//fin del mensaje

					break;

				case S_NUEVOMAIL:
					printf("\n¿Desea mandar un nuevo mensaje? (Escriba si o no)\n");
					if(strcmp(Si,gets(opcion))==0){
						estado=-1;
					}
					else{
						printf("¿Desea establecer una nueva conexion? (Escriba si o no)\n");
						if(strcmp(Si,gets(opcion))==0){
							continuar=1;
							estado=6;
							continue;
						}
						else{
							continuar=0;
							estado=6;
							continue;
						}
					}
					break;

				case S_QUIT:
					//envio del quit
					sprintf(buffer_out,"QUIT \r\n",mensaje);
					break;
				
				}
				

				//Envio
				if(estado!=-1){
					if(send(sockfd,buffer_out,(int)strlen(buffer_out),0)<=0){
						closesocket(sockfd);
						printf("\nerror al enviar datos al servidor en el echo");
						_getche();
						exit(-1);
					}
				}


				//Recibo
				if(estado>=0){
					recibidos=recv(sockfd,buffer_in,512,0);
					if(recibidos<=0){
						DWORD error=GetLastError();
						if(recibidos<0){
							printf("Error %d en la recepción de datos.\r\nPulse una tecla para salir.",error);
							_getch();
							exit(-1);
						}
						else{
							printf("Conexión con el servidor cerrada.\r\nPulse una tecla para salir.");
							_getch();
							exit(-1);

					
						}
					}
					if(strncmp("2",buffer_in,1)!=0){
						if(strncmp("3",buffer_in,1)!=0){
							printf("Envio erroneo");
							estado--;
						}
					}
					buffer_in[recibidos]=0x00;
					printf("\n%s",buffer_in);
					estado++;
				}
				//caso de seguir mandando mensajes
				else{
					estado=0;
					estado++;
				}
			}while(estado!=S_EXIT);
	
		
		}
		else{
			printf("\nERROR AL CONECTAR CON %s:%d",ipdest,TCP_SERVER_PORT);
		}		
		// fin de la conexion de transporte
		closesocket(sockfd);
		estado=0;
		//_getche();

	}
	//return(0);

}while(continuar==1);
printf("\r\nPulse una tecla para salir.");
			_getch();
			exit(-1);	
}
