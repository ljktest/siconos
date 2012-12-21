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
#include <string.h>
#include <time.h>
#include <float.h>
#include <assert.h>

#include "LA.h"
#include "NumericsOptions.h"
#include "GlobalFrictionContact3D_Solvers.h"
#include "NonSmoothDrivers.h"
#include "FrictionContact3D_Solvers.h"
#include "cond.h"
#include "pinv.h"
#include <string.h>
//#define TEST_COND
//#define OUTPUT_DEBUG
extern int *Global_ipiv;
extern int  Global_MisInverse;
extern int  Global_MisLU;

/* Global Variable for the reformulation of the problem */

int reformulationIntoLocalProblem(GlobalFrictionContactProblem* problem, FrictionContactProblem* localproblem)
{
  int info = -1;

  NumericsMatrix *M = problem->M;
  NumericsMatrix *H = problem->H;


  localproblem->numberOfContacts = problem->numberOfContacts;
  localproblem->dimension =  problem->dimension;
  localproblem->mu =  problem->mu;

  assert(M);
  assert(H);

  if (H->storageType != M->storageType)
  {
    //     if(verbose==1)
    printf(" ->storageType != M->storageType :This case is not taken into account\n");
    return info;
  }

  if (M->storageType == 0)
  {


    int n = M->size0;
    int m = H->size1;
    int nm = n * m;
    int infoDGETRF = 0;
    int infoDGETRS = 0;
    Global_ipiv = (int *)malloc(n * sizeof(int));


    double *Htmp = (double*)malloc(nm * sizeof(double));
    // compute W = H^T M^-1 H
    //Copy Htmp <- H
    DCOPY(nm,  H->matrix0 , 1, Htmp, 1);
    //Compute Htmp   <- M^-1 Htmp
    Global_MisLU = 0; /*  Assume that M is not already LU */
    DGETRF(n, n, M->matrix0, n, Global_ipiv, &infoDGETRF);
    assert(!infoDGETRF);
    Global_MisLU = 1;
#ifdef USE_MKL
    DGETRS(CLA_NOTRANS, n, m,  M->matrix0, n, Global_ipiv, Htmp, n, infoDGETRS);
#else
    DGETRS(LA_NOTRANS, n, m,  M->matrix0, n, Global_ipiv, Htmp, n, &infoDGETRS);
#endif
    assert(!infoDGETRS);
    /*      DGESV(n, m, M->matrix0, n, ipiv, Htmp, n, infoDGESV); */

    localproblem->M = (NumericsMatrix *)malloc(sizeof(NumericsMatrix));
    NumericsMatrix *Wnum = localproblem->M;
    Wnum->storageType = 0;
    Wnum-> size0 = m;
    Wnum-> size1 = m;
    Wnum->matrix0 = (double*)malloc(m * m * sizeof(double));
    Wnum->matrix1 = NULL;
    Wnum->matrix2 = NULL;
    Wnum->matrix3 = NULL;
    // Compute W <-  H^T M^1 H



    assert(H->matrix0);
    assert(Htmp);
    assert(Wnum->matrix0);



    DGEMM(LA_TRANS, LA_NOTRANS, m, m, n, 1.0, H->matrix0, n, Htmp, n, 0.0, Wnum->matrix0, m);
    /*     DGEMM(LA_TRANS,LA_NOTRANS,m,m,n,1.0,H->matrix0,n,Htmp,n,0.0,Wnum->matrix0,m); */

    // compute localq = H^T M^(-1) q +b

    //Copy localq <- b
    localproblem->q = (double*)malloc(m * sizeof(double));
    DCOPY(m, problem->b , 1, localproblem->q, 1);

    double* qtmp = (double*)malloc(n * sizeof(double));
    DCOPY(n,  problem->q, 1, qtmp, 1);

    // compute H^T M^(-1) q + b

    assert(Global_MisLU);
#ifdef USE_MKL
    DGETRS(CLA_NOTRANS, n, 1,  M->matrix0, n, Global_ipiv, qtmp , n, infoDGETRS);
#else
    DGETRS(LA_NOTRANS, n, 1,  M->matrix0, n, Global_ipiv, qtmp , n, &infoDGETRS);
#endif

    /*      DGESV(n, m, M->matrix0, n, ipiv, problem->q , n, infoDGESV); */

    DGEMV(LA_TRANS, n, m, 1.0, H->matrix0 , n, qtmp, 1, 1.0, localproblem->q, 1);
    // Copy mu
    localproblem->mu = problem->mu;



    free(Htmp);
    free(qtmp);


  }

  else
  {
    int n = M->size0;
    int m = H->size1;

    int infoInverseSBM = 0;
    assert(!Global_ipiv);
    Global_ipiv = (int *)malloc(n * sizeof(int));

    // compute W = H^T M^-1 H
    //Copy Htmp <- H
    SparseBlockStructuredMatrix *HtmpSBM = (SparseBlockStructuredMatrix*)malloc(sizeof(SparseBlockStructuredMatrix));
    /* copySBM(H->matrix1 , HtmpSBM); */

#ifdef OUTPUT_DEBUG
    FILE* fileout;
    fileout = fopen("dataM.sci", "w");
    printInFileForScilab(M, fileout);
    fclose(fileout);
    printf("Display M\n");
    printSBM(M->matrix1);
#endif
    //Compute Htmp   <- M^-1 HtmpSBM
    /* DGESV(n, m, M->matrix0, n, ipiv, Htmp, n, infoDGESV); */
    infoInverseSBM = inverseDiagSBM(M->matrix1);
    assert(!infoInverseSBM);
    Global_MisInverse = 1;
#ifdef OUTPUT_DEBUG
    fileout = fopen("dataMinv.sci", "w");
    printInFileForScilab(M, fileout);
    fclose(fileout);
    printf("Display Minv\n");
    printSBM(M->matrix1);
#endif
    allocateMemoryForProdSBMSBM(M->matrix1, H->matrix1, HtmpSBM);
    double alpha = 1.0, beta = 1.0;

    prodSBMSBM(alpha, M->matrix1, H->matrix1, beta, HtmpSBM);
#ifdef OUTPUT_DEBUG
    fileout = fopen("dataH.sci", "w");
    printInFileForScilab(H, fileout);
    fclose(fileout);
    printf("Display H\n");
    printSBM(H->matrix1);

    fileout = fopen("dataHtmpSBM.sci", "w");
    printInFileSBMForScilab(HtmpSBM, fileout);
    fclose(fileout);
    printf("Display HtmpSBM\n");
    printSBM(HtmpSBM);
#endif





    SparseBlockStructuredMatrix *Htrans = (SparseBlockStructuredMatrix*)malloc(sizeof(SparseBlockStructuredMatrix));
    transposeSBM(H->matrix1, Htrans);
#ifdef OUTPUT_DEBUG
    fileout = fopen("dataHtrans.sci", "w");
    printInFileSBMForScilab(Htrans, fileout);
    fclose(fileout);
    printf("Display Htrans\n");
    printSBM(Htrans);
#endif
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
#ifdef OUTPUT_DEBUG
    fileout = fopen("dataW.sci", "w");
    printInFileForScilab(Wnum, fileout);
    fclose(fileout);
    printf("Display W\n");
    printSBM(W);
#endif

#ifdef TEST_COND
    NumericsMatrix *WnumInverse = (NumericsMatrix*)malloc(sizeof(NumericsMatrix));
    WnumInverse->storageType = 0;
    WnumInverse-> size0 = m;
    WnumInverse-> size1 = m;
    WnumInverse->matrix1 = NULL;
    WnumInverse->matrix2 = NULL;
    WnumInverse->matrix3 = NULL;
    WnumInverse->matrix0 = (double*)malloc(m * m * sizeof(double));
    double * WInverse = WnumInverse->matrix0;
    SBMtoDense(W, WnumInverse->matrix0);

    FILE * file1 = fopen("dataW.dat", "w");
    printInFileForScilab(WnumInverse, file1);
    fclose(file1);

    double * WInversetmp = (double*)malloc(m * m * sizeof(double));
    memcpy(WInversetmp, WInverse, m * m * sizeof(double));
    double  condW;
    condW = cond(WInverse, m, m);

    int* ipiv = (int *)malloc(m * sizeof(*ipiv));
    int infoDGETRF = 0;
    DGETRF(m, m, WInverse, m, ipiv, &infoDGETRF);
    assert(!infoDGETRF);
    int infoDGETRI = 0;
    DGETRI(m, WInverse, m, ipiv, &infoDGETRI);


    free(ipiv);
    assert(!infoDGETRI);


    double  condWInverse;
    condWInverse = cond(WInverse, m, m);




    FILE * file2 = fopen("dataWInverse.dat", "w");
    printInFileForScilab(WnumInverse, file2);
    fclose(file2);

    double tol = 1e-24;
    pinv(WInversetmp, m, m, tol);
    NumericsMatrix *WnumInversetmp = (NumericsMatrix*)malloc(sizeof(NumericsMatrix));
    WnumInversetmp->storageType = 0;
    WnumInversetmp-> size0 = m;
    WnumInversetmp-> size1 = m;
    WnumInversetmp->matrix1 = NULL;
    WnumInversetmp->matrix2 = NULL;
    WnumInversetmp->matrix3 = NULL;
    WnumInversetmp->matrix0 = WInversetmp ;

    FILE * file3 = fopen("dataWPseudoInverse.dat", "w");
    printInFileForScilab(WnumInversetmp, file3);
    fclose(file3);


    free(WInverse);
    free(WInversetmp);
    free(WnumInverse);
#endif

    localproblem->q = (double*)malloc(m * sizeof(double));
    //Copy q<- b
    DCOPY(m, problem->b  , 1, localproblem->q, 1);
    // compute H^T M^-1 q+ b
    double* qtmp = (double*)malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) qtmp[i] = 0.0;
    double beta2 = 0.0;
    prodSBM(n, n, alpha, M->matrix1, problem->q, beta2, qtmp);
    prodSBM(n, m, alpha, Htrans, qtmp, beta, localproblem->q);

    localproblem->mu = problem->mu;

    /*     FILE * filecheck = fopen("localproblemcheck.dat","w"); */
    /*     frictionContact_printInFile(localproblem,filecheck); */
    /*     fclose(filecheck); */

    freeSBM(HtmpSBM);
    freeSBM(Htrans);
    free(HtmpSBM);
    free(Htrans);
    free(qtmp);
  }


  return info;
}
int computeGlobalVelocity(GlobalFrictionContactProblem* problem, double * reaction, double * globalVelocity)
{
  int info = -1;

  if (problem->M->storageType == 0)
  {
    int n = problem->M->size0;
    int m = problem->H->size1;


    /* Compute globalVelocity   <- H reaction + q*/

    /* globalVelocity <- problem->q */
    DCOPY(n,  problem->q , 1, globalVelocity, 1);
    /* globalVelocity <-  H*reaction + globalVelocity*/
    DGEMV(LA_NOTRANS, n, m, 1.0, problem->H->matrix0 , n, reaction , 1, 1.0, globalVelocity, 1);
    /* Compute globalVelocity <- M^(-1) globalVelocity*/
    assert(Global_ipiv);
    assert(Global_MisLU);
    int infoDGETRS = 0;
#ifdef USE_MKL
    DGETRS(CLA_NOTRANS, n, 1,   problem->M->matrix0, n, Global_ipiv, globalVelocity , n, infoDGETRS);
#else
    DGETRS(LA_NOTRANS, n, 1,   problem->M->matrix0, n, Global_ipiv, globalVelocity , n, &infoDGETRS);
#endif
    assert(!infoDGETRS);

    free(Global_ipiv);
    Global_ipiv = NULL;


  }
  else
  {
    int n = problem->M->size0;
    int m = problem->H->size1;

    /* Compute qtmp   <- H reaction + q*/

    double* qtmp = (double*)malloc(n * sizeof(double));
    double alpha = 1.0;
    double beta = 1.0;

    DCOPY(n,  problem->q , 1, qtmp, 1);
    prodSBM(m, n, alpha, problem->H->matrix1, reaction, beta, qtmp);
    /* Compute global velocity = M^(-1) qtmp*/


    /*      inverseDiagSBM(M->matrix1); We assume that M->matrix1 is already inverse*/
    assert(Global_MisInverse);

    double beta2 = 0.0;
    prodSBM(n, n, alpha,  problem->M->matrix1, qtmp, beta2, globalVelocity);

    free(qtmp);
    free(Global_ipiv);
    Global_ipiv = NULL;
  }

  return info;
}

