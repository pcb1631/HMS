/**
 * @file logins.h
 * @brief User login management and hashing.
 *
 * This file uses uthash.h as a library
 * https://github.com/troydhanson/uthash
 * https://troydhanson.github.io/uthash/
 *
 * Ideally, I would run a server that accepts requests for lookup, add, and delete to prevent race condition. But that is outside the scope of the project.
 */
#ifndef LOGINS_H
#define LOGINS_H

#include "../uthash.h"

#define MAX_USERNAME_LEN 64
#define MAX_PASSWORD_LEN 64

typedef struct {
    /** key */
    char username[MAX_USERNAME_LEN];
    
    /** value */
    char password[MAX_PASSWORD_LEN];

    /** makes this structure hashable */
    UT_hash_handle hh; 
} user_login_t;


/**
 * @brief Returns pointer to login data if found, else returns NULL.
 * @param username Username to lookup
 * @param logins Pointer to the hash table
 * @return Pointer to user_login_t if found, NULL otherwise
 */
user_login_t *lookup_user(const char *username, user_login_t **logins);

/**
 * @brief Add or update user credential
 * @param username Username to add/update
 * @param password Password for the user
 * @param logins Pointer to the hash table
 */
void add_user(const char *username, const char *password, user_login_t **logins);

/**
 * @brief Lookup a user
 * @param username Username to lookup
 * @param logins Pointer to the hash table
 * @return Pointer to user_login_t if found, NULL otherwise
 */
user_login_t *user(const char *username, user_login_t **logins);

/**
 * @brief Delete a login by username
 * @param username Username to delete
 * @param logins Pointer to the hash table
 */
void delete_user(const char *username, user_login_t **logins);

/**
 * @brief Read the hash table from a file (format: username=password)
 * @param filename File to read from
 * @param logins Pointer to the hash table
 * @return 1 on success, -1 on failure
 */
int load_users_from_file(const char *filename, user_login_t **logins);

/**
 * @brief Save the hash table to a file
 * @param filename File to save to
 * @param logins Pointer to the hash table
 * @return 1 on success, -1 on failure
 */
int save_users_to_file(const char *filename, user_login_t **logins);

/**
 * @brief Free all entries in the hash table
 * @param logins Pointer to the hash table
 */
void free_all_users(user_login_t **logins);

/**
 * @brief Hash a password using bcrypt
 * @param password Password to hash
 * @param hashout Buffer to store the hash
 * @return 0 on success, -1 on failure
 */
int hash_password(const char * password, char * hashout);
/**
 * @brief Verify a password against a stored hash
 * @param password Password to verify
 * @param stored_hash Stored hash to verify against
 * @return 1 if match, 0 if no match, -1 on error
 */
int verify_password(const char * password, const char * stored_hash);

int register_user_hashtable(const char *current_username, char *password, const char *pw_file);
#endif /* LOGINS_H */
