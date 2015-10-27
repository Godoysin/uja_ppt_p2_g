#ifndef protocolostpte_practicas_headerfile
#define protocolostpte_practicas_headerfile
#endif

// COMANDOS DE APLICACION
#define HE "HELO"  // SOLICITUD DE CONEXION USER usuario 
#define MA "MAIL FROM:"  // Password del usuario  PASS password
#define RE "RCPT TO:"
#define DA "DATA"

#define SD  "QUIT"  // Finalizacion de la conexion de aplicacion
#define SD2 "EXIT"  // Finalizacion de la conexion de aplicacion y finaliza servidor

// RESPUESTAS A COMANDOS DE APLICACION
#define OK  "2"
#define ER  "ERROR"

//FIN DE RESPUESTA
#define CRLF "\r\n"

//ESTADOS
#define S_WAIT 0
#define S_HELO 1
#define S_MAIL 2
#define S_RCPT 3
#define S_DATA 4
#define S_QUIT 5
#define S_EXIT 6

//PUERTO DEL SERVICIO
#define TCP_SERVICE_PORT	25

// NOMBRE Y PASSWORD AUTORIZADOS
#define USER		"alumno"
#define PASSWORD	"123456"

// COMANDOS
#define SUM			"SUM"