int freeLocalProblem(FrictionContactProblem* localproblem)
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



void  globalFrictionContact3D_nsgs_wr(GlobalFrictionContactProblem* problem, double *reaction , double *velocity, double* globalVelocity, int *info, SolverOptions* options)
{

  // Reformulation
  FrictionContactProblem* localproblem = (FrictionContactProblem *) malloc(sizeof(FrictionContactProblem));

  reformulationIntoLocalProblem(problem, localproblem);

  frictionContact3D_nsgs(localproblem, reaction , velocity , info , options->internalSolvers);

  computeGlobalVelocity(problem, reaction, globalVelocity);
  freeLocalProblem(localproblem);


}
int globalFrictionContact3D_nsgs_wr_setDefaultSolverOptions(SolverOptions* options)
{


  if (verbose > 0)
  {
    printf("Set the Default SolverOptions for the NSGS_WR Solver\n");
  }

  options->solverId = SICONOS_FRICTION_3D_GLOBAL_NSGS_WR;

  options->numberOfInternalSolvers = 1;
  options->isSet = 1;
  options->filterOn = 1;
  options->iSize = 0;
  options->dSize = 0;
  options->iparam = NULL;
  options->dparam = NULL;
  options->dWork = NULL;
  options->iWork = NULL;
  options->internalSolvers = (SolverOptions *)malloc(sizeof(SolverOptions));
  frictionContact3D_nsgs_setDefaultSolverOptions(options->internalSolvers);
  return 0;
}



