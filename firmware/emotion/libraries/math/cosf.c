
#include "math_private.h"

static const float 
one =  1.0000000000e+00, /* 0x3f800000 */
C1  =  4.1666667908e-02, /* 0x3d2aaaab */
C2  = -1.3888889225e-03, /* 0xbab60b61 */
C3  =  2.4801587642e-05, /* 0x37d00d01 */
C4  = -2.7557314297e-07, /* 0xb493f27c */
C5  =  2.0875723372e-09, /* 0x310f74f6 */
C6  = -1.1359647598e-11; /* 0xad47d74e */

float j_k_cosf(float x, float y)
{
	float a,hz,z,r,qx;
	int32_t ix;
	GET_FLOAT_WORD(ix,x);
	ix &= 0x7fffffff;			/* ix = |x|'s high word*/
	if(ix<0x32000000) {			/* if x < 2**27 */
	    if(((int)x)==0) return one;		/* generate inexact */
	}
	z  = x*x;
	r  = z*(C1+z*(C2+z*(C3+z*(C4+z*(C5+z*C6)))));
	if(ix < 0x3e99999a) 			/* if |x| < 0.3 */ 
	    return one - ((float)0.5*z - (z*r - x*y));
	else {
	    if(ix > 0x3f480000) {		/* x > 0.78125 */
		qx = (float)0.28125;
	    } else {
	        SET_FLOAT_WORD(qx,ix-0x01000000);	/* x/4 */
	    }
	    hz = (float)0.5*z-qx;
	    a  = one-qx;
	    return a - (hz - (z*r-x*y));
	}
}

float j_cosf(float x)
{
	float y[2], z = 0.0;
	int32_t n, ix;

	GET_FLOAT_WORD(ix, x);

	/* |x| ~< pi/4 */
	ix &= 0x7fffffff;
	if (ix <= 0x3f490fd8) return j_k_cosf(x, z);

	/* cos(Inf or NaN) is NaN */
	else if (ix >= 0x7f800000) {
		/*if (ix == 0x7f800000)
			return 0; //__set_errno(EDOM);*/
		return x - x;
	}

	/* argument reduction needed */
	else {
		n = j_rem_pio2f(x, y);
		switch (n & 3) {
		case 0: return  j_k_cosf(y[0], y[1]);
		case 1: return -j_k_sinf(y[0], y[1], 1);
		case 2: return -j_k_cosf(y[0], y[1]);
		default:
		return  j_k_sinf(y[0], y[1], 1);

		}
	}
}