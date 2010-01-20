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

#include "FrictionContact3D_Solvers.h"
#include "FrictionContact3D_compute_error.h"
#include "NCP_Solvers.h"
#include "LA.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


void frictionContact3D_proximal(FrictionContact_Problem* problem, double *reaction, double *velocity, int* info, Solver_Options* options)
{
  /* int and double parameters */
  int* iparam = options->iparam;
  double* dparam = options->dparam;
  /* Number of contacts */
  int nc = problem->numberOfContacts;
  double* q = problem->q;
  NumericsMatrix* M = problem->M;
  /* Dimension of the problem */
  int n = 3 * nc;

  /* Maximum number of iterations */
  int itermax = iparam[0];
  /* Tolerance */
  double tolerance = dparam[0];

  /* Check for trivial case */
  *info = checkTrivialCase(n, q, velocity, reaction, iparam, dparam);

  if (*info == 0)
    return;

  if (options->numberOfInternalSolvers < 1)
  {
    numericsError("frictionContact3D_proximal", "The PROX method needs options for the internal solvers, options[0].numberOfInternalSolvers should be >1");
  }
  assert(&options[1]);
  Solver_Options *internalsolver_options = &options[1];


  /*****  PROXIMAL Iterations *****/
  int iter = 0; /* Current iteration number */
  double error = 1.; /* Current error */
  int hasNotConverged = 1;







  double rho = dparam[3];
  double minusrho = -1.0 * rho;

  double * reactionold = malloc(n * sizeof(double));
  DCOPY(n , reaction , 1 , reactionold , 1);


  internalSolverPtr internalsolver;

  if (strcmp(internalsolver_options->solverName, "NSGS") == 0) internalsolver = &frictionContact3D_nsgs;
  else if (strcmp(internalsolver_options->solverName, "DeSaxceFixedPoint") == 0)internalsolver = &frictionContact3D_DeSaxceFixedPoint;
  else  internalsolver = &frictionContact3D_nsgs;




  while ((iter < itermax) && (hasNotConverged > 0))
  {
    ++iter;

    DCOPY(n , reaction , 1 , reactionold , 1);
    //Add proximal regularization on q
    DAXPY(n, minusrho, reactionold, 1, problem->q , 1) ;
    //Add proximal regularization on M
    if (M->storageType == 0)
    {
      for (int i = 0 ; i < n ; i++) M->matrix0[i + i * n] += rho;
    }
    else if (M->storageType == 1)
    {
      for (int ic = 0 ; ic < nc ; ic++)
      {
        int diagPos = getDiagonalBlockPos(M->matrix1, ic);
        for (int i = 0 ; i < 3 ; i++) M->matrix1->block[diagPos][i + 3 * i] += rho ;
      }
    }
    // internal solver for the regularized problem
    /*       frictionContact3D_nsgs(problem, reaction , velocity , info , internalsolver_options); */
    (*internalsolver)(problem, reaction , velocity , info , internalsolver_options);


    /* **** Criterium convergence **** */
    //substract proximal regularization on q
    DAXPY(n, rho, reactionold, 1, problem->q, 1) ;
    //substract proximal regularization on M
    if (M->storageType == 0)
    {
      for (int i = 0 ; i < n ; i++) M->matrix0[i + i * n] -= rho;
    }
    else if (M->storageType == 1)
    {
      for (int ic = 0 ; ic < nc ; ic++)
      {
        int diagPos = getDiagonalBlockPos(M->matrix1, ic);
        for (int i = 0 ; i < 3 ; i++) M->matrix1->block[diagPos][i + 3 * i] -= rho ;
      }
    }

    FrictionContact3D_compute_error(problem, reaction , velocity, tolerance, &error);

    if (verbose > 0)
      printf("------------------------ FC3D - PROXIMAL - Iteration %i Error = %14.7e\n", iter, error);

    if (error < tolerance) hasNotConverged = 0;
    *info = hasNotConverged;
  }
  printf("----------------------------------- FC3D - PROXIMAL - # Iteration %i Final Error = %14.7e\n", iter, error);
  dparam[0] = tolerance;
  dparam[1] = error;

  free(reactionold);



}


int frictionContact3D_proximal_setDefaultSolverOptions(Solver_Options** arrayOfSolver_Options)
{
  int i;
  if (verbose > 0)
  {
    printf("Set the Default Solver_Options for the PROX Solver\n");
  }
  int nbSolvers = 3 ;
  Solver_Options * options = (Solver_Options *)malloc(nbSolvers * sizeof(Solver_Options));
  arrayOfSolver_Options[0] = options;

  strcpy(options->solverName, "PROX");

  options->numberOfInternalSolvers = 1;
  options->isSet = 1;
  options->filterOn = 1;
  options->iSize = 5;
  options->dSize = 5;
  options->iparam = (int *)malloc(options->iSize * sizeof(int));
  options->dparam = (double *)malloc(options->dSize * sizeof(double));
  options->dWork = NULL;
  options->iWork = NULL;
  for (i = 0; i < 5; i++)
  {
    options->iparam[i] = 0;
    options->dparam[i] = 0.0;
  }
  options->iparam[0] = 1001;
  options->dparam[0] = 1e-08;
  options->dparam[3] = 1.0;

  strcpy(options[1].solverName, "NSGS");
  options[1].numberOfInternalSolvers = 1;
  options[1].isSet = 1;
  options[1].filterOn = 1;
  options[1].iSize = 5;
  options[1].dSize = 5;
  options[1].iparam = (int *)malloc(options[1].iSize * sizeof(int));
  options[1].dparam = (double *)malloc(options[1].dSize * sizeof(double));
  options[1].dWork = NULL;
  options[1].iWork = NULL;
  for (i = 0; i < 5; i++)
  {
    options[1].iparam[i] = 0;
    options[1].dparam[i] = 0.0;
  }
  options[1].iparam[0] = 100;
  options[1].dparam[0] = 1e-8;


  strcpy(options[2].solverName, "AlartCurnierNewton");
  options[2].numberOfInternalSolvers = 0;
  options[2].isSet = 1;
  options[2].filterOn = 1;
  options[2].iSize = 5;
  options[2].dSize = 5;
  options[2].iparam = (int *)malloc(options[2].iSize * sizeof(int));
  options[2].dparam = (double *)malloc(options[2].dSize * sizeof(double));
  options[2].dWork = NULL;
  options[2].iWork = NULL;
  for (i = 0; i < 5; i++)
  {
    options[2].iparam[i] = 0;
    options[2].dparam[i] = 0.0;
  }
  options[2].iparam[0] = 10;
  options[2].dparam[0] = 1e-8;


  return 0;
}

int frictionContact3D_proximal_deleteDefaultSolverOptions(Solver_Options** arrayOfSolver_Options)
{
  int i;
  if (verbose > 0)
  {
    printf("Set the Default Solver_Options for the PROX Solver\n");
  }

  Solver_Options * options = arrayOfSolver_Options[0];

  int nbSolvers = 3 ;
  for (i = 0; i < nbSolvers; i++)
  {
    if (options[i].iparam) free(options[i].iparam);
    options[i].iparam = NULL;
    if (options[i].dparam) free(options[i].dparam);
    options[i].dparam = NULL;
    if (options[i].dWork)  free(options[i].dWork);
    options[i].dWork = NULL;
    if (options[i].iWork)  free(options[i].iWork);
    options[i].iWork = NULL;
  }
  free(options);


  return 0;
}
