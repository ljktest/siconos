/* Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 *
 * Copyright 2016 INRIA.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
#include <stdio.h>
#include <stdlib.h>
#include "NonSmoothDrivers.h"
#include "globalFrictionContact_test_function.h"
#include "Friction_cst.h"
#include "gfc3d_Solvers.h"
#include "SolverOptions.h"



int main(void)
{
  int info = 0 ;

  char filename[50] = "./data/problem-checkTwoRods1.dat";

  printf("Test on %s\n", filename);

  FILE * finput  =  fopen(filename, "r");

  SolverOptions * options = (SolverOptions *)malloc(sizeof(SolverOptions));

  gfc3d_setDefaultSolverOptions(options, SICONOS_GLOBAL_FRICTION_3D_NSGS_WR);
  options->internalSolvers->internalSolvers->solverId = SICONOS_FRICTION_3D_ONECONTACT_NSN_AC_GP;
  options->internalSolvers->internalSolvers->iparam[0] = 100;
  options->internalSolvers->internalSolvers->iparam[1] = 15;

  info = globalFrictionContact_test_function(finput, options);
  solver_options_delete(options);
  free(options);
  fclose(finput);
  printf("End of test on %s\n", filename);


  return info;
}
