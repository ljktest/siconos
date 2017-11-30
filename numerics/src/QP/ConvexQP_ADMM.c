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


#include "ConvexQP_Solvers.h"
#include "ConvexQP_computeError.h"
#include "NumericsMatrix.h"
#include "SparseBlockMatrix.h"
#include "NumericsMatrix.h"
#include "NumericsSparseMatrix.h"
#include "SiconosBlas.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "ConvexQP_cst.h"
#include "numerics_verbose.h"

//#define VERBOSE_DEBUG
const char* const   SICONOS_CONVEXQP_ADMM_STR = "CONVEXQP ADMM";


void convexQP_ADMM(ConvexQP* problem, double *z, double *w, int* info, SolverOptions* options)
{
  /* int and double parameters */
  int* iparam = options->iparam;
  double* dparam = options->dparam;



  double* q = problem->q;
  NumericsMatrix* M = problem->M;

  NumericsMatrix* A = problem->A;
  double * b = problem->b;

  /* Dimension of the problem */
  int n =  problem->size;
  if (A) /* A is considered to be the identity and b =0 */
  {
    problem->m = n;
    if (b)
    {

    }

  }

  int m =  problem->m;

  /* Maximum number of iterations */
  int itermax = iparam[SICONOS_IPARAM_MAX_ITER];
  /* Tolerance */
  double tolerance = dparam[SICONOS_DPARAM_TOL];

  /*****  ADMM iterations *****/
  int iter = 0; /* Current iteration number */
  double error = 1.; /* Current error */
  int hasNotConverged = 1;

  double rho = 0.0;
 
  rho = dparam[SICONOS_CONVEXQP_PGOC_RHO];

  if (rho == 0.0)
    numerics_error("ConvexQP_ADMM", "dparam[SICONOS_CONVEXQP_PGOC_RHO] must be nonzero");

  double * z_k;
  double * w_k;
  double * u_k;
  double * u_tmp;
  double * xsi_k;
  double * xsi_tmp;

  z_k = (double *)malloc(n * sizeof(double));
  w_k = (double *)malloc(n * sizeof(double));
  u_k = (double *)calloc(m,sizeof(double));
  u_tmp = (double *)calloc(m,sizeof(double));
  xsi_k = (double *)calloc(m,sizeof(double));
  xsi_tmp = (double *)calloc(m,sizeof(double));


  double alpha = 1.0;
  double beta = 1.0;


  /* Compute M + rho A^T A (storage in M)*/
  NumericsMatrix *Atrans;
  if (A)
  {
    if (M->storageType != A->storageType)
    {
      numerics_error("ConvexQP_ADMM", "M and A must have the same storage");
    }
  }
  if (A)
  {
    NM_add_to_diag3(M, rho);
  }
  else
  {

    if (M->storageType == NM_SPARSE_BLOCK)
    {
      Atrans = NM_create(NM_SPARSE_BLOCK, A->size1, A->size0);
      SBM_transpose(A->matrix1, Atrans->matrix1);
    }
    else if (A->storageType == NM_SPARSE)
    {
      NM_csc_trans(A);
      Atrans = NM_new();
      Atrans->storageType = NM_SPARSE;
      Atrans-> size0 = m;
      Atrans-> size1 = n;
      NM_csc_alloc(Atrans, 0);
      Atrans->matrix2->origin = NS_CSC;
      Atrans->matrix2->csc = NM_csc_trans(A);
    }
    else
    {
      printf("gfc3d_reformulation_local_problem :: unknown matrix storage");
      exit(EXIT_FAILURE);
    }
    NM_gemm(rho, Atrans, A, beta, M);
  }




  while ((iter < itermax) && (hasNotConverged > 0))
  {
    ++iter;
    /* compute the rhs */
    /* q --> w_k */
    cblas_dcopy(n , q , 1 , w_k, 1);
    /* b+ xsi_k - u_k --> u_k */
    cblas_daxpy(m, alpha, b, -1, u_k , 1);
    cblas_daxpy(m, 1, xsi_k, 1, u_k , 1);
    /* rho A^T (b+ xsi_k - u_k ) + q --> w_k */
    NM_gemv(rho, Atrans, u_k, beta, w_k);

    /* Linear system solver */
    cblas_dcopy(n , w_k , 1 , z_k, 1);
    NM_gesv_expert(M,z_k,NM_KEEP_FACTORS);

    /* A z_k + xsi_k + b */
    cblas_dcopy(m , b , 1 , u_tmp, 1);
    cblas_daxpy(m, 1, xsi_k, 1, u_tmp , 1);
    NM_gemv(rho, A, z_k, beta, u_tmp);
    problem->ProjectionOnC(problem,u_tmp,u_k);

    /* */
    cblas_dcopy(m , b , 1 , xsi_k, 1);
    NM_gemv(rho, A, z_k, beta, xsi_k);
    cblas_daxpy(m, -1, u_k, 1, xsi_k , 1);
    
    cblas_daxpy(m,rho, u_k, 0 , w_k, 1);

    /* **** Criterium convergence **** */
    convexQP_computeError(problem, z_k , w_k, tolerance, options, &error);

    if (verbose > 0)
      printf("--------------- ConvexQP - Projected Gradient (PG) - Iteration %i rho = %14.7e \tError = %14.7e\n", iter, rho, error);

    if (error < tolerance) hasNotConverged = 0;
    *info = hasNotConverged;
  }


  //verbose=1;

  if (verbose > 0)
  printf("---------------  ConvexQP - Projected Gradient (PG) - #Iteration %i Final Residual = %14.7e\n", iter, error);
  dparam[SICONOS_DPARAM_RESIDU] = error;
  iparam[SICONOS_IPARAM_ITER_DONE] = iter;

  free(z_k);
  free(w_k);
  free(u_k);
  free(xsi_k);

}


int convexQP_ADMM_setDefaultSolverOptions(SolverOptions* options)
{
  if (verbose > 0)
  {
    printf("Set the Default SolverOptions for the ADMM Solver\n");
  }

  options->solverId = SICONOS_CONVEXQP_ADMM;

  options->numberOfInternalSolvers = 0;
  options->isSet = 1;
  options->filterOn = 1;
  options->iSize = 20;
  options->dSize = 20;

  options->iparam = (int *)calloc(options->iSize, sizeof(int));
  options->dparam = (double *)calloc(options->dSize, sizeof(double));
  options->dWork = NULL;
  solver_options_nullify(options);

  options->iparam[SICONOS_IPARAM_MAX_ITER] = 20000;

  options->dparam[SICONOS_DPARAM_TOL] = 1e-6;

  options->dparam[SICONOS_CONVEXQP_PGOC_RHO] = -1.e-3; /* rho is variable by default */
  options->dparam[SICONOS_CONVEXQP_PGOC_RHOMIN] = 1e-9;
  options->dparam[SICONOS_CONVEXQP_PGOC_LINESEARCH_MU] =0.9;
  options->dparam[SICONOS_CONVEXQP_PGOC_LINESEARCH_TAU]  = 2.0/3.0;

  options->internalSolvers = NULL;


  return 0;
}