void  globalFrictionContact3D_globalAlartCurnier_wr(GlobalFrictionContactProblem* problem, double *reaction , double *velocity, double* globalVelocity, int *info, SolverOptions* options)
{

  // Reformulation
  FrictionContactProblem* localproblem = (FrictionContactProblem *) malloc(sizeof(FrictionContactProblem));

  reformulationIntoLocalProblem(problem, localproblem);


  // From StorageType==1 to Storage==0
  if (localproblem->M->storageType == 1)
  {
    printf("Warning: frictionContact3D_globalAlartCurnier is only implemented for dense matrices.\n");
    printf("Warning: The problem is reformulated.\n");

    localproblem->M->matrix0 = (double*)malloc(localproblem->M->size0 * localproblem->M->size1 * sizeof(double));

    SBMtoDense(localproblem->M->matrix1, localproblem->M->matrix0);

    freeSBM(localproblem->M->matrix1);
    free(localproblem->M->matrix1);
    localproblem->M->matrix1 = NULL;
    localproblem->M->matrix2 = NULL;
    localproblem->M->matrix3 = NULL;
    localproblem->M->storageType = 0;
  }


  //
  frictionContact3D_globalAlartCurnier(localproblem, reaction , velocity , info , options->internalSolvers);

  computeGlobalVelocity(problem, reaction, globalVelocity);
  freeLocalProblem(localproblem);


}
int globalFrictionContact3D_globalAlartCurnier_wr_setDefaultSolverOptions(SolverOptions* options)
{


  if (verbose > 0)
  {
    printf("Set the Default SolverOptions for the LOCALAC_WR Solver\n");
  }

  options->solverId = SICONOS_FRICTION_3D_GLOBAL_LOCALAC_WR;

  options->numberOfInternalSolvers = 1;
  options->isSet = 1;
  options->filterOn = 1;
  options->iSize = 0;
  options->dSize = 0;
  options->iparam = NULL;
  options->dparam = NULL;
  options->dWork = NULL;
  options->iWork = NULL;
  options->internalSolvers = (SolverOptions *)malloc(sizeof(SolverOptions));
  frictionContact3D_globalAlartCurnier_setDefaultSolverOptions(options->internalSolvers);
  return 0;
}

