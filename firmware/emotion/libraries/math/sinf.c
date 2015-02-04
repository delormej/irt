/* Hacked and copied from:
 *  https://sourceware.org/git/?p=glibc.git;a=tree;f=sysdeps/ieee754/flt-32;hb=4ffffbd272264f083f35783ed81339304025f855
 *
 * Temporary solution in order to save space since using the real trig functions bloats the code way too much
 * to be used.
 */

#include "math_private.h"

static const float 
half =  5.0000000000e-01,/* 0x3f000000 */
S1  = -1.6666667163e-01, /* 0xbe2aaaab */
S2  =  8.3333337680e-03, /* 0x3c088889 */
S3  = -1.9841270114e-04, /* 0xb9500d01 */
S4  =  2.7557314297e-06, /* 0x3638ef1b */
S5  = -2.5050759689e-08, /* 0xb2d72f34 */
S6  =  1.5896910177e-10; /* 0x2f2ec9d3 */

float j_k_sinf(float x, float y, int iy)
{
	float z,r,v;
	int32_t ix;
	GET_FLOAT_WORD(ix,x);
	ix &= 0x7fffffff;			/* high word of x */
	if(ix<0x32000000)			/* |x| < 2**-27 */
	   {if((int)x==0) return x;}		/* generate inexact */
	z	=  x*x;
	v	=  z*x;
	r	=  S2+z*(S3+z*(S4+z*(S5+z*S6)));
	if(iy==0) return x+v*(S1+z*r);
	else      return x-((z*(half*y-v*r)-y)-v*S1);
}

float j_sinf(float x)
{
	float y[2], z = 0.0;
	int32_t n, ix;

	GET_FLOAT_WORD(ix, x);

	/* |x| ~< pi/4 */
	ix &= 0x7fffffff;
	if (ix <= 0x3f490fd8) return j_k_sinf(x, z, 0);

	/* sin(Inf or NaN) is NaN */
	else if (ix >= 0x7f800000) {
		/*if (ix == 0x7f800000)
			return 0; //  __set_errno(EDOM); */
		return x - x;
	}

	/* argument reduction needed */
	else {
		n = j_rem_pio2f(x, y);
		switch (n & 3) {
		case 0: return  j_k_sinf(y[0], y[1], 1);
		case 1: return  j_k_cosf(y[0], y[1]);
		case 2: return -j_k_sinf(y[0], y[1], 1);
		default:
		return -j_k_cosf(y[0], y[1]);
		}
	}
}
