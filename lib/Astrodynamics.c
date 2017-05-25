/*
======================================
Introduction to MPI
David de la Torre / Aerospace Terrassa / UPC
Spring 2017
======================================
*/

#include <math.h>

#include "Astrodynamics.h"

// From True anomaly to Mean anomaly
double O2M ( double nu, double ecc )
{
  // Variables
  double E, M;
  
  // Eccentric anomaly E
  E = 2.0 * atan(sqrt((1.0 - ecc) / (1.0 + ecc)) * tan(nu / 2.0));

  // Mean anomaly M
  M = E - ecc * sin(E);

  return(M);
}

// From Mean anomaly to True anomaly
double M2O ( double M, double ecc )
{
  // Variables
  double E, Ebkp, nu;
  
  // Eccentric anomaly E
  Ebkp = E = M; // Initial guess & backup value
  for (int i=0; i<100; i++)
  {
    E = E - (E - ecc * sin(E) - M) / (1.0 - ecc * cos(E)); // Newton-Raphson
    if (fabs(E - Ebkp) < 1E-9) break; // Tolerance achieved
    Ebkp = E; // Backup value
  }

  // True anomaly nu
  nu = 2.0 * atan(sqrt((1.0 + ecc) / (1.0 - ecc)) * tan(E / 2.0));

  return(nu);
}

// From KEP elements to ICF position
void KEP2ICF ( double r_ijk[3], double sma, double ecc, double inc, double nu, double argp, double raan )
{
  // Variables
  double p, r, rp, rq, rw;
  double cos_raan, sin_raan, cos_argp, sin_argp, cos_inc, sin_inc;
  double Px, Py, Pz, Qx, Qy, Qz, Wx, Wy, Wz;

  // Calculate p parameter (semi-latus rectum)
  p = sma * (1.0 - ecc*ecc);

  // Calculate position (norm)
  r = p / (1.0 + ecc * cos(nu));

  // Position vector in Local Coordinate Frame LCF (pqw)
  rp = r * cos(nu);
  rq = r * sin(nu);
  rw = 0.0;

  // Common terms for the rotation matrix
  cos_raan = cos(raan);
  sin_raan = sin(raan);
  cos_argp = cos(argp);
  sin_argp = sin(argp);
  cos_inc = cos(inc);
  sin_inc = sin(inc);

  // Components of the rotation matrix
  Px = + cos_raan * cos_argp - sin_raan * cos_inc * sin_argp;
  Py = + sin_raan * cos_argp + cos_raan * cos_inc * sin_argp;
  Pz = + sin_inc  * sin_argp;
  Qx = - cos_raan * sin_argp - sin_raan * cos_inc * cos_argp;
  Qy = - sin_raan * sin_argp + cos_raan * cos_inc * cos_argp;
  Qz = + sin_inc  * cos_argp;
  Wx = + sin_raan * sin_inc;
  Wy = - cos_raan * sin_inc;
  Wz = + cos_inc;

  // Components of the position vector rotated into ICF (r_ijk = M * r_pqw)
  r_ijk[0] = rp * Px + rq * Qx + rw * Wx;
  r_ijk[1] = rp * Py + rq * Qy + rw * Wy;
  r_ijk[2] = rp * Pz + rq * Qz + rw * Wz;
}

// Propagate KEP elements and convert them to ICF position
void Propagate_KEP2ICF ( double r_ijk[3], double sma, double ecc, double inc, double argp, double raan, double M0, double dt, double mu )
{
  // Variables
  double nn, M, nu;
  
  // Compute true anomaly
  nn = sqrt(mu / (sma*sma*sma)); // Compute mean motion
  M = M0 + nn * dt; // Propagate mean anomaly
  M = fmod ( M, 2.0 * M_PI ); // Wrap 0-2pi
  nu = M2O ( M, ecc ); // Compute true anomaly

  // Orbital elements to 3D position and velocity vectors in ICF frame
  KEP2ICF ( r_ijk, sma, ecc, inc, nu, argp, raan );
}

// ----------------------------------------------------

