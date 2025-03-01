OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown OS')

ifeq ($(OS),Darwin)  # macOS
CFLAGS = -Wall -Wextra -pthread
LDFLAGS = 
CC = clang
RM = rm -f
endif
ifeq ($(OS),Linux)  # Linux
CFLAGS = -Wall -Wextra -pthread
LDFLAGS =
CC = gcc
RM = rm -f
endif
INC_DIR = ../include
IPC_DIR = ../netcan
IPC_PORT = 60000

TARGET1 = can_recv
TARGET2 = can_send
TARGET3 = canfd_recv
TARGET4 = canfd_send

all: $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)

tcp_client.o: $(IPC_DIR)/tcp_client.c $(IPC_DIR)/tcp_client.h
	$(CC) -c $(IPC_DIR)/tcp_client.c $(CFLAGS) -I$(IPC_DIR)

can_recv: can_recv.c tcp_client.o $(IPC_DIR)/tcp_client.h
	$(CC) can_recv.c tcp_client.o $(CFLAGS) -DPORT=$(IPC_PORT) -I$(INC_DIR) -I$(IPC_DIR) $(LDFLAGS) -o can_recv

can_send: can_send.c tcp_client.o $(IPC_DIR)/tcp_client.h
	$(CC) can_send.c tcp_client.o $(CFLAGS) -DPORT=$(IPC_PORT) -I$(INC_DIR) -I$(IPC_DIR) $(LDFLAGS) -o can_send

canfd_recv: canfd_recv.c tcp_client.o $(IPC_DIR)/tcp_client.h
	$(CC) canfd_recv.c tcp_client.o $(CFLAGS) -DPORT=$(IPC_PORT) -I$(INC_DIR) -I$(IPC_DIR) $(LDFLAGS) -o canfd_recv

canfd_send: canfd_send.c tcp_client.o $(IPC_DIR)/tcp_client.h
	$(CC) canfd_send.c tcp_client.o $(CFLAGS) -DPORT=$(IPC_PORT) -I$(INC_DIR) -I$(IPC_DIR) $(LDFLAGS) -o canfd_send

clean:
	@-$(RM) *.o *~ $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)

pristine:
	@-$(RM) *.o *~ $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)
