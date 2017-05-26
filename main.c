#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include "mpi.h"
#include "sgp_lib/sgdp4h.h"
#include "lib/ReadDB.h"

// Earth cte and radius
long int GM = 3.98600415e5;
long int RT = 6371;

void checkr(int r,char *txt) { 
  if (r!=MPI_SUCCESS) {
    fprintf(stderr,"Error: %s\n",txt);
    exit(-1);
  }
}

int quisoc() {
  int a,b;
  a=MPI_Comm_rank(MPI_COMM_WORLD,&b);
  checkr(a,"quisoc");
  return(b);
}

int quants() {
  int a,b;
  a=MPI_Comm_size(MPI_COMM_WORLD,&b);
  checkr(a,"quants");
  return(b);
}


//  returns x,y,z vector for a given time.
void xyz_position(double jd, xyz_t *pos) {
	// satpos_xyz propagates the orbital elements given in init_sgpd4 and converts to xyz
	if(satpos_xyz(jd, pos, NULL) == SGDP4_ERROR) printf("SGP4_ERROR");
	return;
}

//  returns 1 if there is a crash and 0 if there is not
//  has to return timestamp and object
int is_crash(xyz_t *sat_pos, xyz_t *deb_pos, int d) {
	double distance;
	distance = sqrt((sat_pos->x - deb_pos->x)*(sat_pos->x - deb_pos->x) + (sat_pos->y - deb_pos->y)*(sat_pos->y - deb_pos->y) + (sat_pos->z - deb_pos->z)*(sat_pos->z - deb_pos->z));

	// printf("Distance: %f\n", distance);
	// printf("Security: %d\n", d);
	if ( distance <= d) {
		return 1;
	}
	return 0;
}

int deb_valid(KEP *deb_object) {
	//  checks mean motion
	double mean_motion = (86400/(2*M_PI*sqrt((RT+deb_object->sma)*(RT+deb_object->sma)*(RT+deb_object->sma)/GM)));
	double eccentricity = deb_object->ecc;
	// printf("Mean Motion: %f\n",mean_motion);
	if( mean_motion > 18 || eccentricity >= 0.98 ) {
		return 1;
	}
	return 0;
}

void check_sgdp4(int *imode) {
	// switch(*imode)
	// {
	// case SGDP4_ERROR     :  printf("# SGDP error\n"); break;
	// case SGDP4_NOT_INIT  :  printf("# SGDP not init\n"); break;
	// case SGDP4_ZERO_ECC  :  printf("# SGDP zero ecc\n"); break;
	// case SGDP4_NEAR_SIMP :  printf("# SGP4 simple\n"); break;
	// case SGDP4_NEAR_NORM :  printf("# SGP4 normal\n"); break;
	// case SGDP4_DEEP_NORM :  printf("# SDP4 normal\n"); break;
	// case SGDP4_DEEP_RESN :  printf("# SDP4 resonant\n"); break;
	// case SGDP4_DEEP_SYNC :  printf("# SDP4 synchronous\n"); break;
	// default: 				printf("# SGDP mode not recognised!\n");
	// }
}
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
// parallel stuff
	int r;
	r=MPI_Init(&argc,&argv); checkr(r,"init");

	//  num of debris object to consider till 17538
	int n = 17000;

	int a1=quisoc()*n/quants()+1;
	int a2=(quisoc()+1)*n/quants();

	KEP *deb_object;
	deb_object = read_sat(a1-1, a2);

	int time_comm = 10; //s
	int collision = 0;
	int CollisionTime;
	int CollisionTimeG;
	char collider[100];