// From calendar date (YMDHMS) to Julian Day J2000
double Cal2JD2K ( int year, int month, int day, int hour, int minute, double second )
{
  // Variables
  int a, y, m, JDN;
  double JD;
  
  // Compute number of years (y) and months (m) since March 1 -4800 (March 1, 4801 BC)
  a = floor((14 - month) / 12);
  y = year + 4800 - a;
  m = month + 12*a - 3;

  // Compute Julian Day Number
  JDN = day + floor((153*m + 2)/5) + 365*y + floor(y/4) - floor(y/100) + floor(y/400) - 32045; // Gregorian calendar

  // Find Julian Date
  JD = JDN + (hour - 12)/24 + minute/1440 + second/86400;

  // Adjust for J2000 reference frame
  JD -= 2451545.0; // JD2K
  
  // Return value
  return(JD);
}

// From Julian Day J2000 to calendar date (YMDHMS)
void JD2K2Cal ( int *year, int *month, int *day, int *hour, int *minute, double *second, const double jd2k )
{
  // Variables
  double jd, fday, alpha;
  int z, a, b, c, d, e;
  double dayf, fracday, dechours, decminutes;
  
  // Adjust for J2000
  jd = jd2k + 2451545.0;

  // Convert JD to Calendar (Gregorian)
  z = (int)(jd + 0.5);
  fday = jd + 0.5 - z;
  if (fday < 0.0)
  {
    fday = fday + 1;
    z = z - 1;
  }
  if (z < 2299161) { a = z; }
  else
  {
    alpha = floor((z - 1867216.25) / 36524.25);
    a = z + 1 + alpha - floor(alpha / 4);
  }
  
  b = a + 1524;
  c = (int)((b - 122.1) / 365.25);
  d = (int)(365.25 * c);
  e = (int)((b - d) / 30.6001);
  dayf = b - d - (int)(30.6001 * e) + fday;
  if (e < 14) { (*month) = e - 1; }
  else { (*month) = e - 13; }
  if ((*month) > 2) { (*year) = c - 4716; }
  else { (*year) = c - 4715; }

  // Get fractional part of the day (H:M:S)
  fracday = dayf - (int)(dayf);
  (*day) = (int)(dayf);
  dechours = fracday * 24;
  (*hour) = (int)(dechours);
  decminutes = (int)(dechours - (*hour)) * 60;
  (*minute) = (int)(decminutes);
  (*second) = (decminutes - (*minute)) * 60;
}

// ----------------------------------------------------

