#include "commands.h"
#include "utils.h"
#include "logins.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Value make_user(Value * list)
{
    int i=0;
    int flag=0;
    user_login_t * logins = NULL;
    char * pw_file = "./users/creds.txt";
    char * pw;

    char * allowed_utypes[4] =
    {
        "Admin",
        "Patient",
        "Nurse",
        "Doctor",
    };

    for(i=0; i<4; i++)
        if(strcmp(allowed_utypes[i], list[0].data.as_string) == 0)
            flag=1;
    if(!flag)
    {
        printf("Usertype not allowed");
        return make_int(-1);
    }

    make_user_GETPW:
    printf("Password: ");
    pw = malloc(sizeof(char)*256);
    fgets(pw, 256, stdin);
    printf("\n");

    if(!string_trim(pw))
    {
        fprintf(stderr, "Password cannot be empty or contain spaces\n");
        free(pw);
        goto make_user_GETPW;
    }




    load_users_from_file(pw_file, &logins);
    register_user_hashtable(list[1].data.as_string, pw, pw_file);


    generate_file_structure(list[0].data.as_string, list[1].data.as_string);
    return make_int(0);
}
