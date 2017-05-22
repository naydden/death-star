#include <stdio.h>
#include <stdlib.h>
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
	N=104;

	a1=quisoc()*N/quants()+1;
	a2=(quisoc()+1)*N/quants();
	Np=a2-a1+1;

	int T=10;
	int dt=1;
	int Ttalk=5;

	printf("First element: %d. Last element: %d. Total elements: %d \n",a1,a2,Np);

	MPI_Finalize();

 	exit(0);

}