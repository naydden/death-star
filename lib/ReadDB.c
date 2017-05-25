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

#include "ReadDB.h"

// Function to read KEP elements of NEO DB file into structure
KEP *read_neo ( const char *fname, int i0, int i1 )
{
  // Allocate structure
  KEP *ast = (KEP*) malloc((i1-i0) * sizeof(KEP)); assert(ast != NULL);

  // Open file
  FILE *fid = fopen(fname,"r"); assert(fid != NULL);

  // Skip the first i0 lines (plus header)
  for (int k=0; k<i0; k++) fscanf(fid,"%*[^\n]\n");

  // Read i0-to-i1 formatted lines
  for (int i=0, st=0; i<(i1-i0) && st!=EOF; i++)
  {
    // Read RAW data
    st = fscanf(fid,"%[^,],%lf,%lf,%lf,%lf,%lf,%lf,%lf,%*[^\n]\n",
      ast[i].name, &ast[i].epoch, &ast[i].sma, &ast[i].ecc,
      &ast[i].inc, &ast[i].argp, &ast[i].raan, &ast[i].M);
    
    // Adjust units
    ast[i].sma = ast[i].sma * AU;
    ast[i].inc = ast[i].inc * M_PI/180.0;
    ast[i].argp = ast[i].argp * M_PI/180.0;
    ast[i].raan = ast[i].raan * M_PI/180.0;
    ast[i].M = ast[i].M * M_PI/180.0;
  }

  // Close file
  fclose(fid);

  // Return KEP structure
  return(ast);
}

// Function to read KEP elements of Astorb DB file into structure
KEP *read_astorb ( int i0, int i1 )
{
  // Allocate struct
  KEP *ast = (KEP*) malloc((i1 - i0) * sizeof(KEP)); assert(ast != NULL);

  // Open files
  FILE *fidN = fopen("astorb_names.txt","r"); assert(fidN != NULL);
  FILE *fidK  = fopen("astorb_kep.txt","r"); assert(fidK != NULL);

  // Skip the first i0 lines (plus header)
  for (int k=0; k<i0; k++)
  {
    fscanf(fidN,"%*[^\n]\n");
    fscanf(fidK,"%*[^\n]\n");
  }

  // Read i0-to-i1 formatted lines
  for (int i=0, st=0; i<(i1-i0) && st!=EOF; i++)
  {
    st = fscanf(fidN,"%[^\n]\n",ast[i].name);
    st = fscanf(fidK,"%lf %lf %lf %lf %lf %lf %lf\n",
      &ast[i].sma, &ast[i].ecc, &ast[i].inc, 
      &ast[i].argp, &ast[i].raan, &ast[i].M, &ast[i].epoch);
    
    // Adjust units
    ast[i].sma = ast[i].sma * AU;
    ast[i].inc = ast[i].inc * M_PI/180.0;
    ast[i].argp = ast[i].argp * M_PI/180.0;
    ast[i].raan = ast[i].raan * M_PI/180.0;
    ast[i].M = ast[i].M * M_PI/180.0;
  }

  // Close file
  fclose(fidN);
  fclose(fidK);

  // Return KEP structure
  return(ast);
}

//Function to read the KEP elements of Objects in orbit DB file into structure
KEP *read_sat ( int i0, int i1 )
{
  // Allocate struct
  KEP *ast = (KEP*) malloc((i1 - i0) * sizeof(KEP)); assert(ast != NULL);

  // Open files
  FILE *fid = fopen("database.csv","r"); assert(fid != NULL);

  // Skip the first i0 lines (plus header)
  for (int k=0; k<i0; k++) fscanf(fid,"%*[^\n]\n");

  // Read i0-to-i1 formatted lines
  for (int i=0, st=0; i<(i1-i0) && st!=EOF; i++)
  {
    // Read RAW data
    st = fscanf(fid,"%s %lf %lf %lf %lf %lf %lf %lf ",
      ast[i].name, &ast[i].sma, &ast[i].ecc,&ast[i].inc, &ast[i].argp, &ast[i].raan, &ast[i].M, &ast[i].epoch); //%*[^\n]\n
    
    // Adjust units
    ast[i].sma = ast[i].sma * 1000;
    ast[i].inc = ast[i].inc * M_PI/180.0;
    ast[i].argp = ast[i].argp * M_PI/180.0;
    ast[i].raan = ast[i].raan * M_PI/180.0;
    ast[i].M = ast[i].M * M_PI/180.0;
  }

  // Close file
  fclose(fid);

  // Return KEP structure
  return(ast);
}