void  globalFrictionContact3D_nsgs_velocity_wr(GlobalFrictionContactProblem* problem, double *reaction , double *velocity, double* globalVelocity, int *info, SolverOptions* options)
{
  // Reformulation
  FrictionContactProblem* localproblem = (FrictionContactProblem *) malloc(sizeof(FrictionContactProblem));

  reformulationIntoLocalProblem(problem, localproblem);

  /* Change into dense if neccessary*/

  int m = localproblem->M->size0;
  assert(localproblem->M->size0 == localproblem->M->size1);

  if (localproblem->M->storageType == 1)
  {

    localproblem->M->matrix0 = (double*)malloc(m * m * sizeof(double));
    SBMtoDense(localproblem->M->matrix1, localproblem->M->matrix0);
    freeSBM(localproblem->M->matrix1);
    free(localproblem->M->matrix1);
    localproblem->M->storageType = 0;
    localproblem->M->matrix1 = NULL;
    localproblem->M->matrix2 = NULL;
    localproblem->M->matrix3 = NULL;
  }

  frictionContact3D_nsgs_velocity(localproblem, reaction , velocity , info , options->internalSolvers);

  computeGlobalVelocity(problem, reaction, globalVelocity);
  freeLocalProblem(localproblem);

}
int globalFrictionContact3D_nsgs_velocity_wr_setDefaultSolverOptions(SolverOptions* options)
{




  if (verbose > 0)
  {
    printf("Set the Default SolverOptions for the NSGSV_WR Solver\n");
  }

  options->solverId = SICONOS_FRICTION_3D_GLOBAL_NSGSV_WR;

  options->numberOfInternalSolvers = 1;
  options->isSet = 1;
  options->filterOn = 1;
  options->iSize = 0;
  options->dSize = 0;
  options->iparam = NULL;
  options->dparam = NULL;
  options->dWork = NULL;
  options->iWork = NULL;
  options->internalSolvers = (SolverOptions *)malloc(sizeof(SolverOptions));
  frictionContact3D_nsgs_velocity_setDefaultSolverOptions(options->internalSolvers);
  return 0;

}