// Return KEP elements of a solar system planet
void GetPlanetKEP ( double *sma_o, double *ecc_o, double *inc_o, double *argp_o, double *raan_o, double *M_o, int body, double jd2k )
{
  // Variables
  double UT, d;
  int year, month, day, hour, minute; 
  double second;
  double sma, ecc, inc, argp, raan, M;
  
  // Compute d reference time
  JD2K2Cal ( &year, &month, &day, &hour, &minute, &second, jd2k );
  UT = (double)(hour) + (double)(minute)/60.0 + (double)(second)/3600.0;
  d = (double)(367*year - 7 * ( year + ((month+9)/12) ) / 4 + 275*(month/9) + day - 730530) + UT/24.0;

  // Planet ephemerides
  switch (body)
  {
    case 1: // Mercury
      raan = 48.3313 + 3.24587E-5 * d;
      inc = 7.0047 + 5.00E-8 * d;
      argp = 29.1241 + 1.01444E-5 * d;
      sma = 0.387098;
      ecc = 0.205635 + 5.59E-10 * d;
      M = 168.6562 + 4.0923344368 * d;
      break;

    case 2: // Venus
      raan = 76.6799 + 2.46590E-5 * d;
      inc = 3.3946 + 2.75E-8 * d;
      argp = 54.8910 + 1.38374E-5 * d;
      sma = 0.723330;
      ecc = 0.006773 - 1.302E-9 * d;
      M = 48.0052 + 1.6021302244 * d;
      break;

    case 3: // Earth
      raan = 0.0;
      inc = 0.0;
      argp = 282.9404 + 4.70935E-5 * d;
      sma = 1.000000;
      ecc = 0.016709 - 1.151E-9 * d;
      M = 356.0470 + 0.9856002585 * d;
      break;

    case 4: // Mars
      raan = 49.5574 + 2.11081E-5 * d;
      inc = 1.8497 - 1.78E-8 * d;
      argp = 286.5016 + 2.92961E-5 * d;
      sma = 1.523688;
      ecc = 0.093405 + 2.516E-9 * d;
      M = 18.6021 + 0.5240207766 * d;
      break;

    case 5: // Jupiter
      raan = 100.4542 + 2.76854E-5 * d;
      inc = 1.3030 - 1.557E-7 * d;
      argp = 273.8777 + 1.64505E-5 * d;
      sma = 5.20256;
      ecc = 0.048498 + 4.469E-9 * d;
      M = 19.8950 + 0.0830853001 * d;
      break;

    case 6: // Saturn
      raan = 113.6634 + 2.38980E-5 * d;
      inc = 2.4886 - 1.081E-7 * d;
      argp = 339.3939 + 2.97661E-5 * d;
      sma = 9.55475;
      ecc = 0.055546 - 9.499E-9 * d;
      M = 316.9670 + 0.0334442282 * d;
      break;

    case 7: // Uranus
      raan = 74.0005 + 1.3978E-5 * d;
      inc = 0.7733 + 1.9E-8 * d;
      argp = 96.6612 + 3.0565E-5 * d;
      sma = 19.18171 - 1.55E-8 * d;
      ecc = 0.047318 + 7.45E-9 * d;
      M = 142.5905 + 0.011725806 * d;
      break;

    case 8: // Neptune
      raan = 131.7806 + 3.0173E-5 * d;
      inc = 1.7700 - 2.55E-7 * d;
      argp = 272.8461 - 6.027E-6 * d;
      sma = 30.05826 + 3.313E-8 * d;
      ecc = 0.008606 + 2.15E-9 * d;
      M = 260.2471 + 0.005995147 * d;
      break;
  }

  // Convert units to [rad] and [km]
  (*sma_o) = sma * AU; // From AU to km
  (*ecc_o) = ecc;
  (*inc_o) = inc * M_PI/180.0; // From deg to rad
  (*raan_o) = fmod(raan*M_PI/180.0,2.0*M_PI); // From deg to rad
  (*argp_o) = fmod(argp*M_PI/180.0,2.0*M_PI); // From deg to rad
  (*M_o) = fmod(M*M_PI/180.0,2.0*M_PI); // From deg to rad
}

// ---------------------------------------------------------------

// Greenwich Mean Sidereal time
double JD2GMST ( double jd2k )
{
  // Variables
  double jc, gmst;
  
  // Julian centuries since J2000
  jc = jd2k / 36525.0;

  // Compute Greenwich mean sidereal time GMST [deg]
  // Equation from [1] pag 88 eq 12.4. Adopted by IAU on 1982.
  gmst = 280.46061837 + 360.98564736629 * jd2k // 1st order approximation
    + 0.000387933 * jc*jc - 1.0/38710000.0 * jc*jc*jc; // Long-term corrections

  // Fix GMST/GAST to 0-360
  gmst = fmod(gmst,360.0);
  
  // Return value
  return(gmst);
}

// Local Mean Sidereal time
double JD2LMST ( double jd2k, double lon )
{
  // Variables
  double gmst, lmst;
  
  // GMST
  gmst = JD2GMST ( jd2k );

  // LMST
  lmst = gmst + lon;
  
  // Fix LMST to 0-360
  lmst = fmod(lmst,360.0);
  
  // Return value
  return(lmst);
}

