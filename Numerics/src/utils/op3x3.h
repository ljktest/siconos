/* Siconos-Numerics, Copyright INRIA 2005-2010.
 * Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 * Siconos is a free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * Siconos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Siconos; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Contact: Vincent ACARY, siconos-team@lists.gforge.inria.fr
*/

#ifndef _op3x3_h_
#define _op3x3_h_

#include <math.h>
#include <float.h>

/* some convenient macros
 * */

#define DECLARE3X3(NAME)                                  \
  static inline void NAME##3x3(double* a, double* b)      \
 
#define DECLARE3(NAME)                            \
  static inline void NAME##3(double* a, double* b)

#define OP3X3(NAME,OP)                            \
  DECLARE3X3(NAME)                                \
  {                                             \
    *b++ OP *a++;                               \
    *b++ OP *a++;                               \
    *b++ OP *a++;                               \
    *b++ OP *a++;                               \
    *b++ OP *a++;                               \
    *b++ OP *a++;                               \
    *b++ OP *a++;                               \
    *b++ OP *a++;                               \
    *b   OP *a  ;                               \
  };                                            \
 
#define OP3(NAME,OP)                            \
  DECLARE3(NAME)                                \
  {                                             \
    *b++ OP *a++;                               \
    *b++ OP *a++;                               \
    *b   OP *a;                                 \
  };

#if defined(OP3X3_C_STORAGE)
#define _00 0
#define _01 1
#define _02 2
#define _10 3
#define _11 4
#define _12 5
#define _20 6
#define _21 7
#define _22 8
#define SET3X3(V)                             \
  double* V##00 = V++;                        \
  double* V##01 = V++;                        \
  double* V##02 = V++;                        \
  double* V##10 = V++;                        \
  double* V##11 = V++;                        \
  double* V##12 = V++;                        \
  double* V##20 = V++;                        \
  double* V##21 = V++;                        \
  double* V##22 = V;
#else // fortran storage
#define _00 0
#define _10 1
#define _20 2
#define _01 3
#define _11 4
#define _21 5
#define _02 6
#define _12 7
#define _22 8
#define SET3X3(V)                                                       \
  double* V##00 = V++;                                                  \
  double* V##10 = V++;                                                  \
  double* V##20 = V++;                                                  \
  double* V##01 = V++;                                                  \
  double* V##11 = V++;                                                  \
  double* V##21 = V++;                                                  \
  double* V##02 = V++;                                                  \
  double* V##12 = V++;                                                  \
  double* V##22 = V;
#endif


/** copy a 3x3 matrix or a vector[9]
 *\param[in] a[9]
 *\param[out] b[9]*/
OP3X3(cpy, =);

/** add a 3x3 matrix or a vector[9]
 *\param[in] a[9]
 *\param[in,out] b[9]*/
OP3X3(add, +=);

/** sub a 3x3 matrix or a vector[9]
 *\param[in] a[9]
 *\param[in,out] b[9]*/
OP3X3(sub, -=);

/** componentwise multiplication of a vector[9]
 *\param[in] a[9]
 *\param[in,out] b[9]*/
OP3X3(mul, *=);

/** componentwise division of a vector[9]
 *\param[in] a[9]
 *\param[in,out] b[9]*/
OP3X3(div, /=);

/** copy a vector[3]
 *\param[in] a[3]
 *\param[out] b[3]*/
OP3(cpy, =);

/** add a vector[3]
 *\param[in] a[3]
 *\param[in,out] b[3]*/
OP3(add, +=);

/** sub a vector[3]
 *\param[in] a[3]
 *\param[in,out] b[3]*/
OP3(sub, -=);

/** componentwise multiplication of a vector[3]
 *\param[in] a[3]
 *\param[in,out] b[3]*/
OP3(mul, *=);

/** componentwise multiplication of a vector[3]
 *\param[in] a[3]
 *\param[in,out] b[3]*/
OP3(div, /=);

