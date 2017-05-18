#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

void print_help(int exval) {
	printf("sat-alarm [-h] -t <object-tle-file.txt> -d <characteristic-diamter-of-satellite in [m]> -f <times-the-diameter-for-security-zone> \n\n");
 	printf("  -h              print this help and exit\n");
 	printf("  -t              TLE file of object.\n");
 	printf("  -d              Characteristic diameter of the object of study.\n");
 	printf("  -f              Times the diameter for the security zone of the object of the study.\n");
 exit(exval);
}

int main(int argc, char **argv)
{

//  an option for the satellite TLE file (-t)
//  an option that gives a characteristic diamter of the satellite. A satellite will be treated as a  sphere. (-d)
//  an option for the minimum distance is given. Distance will be in times of the diameter of the satellite (-f)
	int opt;
	int tle_satus = -1;
	int diameter_satuts = -1;
	int zone_status = -1;

	char tle[210];
	char diameter[10];
	char security_ratio[3];

	if(argc == 1) {
		fprintf(stderr, "This program needs arguments....\n\n");
		print_help(1);
	}
	while((opt = getopt(argc, argv, "ht:d:f:")) != -1) {
		switch(opt) {
			case 'h':
				print_help(0);
				break;
			case 't':
				strcpy(tle, optarg);
				tle_satus = 1;
				break;
			case 'd':
				strcpy(diameter, optarg);
				diameter_satuts = 1;
				break;
			case 'f':
				strcpy(security_ratio, optarg);
				zone_status = 1;
				break;
			case ':':
				fprintf(stderr, "Error - Option `%c' needs a value\n\n", optopt);
				print_help(1);
				break;
			case '?':
				fprintf(stderr, "Error - No such option: `%c'\n\n", optopt);
				print_help(1);
		}
	}

	// Check mandatory parameters:
	if (tle_satus == -1 | diameter_satuts == -1 | zone_status == -1) {
		printf("-t, -d and -f  are mandatory!\n");
		exit(1);
	}

	FILE *ptr_file;
	char buf[1000];
	ptr_file =fopen("ISS.txt","r");
	if (!ptr_file)
		return 1;
	fgets(buf,1000, ptr_file);
	while (fgets(buf,1000, ptr_file)!=NULL)
		printf("%s",buf);
		printf("\n");
	fclose(ptr_file);

	printf("Diameter %d \n",atoi(diameter));
	printf("Security ratio %d \n",atoi(security_ratio));

	exit(0);
}