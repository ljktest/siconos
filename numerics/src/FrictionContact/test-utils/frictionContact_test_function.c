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
#include "frictionContact_test_function.h"

#if defined(WITH_FCLIB)
#include <fclib.h>
#include <fclib_interface.h>
#endif


void frictionContact_test_gams_opts(SN_GAMSparams* GP, int solverId)
{
  if (solverId == SICONOS_FRICTION_3D_GAMS_PATHVI ||
      solverId == SICONOS_FRICTION_3D_GAMS_LCP_PATHVI ||
      solverId == SICONOS_GLOBAL_FRICTION_3D_GAMS_PATHVI
      )
  {
    add_GAMS_opt_str(GP, "avi_start", "ray_first", GAMS_OPT_SOLVER);
    add_GAMS_opt_str(GP, "ratio_tester", "expand", GAMS_OPT_SOLVER);
    add_GAMS_opt_double(GP, "expand_eps", 0., GAMS_OPT_SOLVER);
    add_GAMS_opt_bool(GP, "ratio_tester_tfirst", false, GAMS_OPT_SOLVER);
//    add_GAMS_opt_int(GP, "scheduler_decompose", 1, GAMS_OPT_SOLVER);
//    add_GAMS_opt_str(GP, "lemke_factorization_method", "minos_blu", GAMS_OPT_SOLVER);
  }
  else if (solverId == SICONOS_FRICTION_3D_GAMS_PATH ||
      solverId == SICONOS_FRICTION_3D_GAMS_LCP_PATH ||
      solverId == SICONOS_GLOBAL_FRICTION_3D_GAMS_PATH
      )
  {
    add_GAMS_opt_int(GP, "linear_model_perturb", 0, GAMS_OPT_SOLVER);
    add_GAMS_opt_double(GP, "proximal_perturbation", 0., GAMS_OPT_SOLVER);
    add_GAMS_opt_double(GP, "proximal_initial_maximum", 0., GAMS_OPT_SOLVER);
    add_GAMS_opt_str(GP, "crash_method", "none", GAMS_OPT_SOLVER);
    add_GAMS_opt_int(GP, "crash_perturb", 0, GAMS_OPT_SOLVER);
    add_GAMS_opt_int(GP, "restart_limit", 0, GAMS_OPT_SOLVER);
//    add_GAMS_opt_str(GP, "lemke_start", "first", GAMS_OPT_SOLVER);
//    add_GAMS_opt_int(GP, "output_linear_model", 1, GAMS_OPT_SOLVER);
//    add_GAMS_opt_int(GP, "output_minor_iterations_frequency", 1, GAMS_OPT_SOLVER);
//    add_GAMS_opt_int(GP, "output_linear_model", 1, GAMS_OPT_SOLVER);

  }
  else
  {
    fprintf(stderr, "frictionContact_test_gams_opts :: ERROR unkown solverId = %d e.g. solver named %s", solverId, idToName(solverId));
  }
  add_GAMS_opt_int(GP, "minor_iteration_limit", 100000, GAMS_OPT_SOLVER);
  add_GAMS_opt_int(GP, "major_iteration_limit", 20, GAMS_OPT_SOLVER);
  add_GAMS_opt_double(GP, "expand_delta", 1e-10, GAMS_OPT_SOLVER);

}

int frictionContact_test_function(FILE * f, SolverOptions * options)
{

  int k, info = -1 ;
  FrictionContactProblem* problem = (FrictionContactProblem *)malloc(sizeof(FrictionContactProblem));

  info = frictionContact_newFromFile(problem, f);

  FILE * foutput  =  fopen("checkinput.dat", "w");
  info = frictionContact_printInFile(problem, foutput);

  NumericsOptions global_options;
  setDefaultNumericsOptions(&global_options);
  global_options.verboseMode = 1; // turn verbose mode to off by default

  int NC = problem->numberOfContacts;
  int dim = problem->dimension;
  //int dim = problem->numberOfContacts;
  
  double *reaction = (double*)calloc(dim * NC, sizeof(double));
  double *velocity = (double*)calloc(dim * NC, sizeof(double));

  if (dim == 2)
  {
    info = fc2d_driver(problem,
                                    reaction , velocity,
                                    options, &global_options);
  }
  else if (dim == 3)
  {
    info = fc3d_driver(problem,
                                    reaction , velocity,
                                    options, &global_options);
  }
  else
  {
    info = 1;
  }
  printf("\n");

  int print_size =10;

  if  (dim * NC >= print_size)
  {
    printf("First values (%i)\n", print_size);
    for (k = 0 ; k < print_size; k++)
    {
      printf("Velocity[%i] = %12.8e \t \t Reaction[%i] = %12.8e\n", k, velocity[k], k , reaction[k]);
    }
    printf(" ..... \n");
  }
  else
  {
    for (k = 0 ; k < dim * NC; k++)
    {
      printf("Velocity[%i] = %12.8e \t \t Reaction[%i] = %12.8e\n", k, velocity[k], k , reaction[k]);
    }
    printf("\n");
  }

  /* for (k = 0 ; k < dim * NC; k++) */
  /* { */
  /*   printf("Velocity[%i] = %12.8e \t \t Reaction[%i] = %12.8e\n", k, velocity[k], k , reaction[k]); */
  /* } */
  /* printf("\n"); */

  if (!info)
  {
    printf("test successful, residual = %g\n", options->dparam[1]);
  }
  else
  {
    printf("test unsuccessful, residual = %g\n", options->dparam[1]);
  }
  free(reaction);
  free(velocity);

  freeFrictionContactProblem(problem);
  fclose(foutput);

  return info;

}


