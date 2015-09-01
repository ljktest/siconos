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


#include "SOCLCP_Solvers.h"
#include "projectionOnCone.h"
#include "projectionOnCylinder.h"
#include "soclcp_compute_error.h"
#include "soclcp_projection.h"
#include "SiconosBlas.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#define VERBOSE_DEBUG

void soclcp_projection_initialize(SecondOrderConeLinearComplementarityProblem * problem,
                                  SecondOrderConeLinearComplementarityProblem * localproblem,
                                  SolverOptions* localsolver_options)
{
  int nc = problem->nc;
  unsigned int dim_max=0;
  for (int i =0; i <nc; i++)
  {
    dim_max=max(dim_max,problem->coneIndex[i+1]-problem->coneIndex[i]);
  }
  /* printf("soclcp_projectionOnCone_initialize. Allocation of dwork\n"); */
  localsolver_options->dWork = (double *)malloc(dim_max * sizeof(double));
}

int soclcp_projectionOnCone_solve(SecondOrderConeLinearComplementarityProblem* localproblem, double* reaction, SolverOptions * options)
{
  /*  /\* Build local problem for the current cone *\/ */
  /*   soclcp_projection_update(cone, reaction); */ 
  double * MLocal = localproblem->M->matrix0;
  double * qLocal = localproblem->q;
  double mu_i = localproblem->mu[0];
  int nLocal = localproblem->n;

  /* this part is critical for the success of the projection */
  //double an = 1./(MLocal[0]+mu_i);
  double an = 1. / (MLocal[0]);

  int incx = 1, incy = 1;
  double * worktmp = options->dWork;

  cblas_dcopy(nLocal , qLocal, incx , worktmp , incy);
  
  cblas_dgemv(CblasColMajor,CblasNoTrans, nLocal, nLocal, 1.0, MLocal, nLocal, reaction, incx, 1.0, worktmp, incy);

  for (int i =0; i < nLocal ; i++)
  {
    reaction[i] -= an * worktmp[i];
  }
  projectionOnSecondOrderCone(reaction, mu_i, nLocal);
  return 0;
}



void soclcp_projection_free(SecondOrderConeLinearComplementarityProblem * problem, SecondOrderConeLinearComplementarityProblem * localproblem, SolverOptions* localsolver_options)
{
  free(localsolver_options->dWork);
  localsolver_options->dWork=NULL;
}
void soclcp_projection_initialize_with_regularization(SecondOrderConeLinearComplementarityProblem * problem, SecondOrderConeLinearComplementarityProblem * localproblem)
{
  if(!localproblem->M->matrix0)
    localproblem->M->matrix0 = (double*)malloc(9 * sizeof(double));
}

void soclcp_projection_update_with_regularization(int cone, SecondOrderConeLinearComplementarityProblem * problem, SecondOrderConeLinearComplementarityProblem * localproblem, double* reaction, SolverOptions* options)
{


  /* Build a local problem for a specific cone
     reaction corresponds to the global vector (size n) of the global problem.
  */

  /* Call the update function which depends on the storage for MGlobal/MBGlobal */
  /* Build a local problem for a specific cone
   reaction corresponds to the global vector (size n) of the global problem.
  */

  /* The part of MGlobal which corresponds to the current block is copied into MLocal */

  NumericsMatrix * MGlobal = problem->M;

  int n = problem->n;


  int storageType = MGlobal->storageType;
  if(storageType == 0)
    // Dense storage
  {
    int in = 3 * cone, it = in + 1, is = it + 1;
    int inc = n * in;
    double * MM = MGlobal->matrix0;
    double * MLocal =  localproblem->M->matrix0;

    /* The part of MM which corresponds to the current block is copied into MLocal */
    MLocal[0] = MM[inc + in];
    MLocal[1] = MM[inc + it];
    MLocal[2] = MM[inc + is];
    inc += n;
    MLocal[3] = MM[inc + in];
    MLocal[4] = MM[inc + it];
    MLocal[5] = MM[inc + is];
    inc += n;
    MLocal[6] = MM[inc + in];
    MLocal[7] = MM[inc + it];
    MLocal[8] = MM[inc + is];
  }
  else if(storageType == 1)
  {
    int diagPos = getDiagonalBlockPos(MGlobal->matrix1, cone);
    /*     for (int i =0 ; i< 3*3 ; i++) localproblem->M->matrix0[i] = MGlobal->matrix1->block[diagPos][i] ; */
    cblas_dcopy(9, MGlobal->matrix1->block[diagPos], 1, localproblem->M->matrix0 , 1);

  }
  else
    numericsError("soclcp_projection -", "unknown storage type for matrix M");

  /****  Computation of qLocal = qBlock + sum over a row of blocks in MGlobal of the products MLocal.reactionBlock,
     excluding the block corresponding to the current cone. ****/
  soclcp_nsgs_computeqLocal(problem, localproblem, reaction, cone, options);

  double rho = options->dparam[3];
  for(int i = 0 ; i < 3 ; i++) localproblem->M->matrix0[i + 3 * i] += rho ;

  double *qLocal = localproblem->q;
  int in = 3 * cone, it = in + 1, is = it + 1;

  /* qLocal computation*/
  qLocal[0] -= rho * reaction[in];
  qLocal[1] -= rho * reaction[it];
  qLocal[2] -= rho * reaction[is];

  /* coefficient for current block*/
  localproblem->mu[0] = problem->mu[cone];


}

