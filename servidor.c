#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define PUERTO 9999
#define TAM_BUFFER 1024

int main() {
    int conector, conector_cliente;
    struct sockaddr_in direccion_servidor;
    int addrlen = sizeof(direccion_servidor);

    /* Apertura del canal */
    conector = socket(AF_INET, SOCK_STREAM, 0);
    if (conector == -1) {
        perror("Error en socket");
        exit(1);
    }

    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_addr.s_addr = INADDR_ANY;
    direccion_servidor.sin_port = htons(PUERTO);

    /* Publicación de la dirección a la red */
    if (bind(conector, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor)) < 0) {
        perror("Error en bind");
        close(conector);
        exit(1);
    }

    /* Disposición para aceptar conexiones */
    if (listen(conector, 5) < 0) {
        perror("Error en listen");
        close(conector);
        exit(1);
    }

    while(1) {
        /* Bloqueo hasta petición */
        conector_cliente = accept(conector, (struct sockaddr *)&direccion_servidor, (socklen_t*)&addrlen);
        if (conector_cliente < 0) {
            perror("Error en accept");
            continue;
        }

        /* Creación del proceso hijo */
        if (fork() == 0) {
            close(conector); /* El hijo cierra el socket general */
            
            /* Abrir archivo con open() estudiado en la práctica 1 */
            int df = open("Google.html", O_RDONLY);
            if (df < 0) {
                perror("Error al abrir archivo");
                close(conector_cliente);
                exit(1);
            }

            char buffer[TAM_BUFFER];
            int bytes_leidos;
            
            /* Leer del archivo con read() y enviar por el socket con write() */
            while ((bytes_leidos = read(df, buffer, TAM_BUFFER)) > 0) {
                write(conector_cliente, buffer, bytes_leidos);
            }

            close(df);
            close(conector_cliente);
            exit(0);
        } else {
            /* El padre cierra el socket del cliente y vuelve al accept */
            close(conector_cliente);
        }
    }
    return 0;
}
