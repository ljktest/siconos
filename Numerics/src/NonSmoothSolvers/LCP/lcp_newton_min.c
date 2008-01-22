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
#include <math.h>
#include "LA.h"

int lcp_compute_error(int n, double *vec , double *q , double *z , int verbose, double *w, double *err);

void lcp_newton_min(int *nn , double *vec , double *q , double *z , double *w , int *info , int *iparamLCP , double *dparamLCP)
{


  int i, j, iter;
  int n = *nn, m, mm, k;
  int itermax, verbose;

  int  incx, incy;
  double err, tol, a1, b1;
  double alpha;
  int infoDGESV;

  int *ipiv;

  double *JacH, *H, *A;

  double *rho;

  incx = 1;
  incy = 1;
  /*input*/

  itermax = iparamLCP[0];
  verbose  = iparamLCP[1];

  tol   = dparamLCP[0];

  /*output*/

  iparamLCP[2] = 0;
  dparamLCP[1] = 0.0;

  for (i = 0; i < n; i++)
  {
    z[i] = 1.0;
    w[i] = 1.0;
  }

  /* rho*/
  rho = (double *)malloc(n * sizeof(double));
  for (i = 0; i < n; i++) rho[i] = 1.0 / vec[i * n + i] ;
  /* /for (i=0;i<n;i++) rho[i]=1.0/n ;
  // Sizw of the problem*/
  m = 2 * n;
  mm = m * m;
  /* / Creation of the gradient of the function H*/

  JacH = (double *)malloc(m * m * sizeof(double));
  A   = (double *)malloc(m * m * sizeof(double));

  for (j = 0; j < n; j++)
  {
    for (i = 0; i < n; i++) JacH[j * m + i] = -vec[j * n + i]; /* / should be replaced by a tricky use of BLAS*/
  }
  for (j = n; j < m; j++)
  {
    for (i = 0; i < n; i++) JacH[j * m + i] = 0.0;
    JacH[j * m + j - n] = 1.0;
  }
  for (j = 0; j < m; j++)
  {
    for (i = n; i < m; i++) JacH[j * m + i] = 0.0;
  }


  /* / Creation of the RHS H, */
  H = (double *)malloc(m * sizeof(double));
  /* / Construction of the RHS*/
  a1 = -1.;
  b1 = -1.;
  /* / q --> H*/
  DCOPY(n , q , incx , H , incy);
  /* / -Mz-q --> H*/
  DGEMV(LA_NOTRANS , n , n , a1 , vec , n , z , incx , b1 , H , incy);
  /* / w+H --> H*/
  alpha = 1.0;
  DAXPY(n , alpha , w , incx , H , incy);     /* / c'est faux*/


  for (i = n; i < m; i++)
  {
    if (w[i - n] > rho[i - n]*z[i - n]) H[i] = rho[i - n] * z[i - n];
    else H[i] = w[i - n];
  }


  ipiv = (int *)malloc(m * sizeof(int));



  iter = 0;
  err  = 1.;



  while ((iter < itermax) && (err > tol))
  {
    ++iter;
    /* / Construction of the directional derivatives of H, JacH*/
    for (i = 0; i < n; i++)
    {
      if (w[i] > rho[i]*z[i])
      {
        JacH[i * m + i + n] = rho[i];
        JacH[(i + n)*m + (i + n)] = 0.0;
      }
      else
      {
        JacH[i * m + i + n] = 0.0;
        JacH[(i + n)*m + (i + n)] = 1.0;
      }
    }


    /* / Computation of the element of the subgradient.*/

    DCOPY(mm , JacH , incx , A , incy);
    k = 1;
    DGESV(m , k , A , m , ipiv , H , m , infoDGESV);

    if (infoDGESV)
    {
      if (verbose > 0)
      {
        printf("Problem in DGESV\n");
      }
      iparamLCP[2] = iter;
      dparamLCP[1] = err;

      free(H);
      free(A);
      free(JacH);
      free(ipiv);
      free(rho);
      *info = 2;
      return;

    }


    /* / iteration*/
    alpha = -1.0;
    DAXPY(n , alpha , H , incx , z , incy);      /* /  z-H --> z*/
    DAXPY(n , alpha , &H[n] , incx , w , incy);  /* /  w-H --> w*/

    /* / Construction of the RHS for the next iterate and for the error evalutaion*/
    a1 = 1.;
    b1 = 1.;
    DCOPY(n , q , incx , H , incy);                                         /* / q --> H*/
    DGEMV(LA_NOTRANS , n , n , a1 , vec , n , z , incx , b1 , H , incy);  /* / Mz+q --> H*/
    alpha = -1.0;
    DAXPY(n , alpha , w , incx , H , incy);                                /* / w-Mz-q --> H*/

    for (i = n; i < m; i++)
    {
      if (w[i - n] > rho[i - n]*z[i - n]) H[i] = rho[i - n] * z[i - n];
      else H[i] = w[i - n];
    }

    /* / Error Evaluation*/

    // err = dnrm2_( (integer *)&m , H , &incx ); // necessary ?
    lcp_compute_error(n, vec, q, z, verbose, w, &err);


  }

  iparamLCP[2] = iter;
  dparamLCP[1] = err;


  if (err > tol)
  {
    printf("Siconos/Numerics: lcp_newton_min: No convergence of NEWTON_MIN after %d iterations\n" , iter);
    printf("Siconos/Numerics: lcp_newton_min: The residue is : %g \n", err);
    *info = 1;
  }
  else
  {
    if (verbose > 0)
    {
      printf("Siconos/Numerics: lcp_newton_min: Convergence of NEWTON_MIN after %d iterations\n" , iter);
      printf("Siconos/Numerics: lcp_newton_min: The residue is : %g \n", err);
    }
    *info = 0;
  }

  free(H);
  free(A);
  free(JacH);
  free(ipiv);
  free(rho);

}
