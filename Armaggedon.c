#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include "./lib/ReadDB.h"
#include "./lib/Astrodynamics.h"
#include "mpi.h"

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

int main(int argc, char **argv){

	int r;
	r=MPI_Init(&argc,&argv); checkr(r,"init");

	double Tstart;
	double Tfinish;
	double Tprogramm;
	Tstart=MPI_Wtime();

	int N;
	int a1;
	int a2;

	//Find the number of asteroids somehow. Meanwhile, use this:
	N=17000;

	a1=quisoc()*N/quants()+1;
	a2=(quisoc()+1)*N/quants();

	KEP satellite;
	//satellite.name="ISS";
	strncpy(satellite.name, "ISS", 32);
	satellite.sma=6775;
	satellite.ecc=0.0005238;
	satellite.inc=0.9012898;
	satellite.argp=2.9158675;
	satellite.raan=3.3322712;
	satellite.M=4.8949644;
	satellite.epoch=6205;
	

	KEP *object;
	object=read_sat(a1-1, a2);

	KEP collider;

	int year=2017;
	int month=1;
	int day=1;
	int hour=0;
	int minute=0;
	double second=0;
	double TimeInit=Cal2JD2K ( year, month, day, hour, minute, second )*86400.0;
	double TimeEnd=TimeInit+400000;
	double TimeStep=50;
	double TimeComm=400*TimeStep;

	int collision=0;
	double CollisionTime=1000*TimeEnd;
	double CollisionTimeG=CollisionTime;

	double rs_ijk[3];
	double ro_ijk[3];
	double distance;
	double SecDistance=10;
	double distanceC;
	int pos=0;
	
	FILE *PositionS;
	FILE *PositionO;
	PositionS=fopen("plots/PositionS.txt", "w");
	PositionO=fopen("plots/PositionO.txt", "w");
	

	double progress;

	for(double Time=TimeInit; Time<=TimeEnd; Time=Time+TimeStep){
		if(quisoc()==0){
			progress=100*(Time-TimeInit)/(TimeEnd-TimeInit);
			if(fabs(fmod(progress,1.0)<=1E-6)) printf("Progress: %d %% \n", (int)progress);
		}
		Propagate_KEP2ICF ( rs_ijk, satellite.sma, satellite.ecc, satellite.inc, satellite.argp, satellite.raan, satellite.M, Time-86400*satellite.epoch, E_MU );
		for(int k=a1; k<=a2; k++){
			Propagate_KEP2ICF ( ro_ijk, object[k-a1].sma, object[k-a1].ecc, object[k-a1].inc, object[k-a1].argp, object[k-a1].raan, object[k-a1].M, Time-86400*object[k-a1].epoch, E_MU );
			distance=(rs_ijk[0]-ro_ijk[0])*(rs_ijk[0]-ro_ijk[0])+(rs_ijk[1]-ro_ijk[1])*(rs_ijk[1]-ro_ijk[1])+(rs_ijk[2]-ro_ijk[2])*(rs_ijk[2]-ro_ijk[2]);
			distance=sqrt(distance);
			if(distance<SecDistance){
				if(collision==0){
					collision=1;
					CollisionTime=Time;
					collider=object[k-a1];
					distanceC=distance;
					pos=k;
				}
			}
			
			if(k==7637){
				fprintf(PositionO, "%f,%f,%f,%f\n",Time-TimeInit,ro_ijk[0],ro_ijk[1],ro_ijk[2]);
			}
			

		}
		
		fprintf(PositionS, "%f,%f,%f,%f\n",Time-TimeInit,rs_ijk[0],rs_ijk[1],rs_ijk[2]);
		
		
		if(fabs(fmod(Time-TimeInit,TimeComm)<=1E-6)){
			if(quisoc()==0) printf("Communication \n");
			MPI_Allreduce(&collision, &collision, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
			if(collision==1){
				MPI_Allreduce(&CollisionTime, &CollisionTimeG, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
				break;
			}
		}
	}
	MPI_Allreduce(&collision, &collision, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
	if(collision==1){
		MPI_Allreduce(&CollisionTime, &CollisionTimeG, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
	}

	Tfinish=MPI_Wtime();
	MPI_Allreduce(&Tstart, &Tstart, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
	MPI_Allreduce(&Tfinish, &Tfinish, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
	Tprogramm=Tfinish-Tstart;

	int yearC;
	int monthC;
	int dayC;
	int hourC;
	int minuteC;
	double secondC;
	JD2K2Cal ( &yearC, &monthC, &dayC, &hourC, &minuteC, &secondC, CollisionTimeG/86400.0 );

	if(collision==1){
		if(CollisionTime==CollisionTimeG){
			printf("Collider: %s Position in Database: %d \n", collider.name, pos);
			printf("Distance: %f Time: %f \n", distanceC, CollisionTimeG-TimeInit);
			printf("Collision Time: %d/%d/%d %d:%d:%d \n", dayC, monthC, yearC, hourC, minuteC, (int)secondC);
			printf("Program time %f \n", Tprogramm);
		}
	}
	else{
		if(quisoc()==0){
			printf("No collision detected \n");
			printf("Program time %f \n", Tprogramm);
		}
	}

	free(object);
	
	fclose(PositionS);
	fclose(PositionO);
	

	MPI_Finalize();

 	exit(0);

}