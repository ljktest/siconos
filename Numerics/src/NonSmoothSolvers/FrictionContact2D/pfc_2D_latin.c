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
#include "FrictionContact2D_Solvers.h"

void pfc_2D_latin(FrictionContact_Problem* problem , double *reaction , double *velocity , int *info, Solver_Options* options)
{
  int nc = problem->numberOfContacts;
  double * vec = problem->M->matrix0;
  double *qq = problem->q;
  double * mu = problem->mu;



  int    i, j, kk, iter1, ino, ddl, info2, info77, nrhs;
  int    n = 2 * nc, idim, jdim, nbno, it_end;
  int    incx = 1, incy = 1;
  int    taille, taillet, taillen, itt;
  int    *ddln;
  int    *ddlt, *vectnt;

  double  errmax, alpha, beta, maxa, k_latin, res;
  double  aa, nt, wn, tc, zc0;
  double  err1, num11, err0;
  double  den11, den22, knz0, ktz0, *ktz, *wf;
  double  *wc, *zc, *wt, *maxwt, *wnum1, *znum1;
  double  *zt, *maxzt;

  double  *kn, *kt;

  //  char    trans='T', diag='N';
  //  char    uplo='U', notrans='N';



  double  *k, *DPO, *kf, *kninv;
  double  *kinvwden1, *kzden1, *kfinv, *knz, *wtnc;



  /*                Recup input                    */


  itt     = options->iparam[0];
  errmax  = options->dparam[0];
  k_latin = options->dparam[2];

  /*               Initialize output                */


  options->iparam[1] = 0;
  options->dparam[1] = 0.0;


  /*               Allocations                      */

  k         = (double*) malloc(n * n * sizeof(double));
  DPO       = (double*) malloc(n * n * sizeof(double));
  kf        = (double*) malloc(n * n * sizeof(double));
  kfinv     = (double*) malloc(n * n * sizeof(double));

  kninv     = (double*) malloc(nc * nc * sizeof(double));
  kn        = (double*) malloc(nc * nc * sizeof(double));
  kt        = (double*) malloc(nc * nc * sizeof(double));

  kinvwden1 = (double*) malloc(n  * sizeof(double));
  kzden1    = (double*) malloc(n  * sizeof(double));
  wc        = (double*) malloc(n  * sizeof(double));
  zc        = (double*) malloc(n  * sizeof(double));
  znum1     = (double*) malloc(n  * sizeof(double));
  wnum1     = (double*) malloc(n  * sizeof(double));
  wt        = (double*) malloc(n  * sizeof(double));
  maxzt     = (double*) malloc(n  * sizeof(double));



  knz       = (double*) malloc(nc * sizeof(double));
  wtnc      = (double*) malloc(nc * sizeof(double));
  ktz       = (double*) malloc(nc * sizeof(double));
  wf        = (double*) malloc(nc * sizeof(double));
  maxwt     = (double*) malloc(nc * sizeof(double));
  zt        = (double*) malloc(nc * sizeof(double));


  vectnt    = (int*) malloc(n * sizeof(int));

  ddln      = (int*) malloc(nc * sizeof(int));
  ddlt      = (int*) malloc(nc * sizeof(int));

  /*                    Initialization                   */



  for (i = 0; i < n * n; i++)
  {
    k[i]     = 0.;
    kf[i]    = 0.;
    kfinv[i] = 0.;

    if (i < nc * nc)
    {

      kn[i]    = 0.0;
      kt[i]    = 0.0;
      kninv[i] = 0.0;


      if (i < n)
      {
        wc[i]    = 0.0;
        zc[i]    = 0.;
        reaction[i]     = 0.;
        velocity[i]     = 0.;
        znum1[i] = 0.;
        wnum1[i] = 0.;
        wt[i]    = 0.;
        maxzt[i] = 0.;

        if (i < nc)
        {
          maxwt[i] = 0.;
          zt[i]    = 0.;
          knz[i]   = 0.;
          ktz[i]   = 0.;
          wf[i]    = 0.;
          wtnc[i]  = 0.;
        }

      }

    }
  }


  for (i = 0; i < n; i++)
  {

    if (fabs(vec[i * n + i]) < 1e-16)
    {

      if (verbose > 0)
        printf("\n Warning nul diagonal term in M matrix \n");

      free(k);
      free(DPO);
      free(kf);
      free(kfinv);
      free(kninv);
      free(kn);
      free(kt);
      free(kinvwden1);
      free(kzden1);
      free(wc);
      free(zc);
      free(znum1);
      free(wnum1);
      free(wt);
      free(maxzt);
      free(knz);
      free(wtnc);
      free(ktz);
      free(wf);
      free(maxwt);
      free(zt);
      free(vectnt);
      free(ddln);
      free(ddlt);

      *info = 3;

      return;


    }
    else
    {

      k[i + n * i] = k_latin / vec[i * n + i];
      vectnt[i] = i + 1;

    }

  }


  for (i = 0; i < nc; i++)
  {
    ddln[i] = vectnt[2 * i];
    if (i != 0) ddlt[i] = vectnt[2 * i - 1];
    else ddlt[i] = 0;

  }


  for (i = 0; i < nc; i++)
  {
    kn[i + nc * i] = k[ddln[i] + n * ddln[i]];
    kt[i + nc * i] = k[ddlt[i] + n * ddlt[i]];
  }




  taillen = sizeof(ddln) / sizeof(ddln[0]);
  taillet = sizeof(ddlt) / sizeof(ddlt[0]);

  idim = 1 +  taillen / taillet;
  jdim = idim - 1;



  taille = 0;
  for (i = 0; i < n; i++)
    taille = sizeof(qq[i]) + taille;

  taille = taille / sizeof(qq[0]);
  nbno = taille / idim;


  for (i = 0; i < nc; i++)
  {
    kf[ddln[i] + n * ddln[i]] = kn[i + nc * i];
    kf[ddlt[i] + n * ddlt[i]] = kt[i + nc * i];
  }


  for (i = 0; i < n; i++)
  {
    kfinv[i + n * i] = 1. / kf[i + n * i];

    if (i < nc)
      kninv[i + nc * i] = 1. / kt[i + nc * i];

  }


  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      DPO[i + n * j] = vec[j * n + i] + kfinv[i + n * j];



  DPOTRF(LA_UP, n, DPO , n, info2);

  if (info2 != 0)
  {
    if (verbose > 0)
      printf("\n Matter with Cholesky factorization \n");

    free(k);
    free(DPO);
    free(kf);
    free(kfinv);
    free(kninv);
    free(kn);
    free(kt);
    free(kinvwden1);
    free(kzden1);
    free(wc);
    free(zc);
    free(znum1);
    free(wnum1);
    free(wt);
    free(maxzt);
    free(knz);
    free(wtnc);
    free(ktz);
    free(wf);
    free(maxwt);
    free(zt);
    free(vectnt);
    free(ddln);
    free(ddlt);

    *info = 2;
    return;
  }

  /*                Iteration loops                  */


  iter1 = 0;
  err1  = 1.;



  while ((iter1 < itt) && (err1 > errmax))
  {

    /*               Linear stage (zc,wc) -> (z,w)         */

    alpha  = 1.;
    beta   = 1.;
    DGEMV(LA_NOTRANS, n, n, alpha, kfinv, n, zc, incx, beta, wc, incy);

    DCOPY(n, qq, incx, znum1, incy);

    alpha = -1.;
    DSCAL(n , alpha , znum1 , incx);

    alpha = 1.;
    DAXPY(n, alpha, wc, incx, znum1, incy);

    nrhs = 1;
    DTRTRS(LA_UP, LA_TRANS, LA_NONUNIT, n, nrhs, DPO, n, znum1, n, info77);

    DTRTRS(LA_UP, LA_NOTRANS, LA_NONUNIT, n, nrhs, DPO, n, znum1, n, info77);

    DCOPY(n, znum1, incx, reaction, incy);

    alpha = -1.;
    beta = 1.;
    DGEMV(LA_NOTRANS, n, n, alpha, kfinv, n, reaction, incx, beta, wc, incy);

    DCOPY(n, wc, incx, velocity, incy);



    /*               Local stage (z,w)->(zc,wc)          */


    for (i = 0; i < n; i++)
    {
      zc[i] = 0.;
      wc[i] = 0.0;
    }


    /*           Normal party                           */



    for (i = 0; i < nc; i++)
    {
      knz0 = 0.;
      for (kk = 0; kk < nc; kk++)
      {
        knz[i] = kt[i + nc * kk] * velocity[ddlt[kk]] + knz0;
        knz0 = knz[i];
      }

      zt[i] = reaction[ddlt[i]] - knz[i];

      if (zt[i] > 0.0)
      {
        zc[ddlt[i]] = zt[i];
        maxzt[i] = 0.0;
      }
      else
      {
        zc[ddlt[i]] = 0.0;
        maxzt[i] = -zt[i];
      }
    }

    for (i = 0; i < nc; i++)
    {
      zc0 = 0.;
      ktz0 = 0.;
      for (j = 0; j < nc; j++)
      {
        wc[ddlt[i]] = kninv[i + nc * j] * maxzt[j] + zc0;
        zc0 = wc[ddlt[i]];
        ktz[i] = kn[i + nc * j] * velocity[ddln[j]] + ktz0;
        ktz0 =  ktz[i];
      }
      wf[i] = reaction[ddln[i]] - ktz[i];
    }


    /*             Loop other nodes              */


    for (ino = 0; ino < nbno; ino++)
    {
      ddl  = ddln[ino];
      nt   = fabs(wf[ino]);


      /*          Tangential vector              */



      if (nt < 1.e-8)  tc = 0.;
      else tc = wf[ino] / nt;



      /*               Tangentiel component             */


      wn = zc[ddlt[ino]];

      aa = nt - mu[ino] * wn;

      if (aa > 0.0)
      {
        maxa = aa;
      }
      else
      {
        maxa = 0.0;
      }

      wc[ddl] = (maxa / (-1 * kn[ino + nc * ino])) * tc;

      aa = -nt + mu[ino] * wn;

      if (aa > 0.0)
      {
        maxa = aa;
      }
      else
      {
        maxa = 0.0;
      }

      zc[ddl] = (mu[ino] * wn - maxa) * tc;

    }

    /*               Convergence criterium                */



    DCOPY(n, reaction, incx, znum1, incy);

    alpha = -1.;
    DAXPY(n, alpha, zc, incx, znum1, incy);

    DCOPY(n, velocity, incx, wnum1, incy);

    DAXPY(n, alpha, wc, incx, wnum1, incy);

    alpha  = 1.;
    beta   = 1.;
    DGEMV(LA_NOTRANS, n, n, alpha, kf, n, wnum1, incx, beta, znum1, incy);

    num11  = 0.;
    alpha  = 1.;
    beta = 0.;
    DGEMV(LA_NOTRANS, n, n, alpha, kfinv, n, znum1, incx, beta, wnum1, incy);

    num11 = DDOT(n, wnum1, incx, znum1, incy);

    DCOPY(n, reaction, incx, znum1, incy);

    alpha  = 1.;
    beta   = 1.;
    DGEMV(LA_NOTRANS, n, n, alpha, kf, n, velocity, incx, beta, znum1, incy);

    alpha  = 1.;
    beta   = 0.;
    DGEMV(LA_NOTRANS, n, n, alpha, kfinv, n, znum1, incx, beta, wnum1, incy);

    den11  = DDOT(n, wnum1, incx, znum1, incy);

    DCOPY(n, zc, incx, znum1, incy);

    alpha  = 1.;
    beta   = 1.;
    DGEMV(LA_NOTRANS, n, n, alpha, kf, n, wc, incx, beta, znum1, incy);

    alpha  = 1.;
    beta   = 0.;
    DGEMV(LA_NOTRANS, n, n, alpha, kfinv, n, znum1, incx, beta, wnum1, incy);

    den22  = DDOT(n, znum1, incx, wnum1, incy);

    err0   = num11 / (den11 + den22);

    err1   = sqrt(err0);

    it_end = iter1;
    res    = err1;

    options->iparam[1] = iter1;
    options->dparam[1] = err1;

    iter1   = iter1 + 1;


  }


  if (err1 > errmax)
  {

    if (verbose > 0)
      printf("No convergence after %d iterations, the residue is %g\n", iter1, err1);

    *info = 1;
  }
  else
  {

    if (verbose > 0)
      printf("Convergence after %d iterations, the residue is %g \n", iter1, err1);

    *info = 0;
  }

  free(k);
  free(DPO);
  free(kf);
  free(kfinv);
  free(kninv);
  free(kn);
  free(kt);
  free(kinvwden1);
  free(kzden1);
  free(wc);
  free(zc);
  free(znum1);
  free(wnum1);
  free(wt);
  free(maxzt);
  free(knz);
  free(wtnc);
  free(ktz);
  free(wf);
  free(maxwt);
  free(zt);
  free(vectnt);
  free(ddln);
  free(ddlt);



}
