#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define PUERTO 9999
#define TAM_BUFFER 1024

/* Función que configura el socket y lo deja escuchando */
int iniciar_servidor(int puerto) {
    int conector;
    struct sockaddr_in direccion;

    conector = socket(AF_INET, SOCK_STREAM, 0);
    if (conector == -1) {
        perror("Error en socket");
        exit(1);
    }

    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(puerto);

    if (bind(conector, (struct sockaddr *)&direccion, sizeof(direccion)) < 0) {
        perror("Error en bind");
        close(conector);
        exit(1);
    }

    if (listen(conector, 5) < 0) {
        perror("Error en listen");
        close(conector);
        exit(1);
    }

    return conector;
}

/* Función ejecutada por el hijo para leer el archivo y enviarlo por la red */
void transferir_archivo(int conector_cliente, char *nombre_archivo) {
    char buffer[TAM_BUFFER];
    int bytes_leidos;
    
    /* Apertura del archivo en modo lectura (O_RDONLY) estudiado en la práctica 1 */
    int df = open(nombre_archivo, O_RDONLY);
    if (df < 0) {
        perror("Error al abrir archivo");
        close(conector_cliente);
        exit(1);
    }

    /* Lectura de datos con read() y envío por el socket con write() */
    while ((bytes_leidos = read(df, buffer, TAM_BUFFER)) > 0) {
        write(conector_cliente, buffer, bytes_leidos);
    }

    close(df);
    close(conector_cliente);
}

int main() {
    int conector_servidor, conector_cliente;
    struct sockaddr_in dir_cliente;
    int addrlen = sizeof(dir_cliente);

    conector_servidor = iniciar_servidor(PUERTO);
    printf("Servidor iniciado. Esperando conexiones en el puerto %d...\n", PUERTO);

    while(1) {
        /* Bloqueo hasta petición */
        conector_cliente = accept(conector_servidor, (struct sockaddr *)&dir_cliente, (socklen_t*)&addrlen);
        if (conector_cliente < 0) {
            perror("Error en accept");
            continue;
        }

        /* Creación del proceso hijo para gestionar la transferencia */
        if (fork() == 0) { 
            close(conector_servidor); /* El hijo cierra el descriptor de escucha */
            transferir_archivo(conector_cliente, "Google.html");
            printf("Transferencia completada por el hijo (PID: %d)\n", getpid());
            exit(0); 
        } else {
            close(conector_cliente); /* El padre cierra el socket del cliente y vuelve a escuchar */
        }
    }
    
    return 0;
}
