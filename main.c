#include <stdio.h>
#include <stdlib.h>


void print_help(int exval) {
	printf("sat-alarm [-h] -s <satellite-name> [-d <dd/mm/aaaa>] -u <url-to-post> \n\n");
 	printf("  -h              print this help and exit\n");
 	printf("  -d              get a date. Default: current date.\n");
 exit(exval);
}

int main(int argc, char **argv)
{

	int opt;
	if(argc == 1) {
		fprintf(stderr, "This program needs arguments....\n\n");
		print_help(1);
	}
	while((opt = getopt(argc, argv, "hs:d:u:")) != -1) {
		switch(opt) {
			case 'h':
				print_help(0);
				break;
			case 's':
				break;
			case 'd':
				break;
			case 'u':
				break;
			case ':':
				fprintf(stderr, "sat-alarm: Error - Option `%c' needs a value\n\n", optopt);
				print_help(1);
				break;
			case '?':
				fprintf(stderr, "sat-alarm: Error - No such option: `%c'\n\n", optopt);
				print_help(1);
		}
	}

	// Check mandatory parameters:
	if (sat == -1 | url == -1) {
		printf("-s  and -u are mandatory!\n");
		exit(1);
	}
	int time_step = 60;
	if(set_date == -1) {
		epoch = time(NULL);
		struct tm tm = *gmtime(&epoch);
		day = tm.tm_mday;
		the_date = (int)epoch;
	}
	get_daily_prediction(time_step,day,epoch,sat_name);

	exit(0);
}