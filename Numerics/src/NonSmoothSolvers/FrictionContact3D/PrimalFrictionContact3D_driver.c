/* Siconos-Numerics version 3.0.0, Copyright INRIA 2005-2008.
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
 * Contact: Vincent ACARY vincent.acary@inrialpes.fr
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <assert.h>

#include "LA.h"
#include "Numerics_Options.h"
#include "PrimalFrictionContact3D_Solvers.h"
#include "NonSmoothDrivers.h"
#include "FrictionContact3D_Solvers.h"


/* Global Variable for the reformulation of the problem */
int * Reformulation_ipiv = NULL;
int  Reformulation_MisInverse;
int  Reformulation_MisLU;

int reformulationIntoLocalProblem(PrimalFrictionContact_Problem* problem, FrictionContact_Problem* localproblem)
{
  int info = -1;
  NumericsMatrix *M = problem->M;
  NumericsMatrix *H = problem->H;


  localproblem->numberOfContacts = problem->numberOfContacts;
  localproblem->isComplete = 0;
  localproblem->mu =  problem->mu;

  assert(M);
  assert(H);

  if (H->storageType != M->storageType)
  {
    //      if(verbose==1)
    printf(" ->storageType != M->storageType :This case is not taken into account\n");
    return info;
  }

  if (M->storageType == 0)
  {


    int n = M->size0;
    int m = H->size1;
    int nm = n * m;
    int infoDGESV;
    int infoDGETRF;
    int infoDGETRS;
    Reformulation_ipiv = (int *)malloc(n * sizeof(int));


    double *Htmp = (double*)malloc(nm * sizeof(double));
    // compute W = H^T M^-1 H
    //Copy Htmp <- H
    DCOPY(nm,  H->matrix0 , 1, Htmp, 1);
    //Compute Htmp   <- M^-1 Htmp
    Reformulation_MisLU = 0; /*  Assume that M is not already LU */
    DGETRF(n, n, M->matrix0, n, Reformulation_ipiv, infoDGETRF);
    assert(!infoDGETRF);
    Reformulation_MisLU = 1;
    DGETRS(LA_NOTRANS, n, m,  M->matrix0, n, Reformulation_ipiv, Htmp, n, infoDGETRS);
    assert(!infoDGETRS);
    /*      DGESV(n, m, M->matrix0, n, ipiv, Htmp, n, infoDGESV); */

    localproblem->M = (NumericsMatrix *)malloc(sizeof(NumericsMatrix));
    NumericsMatrix *Wnum = localproblem->M;
    Wnum->storageType = 0;
    Wnum-> size0 = m;
    Wnum-> size1 = m;
    Wnum->matrix0 = (double*)malloc(m * m * sizeof(double));
    Wnum->matrix0 = NULL;
    // Compute W <-  H^T M^1 H

    DGEMM(LA_TRANS, LA_NOTRANS, m, m, n, 1.0, H->matrix0, n, Htmp, n, 0.0, Wnum->matrix0, m);
    /*     DGEMM(LA_TRANS,LA_NOTRANS,m,m,n,1.0,H->matrix0,n,Htmp,n,0.0,Wnum->matrix0,m); */
    // compute localq = H^T M^(-1) q +b

    //Copy localq <- b
    localproblem->q = (double*)malloc(m * sizeof(double));
    DCOPY(m, problem->b , 1, localproblem->q, 1);

    double* qtmp = (double*)malloc(n * sizeof(double));
    DCOPY(n,  problem->q, 1, qtmp, 1);

    // compute H^T M^(-1) q + b

    assert(Reformulation_MisLU);
    DGETRS(LA_NOTRANS, n, 1,  M->matrix0, n, Reformulation_ipiv, qtmp , n, infoDGETRS);


    /*      DGESV(n, m, M->matrix0, n, ipiv, problem->q , n, infoDGESV); */

    DGEMV(LA_TRANS, n, m, 1.0, H->matrix0 , n, qtmp, 1, 1.0, localproblem->q, 1);
    // Copy mu
    localproblem->mu = problem->mu;


    localproblem->isComplete = 1;

    free(Htmp);
    free(qtmp);


  }

  else
  {
    int n = M->size0;
    int m = H->size1;
    int nm = n * m;
    int infoInverseSBM = 0;
    assert(!Reformulation_ipiv);
    Reformulation_ipiv = (int *)malloc(n * sizeof(int));

    // compute W = H^T M^-1 H
    //Copy Htmp <- H
    SparseBlockStructuredMatrix *HtmpSBM = (SparseBlockStructuredMatrix*)malloc(sizeof(*HtmpSBM));
    /* copySBM(H->matrix1 , HtmpSBM); */

    //Compute Htmp   <- M^-1 HtmpSBM
    /* DGESV(n, m, M->matrix0, n, ipiv, Htmp, n, infoDGESV); */
    infoInverseSBM = inverseDiagSBM(M->matrix1);
    assert(!infoInverseSBM);
    Reformulation_MisInverse = 1;

    allocateMemoryForProdSBMSBM(M->matrix1, H->matrix1, HtmpSBM);
    double alpha = 1.0, beta = 1.0;

    prodSBMSBM(alpha, M->matrix1, H->matrix1, beta, HtmpSBM);

    SparseBlockStructuredMatrix *Htrans = (SparseBlockStructuredMatrix*)malloc(sizeof(*Htrans));
    transposeSBM(H->matrix1, Htrans);

    localproblem->M = (NumericsMatrix *)malloc(sizeof(NumericsMatrix));;
    NumericsMatrix *Wnum = localproblem->M;
    Wnum->storageType = 1;
    Wnum-> size0 = m;
    Wnum-> size1 = m;
    Wnum->matrix1 = (SparseBlockStructuredMatrix*)malloc(sizeof(SparseBlockStructuredMatrix));
    Wnum->matrix0 = NULL;
    SparseBlockStructuredMatrix *W =  Wnum->matrix1;

    allocateMemoryForProdSBMSBM(Htrans, HtmpSBM, W);
    prodSBMSBM(alpha, Htrans, HtmpSBM, beta, W);


    localproblem->q = (double*)malloc(m * sizeof(double));
    //Copy q<- b
    DCOPY(m, problem->b  , 1, localproblem->q, 1);
    // compute H^T M^-1 q+ b
    double* qtmp = (double*)malloc(n * sizeof(double));
    double beta2 = 0.0;
    prodSBM(n, n, alpha, M->matrix1, problem->q, beta2, qtmp);
    prodSBM(n, m, alpha, Htrans, qtmp, beta, localproblem->q);

    localproblem->mu = problem->mu;
    localproblem->isComplete = 1;

    freeSBM(HtmpSBM);
    freeSBM(Htrans);
    free(HtmpSBM);
    free(Htrans);
    free(qtmp);
  }


  return info;
}
int computeGlobalVelocity(PrimalFrictionContact_Problem* problem, double * reaction, double * globalVelocity)
{
  int info = -1;

  if (problem->M->storageType == 0)
  {
    int n = problem->M->size0;
    int m = problem->H->size1;
    int nm = n * m;

    /* Compute globalVelocity   <- H reaction + q*/
    double alpha = 1.0;
    double beta = 1.0;
    /* globalVelocity <- problem->q */
    DCOPY(n,  problem->q , 1, globalVelocity, 1);
    /* globalVelocity <-  H*reaction + globalVelocity*/
    DGEMV(LA_NOTRANS, n, m, 1.0, problem->H->matrix0 , n, reaction , 1, 1.0, globalVelocity, 1);
    /* Compute globalVelocity <- M^(-1) globalVelocity*/
    assert(Reformulation_ipiv);
    assert(Reformulation_MisLU);
    int infoDGETRS;
    DGETRS(LA_NOTRANS, n, 1,   problem->M->matrix0, n, Reformulation_ipiv, globalVelocity , n, infoDGETRS);
    assert(!infoDGETRS);

    free(Reformulation_ipiv);
    Reformulation_ipiv = NULL;


  }
  else
  {
    int n = problem->M->size0;
    int m = problem->H->size1;
    int nm = n * m;

    /* Compute qtmp   <- H reaction + q*/

    double* qtmp = (double*)malloc(n * sizeof(double));
    double alpha = 1.0;
    double beta = 1.0;

    DCOPY(n,  problem->q , 1, qtmp, 1);
    prodSBM(m, n, alpha, problem->H->matrix1, reaction, beta, qtmp);
    /* Compute global velocity = M^(-1) qtmp*/


    /*      inverseDiagSBM(M->matrix1); We assume that M->matrix1 is already inverse*/
    assert(Reformulation_MisInverse);

    double beta2 = 0.0;
    prodSBM(n, n, alpha,  problem->M->matrix1, qtmp, beta2, globalVelocity);

    free(qtmp);
    free(Reformulation_ipiv);
    Reformulation_ipiv = NULL;
  }

  return info;
}

