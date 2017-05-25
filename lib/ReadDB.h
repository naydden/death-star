/*
======================================
Introduction to MPI
David de la Torre / Aerospace Terrassa / UPC
Spring 2017
======================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifndef _H_READDB
#define _H_READDB

// Constants
#ifndef MU
#define MU (1.32712440018E11)
#endif

#ifndef AU
#define AU (1.495978707E8)
#endif

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

// Structure with KEP elements
typedef struct _S_KEP {
  char name[32];
  double sma;
  double ecc;
  double inc;
  double argp;
  double raan;
  double M;
  double epoch;
} KEP;

// Function to read KEP elements of NEO DB file into structure
KEP *read_neo ( const char *fname, int i0, int i1 );

// Function to read KEP elements of Astorb DB file into structure
KEP *read_astorb ( int i0, int i1 );

// Function to read KEP elements of an objects around the Earth DB file into structure
KEP *read_sat ( int i0, int i1 );

#endif