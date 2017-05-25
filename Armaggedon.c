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

	int N;
	int Np;
	int a1;
	int a2;

	//Find the number of asteroids somehow. Meanwhile, use this:
	N=20;

	a1=quisoc()*N/quants()+1;
	a2=(quisoc()+1)*N/quants();
	Np=a2-a1+1;

	KEP *object;
	object=read_sat(a1-1, a2);

	double collision=0;
	double CollisionTime=1000;
	int t=0;

	int TimeInit=0;
	int TimeEnd=15;
	int TimeStep=1;
	int TimeComm=5;

	for(int Time=TimeInit+1; Time<=TimeEnd; Time=Time+TimeStep){

		//Propagate things and other stuff


		if(Time%TimeComm==0){
			r=MPI_Barrier(MPI_COMM_WORLD);
			MPI_Allreduce(&collision, &collision, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
			MPI_Allreduce(&CollisionTime, &CollisionTime, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
			if(collision==1){
				break;
			}
			r=MPI_Barrier(MPI_COMM_WORLD);
		}
	}
	if(quisoc()==0){
		printf("Collision= %f Time of collision= %f \n", collision, CollisionTime);
	}

	MPI_Finalize();

 	exit(0);

}