// From RA/DEC to AZ/EL
void RADEC2AZEL ( double *az, double *el, double ra, double dec, double lon, double lat, double jd2k )
{
  // Variables
  double lmst, ha, sin_az, cos_az;
  
  // Compute Local Mean Sidereal Time [deg] [1]
  lmst = JD2LMST ( jd2k, lon ); // [deg]

  // Compute hour angle [deg] [2] Pag. 174
  ha = lmst - ra;
  
  // Convert magnitudes to radians
  lat *= M_PI/180.0;
  dec *= M_PI/180.0;
  ha *= M_PI/180.0;
  (*el) *= M_PI/180.0;

  // Elevation angle [deg] [2] Pag. 175
  (*el) = asin(sin(lat) * sin(dec) + cos(lat) * cos(dec) * cos(ha));

  // Compute azimuth (sine) [deg] [2] Pag. 175
  sin_az = - (sin(ha) * cos(dec) * cos(lat)) / (cos((*el)) * cos(lat));

  // Compute azimuth (cosine) [deg] [2] Pag. 175
  cos_az = (sin(dec) - sin((*el)) * sin(lat)) / (cos((*el)) * cos(lat));
  (*az) = acos(cos_az);

  // Convert to degrees
  (*az) *= 180.0/M_PI;
  (*el) *= 180.0/M_PI;
  
  // Detect quadrant
  if (sin_az < 0.0) (*az) = 360.0 - (*az); // 3rd/4rd quadrants

  // Wrap azimuth angle
  (*az) = fmod((*az),360.0);
}

// From ECI to RA/DEC
void ECI2RADEC ( double *ra, double *dec, double r_eci[3] )
{
  // Variables
  double ri, rj, rk, norm_r_eci;
  
  // Position vector components
  ri = r_eci[0];
  rj = r_eci[1];
  rk = r_eci[2];

  // Compute norm of vector
  norm_r_eci = sqrt(r_eci[0]*r_eci[0] + r_eci[1]*r_eci[1] + r_eci[2]*r_eci[2]);
  
  // Compute declination angle [rad]
  (*dec) = asin(rk / norm_r_eci);

  // Compute right ascension [rad]
  (*ra) = atan2(rj,ri);

  // Convert units to degrees
  (*ra) = fmod((*ra) * 180/M_PI, 360.0);
  (*dec) = (*dec) * 180/M_PI;
}

// From ECEI to ECI
void ECEI2ECI ( double r_eci[3], double r_ecei[3], double jd2k )
{
  // Variables
  double t, e, x, y, z;
  
  // Auxiliary time magnitudes
  t = jd2k / 36525.0; // Julian centuries from epoch J2000.0
  
  // Compute mean obliquity of the ecliptic angle of date [2]
  e = 84381.448 - 46.8150 * t - 0.00059 * t*t + 0.001813 * t*t*t; // [arcsec]
  e = e / 3600.0; // [deg]
  e = e * M_PI/180.0; // [rad]
  
  // Coordinate transformation between ECEI and ECI [1] Eq. 2.40
  x = r_ecei[0];
  y = r_ecei[1];
  z = r_ecei[2];
  r_eci[0] = x;
  r_eci[1] = y*cos(e) - z*sin(e);
  r_eci[2] = z*cos(e) + y*sin(e);
}

// From ICF to ECEI
void ICF2ECEI ( double r_ecei[3], double r_icf[3], double jd2k )
{
  // Variables
  double r_icf_earth[3];
  double sma, ecc, inc, argp, raan, M;
  
  // Compute Earth (id=3) position in ICF coordinates
  GetPlanetKEP ( &sma, &ecc, &inc, &argp, &raan, &M, 3, jd2k );
  
  // Propagate KEP and convert it to ICF
  Propagate_KEP2ICF ( r_icf_earth, sma, ecc, inc, argp, raan, M, 0, MU );
  
  // Translate r_icf vector to Earth centered reference frame
  r_ecei[0] = r_icf[0] - r_icf_earth[0];
  r_ecei[1] = r_icf[1] - r_icf_earth[1];
  r_ecei[2] = r_icf[2] - r_icf_earth[2];
  
}

// From ICF to AZ/EL
void ICF2AER ( double *az, double *el, double r_icf[3], double jd2k, double lon, double lat )
{
  // Variables
  double r_ecei[3], r_eci[3], ra, dec;
  
  // ICF -> ECEI
  ICF2ECEI ( r_ecei, r_icf, jd2k );

  // ECEI -> ECI
  ECEI2ECI ( r_eci, r_ecei, jd2k );

  // ECI -> Ra/Dec
  ECI2RADEC ( &ra, &dec, r_eci );

  // Ra/Dec -> Az/El
  RADEC2AZEL ( az, el, ra, dec, lon, lat, jd2k );
}

// ----------------------------------------------------------
