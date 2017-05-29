#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
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

	KEP *satellite;
	satellite=read_sat(0, 1);
	//satellite[0].name="ISS";
	
	satellite[0].sma=6775;
	satellite[0].ecc=0.0005238;
	satellite[0].inc=0.9012898;
	satellite[0].argp=2.9158675;
	satellite[0].raan=3.3322712;
	satellite[0].M=4.8949644;
	satellite[0].epoch=6342.53;
	

	KEP *object;
	object=read_sat(a1-1, a2);

	KEP collider;

	int year=2017;
	int month=5;
	int day=29;
	int hour=18;
	int minute=30;
	double second=0;
	double TimeInit=Cal2JD2K ( year, month, day, hour, minute, second )*86400.0;
	double TimeEnd=TimeInit+100000;
	double TimeStep=100;
	double TimeComm=100*TimeStep;

	int collision=0;
	int prevColl=0;
	double CollisionTime=1000*TimeEnd;
	double CollisionTimeG=CollisionTime;

	double rs_ijk[3];
	double ro_ijk[3];
	double distance;
	double SecDistance=10;

	double progress;

	for(double Time=TimeInit; Time<=TimeEnd; Time=Time+TimeStep){
		if(quisoc()==0){
			progress=100*(Time-TimeInit)/(TimeEnd-TimeInit);
			if(fabs(fmod(progress,1.0)<=1E-6)) printf("Progress: %d %% \n", (int)progress);
		}
		Propagate_KEP2ICF ( rs_ijk, satellite[0].sma, satellite[0].ecc, satellite[0].inc, satellite[0].argp, satellite[0].raan, satellite[0].M, satellite[0].epoch-Time, E_MU );
		for(int k=a1; k<=a2; k++){
			Propagate_KEP2ICF ( ro_ijk, object[k-a1].sma, object[k-a1].ecc, object[k-a1].inc, object[k-a1].argp, object[k-a1].raan, object[k-a1].M, object[k-a1].epoch-Time, E_MU );
			distance=(rs_ijk[0]-ro_ijk[0])*(rs_ijk[0]-ro_ijk[0])+(rs_ijk[1]-ro_ijk[1])*(rs_ijk[1]-ro_ijk[1])+(rs_ijk[2]-ro_ijk[2])*(rs_ijk[2]-ro_ijk[2]);
			distance=sqrt(distance);
			if(distance<SecDistance){
				if(prevColl==0){
					collision=1;
					prevColl=1;
					CollisionTime=Time;
					collider=object[k-a1];
				}
			}
		}
		
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
			printf("Collider: %s \nYear: %d Month: %d Day: %d Hour: %d Minute: %d Seconds: %f  \n", collider.name, yearC, monthC, dayC, hourC, minuteC, secondC);
			printf("Program time %f \n", Tprogramm);
		}
	}
	else{
		if(quisoc()==0){
			printf("No collision detected \n");
			printf("Program time %f \n", Tprogramm);
		}
	}

	free(satellite);
	free(object);

	MPI_Finalize();

 	exit(0);

}