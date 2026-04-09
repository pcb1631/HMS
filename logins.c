#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logins.h"

user_login_t *logins = NULL;

void add_login(const char *username, const char *password, user_login_t *logins) {
    user_login_t *s;

    HASH_FIND_STR(logins, username, s);
    if (s == NULL) {
        s = (user_login_t *)malloc(sizeof(user_login_t));
        if (s == NULL) return; /* Out of memory */

        strncpy(s->username, username, MAX_USERNAME_LEN - 1);
        s->username[MAX_USERNAME_LEN - 1] = '\0';

        HASH_ADD_STR(logins, username, s);
    }

    strncpy(s->password, password, MAX_PASSWORD_LEN - 1);
    s->password[MAX_PASSWORD_LEN - 1] = '\0';
}

user_login_t *lookup_login(const char *username, user_login_t *logins) {
    user_login_t *s;
    HASH_FIND_STR(logins, username, s);
    return s;
}

void delete_login(const char *username, user_login_t *logins) {
    user_login_t *s = lookup_login(username, logins);
    if (s != NULL) {
        HASH_DEL(logins, s);
        free(s);
    }
}

int load_logins_from_file(const char *filename, user_login_t *logins) {
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

            add_login(username, password, logins);
        }
    }

    fclose(fp);
    return 0;
}

int save_logins_to_file(const char *filename, user_login_t *logins) {
    FILE *fp;
    user_login_t *s, *tmp;

    fp = fopen(filename, "w");
    if (fp == NULL) {
        return -1;
    }

    HASH_ITER(hh, logins, s, tmp) {
        fprintf(fp, "%s=%s\n", s->username, s->password);
    }

    fclose(fp);
    return 0;
}

void free_all_logins(void) {
    user_login_t *current_user, *tmp;
    HASH_ITER(hh, logins, current_user, tmp) {
        HASH_DEL(logins, current_user);
        free(current_user);
    }
}
