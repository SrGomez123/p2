#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h> /* Incluye la ayuda inet_addr() citada en el PDF */

#define PUERTO 9999
#define TAM_BUFFER 1024

/* Función que traduce la IP, crea el socket y establece la conexión */
int conectar_servidor(char *ip_servidor, int puerto) {
    int conector_cliente;
    struct sockaddr_in direccion_servidor;

    conector_cliente = socket(AF_INET, SOCK_STREAM, 0);
    if (conector_cliente < 0) {
        perror("Error al crear el socket");
        exit(1);
    }

    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_port = htons(puerto);
    direccion_servidor.sin_addr.s_addr = inet_addr(ip_servidor); 

    if (connect(conector_cliente, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor)) < 0) {
        perror("Error de conexión");
        close(conector_cliente);
        exit(1);
    }

    return conector_cliente;
}

/* Función que lee el flujo del socket y lo vuelca por pantalla */
void recibir_y_mostrar(int conector_cliente) {
    char buffer[TAM_BUFFER];
    int bytes_recibidos;
    
    /* Lectura de la red con read() */
    while ((bytes_recibidos = read(conector_cliente, buffer, TAM_BUFFER)) > 0) {
        /* Escritura en la Salida Estándar (descriptor 1, Pantalla) con write() */
        write(1, buffer, bytes_recibidos);
    }

    if (bytes_recibidos < 0) {
        perror("Error en read");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s IP_Servidor\n", argv[0]);
        exit(1);
    }

    int conector_cliente = conectar_servidor(argv[1], PUERTO);
    
    recibir_y_mostrar(conector_cliente);

    close(conector_cliente);
    return 0;
}
