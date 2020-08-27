# Compiler flags
CC = g++
CFLAGS = -DQUANTA_USE_PTHREADS -g
LDFLAGS = 
THREAD_LIB = -lpthread

QUANTA_OBJ =	QUANTAinit.o\
		QUANTAnet_iovec_c.o\
		QUANTAnet_socketbase_c.o\
		QUANTAnet_udp_c.o\
		QUANTAts_thread_c.o\
		QUANTAmisc_debug.o\
		gettimeofday.o

CLIENT_OBJ =	$(QUANTA_OBJ) UDPClient.o UDPClientTest.o
SERVER_OBJ =	$(QUANTA_OBJ) UDPServer.o UDPServerTest.o

CLIENT_TARGET = UDPClientTest
SERVER_TARGET = UDPServerTest
.SUFFIXES: .cxx

#############################################################################
# Targets

all: $(CLIENT_TARGET) $(SERVER_TARGET)

.cxx.o:	
	$(CC) $(CFLAGS) -c $<

.cpp.o:	
	$(CC) $(CFLAGS) -c $<

$(CLIENT_TARGET): $(CLIENT_OBJ)
	$(CC) -o $(CLIENT_TARGET) $(CLIENT_OBJ) $(THREAD_LIB)

$(SERVER_TARGET): $(SERVER_OBJ)
	$(CC) -o $(SERVER_TARGET) $(SERVER_OBJ) $(THREAD_LIB)

clean:
	touch _.o; \rm -rf *.o $(TARGET)
