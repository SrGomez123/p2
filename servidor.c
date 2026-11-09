#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PUERTO 9999
#define TAM_BUFFER 1024

// Función para configurar el socket, asignar puerto y ponerlo en escucha
int iniciar_servidor(int puerto) {
    int conector;
    struct sockaddr_in direccion_servidor;

    conector = socket(AF_INET, SOCK_STREAM, 0);
    if (conector == -1) {
        perror("Error al crear el socket del servidor");
        exit(EXIT_FAILURE);
    }

    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_addr.s_addr = INADDR_ANY;
    direccion_servidor.sin_port = htons(puerto);

    if (bind(conector, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor)) < 0) {
        perror("Error en bind");
        close(conector);
        exit(EXIT_FAILURE);
    }

    if (listen(conector, 5) < 0) {
        perror("Error en listen");
        close(conector);
        exit(EXIT_FAILURE);
    }

    return conector;
}

// Función que ejecuta el proceso hijo para enviar el archivo
void transferir_archivo(int conector_cliente, const char *nombre_archivo) {
    FILE *archivo = fopen(nombre_archivo, "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        close(conector_cliente);
        exit(EXIT_FAILURE);
    }

    char buffer[TAM_BUFFER];
    int bytes_leidos;
    
    while ((bytes_leidos = fread(buffer, 1, sizeof(buffer), archivo)) > 0) {
        if (send(conector_cliente, buffer, bytes_leidos, 0) < 0) {
            perror("Error al enviar los datos");
            break;
        }
    }

    printf("Transferencia completada con éxito por el proceso hijo (PID: %d).\n", getpid());
    fclose(archivo);
    close(conector_cliente);
}

int main() {
    int conector_servidor, conector_cliente;
    struct sockaddr_in direccion_cliente;
    int addrlen = sizeof(direccion_cliente);

    conector_servidor = iniciar_servidor(PUERTO);
    printf("Servidor iniciado. Escuchando en el puerto %d...\n", PUERTO);

    while(1) {
        conector_cliente = accept(conector_servidor, (struct sockaddr *)&direccion_cliente, (socklen_t*)&addrlen);
        if (conector_cliente < 0) {
            perror("Error en accept");
            continue;
        }

        printf("Nueva conexión aceptada. Transfiriendo archivo...\n");

        if (fork() == 0) { 
            close(conector_servidor); // El hijo cierra el socket principal
            transferir_archivo(conector_cliente, "Google.html");
            exit(0); 
        } else {
            close(conector_cliente);  // El padre cierra el socket del cliente
        }
    }
    
    close(conector_servidor);
    return 0;
}
