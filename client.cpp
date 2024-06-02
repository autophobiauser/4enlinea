#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <limits>

#define MAX 80
#define SA struct sockaddr

void printBoard(char board[6][7]) {
    std::cout << "== Conecta 4 ==" << std::endl;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j++) {
            std::cout << (board[i][j] ? board[i][j] : '.') << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "0 1 2 3 4 5 6" << std::endl;
}

bool isColumnFull(char board[6][7], int col) {
    return board[0][col] != 0;
}

void playGame(int sockfd) {
    char board[6][7] = {};
    char buffer[MAX];
    int n;
    bool gameOver = false;
    int moves = 0; // Contador de fichas jugadas

    while (true) {
        bzero(buffer, sizeof(buffer));
        read(sockfd, buffer, sizeof(buffer));

        if (strcmp(buffer, "WIN") == 0) {
            std::cout << "¡Has ganado!" << std::endl;
            gameOver = true;
        } else if (strcmp(buffer, "LOSE") == 0) {
            std::cout << "¡Has perdido!" << std::endl;
            gameOver = true;
        } else if (strcmp(buffer, "DRAW") == 0) {
            std::cout << "¡El juego ha terminado en empate!" << std::endl;
            gameOver = true;
        } else {
            std::memcpy(board, buffer, sizeof(board));
            printBoard(board);
        }

        if (gameOver) {
            break;
        }

        bool validMove = false;
        while (!validMove) {
            std::cout << "Ingresa la columna (0-6) donde deseas colocar tu ficha: ";
            std::cin >> n;

            if (std::cin.fail() || n < 0 || n > 6) {
                std::cin.clear(); // Limpiar el flag de error
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar la entrada inválida
                std::cout << "Número de columna inválido. Por favor, inténtelo de nuevo." << std::endl;
            } else if (isColumnFull(board, n)) {
                std::cout << "La columna está llena. Por favor, elige otra columna." << std::endl;
            } else {
                validMove = true;
            }
        }

        bzero(buffer, MAX);
        sprintf(buffer, "%d", n);
        write(sockfd, buffer, sizeof(buffer));
        moves++; // Incrementar el contador de fichas jugadas

        if (moves >= 21) { // Verificar si se ha alcanzado el límite de 21 fichas
            std::cout << "Se ha alcanzado el límite de 21 fichas. El juego termina en empate." << std::endl;
            bzero(buffer, MAX);
            std::strcpy(buffer, "DRAW");
            write(sockfd, buffer, sizeof(buffer));
            break;
        }
    }

    close(sockfd);  // Cerrar el socket cuando el juego termine
    exit(0);  // Salir del programa cuando el juego termine
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <IP> <puerto>" << std::endl;
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cout << "La creación del socket falló..." << std::endl;
        exit(0);
    } else {
        std::cout << "Socket creado con éxito.." << std::endl;
    }
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        std::cout << "La conexión con el servidor falló..." << std::endl;
        exit(0);
    } else {
        std::cout << "Conectado al servidor.." << std::endl;
    }

    playGame(sockfd);

    return 0;  // Salir del programa cuando el juego termine
}
