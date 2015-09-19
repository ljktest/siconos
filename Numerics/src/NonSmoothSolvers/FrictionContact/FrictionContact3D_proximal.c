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

#include "FrictionContact3D_Solvers.h"
#include "FrictionContact3D_compute_error.h"
#include "NCP_Solvers.h"
#include "SiconosBlas.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
/* #define DEBUG_STDOUT */
/* #define DEBUG_MESSAGES */
#include "debug.h"
#include <math.h>

void frictionContact3D_proximal(FrictionContactProblem* problem, double *reaction, double *velocity, int* info, SolverOptions* options)
{
  /* int and double parameters */
  int* iparam = options->iparam;
  double* dparam = options->dparam;

  /* Number of contacts */
  int nc = problem->numberOfContacts;
  NumericsMatrix* M = problem->M;

  /* Dimension of the problem */
  int n = 3 * nc;

  /* Maximum number of iterations */
  int itermax = iparam[0];
  /* Tolerance */
  double tolerance = dparam[0];

#ifdef HAVE_MPI
  /* Flag to keep track of mpi initialization */
  int mpi_init = 0;
#endif

  if (options->numberOfInternalSolvers < 1)
  {
    numericsError("frictionContact3D_proximal", "The PROX method needs options for the internal solvers, options[0].numberOfInternalSolvers should be >1");
  }
  SolverOptions *internalsolver_options = options->internalSolvers;


  /*****  PROXIMAL Iterations *****/
  int iter = 0; /* Current iteration number */
  double error = 1.; /* Current error */
  int hasNotConverged = 1;

  double alpha = dparam[3];
  if (dparam[3] < 1e-12)
  {
    dparam[3] = 1.0;
  }
  /* parameters to set the value of alpha */
  double sigma = options->dparam[4];
  double nu = options->dparam[5];

  DEBUG_PRINTF("options->iparam[2] = %i\n",options->iparam[2]);
  DEBUG_PRINTF("options->iparam[3] = %i\n",options->iparam[3]);
  DEBUG_PRINTF("options->dparam[4] = %i\n",options->dparam[4]);
  DEBUG_PRINTF("options->dparam[5] = %i\n",options->dparam[5]);



  double * reactionold = (double *)malloc(n * sizeof(double));
  cblas_dcopy(n , reaction , 1 , reactionold , 1);


  internalSolverPtr internalsolver =0;
  int iter_iparam =7;
  options->iparam[6]= 0;

  if (internalsolver_options) /* we assume that the solver option has been coherently set*/
  {
    if (internalsolver_options->solverId == SICONOS_FRICTION_3D_NSGS)
    {
      internalsolver = &frictionContact3D_nsgs;
    }
    else if (internalsolver_options->solverId == SICONOS_FRICTION_3D_DeSaxceFixedPoint)
    {
      internalsolver = &frictionContact3D_DeSaxceFixedPoint;
    }
    else if (internalsolver_options->solverId == SICONOS_FRICTION_3D_EG)
    {
      internalsolver = &frictionContact3D_ExtraGradient;
    }
    else if (internalsolver_options->solverId == SICONOS_FRICTION_3D_LOCALAC)
    {
      internalsolver = &frictionContact3D_localAlartCurnier;

#ifdef HAVE_MPI
      if (internalsolver_options->iparam[13] == 1)   /* MUMPS */
      {
        if (internalsolver_options->solverData == MPI_COMM_NULL) /* default */
        {
          internalsolver_options->solverData = NM_MPI_com(NULL);
          mpi_init = 1;
        }
      }
#endif
      iter_iparam =1;
    }
    else if (internalsolver_options->solverId == SICONOS_FRICTION_3D_LOCALFB)
    {
      if (internalsolver_options->iparam[13] == 1)   /* MUMPS */
      {
        internalsolver = &frictionContact3D_localFischerBurmeister;

#ifdef HAVE_MPI
        if (internalsolver_options->solverData == MPI_COMM_NULL) /* default */
        {
          internalsolver_options->solverData = NM_MPI_com(NULL);
          mpi_init = 1;
        }
#endif
      }
      iter_iparam =1;
    }
  }
  else
  {
    numericsError("frictionContact3D_proximal", "The PROX method needs options for the internal solvers, soptions->internalSolvers should be diffrent from NULL");
  }




  options->internalSolvers->iparam[0] = options->iparam[3]+1; // Default Maximum iteration

  FrictionContact3D_compute_error(problem, reaction , velocity, tolerance, options, &error);


  internalsolver_options->dparam[0] = options->dparam[0];
  internalsolver_options->dparam[0] = error;

  alpha = sigma*pow(error,nu);


  DEBUG_PRINTF("options->iparam[2] = %i\n",options->iparam[2]);
  DEBUG_PRINTF("options->iparam[3] = %i\n",options->iparam[3]);



  while ((iter < itermax) && (hasNotConverged > 0))
  {
    ++iter;
    cblas_dcopy(n , reaction , 1 , reactionold , 1);
    //Add proximal regularization on q
    cblas_daxpy(n, -alpha, reactionold, 1, problem->q , 1) ;

    //Add proximal regularization on M
    if (M->storageType == 0)
    {
      for (int i = 0 ; i < n ; i++) M->matrix0[i + i * n] += alpha;
    }
    else if (M->storageType == 1)
    {
      for (int ic = 0 ; ic < nc ; ic++)
      {
        int diagPos = getDiagonalBlockPos(M->matrix1, ic);
        for (int i = 0 ; i < 3 ; i++) M->matrix1->block[diagPos][i + 3 * i] += alpha ;
      }
    }
    // internal solver for the regularized problem
    /*       frictionContact3D_nsgs(problem, reaction , velocity , info , internalsolver_options); */

    /* internalsolver_options->dparam[0] = max(error/10.0, options->dparam[0]); */
    //internalsolver_options->dparam[0] = options->dparam[0];

    internalsolver_options->dparam[0] = alpha*error;
    DEBUG_PRINTF("internal solver tolerance = %21.8e \n",internalsolver_options->dparam[0]);
    (*internalsolver)(problem, reaction , velocity , info , internalsolver_options);

    /* **** Criterium convergence **** */
    //substract proximal regularization on q
    cblas_daxpy(n, alpha, reactionold, 1, problem->q, 1) ;
    //substract proximal regularization on M
    if (M->storageType == 0)
    {
      for (int i = 0 ; i < n ; i++) M->matrix0[i + i * n] -= alpha;
    }
    else if (M->storageType == 1)
    {
      for (int ic = 0 ; ic < nc ; ic++)
      {
        int diagPos = getDiagonalBlockPos(M->matrix1, ic);
        for (int i = 0 ; i < 3 ; i++) M->matrix1->block[diagPos][i + 3 * i] -= alpha ;
      }
    }

    FrictionContact3D_compute_error(problem, reaction , velocity, tolerance, options, &error);
    //update the alpha with respect to the number of internal iterations.

    int iter_internalsolver = internalsolver_options->iparam[iter_iparam];
    options->iparam[6] +=iter_internalsolver;
    DEBUG_PRINTF("iter_internalsolver = %i\n",iter_internalsolver);
    DEBUG_PRINTF("info = %i\n",*info);
    DEBUG_PRINTF("options->iparam[1] = %i\n",options->iparam[1]);

    /* if (iter_internalsolver < options->iparam[2])// || (*info == 0))  */
    /* { */
    /*   alpha = alpha/sigma; */
    /*   DEBUG_PRINTF("We decrease alpha = %8.4e\n",alpha); */
    /* } */
    /* else if (iter_internalsolver > options->iparam[3]) */
    /* { */
    /*   alpha = sigma *alpha; */
    /*   DEBUG_PRINTF("We increase alpha = %8.4e\n",alpha); */
    /* } */

    alpha = sigma*pow(error,nu);


    DEBUG_PRINTF("alpha = %8.4e\n",alpha);

    if (options->callback)
    {
      options->callback->collectStatsIteration(options->callback->env, nc * 3,
                                      reaction, velocity,
                                      error, NULL);
    }

    if (verbose > 0)
      printf("------------------------ FC3D - PROXIMAL - Iteration %i Error = %14.7e with alpha = %12.8e\n\n", iter, error, alpha);

    if (error < tolerance) hasNotConverged = 0;
    *info = hasNotConverged;
  }
  if (verbose > 0)
  {
    printf("------------------------ FC3D - PROXIMAL - # Iteration %i Final Error = %14.7e  \n", iter, error);
    printf("------------------------ FC3D - PROXIMAL - # Iteration of internal solver %i \n", options->iparam[6]);
  }

  iparam[7] = iter;
  dparam[0] = tolerance;
  dparam[1] = error;

  free(reactionold);

#ifdef HAVE_MPI
  if (mpi_init)
  {
      MPI_Finalize();
  }
#endif

}


