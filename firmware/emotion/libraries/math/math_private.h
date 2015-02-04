/* Hacked and copied from:
 *  https://sourceware.org/git/?p=glibc.git;a=tree;f=sysdeps/ieee754/flt-32;hb=4ffffbd272264f083f35783ed81339304025f855
 *
 * Temporary solution in order to save space since using the real trig functions bloats the code way too much
 * to be used.
 */

#ifndef _MATH_PRIVATE_H_
#define	_MATH_PRIVATE_H_

#define u_int32_t uint32_t

#include <stdint.h>
#include <float.h>

/*
 * A union which permits us to convert between a float and a 32 bit
 * int.
 */

typedef union
{
  float value;
  /* FIXME: Assumes 32 bit int.  */
  unsigned int word;
} ieee_float_shape_type;

/* Get a 32 bit int from a float.  */

#define GET_FLOAT_WORD(i,d)					\
do {								\
  ieee_float_shape_type gf_u;					\
  gf_u.value = (d);						\
  (i) = gf_u.word;						\
} while (0)

/* Set a float from a 32 bit int.  */

#define SET_FLOAT_WORD(d,i)					\
do {								\
  ieee_float_shape_type sf_u;					\
  sf_u.word = (i);						\
  (d) = sf_u.value;						\
} while (0)

int32_t j_rem_pio2f(float x, float *y);

float j_k_sinf(float x, float y, int iy);
float j_k_cosf(float x, float y);

float j_acosf(float x);
float j_sqrtf(float x);
float j_sinf(float x);
float j_cosf(float x);
float j_fabsf(float x);
float j_powf(float x, float y);

#endif /* !_MATH_PRIVATE_H_ */
