#ifndef LOGINS_H
#define LOGINS_H

#include "uthash.h"

#define MAX_USERNAME_LEN 64
#define MAX_PASSWORD_LEN 64

typedef struct {
	/* key */
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    /* makes this structure hashable */
    UT_hash_handle hh;
} user_login_t;


/* Add or update a login */
void add_user(const char *username, const char *password, user_login_t *logins);

/* Lookup a login by username */
user_login_t *user(const char *username, user_login_t *logins);

/* Delete a login by username */
void delete_user(const char *username, user_login_t *logins);

/* Read the hash table from a file (format: username=password) */
int load_users_from_file(const char *filename, user_login_t *logins);

/* Save the hash table to a file */
int save_users_to_file(const char *filename, user_login_t *logins);

/* Free all entries in the hash table */
void free_all_users(void);

#endif /* LOGINS_H */
