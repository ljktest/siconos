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
#include "lcp_test_function.h"


void fillParamWithRespectToSolver(Solver_Options *options, char * solvername, LinearComplementarity_Problem* problem)
{
  int maxIter = 1001;
  double tolerance = 1e-8;
  double lighttolerance = 1e-5;

  if (strcmp(solvername , "PGS") == 0 || strcmp(solvername , "CPG") == 0 || strcmp(solvername , "Lemke") == 0 || strcmp(solvername , "NewtonMin") == 0)
  {
    options->iSize = 2;
    options->dSize = 2;
    options->iparam[0] = maxIter;
    options->dparam[0] = tolerance;
  }

  else if (strcmp(solvername , "RPGS") == 0)
  {
    options->iSize = 2;
    options->dSize = 3;
    options->iparam[0] = maxIter;
    options->dparam[0] = tolerance;
    options->dparam[2] = 1.0;
  }
  else if (strcmp(solvername , "Latin") == 0)
  {
    options->iSize = 2;
    options->dSize = 3;
    options->iparam[0] = maxIter;
    options->dparam[0] = lighttolerance;
    options->dparam[2] = 0.3;
  }
  else if (strcmp(solvername , "Latin_w") == 0)
  {
    options->iSize = 2;
    options->dSize = 4;
    options->iparam[0] = maxIter;
    options->dparam[0] = lighttolerance;
    options->dparam[2] = 0.3;
    options->dparam[3] = 1.0;
  }
  else if (strcmp(solvername , "PATH") == 0 || strcmp(solvername , "QP") == 0 || strcmp(solvername , "NSQP") == 0)
  {
    options->iSize = 0;
    options->dSize = 2;
    options->dparam[0] = tolerance;
  }
  else if (strcmp(solvername , "ENUM") == 0)
  {
    options->iSize = 2;
    options->dSize = 2;
    options->dparam[0] = tolerance;
    options->dWork = (double*) malloc((3 * problem->size + problem->size * problem->size) * sizeof(double));
    options->iWork = (int*) malloc(2 * problem->size * sizeof(int));
  }
  else if (strcmp(solvername , "NewtonFB") == 0)
  {
    options->iSize = 2;
    options->dSize = 2;
    options->iparam[0] = maxIter;
    options->dparam[0] = tolerance;

  }


}


void fillParamWithRespectToSolver_SBM(Solver_Options *options, char * solvername, LinearComplementarity_Problem* problem)
{
  int maxIter = 1001;
  double tolerance = 1e-8;
  double lighttolerance = 1e-8;

  if (strcmp(solvername , "PGS") == 0 || strcmp(solvername , "CPG") == 0 || strcmp(solvername , "Lemke") == 0 || strcmp(solvername , "NewtonMin") == 0)
  {
    options->iparam[0] = maxIter;
    options->dparam[0] = tolerance;
  }

  else if (strcmp(solvername , "RPGS") == 0)
  {
    options->iparam[0] = maxIter;
    options->dparam[0] = tolerance;
    options->dparam[2] = 1.0;
  }
  else if (strcmp(solvername , "Latin") == 0)
  {
    options->iparam[0] = maxIter;
    options->dparam[0] = lighttolerance;
    options->dparam[2] = 1.0;
  }
  else if (strcmp(solvername , "Latin_w") == 0)
  {
    options->iparam[0] = maxIter;
    options->dparam[0] = lighttolerance;
    options->dparam[2] = 0.3;
    options->dparam[3] = 1.0;
  }
  else if (strcmp(solvername , "PATH") == 0 || strcmp(solvername , "QP") == 0 || strcmp(solvername , "NSQP") == 0)
  {
    options->dparam[0] = tolerance;
  }
  else if (strcmp(solvername , "ENUM") == 0)
  {
    options->dparam[0] = tolerance;
    options->dWork = (double*) malloc((3 * problem->size + problem->size * problem->size) * sizeof(double));
    options->iWork = (int*) malloc(2 * problem->size * sizeof(int));
  }
  else if (strcmp(solvername , "NewtonFB") == 0)
  {
    options->iparam[0] = maxIter;
    options->dparam[0] = tolerance;

  }


}