void soclcp_projectionOnConeWithLocalIteration_initialize(SecondOrderConeLinearComplementarityProblem * problem,
                                                          SecondOrderConeLinearComplementarityProblem * localproblem,
                                                          SolverOptions* localsolver_options)
{
  int nc = problem->nc;
  unsigned int dim_max=0;
  for (int i =0; i <nc; i++)
  {
    dim_max=max(dim_max,problem->coneIndex[i+1]-problem->coneIndex[i]);
  }
  /* printf("soclcp_projectionOnConeWithLocalIteration_initialize. Allocation of dwork\n"); */
  localsolver_options->dWork = (double *)malloc((3*dim_max+nc) * sizeof(double));
  localsolver_options->iWork = (int *)malloc(sizeof(int));
  localsolver_options->iWork[0]=3*dim_max;
  for(unsigned int i = 3*dim_max; i < nc+3*dim_max; i++)
  {
    localsolver_options->dWork[i]=1.0;
  }
    
}

void soclcp_projectionOnConeWithLocalIteration_free(SecondOrderConeLinearComplementarityProblem * problem, SecondOrderConeLinearComplementarityProblem * localproblem, SolverOptions* localsolver_options)
{
  free(localsolver_options->dWork);
  localsolver_options->dWork=NULL;
  free(localsolver_options->iWork);
  localsolver_options->iWork=NULL;
}

int soclcp_projectionOnConeWithLocalIteration_solve(SecondOrderConeLinearComplementarityProblem* localproblem, double* r, SolverOptions* options)
{
  /* int and double parameters */
  int* iparam = options->iparam;
  double* dparam = options->dparam;

  double * MLocal = localproblem->M->matrix0;
  double * qLocal = localproblem->q;
  double mu_i = localproblem->mu[0];
  int nLocal = localproblem->n;;


  /*   /\* Build local problem for the current cone *\/ */
  /*   soclcp_projection_update(localproblem, reaction); */


  /*double an = 1./(MLocal[0]);*/
  /*   double alpha = MLocal[nLocal+1] + MLocal[2*nLocal+2]; */
  /*   double det = MLocal[1*nLocal+1]*MLocal[2*nLocal+2] - MLocal[2*nLocal+1] + MLocal[1*nLocal+2]; */
  /*   double beta = alpha*alpha - 4*det; */
  /*   double at = 2*(alpha - beta)/((alpha + beta)*(alpha + beta)); */

  /* double an = 1. / (MLocal[0]); */

  /* double at = 1.0 / (MLocal[4] + mu_i); */
  /* double as = 1.0 / (MLocal[8] + mu_i); */
  /* at = an; */
  /* as = an; */
  double rho=   options->dWork[options->iparam[4]+options->iWork[0]] , rho_k;
  /* printf ("saved rho = %14.7e\n",rho ); */
  /* printf ("options->iparam[4] = %i\n",options->iparam[4] ); */

  int incx = 1, incy = 1;

  double * v = options->dWork;
  double * v_k = options->dWork + nLocal;
  double * r_k = options->dWork + nLocal+ nLocal;

  double localerror = 1.0;
  //printf ("localerror = %14.7e\n",localerror );
  int localiter = 0;
  double localtolerance = dparam[0];


  /* Variable for Line_search */
  double a1,a2;
  int success = 0;
  double localerror_k;
  int ls_iter = 0;
  int ls_itermax = 10;
  double tau=0.6, L= 0.9, Lmin =0.3, taumin=0.7;




  /*     printf ("localtolerance = %14.7e\n",localtolerance ); */
  while((localerror > localtolerance) && (localiter < iparam[0]))
  {
    localiter ++;

    /*    printf ("r[0] = %14.7e\n",r[0]); */
    /*    printf ("r[1] = %14.7e\n",r[1]); */
    /*    printf ("r[2] = %14.7e\n",r[2]); */

    /* Store the error */
    localerror_k = localerror;

    /* store r at the beginning of the iteration */
    cblas_dcopy(nLocal , r , 1 , r_k, 1);

    /* velocity_k <- q  */
    cblas_dcopy(nLocal , qLocal , 1 , v_k, 1);

    /* velocity_k <- q + M * reaction  */
    cblas_dgemv(CblasColMajor,CblasNoTrans, nLocal, nLocal, 1.0, MLocal, nLocal, r, incx, 1.0, v_k, incy);


    ls_iter = 0 ;
    success =0;

    while(!success && (ls_iter < ls_itermax))
    {

      rho_k = rho * pow(tau,ls_iter);
      r[0] = r_k[0] - rho_k * v_k[0];
      r[1] = r_k[1] - rho_k * v_k[1];
      r[2] = r_k[2] - rho_k * v_k[2];

      projectionOnSecondOrderCone(&r[0], mu_i,nLocal);

      /* v <- q  */
      cblas_dcopy(nLocal , qLocal , 1 , v, 1);

      /* v <- q + M * r  */
      cblas_dgemv(CblasColMajor,CblasNoTrans, nLocal, nLocal, 1.0, MLocal, nLocal, r, incx, 1.0, v, incy);

      a1 = sqrt((v_k[0] - v[0]) * (v_k[0] - v[0]) +
                (v_k[1] - v[1]) * (v_k[1] - v[1]) +
                (v_k[2] - v[2]) * (v_k[2] - v[2]));

      a2 = sqrt((r_k[0] - r[0]) * (r_k[0] - r[0]) +
                (r_k[1] - r[1]) * (r_k[1] - r[1]) +
                (r_k[2] - r[2]) * (r_k[2] - r[2]));



      success = (rho_k*a1 <= L * a2)?1:0;

      /* printf("rho_k = %12.8e\t", rho_k); */
      /* printf("a1 = %12.8e\t", a1); */
      /* printf("a2 = %12.8e\t", a2); */
      /* printf("norm r = %12.8e\t",sqrt(( r[0]) * (r[0]) + */
      /*           ( r[1]) *  r[1]) + */
      /*           ( r[2]) * ( r[2])); */
      /* printf("success = %i\n", success); */

      ls_iter++;
    }


    /* compute local error */
    localerror =0.0;
    soclcp_unitary_compute_and_add_error(r , v, nLocal, mu_i, &localerror, r_k);

    /* printf("----------------------  localiter = %i\t, rho= %.10e\t, error = %.10e \n", localiter, rho, localerror);  */

    /*Update rho*/
    if((rho_k*a1 < Lmin * a2) && (localerror < localerror_k))
    {
      rho =rho_k/taumin;
    }
    else
      rho =rho_k;

    if(verbose > 1)
    {
      printf("----------------------  localiter = %i\t, rho= %.10e\t, error = %.10e \n", localiter, rho, localerror);
    }

    options->dWork[options->iparam[4]] =rho;

  }

  if(localerror > localtolerance)
    return 1;
  return 0;

}


