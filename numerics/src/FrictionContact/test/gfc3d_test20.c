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
#include <stdio.h>
#include <stdlib.h>
#include "NonSmoothDrivers.h"
#include "globalFrictionContact_test_function.h"
#include "fclib_interface.h"



int main(void)
{

  char filename[51] = "./data/LMGC_GlobalFrictionContactProblem00046.hdf5";

  printf("Test on %s\n", filename);

  SolverOptions * options = (SolverOptions *)malloc(sizeof(SolverOptions));

  gfc3d_setDefaultSolverOptions(options, SICONOS_GLOBAL_FRICTION_3D_NSN_AC);

  int k, info = -1 ;

  GlobalFrictionContactProblem* problem = globalFrictionContact_fclib_read(filename);
  globalFrictionContact_display(problem);

  FILE * foutput  =  fopen("checkinput.dat", "w");
  info = globalFrictionContact_printInFile(problem, foutput);

  NumericsOptions global_options;
  setDefaultNumericsOptions(&global_options);
  global_options.verboseMode = 1; // turn verbose mode to off by default

  int NC = problem->numberOfContacts;
  int dim = problem->dimension;
  int n = problem->M->size1;

  double *reaction = (double*)calloc(dim * NC, sizeof(double));
  double *velocity = (double*)calloc(dim * NC, sizeof(double));
  double *globalvelocity = (double*)calloc(n, sizeof(double));

  if (dim == 2)
  {
    info = 1;
  }
  else if (dim == 3)
  {
    info = gfc3d_driver(problem,
                                          reaction , velocity, globalvelocity,
                                          options, &global_options);
  }
  printf("\n");
  for (k = 0 ; k < dim * NC; k++)
  {
    printf("Velocity[%i] = %12.8e \t \t Reaction[%i] = %12.8e\n", k, velocity[k], k , reaction[k]);
  }
  for (k = 0 ; k < n; k++)
  {
    printf("GlocalVelocity[%i] = %12.8e\n", k, globalvelocity[k]);
  }
  printf("\n");

  if (!info)
  {
    printf("test succeeded\n");
  }
  else
  {
    printf("test unsuccessful\n");
  }
  free(reaction);
  free(velocity);
  free(globalvelocity);
  fclose(foutput);

  freeGlobalFrictionContactProblem(problem);

  deleteSolverOptions(options);
  free(options);

  printf("End of test on %s\n", filename);


  return info;
}
