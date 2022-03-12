CXXFLAGS= -std=c++2b -O3 -fomit-frame-pointer -ffast-math -IInclude -pthread 
SERVERFLAGS= -D__SERVER__=1
CLIENTFLAGS=
CC=gcc
CXX=g++

# todo
CLIENT_OBJS= \

SERVER_OBJS= \
	enet/callbacks.o \
	enet/compress.o \
	enet/host.o \
	enet/list.o \
	enet/packet.o \
	enet/peer.o \
	enet/protocol.o \
	enet/unix.o \
	Engine/Command-server.o \
	Engine/Network-server.o \
	Engine/Map-server.o \
	Engine/Server-Main-server.o \
	Engine/Texture-server.o \
	Engine/GL-server.o \
	Game/Server-server.o \
	Game/Tower-server.o \

all: client server

install: install-client install-server

client: $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $(CLIENTFLAGS) $(CLIENT_OBJS) -o client

server: $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $(SERVERFLAGS) $(SERVER_OBJS) -o server

install-client: client
	mkdir -p ../bin/
	mv client ../bin/client

install-server: server
	mkdir -p ../bin/
	mv server ../bin/server

clean:
	rm -rf $(CLIENT_OBJS) $(SERVER_OBJS) client server ../bin/

%-server.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SERVERFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CLIENTFLAGS) -c -o $@ $<

%.o: %.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<