#if defined(WITH_FCLIB)
int frictionContact_test_function_hdf5(const char * path, SolverOptions * options)
{

  int k, info = -1 ;
  /* FrictionContactProblem* problem = (FrictionContactProblem *)malloc(sizeof(FrictionContactProblem)); */
  /* info = frictionContact_newFromFile(problem, f); */
  
  FrictionContactProblem* problem = frictionContact_fclib_read(path);
  FILE * foutput  =  fopen("checkinput.dat", "w");
  info = frictionContact_printInFile(problem, foutput);

  
  NumericsOptions global_options;
  setDefaultNumericsOptions(&global_options);
  global_options.verboseMode = 1; // turn verbose mode to off by default

  int NC = problem->numberOfContacts;
  int dim = problem->dimension;
  //int dim = problem->numberOfContacts;
  
  double *reaction = (double*)calloc(dim * NC, sizeof(double));
  double *velocity = (double*)calloc(dim * NC, sizeof(double));

  if (dim == 2)
  {
    info = fc2d_driver(problem,
                                    reaction , velocity,
                                    options, &global_options);
  }
  else if (dim == 3)
  {
    info = fc3d_driver(problem,
                                    reaction , velocity,
                                    options, &global_options);
  }
  else
  {
    info = 1;
  }
  printf("\n");

  int print_size =10;

  if  (dim * NC >= print_size)
  {
    printf("First values (%i)\n", print_size);
    for (k = 0 ; k < print_size; k++)
    {
      printf("Velocity[%i] = %12.8e \t \t Reaction[%i] = %12.8e\n", k, velocity[k], k , reaction[k]);
    }
    printf(" ..... \n");
  }
  else
  {
    for (k = 0 ; k < dim * NC; k++)
    {
      printf("Velocity[%i] = %12.8e \t \t Reaction[%i] = %12.8e\n", k, velocity[k], k , reaction[k]);
    }
    printf("\n");
  }

  /* for (k = 0 ; k < dim * NC; k++) */
  /* { */
  /*   printf("Velocity[%i] = %12.8e \t \t Reaction[%i] = %12.8e\n", k, velocity[k], k , reaction[k]); */
  /* } */
  /* printf("\n"); */

  if (!info)
  {
    printf("test successful, residual = %g\n", options->dparam[1]);
  }
  else
  {
    printf("test unsuccessful, residual = %g\n", options->dparam[1]);
  }
  free(reaction);
  free(velocity);

  freeFrictionContactProblem(problem);
  fclose(foutput);

  return info;

}

int gfc3d_test_function_hdf5(const char* path, SolverOptions* options)
{

  int k, info = -1 ;

  GlobalFrictionContactProblem* problem = globalFrictionContact_fclib_read(path);
  FILE * foutput  =  fopen("checkinput.dat", "w");
  info = globalFrictionContact_printInFile(problem, foutput);

  NumericsOptions global_options;
  setDefaultNumericsOptions(&global_options);
  global_options.verboseMode = 1; // turn verbose mode to off by default

  int NC = problem->numberOfContacts;
  int dim = problem->dimension;
  int n = problem->M->size0;

  double *reaction = (double*)calloc(dim * NC, sizeof(double));
  double *velocity = (double*)calloc(dim * NC, sizeof(double));
  double *global_velocity = (double*)calloc(n, sizeof(double));

  if (dim == 3)
  {
    info = gfc3d_driver(problem, reaction, velocity, global_velocity, options, &global_options);
  }
  else
  {
    fprintf(stderr, "gfc3d_test_function_hdf5 :: problem size != 3\n");
    return 1;
  }
  printf("\n");

  int print_size = 10;

  if  (dim * NC >= print_size)
  {
    printf("First values (%i)\n", print_size);
    for (k = 0 ; k < print_size; k++)
    {
      printf("Velocity[%i] = %12.8e \t \t Reaction[%i] = %12.8e\n", k, velocity[k], k , reaction[k]);
    }
    printf(" ..... \n");
  }
  else
  {
    for (k = 0 ; k < dim * NC; k++)
    {
      printf("Velocity[%i] = %12.8e \t \t Reaction[%i] = %12.8e\n", k, velocity[k], k , reaction[k]);
    }
    printf("\n");
  }

  /* for (k = 0 ; k < dim * NC; k++) */
  /* { */
  /*   printf("Velocity[%i] = %12.8e \t \t Reaction[%i] = %12.8e\n", k, velocity[k], k , reaction[k]); */
  /* } */
  /* printf("\n"); */

  if (!info)
  {
    printf("test successful, residual = %g\n", options->dparam[1]);
  }
  else
  {
    printf("test unsuccessful, residual = %g\n", options->dparam[1]);
  }
  free(reaction);
  free(velocity);
  free(global_velocity);

  freeGlobalFrictionContactProblem(problem);
  fclose(foutput);

  return info;

}


#endif