int frictionContact3D_proximal_setDefaultSolverOptions(SolverOptions* options)
{
  int i;
  if (verbose > 0)
  {
    printf("Set the Default SolverOptions for the PROX Solver\n");
  }

  /*  strcpy(options->solverName,"PROX");*/
  options->solverId = SICONOS_FRICTION_3D_PROX;
  options->numberOfInternalSolvers = 1;
  options->isSet = 1;
  options->filterOn = 1;
  options->iSize = 10;
  options->dSize = 10;
  options->iparam = (int *)malloc(options->iSize * sizeof(int));
  options->dparam = (double *)malloc(options->dSize * sizeof(double));
  options->dWork = NULL;
  null_SolverOptions(options);
  for (i = 0; i < 10; i++)
  {
    options->iparam[i] = 0;
    options->dparam[i] = 0.0;
  }
  options->iparam[0] = 1000;
  options->iparam[2] = 5;    /* Default mimimun iteration of the internal
                                solver for decreasing alpha */
  options->iparam[3] = 15;   /* Default maximum iteration of the internal
                                solver for increasing alpha */
  options->iparam[8] = 0;    /* no overrelaxation by default */
  options->dparam[0] = 1e-4;
  options->dparam[3] = 1.e4; /* default value for proximal parameter alpha; */
  options->dparam[4] = 5.0;  /* default value for sigma; */
  options->dparam[5] = 1.0;  /* default value for nu; */
  options->dparam[8] = 1.5;  /* default value for relaxation parameter omega */

  options->internalSolvers = (SolverOptions *)malloc(sizeof(SolverOptions));
  options->internalSolvers->solverId = SICONOS_FRICTION_3D_LOCALAC;
  frictionContact3D_AlartCurnier_setDefaultSolverOptions(options->internalSolvers);
  return 0;
}