void soclcp_projection_with_regularization_free(SecondOrderConeLinearComplementarityProblem * problem, SecondOrderConeLinearComplementarityProblem * localproblem, SolverOptions* localsolver_options)
{
  free(localproblem->M->matrix0);
  localproblem->M->matrix0 = NULL;
}

int soclcp_projectionOnCylinder_solve(SecondOrderConeLinearComplementarityProblem* localproblem, double* reaction, SolverOptions * options)
{

  /*  /\* Build local problem for the current cone *\/ */
  /*   soclcp_projection_update(cone, reaction); */

  double * MLocal = localproblem->M->matrix0;
  double * qLocal = localproblem->q;
  double mu_i = localproblem->mu[0];
  int nLocal = localproblem->n;

  /* this part is critical for the success of the projection */
  //double an = 1./(MLocal[0]+mu_i);
  double an = 1. / (MLocal[0]);


  int incx = 1, incy = 1;
  double * worktmp = (double*)malloc(nLocal*sizeof(double));

  cblas_dcopy(nLocal , qLocal, incx , worktmp , incy);

  cblas_dgemv(CblasColMajor,CblasNoTrans, nLocal, nLocal, 1.0, MLocal, nLocal, reaction, incx, 1.0, worktmp, incy);

  for (int i =0; i < nLocal ; i++)
  {
    reaction[i] -= an * worktmp[i];
  }

  free(worktmp);

  projectionOnGeneralCylinder(reaction, mu_i, nLocal);
  return 0;

}




int soclcp_projection_setDefaultSolverOptions(SolverOptions* options)
{
  int i;
  if(verbose > 0)
  {
    printf("Set the Default SolverOptions for the local SOCLCP Solver\n");
  }

  options->solverId = SICONOS_SOCLCP_ProjectionOnCone;
  options->numberOfInternalSolvers = 0;
  options->isSet = 1;
  options->filterOn = 1;
  options->iSize = 5;
  options->dSize = 5;
  options->iparam = (int *)malloc(options->iSize * sizeof(int));
  options->dparam = (double *)malloc(options->dSize * sizeof(double));
  options->dWork = NULL;
  options->iWork = NULL;
  options->callback = NULL;
  options->numericsOptions = NULL;
  for(i = 0; i < 5; i++)
  {
    options->iparam[i] = 0;
    options->dparam[i] = 0.0;
  }

  options->iparam[0] = 10;
  options->iparam[1] = 10;
  options->dparam[0] = 1e-16;
  return 0;
}
