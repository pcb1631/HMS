SRCS =	main.c \
		utils.c \
       	deps/crypt_blowfish/crypt_blowfish.c \
        deps/crypt_blowfish/crypt_gensalt.c \
        deps/crypt_blowfish/x86.S
CFLAGS = -ansi -pedantic -Ideps/crypt_blowfish
