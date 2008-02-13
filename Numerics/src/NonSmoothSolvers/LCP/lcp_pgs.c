/* Siconos-Numerics version 2.1.1, Copyright INRIA 2005-2007.
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
 * Contact: Vincent ACARY vincent.acary@inrialpes.fr
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LA.h"
#include <math.h>
#include "LCP_Solvers.h"

void lcp_pgs(LinearComplementarity_Problem* problem, double *z, double *w, int *info , Solver_Options* options)
{
  /* matrix M/vector q of the lcp */
  double * M = problem->M->matrix0;

  double * q = problem->q;

  /* size of the LCP */
  int n = problem->size;

  int incx = 1, incy = 1;
  int i, iter;
  int itermax = options->iparam[0];

  /*  int incxn = n; */
  double qs, err, den, zi;
  double tol = options->dparam[0];

  /* Allocation */

  double * diag = (double*)malloc(n * sizeof(double));

  /* Initialize output */

  options->iparam[1] = 0;
  options->dparam[1] = 0.0;

  /* Check for non trivial case */
  qs = DNRM2(n , q , incx);

  if (verbose > 0) printf("\n ||q||= %g \n", qs);

  den = 1.0 / qs;

  /* Intialization of w */

  incx = 1;
  incy = 1;
  DCOPY(n , q , incx , w , incy);

  /* Preparation of the diagonal of the inverse matrix */

  for (i = 0 ; i < n ; ++i)
  {
    if (fabs(M[i * n + i]) < 1e-16)
    {

      if (verbose > 0)
      {
        printf(" Vanishing diagonal term \n");
        printf(" The problem cannot be solved with this method \n");
      }

      *info = 2;
      free(diag);

      return;
    }
    else diag[i] = 1.0 / M[i * n + i];
  }

  /*start iterations*/

  iter = 0;
  err  = 1.;

  incx = 1;
  incy = 1;

  DCOPY(n , q , incx , w , incy);

  while ((iter < itermax) && (err > tol))
  {

    ++iter;

    incx = 1;
    incy = 1;

    DCOPY(n , q , incx , w , incy);

    for (i = 0 ; i < n ; ++i)
    {

      incx = n;
      incy = 1;

      z[i] = 0.0;

      zi = -(q[i] + DDOT(n , &M[i] , incx , z , incy)) * diag[i];

      if (zi < 0) z[i] = 0.0;
      else z[i] = zi;

      /* z[i]=fmax(0.0,-( q[i] + ddot_( (integer *)&n , &M[i] , (integer *)&incxn , z , (integer *)&incy ))*diag[i]);*/

    }

    /* **** Criterium convergence **** */

    /*     incx =  1; */
    /*     incy =  1; */

    /*     a1 = 1.0; */
    /*     b1 = 1.0; */

    /*     dgemv_( &NOTRANS , (integer *)&n , (integer *)&n , &a1 , M , (integer *)&n , z , (integer *)&incx , &b1 , w , (integer *)&incy ); */

    /*     qs   = -1.0; */
    /*     daxpy_( (integer *)&n , &qs , w , (integer *)&incx , ww , (integer *)&incy ); */

    /*     num = dnrm2_( (integer *)&n, ww , (integer *)&incx ); */

    /*     err = num*den; */

    /* **** Criterium convergence compliant with filter_result_LCP **** */



    lcp_compute_error(n, M, q, z, verbose, w, &err);

    //err = err ;



    if (verbose == 2)
    {
      printf(" # i%d -- %g : ", iter, err);
      for (i = 0 ; i < n ; ++i) printf(" %g", z[i]);
      for (i = 0 ; i < n ; ++i) printf(" %g", w[i]);
      printf("\n");
    }

    /* **** ********************* **** */

  }

  options->iparam[1] = iter;
  options->dparam[1] = err;

  if (err > tol)
  {
    printf("Siconos/Numerics: lcp_pgs: No convergence of PGS after %d iterations\n" , iter);
    printf("Siconos/Numerics: lcp_pgs: The residue is : %g \n", err);
    *info = 1;
  }
  else
  {
    if (verbose > 0)
    {
      printf("Siconos/Numerics: lcp_pgs: Convergence of PGS after %d iterations\n" , iter);
      printf("Siconos/Numerics: lcp_pgs: The residue is : %g \n", err);
    }
    *info = 0;
  }

  free(diag);

  return;
}
