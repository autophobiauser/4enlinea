# Compilar ambos programas
all: servidor cliente

# Compilar el servidor
servidor: server.cpp
	g++ -o servidor server.cpp -pthread

# Compilar el cliente
cliente: client.cpp
	g++ -o cliente client.cpp

# Limpiar los archivos binarios generados
clean:
	rm -f servidor cliente
