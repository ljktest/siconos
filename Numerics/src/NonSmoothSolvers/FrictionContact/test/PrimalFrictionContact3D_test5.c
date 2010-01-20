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
#include "NonSmoothDrivers.h"
#include "primalFrictionContact_test_function.h"

int setSolver_Options(Solver_Options * options)
{

  int i;

  strcpy(options->solverName, "DSFP_WR");
  printf("solverName ==> %s\n", options->solverName);
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
  options->iparam[0] = 2001;
  options->dparam[0] = 1e-03;
  options->dparam[3] = 0.001;

  return 0;
}





int main(void)
{
  int info = 0 ;

  char filename[50] = "./data/Example_PrimalFrictionContact.dat";

  printf("Test on %s\n", filename);

  FILE * finput  =  fopen(filename, "r");

  int nbsolvers = 1;
  Solver_Options * options = (Solver_Options *)malloc(nbsolvers * sizeof(*options));
  info = setSolver_Options(options);

  info = primalFrictionContact_test_function(finput, options);

  for (int i = 0; i < nbsolvers; i++)
  {
    free(options[i].iparam);
    free(options[i].dparam);
    if (!options[i].dWork) free(options[i].dWork);
    if (!options[i].iWork) free(options[i].iWork);
  }
  free(options);
  fclose(finput);
  printf("End of test on %s\n", filename);


  return info;
}
