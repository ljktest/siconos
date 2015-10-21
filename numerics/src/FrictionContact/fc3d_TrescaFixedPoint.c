
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

#include "fc3d_Solvers.h"
#include "fc3d_compute_error.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#define max(a,b) (a>=b?a:b)
#define VERBOSE_DEBUG
#include "Friction_cst.h"
void fc3d_TrescaFixedPoint(FrictionContactProblem* problem, double *reaction, double *velocity, int* info, SolverOptions* options)
{
  /* int and double parameters */
  int* iparam = options->iparam;
  double* dparam = options->dparam;

  /* Number of contacts */
  int nc = problem->numberOfContacts;


  /* Maximum number of iterations */
  int itermax = iparam[0];
  /* Tolerance */
  double tolerance = dparam[0];



  if (options->numberOfInternalSolvers < 1)
  {
    numericsError("fc3d_TrescaFixedpoint", "The Tresca Fixed Point method needs options for the internal solvers, options[0].numberOfInternalSolvers should be >1");
  }

  SolverOptions * internalsolver_options = options->internalSolvers;

  if (verbose > 0)
  {
    printSolverOptions(options);
  }


  /*****  Fixed Point Iterations *****/
  int iter = 0; /* Current iteration number */
  double error = 1.; /* Current error */
  int hasNotConverged = 1;

  internalSolverPtr internalsolver;
  options->dWork = (double *) malloc(nc * sizeof(double));
  double * mu = options->dWork;
  internalsolver_options->dWork = options->dWork;


  if (internalsolver_options->solverId == SICONOS_FRICTION_3D_NSGS)
  {
    if (verbose == 1)
      printf(" ========================== Call NSGS solver for Friction-Contact 3D problem ==========================\n");
    internalsolver = &fc3d_nsgs;
    //internalsolver_options->internalSolvers->dWork = options->dWork;
  }
  else if (internalsolver_options->solverId == SICONOS_FRICTION_3D_PGoC)
  {
    if (verbose == 1)
      printf(" ========================== Call PGoC solver for Friction-Contact 3D problem ==========================\n");
    internalsolver = &fc3d_ProjectedGradientOnCylinder;
  }
  else
  {
    fprintf(stderr, "Numerics, fc3d_TrescaFixedPoint failed. Unknown internal solver.\n");
    exit(EXIT_FAILURE);
  }



  int cumul_internal=0;

  while ((iter < itermax) && (hasNotConverged > 0))
  {
    ++iter;

    // internal solver for the regularized problem

    /* Compute the value of the initial value friction threshold*/
    for (int ic = 0 ; ic < nc ; ic++) mu[ic] = fmax(0.0, problem->mu[ic] *  reaction [ic * 3]);
    if (iparam[1] == 0 )
    {
      internalsolver_options->dparam[0] = max(error/10.0, options->dparam[0]/problem->numberOfContacts);
    }
    else if (iparam[1] ==1)
    {
      internalsolver_options->dparam[0] = options->dparam[0]/2.0;
    }
    else
    {
      fprintf(stderr, "Numerics, fc3d_TrescaFixedPoint failed. Unknown startegy for driving tolerence of internal.\n");
    exit(EXIT_FAILURE);
    }
    (*internalsolver)(problem, reaction , velocity , info , internalsolver_options);

    cumul_internal += internalsolver_options->iparam[7];

    /* **** Criterium convergence **** */

    fc3d_compute_error(problem, reaction , velocity, tolerance, options, &error);

    if (options->callback)
    {
      options->callback->collectStatsIteration(options->callback->env, nc * 3, 
                                      reaction, velocity, error, NULL);
    }

    if (verbose > 0)
      printf("------------------------ FC3D - TFP - Iteration %i Error = %14.7e\n", iter, error);

    if (error < tolerance) hasNotConverged = 0;
    *info = hasNotConverged;
  }
  if (verbose > 0){
    printf("----------------------------------- FC3D - TFP - # Iteration %i Final Error = %14.7e\n", iter, error);
    printf("----------------------------------- FC3D - TFP - #              Internal iteration = %i\n", cumul_internal);
  }
  free(options->dWork);
  options->dWork = NULL;
  internalsolver_options->dWork = NULL;

  if (internalsolver_options->internalSolvers != NULL)
    internalsolver_options->internalSolvers->dWork = NULL;
  dparam[0] = tolerance;
  dparam[1] = error;
  iparam[7] = iter;

}



int fc3d_TrescaFixedPoint_setDefaultSolverOptions(SolverOptions* options)
{
  int i;
  if (verbose > 0)
  {
    printf("Set the Default SolverOptions for the TFP Solver\n");
  }


  options->solverId = SICONOS_FRICTION_3D_TFP;
  options->numberOfInternalSolvers = 1;
  options->isSet = 1;
  options->filterOn = 1;
  options->iSize = 8;
  options->dSize = 8;
  options->iparam = (int *)malloc(options->iSize * sizeof(int));
  options->dparam = (double *)malloc(options->dSize * sizeof(double));
  options->dWork = NULL;
  null_SolverOptions(options);
  for (i = 0; i < 8; i++)
  {
    options->iparam[i] = 0;
    options->dparam[i] = 0.0;
  }
  options->iparam[0] = 1000;
  options->dparam[0] = 1e-4;

  options->internalSolvers = (SolverOptions *)malloc(sizeof(SolverOptions));
  fc3d_nsgs_setDefaultSolverOptions(options->internalSolvers);
  options->internalSolvers->iparam[0]=1000;

  SolverOptions * subsubsolver = options->internalSolvers->internalSolvers;

  subsubsolver->iparam[0] = 50;
  subsubsolver->dparam[0] = 1e-14;

  subsubsolver->solverId = SICONOS_FRICTION_3D_ONECONTACT_ProjectionOnCylinderWithLocalIteration;

  return 0;
}
