#ifndef protocolostpte_practicas_headerfile
#define protocolostpte_practicas_headerfile
#endif

// COMANDOS DE APLICACION
#define HE	"HELO"			// 
#define MA	"MAIL FROM:"	// 
#define RE	"RCPT TO:"		//
#define DA	"DATA"			//
#define QU	"QUIT"			// Finalizacion de la conexion de aplicacion

#define SD2 "EXIT"  // Finalizacion de la conexion de aplicacion y finaliza servidor

// RESPUESTAS A COMANDOS DE APLICACION
#define OK		"2"
#define OK2		"3"
#define ER		"ERROR"

//FIN DE RESPUESTA
#define CRLF "\r\n"
#define PNT "."

//ESTADOS
#define S_WAIT 0
#define S_HELO 1
#define S_MAIL 2
#define S_RCPT 3
#define S_DATA 4
#define S_SEND 5
#define S_CHOO 6
#define S_QUIT 7
#define S_EXIT 8

//PUERTO DEL SERVICIO
#define TCP_SERVICE_PORT	25

// NOMBRE Y PASSWORD AUTORIZADOS
#define USER		"alumno"
#define PASSWORD	"123456"

// COMANDOS
#define SUM			"SUM"