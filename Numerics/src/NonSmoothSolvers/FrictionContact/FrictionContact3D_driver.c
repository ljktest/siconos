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
#include "LA.h"
#include "NumericsOptions.h"
#include "FrictionContact3D_Solvers.h"
#include "NonSmoothDrivers.h"

int frictionContact3D_driver(FrictionContactProblem* problem, double *reaction , double *velocity, SolverOptions* options, NumericsOptions* global_options)
{
  if (options == NULL || global_options == NULL)
    numericsError("FrictionContact3D_driver", "null input for solver and/or global options");
  /* Set global options */
  setNumericsOptions(global_options);

  /* If the options for solver have not been set, read default values in .opt file */
  int NoDefaultOptions = options->isSet; /* true(1) if the SolverOptions structure has been filled in else false(0) */

  if (!NoDefaultOptions)
    readSolverOptions(3, options);

  if (verbose > 0)
    printSolverOptions(options);

  /* Solver name */
  char * name = options->solverName;


  int info = -1 ;

  if (problem->dimension != 3)
    numericsError("FrictionContact3D_driver", "Dimension of the problem : problem-> dimension is not compatible or is not set");


  /* Non Smooth Gauss Seidel (NSGS) */
  if (strcmp(name, "NSGS") == 0)
  {
    if (verbose == 1)
      printf(" ========================== Call NSGS solver for Friction-Contact 3D problem ==========================\n");
    frictionContact3D_nsgs(problem, reaction , velocity , &info , options);
  }
  else if (strcmp(name, "NSGSV") == 0)
  {
    if (verbose == 1)
      printf(" ========================== Call NSGSV solver for Friction-Contact 3D problem ==========================\n");
    frictionContact3D_nsgs_velocity(problem, reaction , velocity , &info , options);
  }
  /* Proximal point algorithm */
  else if (strcmp(name, "PROX") == 0)
  {
    if (verbose == 1)
      printf(" ========================== Call PROX (Proximal Point) solver for Friction-Contact 3D problem ==========================\n");
    frictionContact3D_proximal(problem, reaction , velocity , &info , options);
  }
  /* Tresca Fixed point algorithm */
  else if (strcmp(name, "TFP") == 0)
  {
    if (verbose == 1)
      printf(" ========================== Call TFP (Tresca Fixed Point) solver for Friction-Contact 3D problem ==========================\n");
    frictionContact3D_TrescaFixedPoint(problem, reaction , velocity , &info , options);
  }
  /* Projected Gradient algorithm */
  else if (strcmp(name, "DSFP") == 0)
  {
    if (verbose == 1)
      printf(" ========================== Call DeSaxce Fized Point (DSFP) solver for Friction-Contact 3D problem ==========================\n");
    frictionContact3D_DeSaxceFixedPoint(problem, reaction , velocity , &info , options);
  }
  /* Global Alart Curnier */
  else if (strcmp(name, "GLOBALAC") == 0)
  {
    if (verbose == 1)
      printf(" ========================== Call Global Alart Curnier solver for Friction-Contact 3D problem ==========================\n");
    frictionContact3D_GlobalAlartCurnier(problem, reaction , velocity , &info , options);
  }
  else
  {
    fprintf(stderr, "Numerics, FrictionContact3D_driver failed. Unknown solver.\n");
    exit(EXIT_FAILURE);

  }

  return info;

}

int checkTrivialCase(int n, double* q, double* velocity, double* reaction, int* iparam, double* dparam)
{
  /* norm of vector q */
  double qs = DNRM2(n , q , 1);
  int i;
  int info = -1;
  if (qs <= DBL_EPSILON)
  {
    // q norm equal to zero (less than DBL_EPSILON)
    // -> trivial solution: reaction = 0 and velocity = q
    for (i = 0 ; i < n ; ++i)
    {
      velocity[i] = q[i];
      reaction[i] = 0.;
    }
    iparam[2] = 0;
    iparam[4] = 0;
    dparam[1] = 0.0;
    dparam[3] = 0.0;
    info = 0;
    if (verbose == 1)
      printf("FrictionContact3D driver, norm(q) = 0, trivial solution reaction = 0, velocity = q.\n");
  }
  return info;
}