int freeLocalProblem(FrictionContact_Problem* localproblem)
{
  int info = -1;

  /*    if (!localproblem->M->storageType) */
  /*  { */
  if (localproblem->M->matrix0)
    free(localproblem->M->matrix0);
  /*  } */
  /*     else */
  /*  { */
  if (localproblem->M->matrix1)
  {
    freeSBM(localproblem->M->matrix1);
    free(localproblem->M->matrix1);
  }
  /*  } */
  free(localproblem->M);
  free(localproblem->q);
  free(localproblem);
  return info;
}



int primalFrictionContact3D_driver(PrimalFrictionContact_Problem* problem, double *reaction , double *velocity, double* globalVelocity,  Solver_Options* options, Numerics_Options* global_options)
{

  /* Set global options */
  setNumericsOptions(global_options);

  /* If the options for solver have not been set, read default values in .opt file */
  int NoDefaultOptions = options->isSet; /* true(1) if the Solver_Options structure has been filled in else false(0) */

  if (!NoDefaultOptions)
    readSolverOptions(3, options);

  if (verbose > 0)
    printSolverOptions(options);

  /* Solver name */
  char * name = options->solverName;


  int info = -1 ;

  // Reformulation
  FrictionContact_Problem* localproblem = (FrictionContact_Problem *) malloc(sizeof(FrictionContact_Problem));

  reformulationIntoLocalProblem(problem, localproblem);



  /* Non Smooth Gauss Seidel (NSGS) */
  if (strcmp(name, "NSGS") == 0)
  {
    if (verbose == 1)
      printf(" ========================== Call NSGS solver for Friction-Contact 3D problem ==========================\n");

    frictionContact3D_nsgs(localproblem, reaction , velocity , &info , options);
  }

  computeGlobalVelocity(problem, reaction, globalVelocity);
  freeLocalProblem(localproblem);


  return info;

}

int checkTrivialCasePrimal(int n, double* q, double* velocity, double* reaction, double * globalVelocity, int* iparam, double* dparam)
{
  /* norm of vector q */
  /*   double qs = DNRM2( n , q , 1 ); */
  /*   int i; */
  int info = -1;
  /*   if( qs <= DBL_EPSILON )  */
  /*     { */
  /*       // q norm equal to zero (less than DBL_EPSILON) */
  /*       // -> trivial solution: reaction = 0 and velocity = q */
  /*       for( i = 0 ; i < n ; ++i ) */
  /*  { */
  /*    velocity[i] = q[i]; */
  /*    reaction[i] = 0.; */
  /*  } */
  /*       iparam[2] = 0; */
  /*       iparam[4]= 0; */
  /*       dparam[1] = 0.0; */
  /*       dparam[3] = 0.0; */
  /*       info = 0; */
  /*       if(iparam[1]>0) */
  /*  printf("FrictionContact3D driver, norm(q) = 0, trivial solution reaction = 0, velocity = q.\n"); */
  /*     } */
  return info;
}