void  globalFrictionContact3D_proximal_wr(GlobalFrictionContactProblem* problem, double *reaction , double *velocity, double* globalVelocity, int *info, SolverOptions* options)
{

  // Reformulation
  FrictionContactProblem* localproblem = (FrictionContactProblem *) malloc(sizeof(FrictionContactProblem));

  reformulationIntoLocalProblem(problem, localproblem);

  frictionContact3D_proximal(localproblem, reaction , velocity , info , options->internalSolvers);

  computeGlobalVelocity(problem, reaction, globalVelocity);
  freeLocalProblem(localproblem);


}
int globalFrictionContact3D_proximal_wr_setDefaultSolverOptions(SolverOptions* options)
{


  if (verbose > 0)
  {
    printf("Set the Default SolverOptions for the PROX_WR Solver\n");
  }

  options->solverId = SICONOS_FRICTION_3D_GLOBAL_PROX_WR;

  options->numberOfInternalSolvers = 1;
  options->isSet = 1;
  options->filterOn = 1;
  options->iSize = 0;
  options->dSize = 0;
  options->iparam = NULL;
  options->dparam = NULL;
  options->dWork = NULL;
  options->iWork = NULL;
  options->internalSolvers = (SolverOptions *)malloc(sizeof(SolverOptions));
  frictionContact3D_proximal_setDefaultSolverOptions(options->internalSolvers);
  return 0;
}
void  globalFrictionContact3D_DeSaxceFixedPoint_wr(GlobalFrictionContactProblem* problem, double *reaction , double *velocity, double* globalVelocity, int *info, SolverOptions* options)
{

  // Reformulation
  FrictionContactProblem* localproblem = (FrictionContactProblem *) malloc(sizeof(FrictionContactProblem));

  reformulationIntoLocalProblem(problem, localproblem);

  frictionContact3D_DeSaxceFixedPoint(localproblem, reaction , velocity , info , options->internalSolvers);

  computeGlobalVelocity(problem, reaction, globalVelocity);
  freeLocalProblem(localproblem);


}
int globalFrictionContact3D_DeSaxceFixedPoint_setDefaultSolverOptions(SolverOptions* options)
{


  if (verbose > 0)
  {
    printf("Set the Default SolverOptions for the DSFP_WR Solver\n");
  }

  options->solverId = SICONOS_FRICTION_3D_GLOBAL_DSFP_WR;

  options->numberOfInternalSolvers = 1;
  options->isSet = 1;
  options->filterOn = 1;
  options->iSize = 0;
  options->dSize = 0;
  options->iparam = NULL;
  options->dparam = NULL;
  options->dWork = NULL;
  options->iWork = NULL;
  options->internalSolvers = (SolverOptions *)malloc(sizeof(SolverOptions));
  frictionContact3D_DeSaxceFixedPoint_setDefaultSolverOptions(options->internalSolvers);
  return 0;
}

void  globalFrictionContact3D_TrescaFixedPoint_wr(GlobalFrictionContactProblem* problem, double *reaction , double *velocity, double* globalVelocity, int *info, SolverOptions* options)
{

  // Reformulation
  FrictionContactProblem* localproblem = (FrictionContactProblem *) malloc(sizeof(FrictionContactProblem));

  reformulationIntoLocalProblem(problem, localproblem);

  frictionContact3D_TrescaFixedPoint(localproblem, reaction , velocity , info , options->internalSolvers);

  computeGlobalVelocity(problem, reaction, globalVelocity);
  freeLocalProblem(localproblem);


}
int globalFrictionContact3D_TrescaFixedPoint_setDefaultSolverOptions(SolverOptions* options)
{


  if (verbose > 0)
  {
    printf("Set the Default SolverOptions for the DSFP_WR Solver\n");
  }

  options->solverId = SICONOS_FRICTION_3D_GLOBAL_TFP_WR;

  options->numberOfInternalSolvers = 1;
  options->isSet = 1;
  options->filterOn = 1;
  options->iSize = 0;
  options->dSize = 0;
  options->iparam = NULL;
  options->dparam = NULL;
  options->dWork = NULL;
  options->iWork = NULL;
  options->internalSolvers = (SolverOptions *)malloc(sizeof(SolverOptions));
  frictionContact3D_TrescaFixedPoint_setDefaultSolverOptions(options->internalSolvers);
  return 0;
}
