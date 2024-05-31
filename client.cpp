#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

const int BUFFER_SIZE = 1024;  // Tamaño del buffer para los mensajes
const int FILAS = 6;
const int COLUMNAS = 7;

// Función para imprimir el tablero
void imprimirTablero(char tablero[FILAS][COLUMNAS]) {
    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COLUMNAS; ++j) {
            std::cout << tablero[i][j] << ' ';
        }
        std::cout << std::endl;
    }
    for (int j = 0; j < COLUMNAS; ++j) {
        std::cout << j + 1 << ' ';
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <ip_servidor> <puerto_servidor>" << std::endl;
        return 1;
    }

    const char *ipServidor = argv[1];
    int puertoServidor = std::stoi(argv[2]);

    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    // Crear el socket del cliente
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error al crear el socket" << std::endl;
        return 1;
    }

    // Configurar la dirección del servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(puertoServidor);

    // Convertir la dirección IP
    if (inet_pton(AF_INET, ipServidor, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Dirección no válida o no soportada" << std::endl;
        close(clientSocket);
        return 1;
    }

    // Conectar al servidor
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error al conectar con el servidor" << std::endl;
        close(clientSocket);
        return 1;
    }

    std::cout << "Conectado al servidor" << std::endl;

    // Inicializar el tablero
    char tablero[FILAS][COLUMNAS];
    while (true) {
        // Leer el estado del tablero desde el servidor
        int bytesLeidos = read(clientSocket, buffer, BUFFER_SIZE);
        if (bytesLeidos <= 0) {
            std::cerr << "Servidor desconectado" << std::endl;
            break;
        }
        buffer[bytesLeidos] = '\0';

        // Actualizar el tablero
        int k = 0;
        for (int i = 0; i < FILAS; ++i) {
            for (int j = 0; j < COLUMNAS; ++j) {
                tablero[i][j] = buffer[k++];
            }
        }

        // Imprimir el tablero
        imprimirTablero(tablero);

        // Leer el mensaje del servidor
        if (bytesLeidos < BUFFER_SIZE) {
            std::cout << buffer << std::endl;
            if (strcmp(buffer, "Cliente ha ganado") == 0 || strcmp(buffer, "Servidor ha ganado") == 0) {
                break;
            }
        }

        // Enviar el movimiento al servidor
        std::cout << "Ingrese una columna (1-7): ";
        std::cin.getline(buffer, BUFFER_SIZE);
        send(clientSocket, buffer, strlen(buffer), 0);
    }

    // Cerrar el socket del cliente
    close(clientSocket);
    return 0;
}
