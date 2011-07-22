/* Siconos-Numerics, Copyright INRIA 2005-2011.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LA.h"
#include <math.h>

void NCP_compute_error(int n, double *vec , double *q , double *z , int verbose, double *w, double *err)
{
  double error, normq;
  double a1, b1;
  int i, incx, incy;
  int param = 1;

  incx = 1;
  incy = 1;
  DCOPY(n , q , incx , w , incy);

  a1 = 1.;
  b1 = 1.;


  // following int param, we recompute the product w = M*z+q
  // The test is then more severe if we compute w because it checks that the linear equation is satisfied

  if (param == 1)
  {
    DGEMV(LA_NOTRANS , n , n , a1 , vec , n , z ,
          incx , b1 , w , incy);
  }



  error = 0.;
  for (i = 0 ; i < n ; i++)
  {
    error += abs(z[i] + w[i]) - (z[i] + w[i]);
  }

  incx  = 1;
  normq = DNRM2(n , q , incx);

  *err = error / normq;

  if (verbose > 0) printf("Siconos/Numerics: NCP_compute_error: Error evaluation = %g \n", *err);
}
