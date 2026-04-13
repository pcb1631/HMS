/*
 * Ideally, I would run a server that accepts requests for lookup, add, and delete to prevent race condition. But that is outside the scope of the project.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logins.h"
#include "deps/crypt_blowfish/ow-crypt.h"
#include "deps/crypt_blowfish/crypt_blowfish.h"
#include "platform_random.h"


void add_user(const char *username, const char *password, user_login_t **logins)
{
    user_login_t *s;

    HASH_FIND_STR(*logins, username, s);
    if (s == NULL) {
        s = (user_login_t *)malloc(sizeof(user_login_t));
        if (s == NULL) return; /* Out of memory */

        strncpy(s->username, username, MAX_USERNAME_LEN - 1);
        s->username[MAX_USERNAME_LEN - 1] = '\0';

        HASH_ADD_STR(*logins, username, s);
    }

    strncpy(s->password, password, MAX_PASSWORD_LEN - 1);
    s->password[MAX_PASSWORD_LEN - 1] = '\0';
}

/*returns pointer to login data if found, else returns NULL. */
user_login_t *lookup_user(const char *username, user_login_t **logins)
{
    user_login_t *s;
    HASH_FIND_STR(*logins, username, s);
    return s;
}

void delete_user(const char *username, user_login_t **logins)
{
    user_login_t *s = lookup_user(username, logins);
    if (s != NULL) {
        HASH_DEL(*logins, s);
        free(s);
    }
}

int load_users_from_file(const char *filename, user_login_t **logins)
{
    FILE *fp;
    char line[256];
    char *eq;
    char *username;
    char *password;
    size_t len;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        return -1;
    }

    while (fgets(line, sizeof(line), fp)) {
        /* Parse username=password */
        eq = strchr(line, '=');
        if (eq != NULL) {
            *eq = '\0';
            username = line;
            password = eq + 1;

            /* Trim trailing newline/carriage return from password */
            len = strlen(password);
            if (len > 0 && password[len - 1] == '\n') {
                password[len - 1] = '\0';
                len--;
            }
            if (len > 0 && password[len - 1] == '\r') {
                password[len - 1] = '\0';
            }

            add_user(username, password, logins);
        }
    }

    fclose(fp);
    return 1;
}

int save_users_to_file(const char *filename, user_login_t **logins)
{
    FILE *fp;
    user_login_t *s, *tmp;

    fp = fopen(filename, "w");
    if (fp == NULL) {
        return -1;
    }

    HASH_ITER(hh, *logins, s, tmp) {
        fprintf(fp, "%s=%s\n", s->username, s->password);
    }

    fclose(fp);
    return 1;
}

void free_all_users(user_login_t **logins)
{
    user_login_t *current_user, *tmp;
    HASH_ITER(hh, *logins, current_user, tmp) {
        HASH_DEL(*logins, current_user);
        free(current_user);
    }
}

void build_patient(const char *username)
{
    char path[256];
    char command[512];
    FILE *fp;

    /* Check if user directory already exists */
    sprintf(path, "./users/%s", username);

    /* Create user directory */
    sprintf(command, "mkdir -p ./users/%s", username);
    system(command);

    /* Create profile.txt */
    sprintf(path, "./users/%s/profile.txt", username);
    fp = fopen(path, "w");
    if (fp != NULL) {
        fprintf(fp, "USERTYPE=Patient\n");
        fprintf(fp, "first_name=\n");
        fprintf(fp, "middle_name=\n");
        fprintf(fp, "last_name=\n");
        fprintf(fp, "blood_type=\n");
        fprintf(fp, "height=\n");
        fprintf(fp, "birth=\n");
        fclose(fp);
    }

    /* Create prefs.txt */
    sprintf(path, "./users/%s/prefs.txt", username);
    fp = fopen(path, "w");
    if (fp != NULL) {
        fclose(fp);
    }

    /* Create diag_report directory */
    sprintf(command, "mkdir -p ./users/%s/diag_report", username);
    system(command);

    /* Create notes directory */
    sprintf(command, "mkdir -p ./users/%s/notes", username);
    system(command);
}

void build_doctor(const char *username)
{
    char path[256];
    char command[512];
    FILE *fp;

    /* Check if user directory already exists */
    sprintf(path, "./users/%s", username);

    /* Create user directory */
    sprintf(command, "mkdir -p ./users/%s", username);
    system(command);

    /* Create profile.txt */
    sprintf(path, "./users/%s/profile.txt", username);
    fp = fopen(path, "w");
    if (fp != NULL) {
        fprintf(fp, "USERTYPE=Doctor\n");
        fprintf(fp, "first_name=\n");
        fprintf(fp, "middle_name=\n");
        fprintf(fp, "last_name=\n");
        fprintf(fp, "birth=\n");
        fclose(fp);
    }

    /* Create prefs.txt */
    sprintf(path, "./users/%s/prefs.txt", username);
    fp = fopen(path, "w");
    if (fp != NULL) {
        fclose(fp);
    }

    /* Create bookings.csv.txt */
    sprintf(path, "./users/%s/bookings.csv.txt", username);
    fp = fopen(path, "w");
    if (fp != NULL) {
        fclose(fp);
    }
}

#define SALT_BUF   32
#define HASH_BUF   64
#define COST       12   /* 2^12 = 4096 iterations; 10-12 is typical */

int hash_password(const char *password, char *hash_out) {
    char rand_bytes[16];
    char salt[SALT_BUF];

    if (get_random_bytes(rand_bytes, sizeof(rand_bytes)) != 0)
        return -1;

    if (!crypt_gensalt_rn("$2b$", COST, rand_bytes, sizeof(rand_bytes), salt, sizeof(salt)))
        return -1;

    if (!_crypt_blowfish_rn(password, salt, hash_out, HASH_BUF))
        return -1;

    return 0;
}

int verify_password(const char *password, const char *stored_hash) {
    char result[HASH_BUF];

    /* Pass stored hash as the "setting", it encodes salt + cost */
    if (!_crypt_blowfish_rn(password, stored_hash, result, HASH_BUF))
        return -1;

    return strcmp(result, stored_hash) == 0 ? 1 : 0;
}
