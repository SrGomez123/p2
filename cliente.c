#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PUERTO 9999
#define TAM_BUFFER 1024

// Función para inicializar el socket y establecer la conexión
int conectar_servidor(const char *ip_servidor, int puerto) {
    int conector_cliente;
    struct sockaddr_in direccion_servidor;

    conector_cliente = socket(AF_INET, SOCK_STREAM, 0);
    if (conector_cliente < 0) {
        perror("Error al crear el socket del cliente");
        exit(EXIT_FAILURE);
    }

    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_port = htons(puerto);
    direccion_servidor.sin_addr.s_addr = inet_addr(ip_servidor); 

    if (connect(conector_cliente, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor)) < 0) {
        perror("Error al conectar con el servidor");
        close(conector_cliente);
        exit(EXIT_FAILURE);
    }

    return conector_cliente;
}

// Función encargada de recibir el flujo de bytes e imprimirlo
void recibir_y_mostrar(int conector_cliente) {
    char buffer[TAM_BUFFER];
    int bytes_recibidos;
    
    while ((bytes_recibidos = recv(conector_cliente, buffer, TAM_BUFFER - 1, 0)) > 0) {
        buffer[bytes_recibidos] = '\0'; 
        printf("%s", buffer);           
    }

    if (bytes_recibidos < 0) {
        perror("Error al recibir los datos");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Error de sintaxis.\nUso correcto: %s <IP_Servidor>\n", argv[0]);
        return -1;
    }

    int conector_cliente = conectar_servidor(argv[1], PUERTO);
    
    recibir_y_mostrar(conector_cliente);

    close(conector_cliente);
    return 0;
}
