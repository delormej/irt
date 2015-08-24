#include "mpfit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* This is the private data structure which contains the data points
   and their uncertainties */
struct vars_struct {
  double *x;
  double *y;
  double *ey;
};

/* Simple routine to print the fit results */
void printresult(double *x, double *xact, mp_result *result) 
{
  int i;

  if ((x == 0) || (result == 0)) return;
  printf("  CHI-SQUARE = %f    (%d DOF)\n", 
	 result->bestnorm, result->nfunc-result->nfree);
  printf("        NPAR = %d\n", result->npar);
  printf("       NFREE = %d\n", result->nfree);
  printf("     NPEGGED = %d\n", result->npegged);
  printf("     NITER = %d\n", result->niter);
  printf("      NFEV = %d\n", result->nfev);
  printf("\n");
  if (xact) {
    for (i=0; i<result->npar; i++) {
      printf("  P[%d] = %f +/- %f     (ACTUAL %f)\n", 
	     i, x[i], result->xerror[i], xact[i]);
    }
  } else {
    for (i=0; i<result->npar; i++) {
      printf("  P[%d] = %f +/- %f\n", 
	     i, x[i], result->xerror[i]);
    }
  }
    
}

/*
* 2nd order polynmoial fit function
*
* m - number of data points
* n - number of parameters (2)
* p - array of fit parameters
* dy - array of residuals to be returned
* vars - private data (struct vars_struct *)
*
* RETURNS: error code (0 = success)
*/
int polyfunc(int m, int n, double *p, double *dy, double **dvec, void *vars)
{
	int i;
	struct vars_struct *v = (struct vars_struct *) vars;
	double *x, *y, *ey;

	x = v->x;
	y = v->y;
	ey = v->ey;
	
	for (i = 0; i<m; i++) {
		//y = f(x) =        a  + b    * x    + c    *x^2
    //dy[i] = (y[i] - p[0] - p[1] * x[i] - p[2] * x[i] * x[i]) / ey[i];
		// p[0]*x[i]^3  + p[1] * x[i]^2 + p[2] * x[i]
    
    // y = -0.0005x2 + 0.0314x + 0.8894
    //y = f(x) =        ax^2 + bx + c
		dy[i] = (y[i] - p[0] * x[i] * x[i] - p[1] * x[i] - p[2]) / ey[i];
	}

	return 0;
}

int testpolyfit()
{
 const int datapoints = 8;
 double x[] = {
    2.2352,
    4.4704,
    6.7056,
    8.9408,
    11.176,
    13.4112,
    15.6464,
    17.8816
   };
  double y[] = {
    0.953536773,
    1.022294531,
    1.081083394,
    1.131913478,
    1.176794903,
    1.217737788,
    1.256752251,
    1.29584841
   };
  double ey[datapoints];
  double p[] = {0.0, 0.0, 0.0};        /* Initial conditions */             
  double pactual[] = {0.0, 0.0, 0.0};  /* Actual values used to make data */
  double perror[3];		       /* Returned parameter errors */      
  int i;
  struct vars_struct v;
  int status;
  mp_result result;

  memset(&result,0,sizeof(result));          /* Zero results structure */
  result.xerror = perror;	                                      
  for (i=0; i<datapoints; i++) ey[i] = 0.2;       /* Data errors */           

  v.x = x;
  v.y = y;
  v.ey = ey;

  /* Call fitting function for 10 data points and 3 parameters */
  status = mpfit(polyfunc, datapoints, 3, p, 0, 0, (void *) &v, &result);

  printf("*** testpolyfit status = %d\n", status);
  printresult(p, pactual, &result);

  return 0;  
}

int main()
{
  testpolyfit();
}