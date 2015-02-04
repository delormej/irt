/* Hacked and copied from:
 *  https://sourceware.org/git/?p=glibc.git;a=tree;f=sysdeps/ieee754/flt-32;hb=4ffffbd272264f083f35783ed81339304025f855
 *
 * Temporary solution in order to save space since using the real trig functions bloats the code way too much
 * to be used.
 */

#include "math_private.h"

float j_fabsf(float x)
{
	u_int32_t ix;
	GET_FLOAT_WORD(ix, x);
	SET_FLOAT_WORD(x, ix & 0x7fffffff);
	return x;
}
