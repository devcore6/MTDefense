CXXFLAGS= -std=c++2b -O3 -fomit-frame-pointer -ffast-math

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
	Engine/Command.o \
	Engine/Network.o \
	Engine/Map.o \
	Engine/Server-Main.o \
	Engine/Texture.o \
	Engine/GL.o \
	Game/Server.o \
	Game/Tower.o \

all: client server

client: $(CLIENT_OBJS)

server: $(SERVER_OBJS)
	$(CXX) $(SERVER_OBJS) -o server

clean:
	rm -f $(CLIENT_OBJS) $(SERVER_OBJS) client server

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<
