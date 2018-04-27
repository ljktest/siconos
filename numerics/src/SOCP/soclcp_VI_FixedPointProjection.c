/* Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 *
 * Copyright 2018 INRIA.
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

#include "SiconosBlas.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "SecondOrderConeLinearComplementarityProblem_as_VI.h"
#include "VariationalInequality_Solvers.h"
#include "SOCLCP_Solvers.h"
#include "soclcp_compute_error.h"

#include "SolverOptions.h"
#include "numerics_verbose.h"

void soclcp_VI_FixedPointProjection(SecondOrderConeLinearComplementarityProblem* problem, double *reaction, double *velocity, int* info, SolverOptions* options)
{
  /* Dimension of the problem */
  int n = problem->n;

  VariationalInequality *vi = (VariationalInequality *)malloc(sizeof(VariationalInequality));

  //vi.self = &vi;
  vi->F = &Function_VI_SOCLCP;
  vi->ProjectionOnX = &Projection_VI_SOCLCP;

  int iter=0;
  double error=1e24;

  SecondOrderConeLinearComplementarityProblem_as_VI *soclcp_as_vi= (SecondOrderConeLinearComplementarityProblem_as_VI*)malloc(sizeof(SecondOrderConeLinearComplementarityProblem_as_VI));
  vi->env =soclcp_as_vi ;
  vi->size =  n;

  /*set the norm of the VI to the norm of problem->q  */
  vi->normVI= cblas_dnrm2(n , problem->q , 1);
  vi->istheNormVIset=1;

  soclcp_as_vi->vi = vi;
  soclcp_as_vi->soclcp = problem;
  /* frictionContact_display(fc3d_as_vi->fc3d); */

  SolverOptions * visolver_options = (SolverOptions *) malloc(sizeof(SolverOptions));
  variationalInequality_setDefaultSolverOptions(visolver_options,
                                                SICONOS_VI_FPP);

  int isize = options->iSize;
  int dsize = options->dSize;
  int vi_isize = visolver_options->iSize;
  int vi_dsize = visolver_options->dSize;
  if (isize != vi_isize )
  {
    printf("size problem in soclcp_VI_FixedPointProjection\n");
  }
  if (dsize != vi_dsize )
  {
    printf("size problem in soclcp_VI_FixedPointProjection\n");
  }
  int i;

  for (i = 0; i < min(isize,vi_isize); i++)
  {
    if (options->iparam[i] != 0 )
      visolver_options->iparam[i] = options->iparam[i] ;
  }
  for (i = 0; i < min(dsize,vi_dsize); i++)
  {
    if (fabs(options->dparam[i]) >= 1e-24 )
      visolver_options->dparam[i] = options->dparam[i] ;
  }

  
  variationalInequality_FixedPointProjection(vi, reaction, velocity , info , visolver_options);



  /* **** Criterium convergence **** */
  soclcp_compute_error(problem, reaction , velocity, options->dparam[0], options, &error);

  /* for (i =0; i< n ; i++) */
  /* { */
  /*   printf("reaction[%i]=%f\t",i,reaction[i]);    printf("velocity[%i]=F[%i]=%f\n",i,i,velocity[i]); */
  /* } */

  error = visolver_options->dparam[SICONOS_DPARAM_RESIDU];
  iter = visolver_options->iparam[SICONOS_IPARAM_ITER_DONE];

  options->dparam[SICONOS_DPARAM_RESIDU] = error;
  options->dparam[SICONOS_VI_EG_DPARAM_RHO] = visolver_options->dparam[SICONOS_VI_EG_DPARAM_RHO];
  options->iparam[SICONOS_IPARAM_ITER_DONE] = iter;


  if (verbose > 0)
  {
    printf("---------------SOCLCP - VI Fixed Point Projection (VI_FPP) - #Iteration %i Final Residual = %14.7e\n", iter, error);
  }
  free(vi);

  solver_options_delete(visolver_options);
  free(visolver_options);
  free(soclcp_as_vi);



}


int soclcp_VI_FixedPointProjection_setDefaultSolverOptions(SolverOptions* options)
{
  if (verbose > 0)
  {
    printf("Set the Default SolverOptions for the FixedPointProjection Solver\n");
  }
  variationalInequality_FixedPointProjection_setDefaultSolverOptions(options);
  options->solverId = SICONOS_SOCLCP_VI_FPP;
  options->iparam[SICONOS_IPARAM_MAX_ITER] =2000;
  return 0;
}