/** matrix vector multiplication
 * \param[in] a[9]
 * \param[in] v[3]
 * \param[out] r[3]
 */
static inline void mv3x3(double* a, double* v, double* r)
{

#if defined(OP3X3_C_STORAGE)
  double* pv;

  pv = v;
  *r++ = *a++ * *pv++ + *a++ * *pv++ + *a++ * *pv++;

  pv = v;
  *r++ = *a++ * *pv++ + *a++ * *pv++ + *a++ * *pv++;

  pv = v;
  *r++ = *a++ * *pv++ + *a++ * *pv++ + *a++ * *pv++;
#else
  double* pr;

  pr = r;

  *pr++ = *a++ * *v;
  *pr++ = *a++ * *v;
  *pr++ = *a++ * *v++;

  pr = r;

  *pr++ += *a++ * *v;
  *pr++ += *a++ * *v;
  *pr++ += *a++ * *v++;

  pr = r;

  *pr++ += *a++ * *v;
  *pr++ += *a++ * *v;
  *pr++ += *a++ * *v++;

#endif

}

/** matrix matrix multiplication : c = a * b
 * \param[in] a[9]
 * \param[in] b[9]
 * \param[out] c[9]
 */
static inline void mm3x3(double* a, double* b, double* c)
{

  SET3X3(a);
  SET3X3(b);
  SET3X3(c);

  *c00 = *a00 * *b00 + *a01 * *b10 + *a02 * *b20;
  *c01 = *a00 * *b01 + *a01 * *b11 + *a02 * *b21;
  *c02 = *a00 * *b02 + *a01 * *b12 + *a02 * *b22;

  *c10 = *a10 * *b00 + *a11 * *b10 + *a12 * *b20;
  *c11 = *a10 * *b01 + *a11 * *b11 + *a12 * *b21;
  *c12 = *a10 * *b02 + *a11 * *b12 + *a12 * *b22;

  *c20 = *a20 * *b00 + *a21 * *b10 + *a22 * *b20;
  *c21 = *a20 * *b01 + *a21 * *b11 + *a22 * *b21;
  *c22 = *a20 * *b02 + *a21 * *b12 + *a22 * *b22;

}

/** add a matrix matrix multiplication : c += a*b
 * \param[in] a[9]
 * \param[in] b[9]
 * \param[out] c[9]
 */
static inline void mmp3x3(double* a, double* b, double* c)
{

  SET3X3(a);
  SET3X3(b);
  SET3X3(c);

  *c00 += *a00 * *b00 + *a01 * *b10 + *a02 * *b20;
  *c01 += *a00 * *b01 + *a01 * *b11 + *a02 * *b21;
  *c02 += *a00 * *b02 + *a01 * *b12 + *a02 * *b22;

  *c10 += *a10 * *b00 + *a11 * *b10 + *a12 * *b20;
  *c11 += *a10 * *b01 + *a11 * *b11 + *a12 * *b21;
  *c12 += *a10 * *b02 + *a11 * *b12 + *a12 * *b22;

  *c20 += *a20 * *b00 + *a21 * *b10 + *a22 * *b20;
  *c21 += *a20 * *b01 + *a21 * *b11 + *a22 * *b21;
  *c22 += *a20 * *b02 + *a21 * *b12 + *a22 * *b22;

}

/** determinant
 * \param[in] double* a
 */
static inline double det3x3(double* a)
{
  SET3X3(a);

  return
    *a00 * *a11 * *a22 + *a01 * *a12 * *a20 + *a02 * *a10 * *a21 -
    *a00 * *a12 * *a21 - *a01 * *a10 * *a22 - *a02 * *a11 * *a20;
}


/** system resolution : x <- sol(Ax = b)
 * \param[in] double* a
 * \param[out] double* x
 * \param[in] double* b
 */
