#include <stdio.h>
#include "logins.h"

int main(void)
{
	user_login_t * logins = NULL;
	char * pwFile = "./users/creds.txt";

	if (load_users_from_file(pwFile, &logins))
	{
		add_user("funnyuser2", "funnypw2", &logins);
		save_users_to_file(pwFile, &logins);
		free_all_users(&logins);
	}else{
		printf("Failed\n");
	}



	return 0;
}
