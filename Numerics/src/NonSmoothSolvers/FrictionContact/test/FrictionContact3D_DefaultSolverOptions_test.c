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
#include "frictionContact_test_function.h"


int main(void)
{
  printf("\n Start of test on Default Solver_Options\n");
  int info = 0 ;
  Solver_Options * options;

  info = frictionContact3D_setDefaultSolverOptions(&options, "NSGS");
  printSolverOptions(options);
  printSolverOptions(&options[1]);
  frictionContact3D_deleteDefaultSolverOptions(&options, "NSGS");

  info = frictionContact3D_setDefaultSolverOptions(&options, "NSGSV");
  printSolverOptions(options);
  printSolverOptions(&options[1]);
  frictionContact3D_deleteDefaultSolverOptions(&options, "NSGSV");

  info = frictionContact3D_setDefaultSolverOptions(&options, "PROX");
  printSolverOptions(options);
  printSolverOptions(&options[1]);
  printSolverOptions(&options[2]);
  frictionContact3D_deleteDefaultSolverOptions(&options, "PROX");

  info = frictionContact3D_setDefaultSolverOptions(&options, "TFP");
  printSolverOptions(options);
  printSolverOptions(&options[1]);
  printSolverOptions(&options[2]);
  frictionContact3D_deleteDefaultSolverOptions(&options, "TFP");

  info = frictionContact3D_setDefaultSolverOptions(&options, "DSFP");
  printSolverOptions(options);

  frictionContact3D_deleteDefaultSolverOptions(&options, "DSFP");

  printf("\n End of test on Default Solver_Options\n");
  return info;
}
