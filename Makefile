# Definir el compilador
CXX = g++

CXXFLAGS = -Wall -std=c++11

TARGETS = server client

SERVER_OBJS = server.o
CLIENT_OBJS = client.o

all: $(TARGETS)

server: $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -o server $(SERVER_OBJS)

client: $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -o client $(CLIENT_OBJS)

server.o: server.cpp
	$(CXX) $(CXXFLAGS) -c server.cpp

client.o: client.cpp
	$(CXX) $(CXXFLAGS) -c client.cpp

# Regla para limpiar el directorio
clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean
