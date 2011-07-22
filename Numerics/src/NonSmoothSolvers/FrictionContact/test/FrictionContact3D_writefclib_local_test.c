/* Siconos-Numerics, Copyright INRIA 2005-2011.
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
#include <stdlib.h>]
#include <string.h>
#include "NonSmoothDrivers.h"
#include "fclib_interface.h"


int main(void)
{
  printf("\n Start of test \n");
  int info = 0 ;

  char filename[50] = "./data/Confeti-ex03-Fc3D-SBM.dat";
  printf("Test on %s\n", filename);


  /* Remove file if it exists */
  FILE * foutput = fopen("./data/Confeti-ex03-Fc3D-SBM.hdf5", "w");
  fclose(foutput);


  FILE * f  =  fopen(filename, "r");

  FrictionContactProblem* problem = (FrictionContactProblem *)malloc(sizeof(FrictionContactProblem));

  info = frictionContact_newFromFile(problem, f);

  int n = 50;
  char * title = (char *)malloc(n * sizeof(char *));
  strcpy(title, "Confeti-ex03-Fc3D-SBM");
  char * description = (char *)malloc(n * sizeof(char *));
  strcpy(description, " rewriting siconos test ./data/Confeti-ex03-Fc3D-SBM.dat");
  char * math_info = (char *)malloc(n * sizeof(char *));
  strcpy(math_info,  "unknown");

  frictionContact_fclib_write(problem,
                              title,
                              description,
                              math_info,
                              "./data/Confeti-ex03-Fc3D-SBM.hdf5");


  freeFrictionContact_problem(problem);
  printf("\n End of test \n");
  return info;
}
