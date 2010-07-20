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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <assert.h>

#include "NumericsOptions.h"
#include "PrimalFrictionContact3D_Solvers.h"
int * Primal_ipiv = NULL;
int  Primal_MisInverse = 0;
int  Primal_MisLU = 0;
char SICONOS_FRICTION_3D_PRIMAL_NSGS_WR_STR [] = "F3DP_NSGS_WR";
char SICONOS_FRICTION_3D_PRIMAL_GLOBALAC_WR_STR [] = "F3DP_GLOBALAC_WR";
char SICONOS_FRICTION_3D_PRIMAL_NSGSV_WR_STR [] = "F3DP_NSGSV_WR";
char SICONOS_FRICTION_3D_PRIMAL_PROX_WR_STR [] = "F3DP_PROX_WR";
char SICONOS_FRICTION_3D_PRIMAL_DSFP_WR_STR [] = "F3DP_DSFP_WR";
char SICONOS_FRICTION_3D_PRIMAL_TFP_WR_STR [] = "F3DP_TFP_WR";
char SICONOS_FRICTION_3D_PRIMAL_NSGS_STR [] = "F3DP_NSGS";


int primalFrictionContact3D_driver(PrimalFrictionContactProblem* problem, double *reaction , double *velocity, double* globalVelocity,  SolverOptions* options, NumericsOptions* global_options)
{

  /* Set global options */
  setNumericsOptions(global_options);

  /* If the options for solver have not been set, read default values in .opt file */
  int NoDefaultOptions = options->isSet; /* true(1) if the SolverOptions structure has been filled in else false(0) */

  if (!NoDefaultOptions)
    readSolverOptions(3, options);

  if (verbose > 0)
    printSolverOptions(options);

  /* Solver name */
  /*  char * name = options->solverName;*/


  int info = -1 ;

  if (problem->dimension != 3)
    numericsError("FrictionContact3D_driver", "Dimension of the problem : problem-> dimension is not compatible or is not set");


  /* Non Smooth Gauss Seidel (NSGS) */
  switch (options->solverId)
  {
  case SICONOS_FRICTION_3D_PRIMAL_NSGS_WR:
  {
    if (verbose == 1)
      printf(" ========================== Call NSGS_WR solver with reformulation into Friction-Contact 3D problem ==========================\n");
    Primal_ipiv = NULL;
    Primal_MisInverse = 0;
    Primal_MisLU = 0;
    primalFrictionContact3D_nsgs_wr(problem, reaction , velocity, globalVelocity, &info, options);
    break;

  }
  case SICONOS_FRICTION_3D_PRIMAL_NSGSV_WR:
  {
    if (verbose == 1)
      printf(" ========================== Call NSGSV_WR solver with reformulation into Friction-Contact 3D problem ==========================\n");
    Primal_ipiv = NULL;
    Primal_MisInverse = 0;
    Primal_MisLU = 0;
    primalFrictionContact3D_nsgs_velocity_wr(problem, reaction , velocity, globalVelocity, &info, options);
    break;
  }
  case SICONOS_FRICTION_3D_PRIMAL_GLOBALAC_WR:
  {
    if (verbose == 1)
      printf(" ========================== Call GLOBALAC_WR solver with reformulation into Friction-Contact 3D problem ==========================\n");
    Primal_ipiv = NULL;
    Primal_MisInverse = 0;
    Primal_MisLU = 0;
    primalFrictionContact3D_globalAlartCurnier_wr(problem, reaction , velocity, globalVelocity, &info, options);
    break;

  }
  case SICONOS_FRICTION_3D_PRIMAL_PROX_WR:
  {
    if (verbose == 1)
      printf(" ========================== Call PROX_WR solver with reformulation into Friction-Contact 3D problem ==========================\n");
    Primal_ipiv = NULL;
    Primal_MisInverse = 0;
    Primal_MisLU = 0;
    primalFrictionContact3D_proximal_wr(problem, reaction , velocity, globalVelocity, &info, options);
    break;

  }
  case SICONOS_FRICTION_3D_PRIMAL_DSFP_WR:
  {
    if (verbose == 1)
      printf(" ========================== Call DSFP_WR solver with reformulation into Friction-Contact 3D problem ==========================\n");
    Primal_ipiv = NULL;
    Primal_MisInverse = 0;
    Primal_MisLU = 0;
    primalFrictionContact3D_DeSaxceFixedPoint_wr(problem, reaction , velocity, globalVelocity, &info, options);
    break;

  }
  case SICONOS_FRICTION_3D_PRIMAL_TFP_WR:
  {
    if (verbose == 1)
      printf(" ========================== Call TFP_WR solver with reformulation into Friction-Contact 3D problem ==========================\n");
    Primal_ipiv = NULL;
    Primal_MisInverse = 0;
    Primal_MisLU = 0;
    primalFrictionContact3D_TrescaFixedPoint_wr(problem, reaction , velocity, globalVelocity, &info, options);
    break;

  }
  case SICONOS_FRICTION_3D_PRIMAL_NSGS:
  {
    Primal_ipiv = NULL;
    Primal_MisInverse = 0;
    Primal_MisLU = 0;
    primalFrictionContact3D_nsgs(problem, reaction , velocity, globalVelocity, &info , options);
    break;

  }
  default:
  {
    fprintf(stderr, "Numerics, PrimalFrictionContact3D_driver failed. Unknown solver.\n");
    exit(EXIT_FAILURE);

  }
  }

  return info;

}

int checkTrivialCasePrimal(int n, double* q, double* velocity, double* reaction, double * globalVelocity, int* iparam, double* dparam)
{
  /* norm of vector q */
  /*   double qs = DNRM2( n , q , 1 ); */
  /*   int i; */
  int info = -1;
  /*   if( qs <= DBL_EPSILON )  */
  /*     { */
  /*       // q norm equal to zero (less than DBL_EPSILON) */
  /*       // -> trivial solution: reaction = 0 and velocity = q */
  /*       for( i = 0 ; i < n ; ++i ) */
  /*  { */
  /*    velocity[i] = q[i]; */
  /*    reaction[i] = 0.; */
  /*  } */
  /*       iparam[2] = 0; */
  /*       iparam[4]= 0; */
  /*       dparam[1] = 0.0; */
  /*       dparam[3] = 0.0; */
  /*       info = 0; */
  /*       if(iparam[1]>0) */
  /*  printf("FrictionContact3D driver, norm(q) = 0, trivial solution reaction = 0, velocity = q.\n"); */
  /*     } */
  return info;
}

