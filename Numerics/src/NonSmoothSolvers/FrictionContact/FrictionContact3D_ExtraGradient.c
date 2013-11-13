/* Siconos-Numerics, Copyright INRIA 2005-2012.
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
#include "projectionOnCone.h"
#include "FrictionContact3D_Solvers.h"
#include "FrictionContact3D_compute_error.h"
#include "SiconosBlas.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void frictionContact3D_ExtraGradient(FrictionContactProblem* problem, double *reaction, double *velocity, int* info, SolverOptions* options)
{
  /* int and double parameters */
  int* iparam = options->iparam;
  double* dparam = options->dparam;
  /* Number of contacts */
  int nc = problem->numberOfContacts;
  double* q = problem->q;
  NumericsMatrix* M = problem->M;
  double* mu = problem->mu;
  /* Dimension of the problem */
  int n = 3 * nc;
  /* Maximum number of iterations */
  int itermax = iparam[0];
  /* Tolerance */
  double tolerance = dparam[0];





  /*****  Fixed point iterations *****/
  int iter = 0; /* Current iteration number */
  double error = 1.; /* Current error */
  int hasNotConverged = 1;
  int contact; /* Number of the current row of blocks in M */
  int nLocal = 3;
  dparam[0] = dparam[2]; // set the tolerance for the local solver


  double * velocitytmp = (double *)malloc(n * sizeof(double));
  double * reactiontmp = (double *)malloc(n * sizeof(double));

  double rho = 0.0, rho_k =0.0;
  int isVariable = 0;

  if (dparam[3] > 0.0)
  {
    rho = dparam[3];
  }
  else
  {
    /* Variable step in fixed*/
    isVariable = 1;
    printf("Extragradient method. Variable stepsize with line--search.\n");
    rho = -dparam[3];
  }

  double alpha = 1.0;
  double beta = 1.0;

  /* Variable for Line_search */
  int success =0;
  double error_k;
  int ls_iter = 0;
  int ls_itermax = 10;
  double tau=0.6, L= 0.9, Lmin =0.3, taumin=0.7;
  double a1=0.0, a2=0.0;
  double * reaction_k =0;
  double * velocity_k =0;

  if (isVariable)
  {
    reaction_k = (double *)malloc(n * sizeof(double));
    velocity_k = (double *)malloc(n * sizeof(double));
  }

  if (!isVariable)
  {
    /*   double minusrho  = -1.0*rho; */
    while ((iter < itermax) && (hasNotConverged > 0))
    {
      ++iter;

      /* reactiontmp <- reaction  */
      cblas_dcopy(n , reaction , 1 , reactiontmp, 1);

      /* velocitytmp <- q  */
      cblas_dcopy(n , q , 1 , velocitytmp, 1);


      prodNumericsMatrix(n, n, alpha, M, reactiontmp, beta, velocitytmp);
      // projection for each contact
      for (contact = 0 ; contact < nc ; ++contact)
      {
        int pos = contact * nLocal;
        double  normUT = sqrt(velocitytmp[pos + 1] * velocitytmp[pos + 1] + velocitytmp[pos + 2] * velocitytmp[pos + 2]);
        reactiontmp[pos] -= rho * (velocitytmp[pos] + mu[contact] * normUT);
        reactiontmp[pos + 1] -= rho * velocitytmp[pos + 1];
        reactiontmp[pos + 2] -= rho * velocitytmp[pos + 2];
        projectionOnCone(&reactiontmp[pos], mu[contact]);
      }
      cblas_dcopy(n , q , 1 , velocitytmp, 1);
      prodNumericsMatrix(n, n, alpha, M, reactiontmp, beta, velocitytmp);
      // projection for each contact
      for (contact = 0 ; contact < nc ; ++contact)
      {
        int pos = contact * nLocal;
        double  normUT = sqrt(velocitytmp[pos + 1] * velocitytmp[pos + 1] + velocitytmp[pos + 2] * velocitytmp[pos + 2]);
        reaction[pos] -= rho * (velocitytmp[pos] + mu[contact] * normUT);
        reaction[pos + 1] -= rho * velocitytmp[pos + 1];
        reaction[pos + 2] -= rho * velocitytmp[pos + 2];
        projectionOnCone(&reaction[pos], mu[contact]);
      }

      /* **** Criterium convergence **** */
      FrictionContact3D_compute_error(problem, reaction , velocity, tolerance, options, &error);

      if (verbose > 0)
      {
        printf("----------------------------------- FC3D - Extra Gradient (EG) - Iteration %i rho = %14.7e \tError = %14.7e\n", iter, rho, error);
      }
      if (error < tolerance) hasNotConverged = 0;
      *info = hasNotConverged;
    }
  }

  if (isVariable)
  {
    while ((iter < itermax) && (hasNotConverged > 0))
    {
      ++iter;


      /* Store the error */
      error_k = error;
      /* store the reaction at the beginning of the iteration */
      cblas_dcopy(n , reaction , 1 , reaction_k, 1);

      /* velocity_k <- q  */
      cblas_dcopy(n , q , 1 , velocity_k, 1);

      /* velocity_k <- q + M * reaction  */
      beta = 1.0;
      prodNumericsMatrix(n, n, alpha, M, reaction, beta, velocity_k);

      ls_iter = 0 ;
      success =0;

      while (!success && (ls_iter < ls_itermax))
      {

        rho_k = rho * pow(tau,ls_iter);

        /* projection for each contact */
        for (contact = 0 ; contact < nc ; ++contact)
        {
          int pos = contact * nLocal;
          double  normUT = sqrt(velocity_k[pos + 1] * velocity_k[pos + 1] + velocity_k[pos + 2] * velocity_k[pos + 2]);
          reaction[pos] = reaction_k[pos] -  rho_k * (velocity_k[pos] + mu[contact] * normUT);
          reaction[pos + 1] = reaction_k[pos+1] - rho_k * velocity_k[pos + 1];
          reaction[pos + 2] = reaction_k[pos+2] - rho_k * velocity_k[pos + 2];

          /* V.A. 12/11/2013 : Why the following lines (that are false) are working better in practise ? */
          /* reaction[pos] -= rho_k * (velocity_k[pos] + mu[contact] * normUT); */
          /* reaction[pos + 1] -= rho_k * velocity_k[pos + 1]; */
          /* reaction[pos + 2] -= rho_k * velocity_k[pos + 2]; */

          projectionOnCone(&reaction[pos], mu[contact]);
        }


        /* velocity <- q  */
        cblas_dcopy(n , q , 1 , velocity, 1);

        /* velocity <- q + M * reaction  */
        beta = 1.0;
        prodNumericsMatrix(n, n, alpha, M, reaction, beta, velocity);

        /* velocitytmp <- velocity */
        cblas_dcopy(n, velocity, 1, velocitytmp , 1) ;

        /* velocitytmp <- velocity - velocity_k   */
        cblas_daxpy(n, -1.0, velocity_k , 1, velocitytmp , 1) ;

        a1 = cblas_dnrm2(n, velocitytmp, 1);

        /* reactiontmp <- reaction */
        cblas_dcopy(n, reaction, 1, reactiontmp , 1) ;

        /* reactiontmp <- reaction - reaction_k   */
        cblas_daxpy(n, -1.0, reaction_k , 1, reactiontmp , 1) ;

        a2 = cblas_dnrm2(n, reactiontmp, 1) ;

        success = (rho_k*a1 < L * a2)?1:0;

        /* printf("rho_k = %12.8e\t", rho_k); */
        /* printf("a1 = %12.8e\t", a1); */
        /* printf("a2 = %12.8e\t", a2); */
        /* printf("norm reaction = %12.8e\t",cblas_dnrm2(n, reaction, 1) ); */
        /* printf("success = %i\n", success); */

        ls_iter++;
      }
      /* velocitytmp <- q  */
      cblas_dcopy(n , q , 1 , velocitytmp, 1);

      prodNumericsMatrix(n, n, alpha, M, reaction, beta, velocitytmp);


      // projection for each contact
      for (contact = 0 ; contact < nc ; ++contact)
      {
        int pos = contact * nLocal;
        double  normUT = sqrt(velocitytmp[pos + 1] * velocitytmp[pos + 1] + velocitytmp[pos + 2] * velocitytmp[pos + 2]);
        /* reaction[pos] -= rho * (velocitytmp[pos] + mu[contact] * normUT); */
        /* reaction[pos + 1] -= rho * velocitytmp[pos + 1]; */
        /* reaction[pos + 2] -= rho * velocitytmp[pos + 2]; */
        reaction[pos] = reaction_k[pos] -  rho_k * (velocity_k[pos] + mu[contact] * normUT);
        reaction[pos + 1] = reaction_k[pos+1] - rho_k * velocity_k[pos + 1];
        reaction[pos + 2] = reaction_k[pos+2] - rho_k * velocity_k[pos + 2];


        projectionOnCone(&reaction[pos], mu[contact]);
      }

      /* **** Criterium convergence **** */
      FrictionContact3D_compute_error(problem, reaction , velocity, tolerance, options, &error);

      /*Update rho*/
      if ((rho_k*a1 < Lmin * a2) && (error < error_k))
      {
        rho =rho_k/taumin;
      }
      else
        rho =rho_k;





      if (verbose > 0)
      {
        printf("----------------------------------- FC3D - Extra Gradient (EG) - Iteration %i rho = %14.7e \tError = %14.7e\n", iter, rho, error);
      }
      if (error < tolerance) hasNotConverged = 0;
      *info = hasNotConverged;
    }
  }


  if (verbose > 0)
  {
    printf("----------------------------------- FC3D -  Extra Gradient (EG) - #Iteration %i Final Error = %14.7e\n", iter, error);
  }
  dparam[0] = tolerance;
  dparam[1] = error;
  iparam[7] = iter;
  free(velocitytmp);
  free(reactiontmp);

}


int frictionContact3D_ExtraGradient_setDefaultSolverOptions(SolverOptions* options)
{
  int i;
  if (verbose > 0)
  {
    printf("Set the Default SolverOptions for the ExtraGradient Solver\n");
  }

  /*strcpy(options->solverName,"DSFP");*/
  options->solverId = SICONOS_FRICTION_3D_EG;
  options->numberOfInternalSolvers = 0;
  options->isSet = 1;
  options->filterOn = 1;
  options->iSize = 8;
  options->dSize = 8;
  options->iparam = (int *)malloc(options->iSize * sizeof(int));
  options->dparam = (double *)malloc(options->dSize * sizeof(double));
  options->dWork = NULL;
  options->iWork = NULL;
  for (i = 0; i < 8; i++)
  {
    options->iparam[i] = 0;
    options->dparam[i] = 0.0;
  }
  options->iparam[0] = 20000;
  options->dparam[0] = 1e-3;
  options->dparam[3] = 1e-3;
  options->dparam[3] = -1.0; // rho is variable by default

  options->internalSolvers = NULL;

  return 0;
}