int lcp_test_function(FILE * f, char * solvername)
{

  int i, info = 0 ;
  LinearComplementarity_Problem* problem = (LinearComplementarity_Problem *)malloc(sizeof(LinearComplementarity_Problem));

  info = linearComplementarity_newFromFile(problem, f);

  FILE * foutput  =  fopen("./lcp_mmc.verif", "w");
  info = linearComplementarity_printInFile(problem, foutput);


  Numerics_Options global_options;
  global_options.verboseMode = 1;
  Solver_Options * options ;
  options = malloc(sizeof(*options));

  strcpy(options->solverName, solvername);
  printf("solverName ==> %s\n", options->solverName);
  options->iSize = 10;
  options->dSize = 10;
  options->iparam = (int *)malloc(options->iSize * sizeof(int));
  options->dparam = (double *)malloc(options->dSize * sizeof(double));
  for (i = 0; i < 10; i++)
  {
    options->iparam[i] = 0;
    options->dparam[i] = 0.0;
  }
  fillParamWithRespectToSolver(options, solvername, problem);

  options->isSet = 1;
  options->filterOn = 1;
  double * z = malloc(problem->size * sizeof(double));
  double * w = malloc(problem->size * sizeof(double));
  for (i = 0; i < problem->size; i++)
  {
    z[i] = 0.0;
    w[i] = 0.0;
  }



  info = linearComplementarity_driver(problem, z , w, options, &global_options);

  for (i = 0 ; i < problem->size ; i++)
  {
    printf("z[%i] = %12.8e\t,w[%i] = %12.8e\n", i, z[i], i, w[i]);
  }

  if (!info)
  {
    printf("test succeeded\n");
  }
  else
  {
    printf("test unsucceeded\n");
  }
  free(z);
  free(w);

  free(options->iparam);
  free(options->dparam);


  if (!options->dWork) free(options->dWork);
  if (!options->iWork) free(options->iWork);

  free(options);

  freeLinearComplementarity_problem(problem);
  printf("End of test on ./data/lcp_mmc.dat\n");


  return info;


}

int lcp_test_function_SBM(FILE * f, char * solvername)
{

  int i, info = 0 ;
  LinearComplementarity_Problem* problem = (LinearComplementarity_Problem *)malloc(sizeof(LinearComplementarity_Problem));

  info = linearComplementarity_newFromFile(problem, f);

  FILE * foutput  =  fopen("./lcp_mmc.verif", "w");
  info = linearComplementarity_printInFile(problem, foutput);


  Numerics_Options global_options;
  global_options.verboseMode = 1;

  Solver_Options * options = (Solver_Options *)malloc(sizeof(Solver_Options));



  info = linearComplementarity_setDefaultSolverOptions(problem, options, "PGS_SBM");

  strcpy(options->internalSolvers->solverName, solvername);

  fillParamWithRespectToSolver_SBM(options->internalSolvers, solvername, problem);



  double * z = malloc(problem->size * sizeof(double));
  double * w = malloc(problem->size * sizeof(double));

  for (i = 0; i < problem->size; i++)
  {
    z[i] = 0.0;
    w[i] = 0.0;
  }


  info = linearComplementarity_driver(problem, z , w, options, &global_options);

  for (i = 0 ; i < problem->size ; i++)
  {
    printf("z[%i] = %12.8e\t,w[%i] = %12.8e\n", i, z[i], i, w[i]);
  }

  if (!info)
  {
    printf("test succeeded\n");
  }
  else
  {
    printf("test unsucceeded\n");
  }
  free(z);
  free(w);
  // info = linearComplementarity_deleteDefaultSolverOptions(&options,solvername);


  deleteSolverOptions(options);
  free(options);

  freeLinearComplementarity_problem(problem);


  return info;


}


