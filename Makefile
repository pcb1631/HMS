CC = gcc

SRCS =	main.c \
		utils.c \
       	deps/crypt_blowfish/crypt_blowfish.c \
        deps/crypt_blowfish/crypt_gensalt.c \
        deps/crypt_blowfish/x86.S
CFLAGS = -ansi -pedantic -Wall -Wextra -Ideps/crypt_blowfish -lncurses -g

TARGET = main.out

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(SRCS)  $(CFLAGS) -o $(TARGET)

# Rule to clean up generated files
clean:
	rm -f $(TARGET)
