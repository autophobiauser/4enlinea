
# 🎮 4 En Línea 🎮

Este repositorio contiene la implementación del juego "Conecta 4" utilizando sockets en C++. 
El proyecto consta de un servidor y un cliente que se comunican a través de la red para jugar al juego.

## Estructura del Proyecto

- `server.cpp`: Código fuente del servidor.
- `client.cpp`: Código fuente del cliente.
- `Makefile`: Archivo para compilar el proyecto.

## Requisitos

Para compilar y ejecutar este proyecto, es necesario:
- Un compilador de C++ (por ejemplo, `g++`).
- `make` para utilizar el Makefile.

## Compilación 

Para compilar el servidor y el cliente:

1. Clonar el repositorio:

   ```bash
   git clone https://github.com/autophobiauser/4enlinea.git
   cd 4enlinea
   ```

2. Utilizar `make` para compilar server.cpp y client.cpp:

   ```bash
   make
   ```

   Esto generará dos ejecutables: `server` y `client`.

## Ejecución

### Servidor 

Para ejecutar el servidor, se utiliza el siguiente comando:

```bash
./server <puerto>
```

Por ejemplo, para iniciar el servidor en el puerto 7777:

```bash
./server 7777
```

El servidor comenzará a escuchar conexiones en el puerto especificado.

### Cliente

Para ejecutar el cliente:

```bash
./client <IP> <puerto>
```

Por ejemplo, para conectar el cliente al servidor en `127.0.0.1` (localhost) y el puerto `7777`:

```bash
./client 127.0.0.1 7777
```

El cliente se conectará al servidor y empezará la partida.

## 🎮 Reglas del Juego 🎮 

- El objetivo del juego es conectar cuatro fichas del mismo tipo en línea (horizontal, vertical o diagonal).
- El cliente y el servidor se turnan para hacer movimientos.
- El juego termina cuando uno de los jugadores gana o se alcanza un empate.

## 📝 Observaciones 📝

- Al iniciar la partida, el jugador que parte es seleccionado al azar.
- Es posible que al cliente, habiendo lanzado una ficha ganadora, le siga apareciendo el tablero para lanzar una "ficha extra". Sin embargo esta no se jugará, aparecerá el mensaje de que ha ganado y la partida terminará.
- Para comprobar el empate, el total de las fichas son 21 entre ambos jugadores.

## Limpieza

Para eliminar los archivos compilados (`server`, `client` y los archivos objeto `*.o`), se puede utilizar el siguiente comando:

```bash
make clean
```

## Colaboradores

- Joaquín Pérez.
- Eduardo Riquelme.
  
Este proyecto fue realizado para la asignatura Comunicación de Datos y Redes.

