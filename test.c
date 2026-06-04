#include <stdio.h>
#include <time.h>

int main(void)
{
	time_t now = time(NULL);
	struct tm * ts = localtime(&now);
	char readable_time[80];

	strftime(readable_time, sizeof(readable_time), "%Y-%m-%d %H:%M:%S", ts);

	printf("%s\n", readable_time);
	return 0;
}