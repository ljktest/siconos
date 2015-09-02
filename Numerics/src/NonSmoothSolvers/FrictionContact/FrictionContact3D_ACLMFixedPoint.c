
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
#include "SOCLCP_Solvers.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#define max(a,b) (a>=b?a:b)
//#define VERBOSE_DEBUG
#include "Friction_cst.h"


#define DEBUG_MESSAGES
#define DEBUG_STDOUT
#include "debug.h"



/** pointer to function used to call internal solver for proximal point solver */
typedef void (*soclcp_InternalSolverPtr)(SecondOrderConeLinearComplementarityProblem*, double*, double*, int *, SolverOptions *);

void frictionContact3D_ACLMFixedPoint(FrictionContactProblem* problem, double *reaction, double *velocity, int* info, SolverOptions* options)
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
    numericsError("frictionContact3D_ACLMFixedpoint", "The ACLM Fixed Point method needs options for the internal solvers, options[0].numberOfInternalSolvers should be >1");
  }

  SolverOptions * internalsolver_options = options->internalSolvers;

  if (verbose > 0)
  {
    printf("Local solver data :");
    printSolverOptions(internalsolver_options);
  }


  /*****  Fixed Point Iterations *****/
  int iter = 0; /* Current iteration number */
  double error = 1.; /* Current error */
  int hasNotConverged = 1;

  soclcp_InternalSolverPtr internalsolver;

  SecondOrderConeLinearComplementarityProblem* soclcp = (SecondOrderConeLinearComplementarityProblem *)malloc(sizeof(SecondOrderConeLinearComplementarityProblem));
  soclcp->n = problem->numberOfContacts * problem->dimension;
  soclcp->nc= problem->numberOfContacts;
  soclcp->M = problem-> M;
  soclcp->q = (double *) malloc(soclcp->n * sizeof(double));
  soclcp->mu = problem->mu;
  soclcp->coneIndex = (unsigned int *) malloc((soclcp->nc+1) * sizeof(unsigned int));

  for (int i=0; i < soclcp->n; i++)
  {
    soclcp->q[i]=problem->q[i];
  }
  for (int i=0; i < soclcp->nc+1; i++)
  {
    soclcp->coneIndex[i] = 3*i;
  }

  if (internalsolver_options->solverId == SICONOS_SOCLCP_NSGS)
  {
    if (verbose == 1)
      printf(" ========================== Call NSGS solver SOCLCP problem ==========================\n");
    internalsolver = &soclcp_nsgs;
    //internalsolver_options->internalSolvers->dWork = options->dWork;
  }
  else
  {
    fprintf(stderr, "Numerics, FrictionContact3D_ACLMFixedPoint failed. Unknown internal solver.\n");
    exit(EXIT_FAILURE);
  }

  double normUT;
  int cumul_iter =0;
  while ((iter < itermax) && (hasNotConverged > 0))
  {
    ++iter;
    // internal solver for the regularized problem

    /* Compute the value of the initial value of q */
    for (int ic = 0 ; ic < nc ; ic++)
    {
      normUT = sqrt(velocity[ic*3+1] * velocity[ic*3+1] + velocity[ic*3+2] * velocity[ic*3+2]);
      soclcp->q[3*ic] = problem->q[3*ic] + problem->mu[ic]*normUT;
    }
    secondOrderConeLinearComplementarityProblem_printInFilename(soclcp,"output.dat");
    // DEBUG_EXPR(for (int ic = 0 ; ic < nc ; ic++) printf("problem->q[%i] = %le\n", 3*ic, problem->q[3*ic]);
    //  for (int ic = 0 ; ic < nc ; ic++) printf("q[%i] = %le\n", 3*ic, soclcp->q[3*ic]); );
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
      fprintf(stderr, "Numerics, frictionContact3D_ACLMFixedPoint failed. Unknown startegy for driving tolerence of internal.\n");
      exit(EXIT_FAILURE);
    }

    (*internalsolver)(soclcp, reaction , velocity , info , internalsolver_options);
    cumul_iter +=  internalsolver_options->iparam[7];
    /* **** Criterium convergence **** */

    FrictionContact3D_compute_error(problem, reaction , velocity, tolerance, options, &error);

    if (options->callback)
    {
      options->callback->collectStatsIteration(options->callback->env, nc * 3,
                                      reaction, velocity, error, NULL);
    }

    if (verbose > 0)
      printf("------------------------ FC3D - ACLMFP - Iteration %i Error = %14.7e\n", iter, error);

    if (error < tolerance) hasNotConverged = 0;
    *info = hasNotConverged;
  }
  if (verbose > 0)
  {
    printf("----------------------------------- FC3D - ACLMFP - # Iteration %i Final Error = %14.7e\n", iter, error);
    printf("----------------------------------- FC3D - ACLMFP - #              internal iteration = %i\n", cumul_iter);
  }
  free(soclcp->q);
  free(soclcp->coneIndex);
  free(soclcp);


  if (internalsolver_options->internalSolvers != NULL)
    internalsolver_options->internalSolvers->dWork = NULL;
  dparam[0] = tolerance;
  dparam[1] = error;
  iparam[7] = iter;

}



int frictionContact3D_ACLMFixedPoint_setDefaultSolverOptions(SolverOptions* options)
{
  int i;
  if (verbose > 0)
  {
    printf("Set the Default SolverOptions for the ACLMFP Solver\n");
  }

  options->solverId = SICONOS_FRICTION_3D_ACLMFP;
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
  options->iparam[1] = 0;
  options->dparam[0] = 1e-4;
  options->internalSolvers = (SolverOptions *)malloc(sizeof(SolverOptions));

  soclcp_nsgs_setDefaultSolverOptions(options->internalSolvers);
  options->internalSolvers->iparam[0] =10000;
  return 0;
}