static inline void solv3x3(double* a, double* x, double* b)
{

  SET3X3(a);
  double* b0 = b++;
  double* b1 = b++;
  double* b2 = b;

  double det =
    *a00 * *a11 * *a22 + *a01 * *a12 * *a20 + *a02 * *a10 * *a21 -
    *a00 * *a12 * *a21 - *a01 * *a10 * *a22 - *a02 * *a11 * *a20;

  if (fabs(det) > DBL_EPSILON)
  {
    double idet = 1.0 / det;
    *x++ = idet * (*a01 * *a12 * *b2 +  *a02 * *a21 * *b1 +
                   *a11 * *a22 * *b0 -  *a01 * *a22 * *b1 -
                   *a02 * *a11 * *b2 -  *a12 * *a21 * *b0);
    *x++ = idet * (*a00 * *a22 * *b1 +  *a02 * *a10 * *b2 +
                   *a12 * *a20 * *b0 -  *a00 * *a12 * *b2 -
                   *a02 * *a20 * *b1 -  *a10 * *a22 * *b0);
    *x   = idet * (*a00 * *a11 * *b2 +  *a01 * *a20 * *b1 +
                   *a10 * *a21 * *b0 -  *a00 * *a21 * *b1 -
                   *a01 * *a10 * *b2 -  *a11 * *a20 * *b0);
  }
  else
  {
    *x++ = NAN;
    *x++ = NAN;
    *x   = NAN;
  }
}

/** check equality : a[9] == b[9]
 * \param[in] double a[9]
 * \param[in] double b[9]
 */
static inline int equal3x3(double* a, double* b)
{
  return *a++ == *b++ &&
         *a++ == *b++ &&
         *a++ == *b++ &&
         *a++ == *b++ &&
         *a++ == *b++ &&
         *a++ == *b++ &&
         *a++ == *b++ &&
         *a++ == *b++ &&
         *a   == *b;
}

/** scalar product : c <- a.b
 * \param[in] double a[3]
 * \param[in] double b[3]
 * \param[out] double c[3]
 */
static inline void scal3(double* a, double* b, double* c)
{
  *c++ = *a++ * * b++;
  *c++ = *a++ * * b++;
  *c   = *a   * * b;
};

/** cross product : c <- a x b
 * \param[in] double a[3]
 * \param[in] double b[3]
 * \param[out] double c[3]
 */
static inline void cross3(double* a, double* b, double* c)
{
  double* a0 = a++;
  double* a1 = a++;
  double* a2 = a;
  double* b0 = b++;
  double* b1 = b++;
  double* b2 = b;

  *c++ = *a1 * *b2 - *a2 * *b1;
  *c++ = *a2 * *b0 - *a0 * *b2;
  *c   = *a0 * *b1 - *a1 * *b0;
}


/** norm : || a ||
 * basic computation, may underflow & overflow
 * \param[in] a[3]
 */
static inline double hypot3(double* a)
{
  double r;

  r = *a * *a;
  a++;
  r += *a * *a;
  a++;
  r += *a * *a;
  return sqrt(r);
}

/* check nan of component
 * \param double* a
 */
static inline int isnan3(double* a)
{
  double* a0 = a++;
  double* a1 = a++;
  double* a2 = a;

  return isnan(*a0) || isnan(*a1) || isnan(*a2);
}


/** print a matrix
 * \param double* a
 */

#include <stdio.h>
static inline void print3x3(double* mat)
{
  SET3X3(mat);

  printf("%10.4g ", *mat00);
  printf("%10.4g ", *mat01);
  printf("%10.4g\n", *mat02);

  printf("%10.4g ", *mat10);
  printf("%10.4g ", *mat11);
  printf("%10.4g\n", *mat12);

  printf("%10.4g ", *mat20);
  printf("%10.4g ", *mat21);
  printf("%10.4g\n", *mat22);

}

/** print a vector
 * \param[in] double* v
 */

static inline void print3(double* v)
{
  printf("%10.4g\n", *v++);
  printf("%10.4g\n", *v++);
  printf("%10.4g\n", *v);
}

#endif
