CC = gcc

SRCS =	main.c \
		utils/utils.c \
		utils/logins.c \
		utils/tui.c \
		user_commands/commands.c \
		user_commands/bookings.c \
		user_commands/complaints.c \
		user_commands/inventory.c \
		utils/globals.c \
		utils/string_utils.c \
       	deps/crypt_blowfish/crypt_blowfish.c \
        deps/crypt_blowfish/crypt_gensalt.c \
        deps/crypt_blowfish/wrapper.c \
        deps/crypt_blowfish/x86.S
CFLAGS = -ansi -pedantic -Ideps/crypt_blowfish -lncurses -g

CWARNINGS = -Wall -Wextra


TARGET = main.out

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(SRCS)  $(CFLAGS) -o $(TARGET) $(CWARNINGS)

# Rule to clean up generated files
clean:
	rm -f $(TARGET)
