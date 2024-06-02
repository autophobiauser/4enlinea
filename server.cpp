#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <cstdlib> // Para srand() y rand()
#include <ctime>   // Para time()

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

bool checkWin(char board[6][7], char token) {
    // horizontal, vertical y diagonal 
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j++) {
            // Comprobar horizontal
            if (j + 3 < 7 && board[i][j] == token && board[i][j+1] == token && board[i][j+2] == token && board[i][j+3] == token)
                return true;
            // Comprobar vertical
            if (i + 3 < 6 && board[i][j] == token && board[i+1][j] == token && board[i+2][j] == token && board[i+3][j] == token)
                return true;
            // Comprobar diagonal 
            if (i + 3 < 6 && j + 3 < 7 && board[i][j] == token && board[i+1][j+1] == token && board[i+2][j+2] == token && board[i+3][j+3] == token)
                return true;
            // Comprobar diagonal 
            if (i - 3 >= 0 && j + 3 < 7 && board[i][j] == token && board[i-1][j+1] == token && board[i-2][j+2] == token && board[i-3][j+3] == token)
                return true;
        }
    }
    return false;
}

bool checkDraw(char board[6][7]) {
    for (int i = 0; i < 7; i++) {
        if (board[0][i] == 0) return false;
    }
    return true;
}

int getRandomMove(char board[6][7]) {
    int col;
    do {
        col = rand() % 7; // Genera un número aleatorio entre 0 y 6
    } while (board[0][col] != 0); // Asegura que la columna no esté llena
    return col;
}

void* playGame(void* sockfd) {
    int connfd = *((int*)sockfd);
    free(sockfd);
    char board[6][7];
    bzero(board, sizeof(board));
    char buffer[MAX];
    char player = 'C';
    char server = 'S';
    bool gameOver = false;
    int moves = 0; // Contador de fichas jugadas

    // Determinar aleatoriamente quién comienza
    bool playerTurn = rand() % 2 == 0;

    if (playerTurn) {
        std::cout << "El cliente comienza el juego." << std::endl;
    } else {
        std::cout << "El servidor comienza el juego." << std::endl;
    }

    while (!gameOver) {
        if (playerTurn) {
            bzero(buffer, MAX);
            std::memcpy(buffer, board, sizeof(board));
            write(connfd, buffer, sizeof(buffer));

            bzero(buffer, MAX);
            read(connfd, buffer, sizeof(buffer));
            int move = atoi(buffer);
            if (move < 0 || move >= 7) continue;

            for (int i = 5; i >= 0; i--) {
                if (board[i][move] == 0) {
                    board[i][move] = player;
                    moves++; // Incrementar el contador de fichas jugadas
                    break;
                }
            }

            std::cout << "\nCliente hizo un movimiento en la columna: " << move << std::endl;
            if (checkWin(board, player)) {
                std::memcpy(buffer, board, sizeof(board));
                write(connfd, buffer, sizeof(buffer));
                std::strcpy(buffer, "WIN");
                write(connfd, buffer, sizeof(buffer));
                std::cout << "Cliente ha ganado." << std::endl;
                gameOver = true;
                continue;
            }

            if (checkDraw(board) || moves >= 21) { // Verificar si se ha alcanzado el límite de 21 fichas
                std::memcpy(buffer, board, sizeof(board));
                write(connfd, buffer, sizeof(buffer));
                std::strcpy(buffer, "DRAW");
                write(connfd, buffer, sizeof(buffer));
                std::cout << "El juego ha terminado en empate." << std::endl;
                gameOver = true;
                continue;
            }
        } else {
            int move = getRandomMove(board);
            for (int i = 5; i >= 0; i--) {
                if (board[i][move] == 0) {
                    board[i][move] = server;
                    moves++; // Incrementar el contador de fichas jugadas
                    break;
                }
            }

            std::cout << "Servidor hizo un movimiento en la columna: " << move << std::endl;
            if (checkWin(board, server)) {
                std::memcpy(buffer, board, sizeof(board));
                write(connfd, buffer, sizeof(buffer));
                std::strcpy(buffer, "LOSE");
                write(connfd, buffer, sizeof(buffer));
                std::cout << "Servidor ha ganado." << std::endl;
                gameOver = true;
                continue;
            } else if (checkDraw(board) || moves >= 21) { // Verificar si se ha alcanzado el límite de 21 fichas
                std::memcpy(buffer, board, sizeof(board));
                write(connfd, buffer, sizeof(buffer));
                std::strcpy(buffer, "DRAW");
                write(connfd, buffer, sizeof(buffer));
                std::cout << "El juego ha terminado en empate." << std::endl;
                gameOver = true;
                continue;
            }
        }

        playerTurn = !playerTurn; // Cambiar el turno
    }

    // Notificar al cliente que el juego ha terminado
    std::strcpy(buffer, "GAME_OVER");
    write(connfd, buffer, sizeof(buffer));

    close(connfd);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <puerto>" << std::endl;
        return 1;
    }

    int port = atoi(argv[1]);
    srand(time(0)); // Inicializa la semilla para los números aleatorios

    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cout << "La creación del socket falló..." << std::endl;
        exit(0);
    } else {
        std::cout << "Socket creado con éxito.." << std::endl;
    }
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        std::cout << "Enlace con el socket fallido..." << std::endl;
        exit(0);
    } else {
        std::cout << "Enlazado al socket con éxito.." << std::endl;
    }

    if ((listen(sockfd, 5)) != 0) {
        std::cout << "Fallo al escuchar..." << std::endl;
        exit(0);
    } else {
        std::cout << "Servidor escuchando.." << std::endl;
    }
    len = sizeof(cli);

    while (true) {
        connfd = accept(sockfd, (SA*)&cli, (socklen_t*)&len);
        if (connfd < 0) {
            std::cout << "La aceptación del cliente falló..." << std::endl;
            exit(0);
        } else {
            std::cout << "Cliente aceptado.." << std::endl;
        }

        pthread_t thread_id;
        int* connfd_ptr = (int*)malloc(sizeof(int));
        *connfd_ptr = connfd;
        pthread_create(&thread_id, NULL, playGame, connfd_ptr);
    }

    close(sockfd);
    return 0;
}
