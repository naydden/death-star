/*
======================================
Introduction to MPI
David de la Torre / Aerospace Terrassa / UPC
Spring 2017
======================================
*/

#include <math.h>

#ifndef _H_ASTRODYNAMICS
#define _H_ASTRODYNAMICS

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

// ----------------------------------------------------------

// From True anomaly to Mean anomaly
double O2M ( double nu, double ecc );

// From Mean anomaly to True anomaly
double M2O ( double M, double ecc );

// From KEP elements to ICF position
void KEP2ICF ( double r_ijk[3], double sma, double ecc, double inc, double nu, double argp, double raan );

// Propagate KEP elements and convert them to ICF position
void Propagate_KEP2ICF ( double r_ijk[3], double sma, double ecc, double inc, double argp, double raan, double M0, double dt, double mu );

// ----------------------------------------------------------

// From calendar date (YMDHMS) to Julian Day J2000
double Cal2JD2K ( int year, int month, int day, int hour, int minute, double second );

// From Julian Day J2000 to calendar date (YMDHMS)
void JD2K2Cal ( int *year, int *month, int *day, int *hour, int *minute, double *second, double jd2k );

// ----------------------------------------------------------

// Return KEP elements of a solar system planet
void GetPlanetKEP ( double *sma_o, double *ecc_o, double *inc_o, double *argp_o, double *raan_o, double *M_o, int body, double jd2k );

// ----------------------------------------------------------

// Greenwich Mean Sidereal time
double JD2GMST ( double jd2k );

// Local Mean Sidereal time
double JD2LMST ( double jd2k, double lon );

// From RA/DEC to AZ/EL
void RADEC2AZEL ( double *az, double *el, double ra, double dec, double lon, double lat, double jd2k );

// From ECI to RA/DEC
void ECI2RADEC ( double *ra, double *dec, double r_eci[3] );

// From ECEI to ECI
void ECEI2ECI ( double r_eci[3], double r_ecei[3], double jd2k );

// From ICF to ECEI
void ICF2ECEI ( double r_ecei[3], double r_ijk[3], double jd2k );

// From ICF to AZ/EL
void ICF2AER ( double *az, double *el, double r_icf[3], double jd2k, double lon, double lat );

// ----------------------------------------------------------

#endif