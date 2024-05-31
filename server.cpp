#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <thread>
#include <vector>
#include <mutex>
#include <cstdlib>
#include <ctime>

const int PORT = 7777;  // Puerto en el que el servidor escuchará
const int BUFFER_SIZE = 1024;  // Tamaño del buffer para los mensajes
const int FILAS = 6;
const int COLUMNAS = 7;

std::mutex mtx;  // Mutex para proteger el acceso a la consola

// Función para inicializar el tablero
void inicializarTablero(char tablero[FILAS][COLUMNAS]) {
    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COLUMNAS; ++j) {
            tablero[i][j] = ' ';
        }
    }
}

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

// Función para verificar si hay un ganador
bool hayGanador(char tablero[FILAS][COLUMNAS], char ficha) {
    // Verificar horizontal
    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COLUMNAS - 3; ++j) {
            if (tablero[i][j] == ficha && tablero[i][j + 1] == ficha && tablero[i][j + 2] == ficha && tablero[i][j + 3] == ficha) {
                return true;
            }
        }
    }
    // Verificar vertical
    for (int i = 0; i < FILAS - 3; ++i) {
        for (int j = 0; j < COLUMNAS; ++j) {
            if (tablero[i][j] == ficha && tablero[i + 1][j] == ficha && tablero[i + 2][j] == ficha && tablero[i + 3][j] == ficha) {
                return true;
            }
        }
    }
    // Verificar diagonal positiva
    for (int i = 0; i < FILAS - 3; ++i) {
        for (int j = 0; j < COLUMNAS - 3; ++j) {
            if (tablero[i][j] == ficha && tablero[i + 1][j + 1] == ficha && tablero[i + 2][j + 2] == ficha && tablero[i + 3][j + 3] == ficha) {
                return true;
            }
        }
    }
    // Verificar diagonal negativa
    for (int i = 3; i < FILAS; ++i) {
        for (int j = 0; j < COLUMNAS - 3; ++j) {
            if (tablero[i][j] == ficha && tablero[i - 1][j + 1] == ficha && tablero[i - 2][j + 2] == ficha && tablero[i - 3][j + 3] == ficha) {
                return true;
            }
        }
    }
    return false;
}

// Función para realizar un movimiento
bool realizarMovimiento(char tablero[FILAS][COLUMNAS], int columna, char ficha) {
    if (columna < 0 || columna >= COLUMNAS || tablero[0][columna] != ' ') {
        return false;
    }
    for (int i = FILAS - 1; i >= 0; --i) {
        if (tablero[i][columna] == ' ') {
            tablero[i][columna] = ficha;
            break;
        }
    }
    return true;
}

// Función que maneja la comunicación con el cliente
void manejarCliente(int clientSocket) {
    char buffer[BUFFER_SIZE];
    int bytesLeidos;
    char tablero[FILAS][COLUMNAS];
    inicializarTablero(tablero);
    char fichaCliente = 'C';
    char fichaServidor = 'S';
    bool esTurnoCliente = rand() % 2 == 0;

    // Enviar el estado inicial del tablero al cliente
    std::string estadoTablero = "";
    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COLUMNAS; ++j) {
            estadoTablero += tablero[i][j];
        }
    }
    send(clientSocket, estadoTablero.c_str(), estadoTablero.size(), 0);

    while (true) {
        if (esTurnoCliente) {
            // Esperar jugada del cliente
            bytesLeidos = read(clientSocket, buffer, BUFFER_SIZE);
            if (bytesLeidos <= 0) {
                std::cerr << "Cliente desconectado" << std::endl;
                break;
            }
            buffer[bytesLeidos] = '\0';
            int columna = atoi(buffer) - 1;

            // Realizar el movimiento del cliente
            if (!realizarMovimiento(tablero, columna, fichaCliente)) {
                std::string mensajeError = "Movimiento inválido";
                send(clientSocket, mensajeError.c_str(), mensajeError.size(), 0);
                continue;
            }

            // Verificar si el cliente ha ganado
            if (hayGanador(tablero, fichaCliente)) {
                std::string mensajeGanador = "Cliente ha ganado";
                send(clientSocket, mensajeGanador.c_str(), mensajeGanador.size(), 0);
                break;
            }
        } else {
            // Movimiento del servidor
            int columna = rand() % COLUMNAS;
            while (!realizarMovimiento(tablero, columna, fichaServidor)) {
                columna = rand() % COLUMNAS;
            }

            // Verificar si el servidor ha ganado
            if (hayGanador(tablero, fichaServidor)) {
                std::string mensajeGanador = "Servidor ha ganado";
                send(clientSocket, mensajeGanador.c_str(), mensajeGanador.size(), 0);
                break;
            }
        }

        // Enviar el estado del tablero al cliente
        estadoTablero = "";
        for (int i = 0; i < FILAS; ++i) {
            for (int j = 0; j < COLUMNAS; ++j) {
                estadoTablero += tablero[i][j];
            }
        }
        send(clientSocket, estadoTablero.c_str(), estadoTablero.size(), 0);

        // Cambiar el turno
        esTurnoCliente = !esTurnoCliente;
    }

    close(clientSocket);
    std::cout << "Cliente desconectado" << std::endl;
}

int main() {
    srand(time(0));  // Inicializar la semilla para números aleatorios
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    // Crear el socket del servidor
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error al crear el socket" << std::endl;
        return 1;
    }

    // Configurar la dirección del servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Enlazar el socket a la dirección y puerto
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error al enlazar el socket" << std::endl;
        close(serverSocket);
        return 1;
    }

    // Escuchar conexiones entrantes
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error al escuchar en el socket" << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Servidor escuchando en el puerto " << PORT << std::endl;

    // Ciclo para aceptar y manejar conexiones de clientes
    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientSocket == -1) {
            std::cerr << "Error al aceptar la conexión" << std::endl;
            continue;
        }

        std::cout << "Nueva conexión desde " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;

        // Crear un nuevo hilo para manejar al cliente
        std::thread(manejarCliente, clientSocket).detach();
    }

    // Cerrar el socket del servidor
    close(serverSocket);
    return 0;
}