//  an option for the satellite TLE file (-t)
//  an option that gives a characteristic diamter of the satellite. A satellite will be treated as a  sphere. (-d)
//  an option for the minimum distance is given. Distance will be in times of the diameter of the satellite (-f)
	int opt;
	int tle_satus = -1;
	int diameter_satuts = -1;
	int zone_status = -1;


	int imode;
	xyz_t sat_pos, deb_pos;
	double jd, tsince;

	long ii,imax=5,iend=-1;
	double ts = 0.0, delta=1.0;

	extern double SGDP4_jd0;

	orbit_t orb;

	char tle[210];
	int diameter;
	int security_ratio;

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
				diameter = atoi(optarg);
				diameter_satuts = 1;
				break;
			case 'f':
				security_ratio = atoi(optarg);
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

	FILE *tle_sat;
	tle_sat = fopen(tle,"rb");

	if (!tle_sat) {
		fprintf(stderr, "Error while reading satellite TLE.'\n");
		return 1;
	}
	int i = 0;
	// read_twoline returns orbital elements out of a TLE
	if (read_twoline(tle_sat, 0, &orb) == 0) {
		print_orb(&orb);
		fclose(tle_sat);

		imode = init_sgdp4(&orb);
		check_sgdp4(&imode);

		for(ii=0; ii <= 86400; ii++)
			{
				if(ii != iend) {
					tsince=ts + ii*delta;
				}
				else {
					tsince=ts + delta;
				}

				jd = SGDP4_jd0 + tsince / 1440.0;
				/*********** SATELLITE **************************************/
				// init_sgdp4 passes all of the required orbital elements to
				// the sgdp4() function together with the pre-calculated constants.
				imode = init_sgdp4(&orb);
				check_sgdp4(&imode);
				//  stops program if data is not valid for SGDP
				if(imode == SGDP4_ERROR) exit(1);
				xyz_position(jd, &sat_pos);

				printf("%12.4f   %16.8f %16.8f %16.8f\n",
					tsince,
					sat_pos.x, sat_pos.y, sat_pos.z);
				/*********** DEBRIS **************************************/
				for(int deb=a1; deb <= a2; deb++) {
					orbit_t orb_deb;
					// printf("%d\n", deb);
					// printf("%s\n", deb_object[deb].name);
					if (deb_valid(&deb_object[deb]) == 1) continue;

					orb_deb.ep_year = (int)deb_object[deb].epoch/365+2000;
					orb_deb.ep_day = 1.0;
					orb_deb.rev = 86400/(2*M_PI*sqrt((RT+deb_object[deb].sma)*(RT+deb_object[deb].sma)*(RT+deb_object[deb].sma)/GM));
					orb_deb.bstar = 1.0608e-05;
					orb_deb.eqinc = deb_object[deb].inc;
					orb_deb.ecc = deb_object[deb].ecc;
					orb_deb.mnan = deb_object[deb].M;
					orb_deb.argp = deb_object[deb].argp;
					orb_deb.ascn = deb_object[deb].raan;
					orb_deb.smjaxs = deb_object[deb].sma;
					orb_deb.norb = orb_deb.rev*(int)deb_object[deb].epoch;
					orb_deb.satno = 2;

					// printf("---------\n");
					// print_orb(&orb_deb);

					imode = init_sgdp4(&orb_deb);
					check_sgdp4(&imode);
					//  stops program if data is not valid for SGDP
					if(imode == SGDP4_ERROR) continue;
					xyz_position(jd, &deb_pos);
					// if block is entered only when there have not been any other collision until now.
					if (is_crash(&sat_pos,&deb_pos, diameter*security_ratio) == 1 && collision == 0) {
						// printf("Crashed with %s\n", deb_object[deb].name);
						// printf("Crashed time %lf\n",tsince);
						collision = 1;
						CollisionTime = tsince;
						strcpy(collider,deb_object[deb].name);
						break;
					}

				}

				if ((int)tsince%time_comm==0) {
					r=MPI_Barrier(MPI_COMM_WORLD);
					MPI_Allreduce(&collision, &collision, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
					if(collision==1){
						MPI_Allreduce(&CollisionTime, &CollisionTimeG, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
						if(CollisionTime==CollisionTimeG){
							printf("Collider: %s Time of collision: %d \n", collider, CollisionTimeG);
						}
							MPI_Finalize();
							exit(0);
					}
				}
			}
		}

	if(quisoc()==0)	printf("No collision detected \n");

	MPI_Finalize();
	exit(0);
}
