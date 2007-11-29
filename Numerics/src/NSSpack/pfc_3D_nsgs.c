/* Siconos-Numerics version 2.0.1, Copyright INRIA 2005-2007.
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
/*!\file pfc_3D_nsgs.c
 *
 * This subroutine allows the primal resolution of contact problems with friction.\n
 *
 *   Try \f$(z,w)\f$ such that:\n
 *   \f$
 *    \left\lbrace
 *     \begin{array}{l}
 *      M z + q = w\\
 *      0 \le z_n \perp w_n \ge 0\\
 *      -w_t \in \partial\psi_{[-\mu z_n, \mu z_n]}(z_t)\\
 *     \end{array}
 *    \right.
 *   \f$
 *
 * here M is an n by n  matrix, q an n-dimensional vector, z an n-dimensional  vector and w an  * n-dimensional vector.
 *
 * \fn  pfc_3D_nsgs( int *nn , double *vec , double *q , double *z , double *w ,
 *                         int *info\n, int *iparamLCP , double *dparamLCP )
 *
 * Generic pfc_3D parameters:\n
 *
 * \param nn      Unchanged parameter which represents the dimension of the system.
 * \param vec     Unchanged parameter which contains the components of the matrix with a fortran storage.
 * \param q       Unchanged parameter which contains the components of the right hand side vector.
 * \param z       Modified parameter which contains the initial solution and returns the solution of the problem.
 * \param w       Modified parameter which returns the solution of the problem.
 * \param info    Modified parameter which returns the termination value\n
 *                0 - convergence\n
 *                1 - iter = itermax\n
 *                2 - negative diagonal term\n
 *
 * Specific NLGS parameters:\n
 *
 * \param iparamLCP[0] = itermax Input unchanged parameter which represents the maximum number of iterations allowed.
 * \param iparamLCP[1] = ispeak  Input unchanged parameter which represents the output log identifiant\n
 *                       0 - no output\n
 *                       0 < active screen output\n
 * \param iparamLCP[2] = it_end  Output modified parameter which returns the number of iterations performed by the algorithm.
 *
 * \param dparamLCP[0] = mu      Input unchanged parameter which represents the friction coefficient.
 * \param dparamLCP[1] = tol     Input unchanged parameter which represents the tolerance required.
 * \param dparamLCP[2] = res     Output modified parameter which returns the final error value.
 *
 *
 * \param dparamLCP[0] = local_tol         Input unchanged parameter which represents the local tolerance required for Newton Method used to solve local Problem.
 *
 * \param iparamLCP[0] = lical_itermax     Input unchanged parameter which represents the maximum number of iterations allowed for Newton method.
 * \param iparamLCP[3] = local_formulation Formulaton of frictional contact problem (Alart-Curnier: 0 and Fischer-Burmeister: 1)
 * \param iparamLCP[4] = local_solver      Solution method used to solve the frictional contact problem (projection: 0 and Newton: 1)
 *
 * Note that the problem is solved with the Non-Smooth Gauss-Seidel method
 *
 * Important: The projection method is done only with Alart-Curnier formulation
 *
 * In progress: The Fischer-Burmeister formulation is nore ready to be used and is in progress
 *
 * This formulation is based on the idea of C. Glocker: Formulation of spatial contact situations
 * in rigid multibodiy systems, Comp. Meth. Appl. Mech. Engrg, 177 (1999), 199-214.
 *
 * Last modification 27/11/2007, H. Khenous
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "LA.h"
#include <time.h>
#include <NSSpack.h>
#include <pfc_3D_Alart_Curnier.h>
#include <pfc_3D_Fischer_Burmeister.h>

void (*pfc_3D_local_solver)(int n , double *C , double *zzz , double *zz , double *ww , double mu , pfc3D_fPtr(*Compute_G), pfc3D_fPtr(*Compute_JacG), double *param1, double *param2, double *param3, int *iparam_local , double *dparam_local) = NULL;

void pfc_3D_nsgs(int *nn , double *vec , double *q , double *z , double *w , int *info, int *iparamLCP , double *dparamLCP)
{

  FILE *f101;

  int n, in, it, is, ispeak, itermax, nc, i, j, ii, iter, Gsize;
  double err, tol, mu;
  double qs, a1, den, num;
  int incx, incy;
  double *W, *C, *ww, *zz, *zzz;

  pfc3D_fPtr Compute_G;
  pfc3D_fPtr Compute_JacG;

  int nb = 5;
  int     iparam_local[nb];
  double  dparam_local[nb];

  //  clock_t t1,t2;

  for (i = 0 ; i < nb ; ++i) iparam_local[i] = 0;
  for (i = 0 ; i < nb ; ++i) dparam_local[i] = 0.0;




  ispeak = 1;
  nc     = *nn;
  incx   = 1;
  incy   = 1;
  n      = 3 * nc;

  /* Recup input */

  itermax = iparamLCP[0];
  ispeak  = iparamLCP[1];

  mu  = dparamLCP[0];
  tol = dparamLCP[1];

  /* Initialize output */

  iparamLCP[2] = 0;
  dparamLCP[2] = 0.0;

  dparam_local[0] = tol;  //dparamLCP[3]; /* local tolerance */
  dparam_local[1] = dparamLCP[4]; /* local error     */

  iparam_local[0] = itermax; //iparamLCP[3]; /* local itermax   */
  iparam_local[1] = iparamLCP[4]; /* local iteration */

  iparam_local[3] = 0; //iparamLCP[5]; /* local formulation */
  iparam_local[4] = 1; //iparamLCP[6]; /* local solver      */

  if (ispeak == 2) f101 = fopen("pfc_3D_nlgs.log" , "w+");

  if (iparam_local[4] == 0)
  {
    /* NSGS with projection is done only for Alart-Curnier formulation, then if local_solver = 1 then local_formulation = 0 */
    iparam_local[3] = 0;
    pfc_3D_local_solver = &pfc_3D_projection;
    Gsize  = 3;
  }
  else
  {
    if (iparam_local[3] == 0)
    {
      Gsize  = 3;
      Compute_G    = &Compute_G_AC;
      Compute_JacG = &Compute_JacG_AC;
      pfc_3D_local_solver = &pfc_3D_newton;
    }

    else
    {
      Gsize  = 5;
      Compute_G    = &Compute_G_FB;
      Compute_JacG = &Compute_JacG_FB;
      pfc_3D_local_solver = &pfc_3D_newton;
    }
  }

  /* Allocation */

  C    = (double*)malloc(3 * 3 * sizeof(double));
  W    = (double*)malloc(n * sizeof(double));
  ww   = (double*)malloc(Gsize * sizeof(double));
  zz   = (double*)malloc(Gsize * sizeof(double));
  zzz  = (double*)malloc(Gsize * sizeof(double));

  /* Intialization */
  for (i = 0 ; i < Gsize ; ++i)
  {
    ww[i] =  zz[i] = zzz[i] = 0.;
  }

  for (i = 0 ; i < 3 ; ++i)
    for (j = 0 ; j < 3 ; ++j)
      C[j * 3 + i] = 0.;


  /* Check for non trivial case */

  qs = DNRM2(n , q , incx);

  if (ispeak > 0) printf("\n ||q||= %g \n" , qs);

  if (qs > 1e-16) den = 1.0 / qs;
  else
  {
    for (i = 0 ; i < n ; ++i)
    {
      w[i] = q[i];
      z[i] = 0.;
    }

    free(W);
    free(C);
    free(ww);
    free(zz);
    free(zzz);

    *info = 0;
    return;
  }

  /* Intialization of w*/
  for (i = 0 ; i < n ; ++i)
  {
    W[i] = 0.;
    w[i] = 0.;
  }

  /*start NSGS iterations*/
  iter = 0;
  err  = 1.;

  incx = 1;
  incy = 1;

  DCOPY(n , q , incx , w , incy);

  printf("---------------- number of contact points =  %i -----------------\n", nc);

  while ((iter < itermax) && (err > tol))
  {

    ++iter;
    /*    printf("---------------- Iteration %i -----------------\n",iter); */

    incx = 1;
    incy = 1;

    DCOPY(n , w , incx , W , incy);
    DCOPY(n , q , incx , w , incy);

    for (i = 0 ; i < nc ; ++i)
    {
      /* printf("---------------- contact point %i -----------------\n",i);  */

      in = 3 * i;
      it = 3 * i + 1;
      is = 3 * i + 2;

      C[0 * 3 + 0] = vec[(in) * n + in];
      C[0 * 3 + 1] = vec[(in) * n + it];
      C[0 * 3 + 2] = vec[(in) * n + is];
      C[1 * 3 + 0] = vec[(it) * n + in];
      C[1 * 3 + 1] = vec[(it) * n + it];
      C[1 * 3 + 2] = vec[(it) * n + is];
      C[2 * 3 + 0] = vec[(is) * n + in];
      C[2 * 3 + 1] = vec[(is) * n + it];
      C[2 * 3 + 2] = vec[(is) * n + is];

      if (iparam_local[3] == 0)
      {

        for (j = 0 ; j < Gsize ; ++j)
          zz[j] = z[Gsize * i + j];

        incx = n;
        incy = 1;

        z[in] = 0.0;
        z[it] = 0.0;
        z[is] = 0.0;

        zzz[0] = q[in] + DDOT(n , &vec[in] , incx , z , incy);
        zzz[1] = q[it] + DDOT(n , &vec[it] , incx , z , incy);
        zzz[2] = q[is] + DDOT(n , &vec[is] , incx , z , incy);

        double *param1, *param2, *param3;

        param1  = (double*)malloc(Gsize * sizeof(double));
        param2  = (double*)malloc(Gsize * sizeof(double));
        param3  = (double*)malloc(Gsize * sizeof(double));

        /* Intialization of G, JacG, ww and www */
        for (j = 0 ; j < Gsize ; ++j)
        {
          param1[j] = param2[j] = param3[j] = 0.;
        }

        (*pfc_3D_local_solver)(Gsize , C , zzz , zz , ww , mu ,  &Compute_G, &Compute_JacG, param1, param2, param3, iparam_local , dparam_local);


        z[in] = zz[0];
        z[it] = zz[1];
        z[is] = zz[2];

        free(param1);
        free(param2);
        free(param3);
      }
      else
      {

        double *IP, *Ip, *I3;

        IP = (double*)malloc(2 * sizeof(double));
        Ip = (double*)malloc(2 * 2 * sizeof(double));
        I3 = (double*)malloc(2 * sizeof(double));

        for (ii = 0 ; ii < 2 ; ++ii)
        {
          IP[ii] = I3[ii] = 0.;
          for (j = 0 ; j < 2 ; ++j)
            Ip[j * 2 + ii] = 0.;
        }


        /* Ip = matrix_inv2(Ip`)*/
        /* IP = matrix_inv2(Ip`)*mup */
        /* I3 = matrix_inv2(Ip)*e3 */

        IP[0] = 0.;
        IP[1] = 2.*mu;

        I3[0] = -1.;
        I3[1] = -1.;

        Ip[0 * 2 + 0] =  1. / sqrt(3.);
        Ip[1 * 2 + 0] = -1. / sqrt(3.);
        Ip[0 * 2 + 1] =  1.;
        Ip[1 * 2 + 1] =  1.;


        zz[0] = z[3 * i + 0];
        zz[1] = mu * z[3 * i + 0] - sqrt(3) * z[3 * i + 1] / 2. - z[3 * i + 2] / 2.;
        zz[2] = mu * z[3 * i + 0] + sqrt(3) * z[3 * i + 1] / 2. - z[3 * i + 2] / 2.;
        zz[3] = 0.;
        zz[4] = 0.;


        incx = n;
        incy = 1;

        z[in] = 0.0;
        z[it] = 0.0;
        z[is] = 0.0;

        zzz[0] = q[in] + DDOT(n , &vec[in] , incx , z , incy);
        zzz[1] = -(Ip[0 * 2 + 0] * (q[it] + DDOT(n , &vec[it] , incx , z , incy)) + Ip[0 * 2 + 1] * (q[is] + DDOT(n , &vec[is] , incx , z , incy)));
        zzz[2] = -(Ip[1 * 2 + 0] * (q[it] + DDOT(n , &vec[it] , incx , z , incy)) + Ip[1 * 2 + 1] * (q[is] + DDOT(n , &vec[is] , incx , z , incy)));

        (*pfc_3D_local_solver)(Gsize , C , zzz , zz , ww , mu , &Compute_G, &Compute_JacG, Ip, IP, I3, iparam_local , dparam_local);


        z[in] = zz[0];
        z[it] = Ip[0 * 2 + 0] * (mu * zz[0] - zz[1]) + Ip[1 * 2 + 0] * (mu * zz[0] - zz[2]);
        z[is] = Ip[0 * 2 + 1] * (mu * zz[0] - zz[1]) + Ip[1 * 2 + 1] * (mu * zz[0] - zz[2]);

        /*  w[in] = ww[0]; */
        /*  w[it] = -(sqrt(3)*ww[1]/2. - sqrt(3)*ww[2]/2. + 2*z[it]*ww[4]); */
        /*  w[is] = -(ww[1]/2. + ww[2]/2. - ww[3] + 2*z[is]*ww[4]); */

        free(IP);
        free(Ip);
        free(I3);

      }
    }

    /* **** Criterium convergence **** */

    NCP_compute_error(n , vec , q , z , ispeak , w , &err);

    //lcp_compute_error( n , vec , q , z , ispeak , w , &err);

    printf("-----------------------------------Iteration %i Erreur NSGS = %14.7e\n", iter, err);
  }

  iparamLCP[2] = iter;
  dparamLCP[2] = err;

  if (ispeak > 0)
  {
    if (err > tol)
    {
      printf(" No convergence of NSGS after %i iterations\n" , iter);
      printf(" The residue is : %e \n", err);
      *info = 1;
    }
    else
    {
      printf(" Convergence of NSGS after %i iterations\n" , iter);
      printf(" The residue is : %e \n", err);
      *info = 0;
    }
  }
  else
  {
    if (err > tol) *info = 1;
    else *info = 0;
  }

  free(W);
  free(C);
  free(ww);
  free(zz);
  free(zzz);

  if (ispeak == 2) fclose(f101);

}
