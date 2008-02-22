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
#include <time.h>
#ifndef MEXFLAG
#include "NonSmoothDrivers.h"
#endif

int pfc_2D_driver(FrictionContact_Problem* problem, double *reaction , double *velocity, Solver_Options* options, Numerics_Options* global_options)
{
  if (options == NULL || global_options == NULL)
    numericsError("pfc_2D_driver", "null input for solver and/or global options");

  /* Set global options */
  setNumericsOptions(global_options);

  /* Checks inputs */
  if (problem == NULL || reaction == NULL || velocity == NULL)
    numericsError("pfc_2D_driver", "null input for FrictionContact_Problem and/or unknowns (reaction,velocity)");

  /* Output info. : 0: ok -  >0: problem (depends on solver) */
  int info = -1;

  int storageType = problem->M->storageType;
  if (storageType == 1)
    numericsError("pfc_2D_driver", "not yet implemented for Sparse Block Storage");

  /* If the options for solver have not been set, read default values in .opt file */
  int NoDefaultOptions = options->isSet; /* true(1) if the Solver_Options structure has been filled in else false(0) */

  if (NoDefaultOptions == 0)
  {
    readSolverOptions(2, options);
    options->filterOn = 1;
  }

  if (verbose > 0)
    printSolverOptions(options);

  /*************************************************
   *  1 - Call specific solver
   *************************************************/

  /* Solver name */
  char * name = options->solverName;
  if (verbose == 1)
    printf(" ========================== Call %s solver for primal Friction Contact 2D problem ==========================\n", name);

  /****** NLGS algorithm ******/
  if (strcmp(name , "NLGS") == 0 || strcmp(name , "PGS") == 0)
    pfc_2D_nlgs(problem, reaction, velocity, &info, options);

  /****** CPG algorithm ******/
  else if (strcmp(name , "CPG") == 0)
    pfc_2D_cpg(problem, reaction, velocity, &info, options);

  /****** Latin algorithm ******/
  else if (strcmp(name , "Latin") == 0)
    pfc_2D_latin(problem, reaction, velocity, &info, options);

  /*error */
  else
  {
    fprintf(stderr, "pfc_2D_driver error: unknown solver named: %s\n", name);
    exit(EXIT_FAILURE);
  }

  return info;

}
