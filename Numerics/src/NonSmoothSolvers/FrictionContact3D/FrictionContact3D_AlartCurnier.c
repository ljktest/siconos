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

#include "LA.h"
#include "FrictionContact3D_Solvers.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/*Static variables */

/* The global problem of size n= 3*nc, nc being the number of contacts, is locally saved in MGlobal and qGlobal */
/* mu corresponds to the vector of friction coefficients */
/* note that either MGlobal or MBGlobal is used, depending on the chosen storage */
static int n = 0;
static const NumericsMatrix* MGlobal = NULL;
static const double* qGlobal = NULL;
static const double* mu = NULL;

/* Local problem operators */
static const int nLocal = 3;
static double* MLocal;
static int isMAllocatedIn = 0; /* True if a malloc is done for MLocal, else false */
static double velocityLocal[3];
static double qLocal[3];
static double mu_i = 0.0;
static double an;
static double at;
static double projN;
static double projT;
static double projS;

void AC_fillMLocal(int contact)
{
  // Dense storage
  int storageType = MGlobal->storageType;
  if (storageType == 0)
  {
    int in = 3 * contact, it = in + 1, is = it + 1;
    int inc = n * in;
    double * MM = MGlobal->matrix0;
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
  else if (storageType == 1)
  {
    int diagPos = getDiagonalBlockPos(MGlobal->matrix1, contact);
    //MLocal = MGlobal->matrix1->block[diagPos];
    DCOPY(9, MGlobal->matrix1->block[diagPos], 1, MLocal, 1);

  }
  else
    numericsError("FrictionContact3D_AlartCurnier:AC_fillMLocal() -", "unknown storage type for matrix M");

}

void frictionContact3D_AC_initialize(int n0, const NumericsMatrix*const M0, const double*const q0, const double*const mu0)
{
  /*
    INPUT: the global problem operators: n0 (size), M0, q0 and mu0, vector of friction coefficients.
    In initialize, these operators are "connected" to their corresponding static variables, that will be used to build local problem
    for each considered contact.
    Local problem is built during call to update (which depends on the storage type for M).
  */

  n = n0;
  MGlobal = M0;
  qGlobal = q0;
  mu = mu0;
  /*  if(MGlobal->storageType == 0) */
  /*     { */
  MLocal = (double*)malloc(nLocal * nLocal * sizeof(*MLocal));
  isMAllocatedIn = 1;
  /*     } */
  /*   else */
  /*     isMAllocatedIn = 0; */
}

void frictionContact3D_AC_update(int contact, double * reaction)
{
  /* Build a local problem for a specific contact
     reaction corresponds to the global vector (size n) of the global problem.
  */
  /* Call the update function which depends on the storage for MGlobal/MBGlobal */
  /* Build a local problem for a specific contact
   reaction corresponds to the global vector (size n) of the global problem.
  */

  /* The part of MGlobal which corresponds to the current block is copied into MLocal */
  AC_fillMLocal(contact);

  /****  Computation of qLocal = qBlock + sum over a row of blocks in MGlobal of the products MLocal.reactionBlock,
   excluding the block corresponding to the current contact. ****/

  int in = 3 * contact, it = in + 1, is = it + 1;
  /* reaction current block set to zero, to exclude current contact block */
  double rin = reaction[in] ;
  double rit = reaction[it] ;
  double ris = reaction[is] ;
  reaction[in] = 0.0;
  reaction[it] = 0.0;
  reaction[is] = 0.0;
  /* qLocal computation*/
  qLocal[0] = qGlobal[in];
  qLocal[1] = qGlobal[it];
  qLocal[2] = qGlobal[is];

  if (MGlobal->storageType == 0)
  {
    double * MM = MGlobal->matrix0;
    int incx = n, incy = 1;
    qLocal[0] += DDOT(n , &MM[in] , incx , reaction , incy);
    qLocal[1] += DDOT(n , &MM[it] , incx , reaction , incy);
    qLocal[2] += DDOT(n , &MM[is] , incx , reaction , incy);
  }
  else if (MGlobal->storageType == 1)
  {
    /* qLocal += rowMB * reaction
    with rowMB the row of blocks of MGlobal which corresponds to the current contact
    */
    rowProdNoDiagSBM(n, 3, contact, MGlobal->matrix1, reaction, qLocal, 0);
  }
  reaction[in] = rin;
  reaction[it] = rit;
  reaction[is] = ris;

  /* Friction coefficient for current block*/
  mu_i = mu[contact];
}

/* Compute function F(Reaction) */
void F_AC(int Fsize, double *reaction , double *F, int up2Date)
{
  if (F == NULL)
  {
    fprintf(stderr, "FrictionContact3D_AlartCurnier::F_AC error:  F == NULL.\n");
    exit(EXIT_FAILURE);
  }
  if (Fsize != nLocal)
  {
    fprintf(stderr, "FrictionContact3D_AlartCurnier::F error, wrong block size.\n");
    exit(EXIT_FAILURE);
  }

  /* up2Date = 1 = true if jacobianF(n, reaction,jacobianF) has been called just before jacobianFMatrix(...). In that case the computation of
     velocityLocal is not required again.
  */
  if (up2Date == 0)
  {
    /* velocityLocal = M.reaction + qLocal */
    int incx = 1, incy = 1;
    DCOPY(Fsize, qLocal, incx, velocityLocal, incy);
    DGEMV(LA_NOTRANS, nLocal, nLocal, 1.0, MLocal, 3, reaction, incx, 1.0, velocityLocal, incy);
    /*   velocityLocal[0] = MLocal[0]*reaction[0] + MLocal[Fsize]*reaction[1] + MLocal[2*Fsize]*reaction[2] + qLocal[0]; */
    /*   velocityLocal[1] = MLocal[1]*reaction[0] + MLocal[Fsize+1]*reaction[1] + MLocal[2*Fsize+1]*reaction[2] + qLocal[1]; */
    /*   velocityLocal[2] = MLocal[2]*reaction[0] + MLocal[Fsize+2]*reaction[1] + MLocal[2*Fsize+2]*reaction[2] + qLocal[2]; */

    an = 1. / MLocal[0];
    double alpha = MLocal[Fsize + 1] + MLocal[2 * Fsize + 2];
    double det = MLocal[1 * Fsize + 1] * MLocal[2 * Fsize + 2] - MLocal[2 * Fsize + 1] * MLocal[1 * Fsize + 2];
    double beta = alpha * alpha - 4 * det;
    if (beta > 0.)
      beta = sqrt(beta);
    else
      beta = 0.;
    at = 2 * (alpha - beta) / ((alpha + beta) * (alpha + beta));

    /* Projection on [0, +infty[ and on D(0, mu*reactionn) */
    projN = reaction[0] - an * velocityLocal[0];
    projT = reaction[1] - at * velocityLocal[1];
    projS = reaction[2] - at * velocityLocal[2];
  }

  double num;
  double coef2 = mu_i * mu_i;
  if (projN > 0)
  {
    F[0] = velocityLocal[0];
  }
  else
  {
    F[0] = reaction[0] / an;
  }

  double mrn = projT * projT + projS * projS;
  if (mrn <= coef2 * reaction[0]*reaction[0])
  {
    F[1] = velocityLocal[1];
    F[2] = velocityLocal[2];
  }
  else
  {
    num  = mu_i / sqrt(mrn);
    F[1] = (reaction[1] - projT * reaction[0] * num) / at;
    F[2] = (reaction[2] - projS * reaction[0] * num) / at;
  }
  /*   for(int l = 0; l < 3 ; ++l) */
  /*     printf(" %lf", F[l]); */
  /*   printf("\n"); */

}

/* Compute Jacobian of function F */
void jacobianF_AC(int Fsize, double *reaction, double *jacobianFMatrix, int up2Date)
{
  if (jacobianFMatrix == NULL)
  {
    fprintf(stderr, "FrictionContact3D_AlartCurnier::jacobianF_AC error: jacobianMatrix == NULL.\n");
    exit(EXIT_FAILURE);
  }
  if (Fsize != nLocal)
  {
    fprintf(stderr, "FrictionContact3D_AlartCurnier::jacobianF_AC error, wrong block size.\n");
    exit(EXIT_FAILURE);
  }

  /* Warning: input reaction is not necessary equal to the last computed value of reactionBlock */

  /* up2Date = 1 = true if F(n, reaction,F) has been called just before jacobianFMatrix(...). In that case the computation of
     velocityLocal is not required again.
  */
  if (up2Date == 0)
  {
    /* velocityLocal = M.reaction + qLocal */
    int incx = 1, incy = 1;
    DCOPY(Fsize, qLocal, incx, velocityLocal, incy);
    DGEMV(LA_NOTRANS, nLocal, nLocal, 1.0, MLocal, 3, reaction, incx, 1.0, velocityLocal, incy);

    an = 1. / MLocal[0];
    double alpha = MLocal[Fsize + 1] + MLocal[2 * Fsize + 2];
    double det = MLocal[1 * Fsize + 1] * MLocal[2 * Fsize + 2] - MLocal[2 * Fsize + 1] * MLocal[1 * Fsize + 2];
    double beta = alpha * alpha - 4 * det;
    if (beta > 0.)
      beta = sqrt(beta);
    else
      beta = 0.;
    at = 2 * (alpha - beta) / ((alpha + beta) * (alpha + beta));

    /* Projection on [0, +infty[ and on D(0, mu*reactionn) */
    projN = reaction[0] - an * velocityLocal[0];
    projT = reaction[1] - at * velocityLocal[1];
    projS = reaction[2] - at * velocityLocal[2];
  }

  double coef2 = mu_i * mu_i;

  int i, j;
  if (projN > 0)
  {
    for (j = 0; j < Fsize; ++j)
      jacobianFMatrix[j * Fsize] = MLocal[j * Fsize];
  }
  else
  {
    jacobianFMatrix[0] = 1.0 / an;
  }

  double mrn = projT * projT + projS * projS;
  double num, rcof, mrn3;
  double tmp;
  if (mrn <= coef2 * reaction[0]*reaction[0])
    for (i = 1; i < Fsize; ++i)
      for (j = 0; j < Fsize; ++j)
        jacobianFMatrix[j * Fsize + i] = MLocal[j * Fsize + i];
  else
  {
    num  = 1. / sqrt(mrn);
    mrn3 = 1. / sqrt(mrn) * sqrt(mrn) * sqrt(mrn);
    rcof = mu_i / at;
    tmp = at * mrn3 * (MLocal[1] * projT + MLocal[2] * projS);
    jacobianFMatrix[1] = -rcof * (num * projT + reaction[0] * projT * tmp);
    jacobianFMatrix[2] = -rcof * (num * projS + reaction[0] * projS * tmp);

    tmp = mrn3 * ((1 - at * MLocal[Fsize + 1]) * projT - at * MLocal[Fsize + 2] * projS);
    jacobianFMatrix[1 * Fsize + 1] = (1 - mu_i * reaction[0] * (num * (1 - at * MLocal[Fsize + 1]) - projT * tmp)) / at;
    jacobianFMatrix[1 * Fsize + 2] =  - rcof * reaction[0] * ((-num * at * MLocal[Fsize + 2]) - projS * tmp);

    tmp = mrn3 * ((1 - at * MLocal[2 * Fsize + 2]) * projS - at * MLocal[2 * Fsize + 1] * projT);
    jacobianFMatrix[2 * Fsize + 1] =  - rcof * reaction[0] * ((-num * at * MLocal[2 * Fsize + 1]) - projT * tmp);
    jacobianFMatrix[2 * Fsize + 2] = (1 - mu_i * reaction[0] * (num * (1 - at * MLocal[2 * Fsize + 2]) - projS * tmp)) / at;
  }
  /*   for(int l = 0; l < 9 ; ++l) */
  /*     printf(" %lf", jacobianFMatrix[l]); */
  /*   printf("\n"); */
}

void frictionContact3D_AC_post(int contact, double* reaction)
{
  /* This function is required in the interface but useless in Alart-Curnier case */
}

void computeFGlobal_AC(double* reaction, double* FGlobal)
{
  int contact, numberOfContacts = n / 3, diagPos = 0, position;
  int in, it, is, inc, incx;
  double * reactionLocal;
  double alpha, det, beta, num, coef2, mrn;
  for (contact = 0; contact < numberOfContacts; ++contact)
  {
    position = 3 * contact;
    if (MGlobal->storageType == 1) /* Sparse storage */
    {
      /* The part of MGlobal which corresponds to the current block is copied into MLocal */
      diagPos = numberOfContacts * contact + contact;
      MLocal = MGlobal->matrix1->block[diagPos];
    }
    else if (MGlobal->storageType == 0)
    {
      in = 3 * contact;
      it = in + 1;
      is = it + 1;
      inc = n * in;
      double *MM = MGlobal->matrix0;
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

    reactionLocal = &reaction[3 * contact];
    incx = 3;
    velocityLocal[0] = DDOT(3 , MLocal , incx , reactionLocal , 1) + qLocal[0];
    velocityLocal[1] = DDOT(3 , MLocal , incx , reactionLocal , 1) + qLocal[1];
    velocityLocal[2] = DDOT(3 , MLocal , incx , reactionLocal , 1) + qLocal[2];
    an = 1. / MLocal[0];
    alpha = MLocal[4] + MLocal[8];
    det = MLocal[4] * MLocal[8] - MLocal[7] + MLocal[5];
    beta = alpha * alpha - 4 * det;
    at = 2 * (alpha - beta) / ((alpha + beta) * (alpha + beta));
    projN = reactionLocal[0] - an * velocityLocal[0];
    projT = reactionLocal[1] - at * velocityLocal[1];
    projS = reactionLocal[2] - at * velocityLocal[2];
    coef2 = mu[contact] * mu[contact];
    if (projN > 0)
    {
      FGlobal[position] = velocityLocal[0];
    }
    else
    {
      FGlobal[position] = reactionLocal[0] / an;
    }

    mrn = projT * projT + projS * projS;
    if (mrn <= coef2 * reactionLocal[0]*reactionLocal[0])
    {
      FGlobal[position + 1] = velocityLocal[1];
      FGlobal[position + 2] = velocityLocal[2];
    }
    else
    {
      num  = mu[contact] / sqrt(mrn);
      FGlobal[position + 1] = (reactionLocal[1] - projT * reactionLocal[0] * num) / at;
      FGlobal[position + 2] = (reactionLocal[2] - projS * reactionLocal[0] * num) / at;
    }
  }
}

void frictionContact3D_AC_free()
{
  MGlobal = NULL;
  qGlobal = NULL;
  mu = NULL;
  if (isMAllocatedIn == 1)
    free(MLocal);
  MLocal = NULL;
}


int AlartCurnierNewton(int Fsize, double * reactionBlock, int *iparam, double *dparam)
{

  int ITMAX = iparam[0];
  int i, j, inew, INEWTON;
  int STATUS;
  double MU = mu_i;
  double TOL = dparam[0];
  double RVN, RVT, RVS, RV, RV1, RV3;
  double PhiT4, PhiS4, Phi3;
  double Treshold, DET, DET1;
  double d1, d2, d3, AA, BB, CC;
  double ERW, WREF;

  double Rloci[3], WRloc[3], WRloci[3], Rerror[3], Verror[3], Rloc[3], S[3];
  double A[3][3], AWB[3][3], W[3][3];
  Rloc[0] = reactionBlock[1];
  S[0] = qLocal[1];
  Rloc[1] = reactionBlock[2];
  S[1] = qLocal[2];
  Rloc[2] = reactionBlock[0];
  S[2] = qLocal[0];
  W[2][2] =  MLocal[0 + 0 * 3];
  W[2][1] =  MLocal[0 + 2 * 3];
  W[2][0] =  MLocal[0 + 1 * 3];
  W[1][2] =  MLocal[2 + 0 * 3];
  W[1][1] =  MLocal[2 + 2 * 3];
  W[1][0] =  MLocal[2 + 1 * 3];
  W[0][2] =  MLocal[1 + 0 * 3];
  W[0][1] =  MLocal[1 + 2 * 3];
  W[0][0] =  MLocal[1 + 1 * 3];



  double RhoN;
  double RhoT;
  double SW;
  double DW;

  SW = W[0][0] + W[1][1];

  DW = SW * SW - 4.0 * (W[0][0] * W[1][1] - W[1][0] * W[0][1]);

  if (DW > 0.0) DW = sqrt(DW);
  else DW = 0.0;

  RhoN = 1.0 / W[2][2];
  RhoT = 2.0 * (SW - DW) / ((SW + DW) * (SW + DW));




  for (i = 0 ; i < 3 ; ++i)  WRloc[i] = W[i][0] * Rloc[0] + W[i][1] * Rloc[1] + W[i][2] * Rloc[2];

  /* début des itérations de Newton, ITMAX est le nombre d'itérations maximum */

  for (inew = 0 ; inew < ITMAX ; ++inew)
  {

    for (i = 0 ; i < 3 ; ++i)
    {
      WRloci[i] =  WRloc[i];
      Rloci[i]  =  Rloc[i];
      for (j = 0 ; j < 3 ; ++j)
      {
        AWB[i][j] = 0.0;
        A[i][j] = 0.0;
      }
    }
    /* On cherche à exprimer la matrice gradient:
     * Dphi=| I  -W  |
     *      | Ap  Bp |
     * p étant l'indice inew.
     * On effectue tour à tour les projections sur le cone
     * positif et sur le cone de Coulomb pour exprimer les différentes
     * valeurs des matrices Ap et Bp.
     */

    RVN = Rloc[2] - RhoN * (WRloc[2] + S[2]);

    if (RVN >= 0.0)
    {

      /* Projection sur le cone positif */

      Phi3 = RhoN * (WRloc[2] + S[2]);

      for (i = 0 ; i < 3 ; ++i) AWB[2][i] = RhoN * W[2][i];
      STATUS = 1;
    }
    else
    {
      Phi3 = Rloc[2];
      AWB[2][2] = 1.0;
      AWB[2][1] = 0.0;
      AWB[2][0] = 0.0;
      STATUS = 0;
    }

    RVT = Rloc[1] - RhoT * (WRloc[1] + S[1]);
    RVS = Rloc[0] - RhoT * (WRloc[0] + S[0]);

    RV = sqrt(RVT * RVT + RVS * RVS);

    Treshold = MU * Rloc[2];

    if ((RV < Treshold) || (RV < 1.e-20))
    {

      PhiT4 = RhoT * (WRloc[1] + S[1]);
      PhiS4 = RhoT * (WRloc[0] + S[0]);

      for (i = 0 ; i < 2 ; ++i)
        for (j = 0 ; j < 3 ; ++j)
          AWB[i][j] = RhoT * W[i][j];
      STATUS = 1;
    }
    else
    {

      RV1 = 1.0 / RV;
      PhiT4 = Rloc[1] - Treshold * RVT * RV1;
      PhiS4 = Rloc[0] - Treshold * RVS * RV1;

      AWB[0][2] = -MU * RVS * RV1;
      AWB[1][2] = -MU * RVT * RV1;

      RV3 = Treshold * RV1 * RV1 * RV1;

      A[1][1] = RV3 * RVS * RVS;
      A[1][0] = -RV3 * RVT * RVS;
      A[0][1] = A[1][0];
      A[0][0] = RV3 * RVT * RVT;

      for (i = 0 ; i < 2 ; ++i) AWB[i][i] = 1.0 - A[i][i];
      AWB[1][0] = - A[1][0];
      AWB[0][1] = - A[0][1];

      for (i = 0 ; i < 2 ; ++i)
        for (j = 0 ; j < 2 ; ++j)
          A[i][j] = RhoT * A[i][j];

      for (i = 0 ; i < 2 ; ++i)
        for (j = 0 ; j < 2 ; ++j) AWB[i][j] += A[i][0] * W[0][j] + A[i][1] * W[1][j];
      STATUS = 2;
    }

    d1 = AWB[1][1] * AWB[2][2] - AWB[1][2] * AWB[2][1];
    d2 = AWB[1][0] * AWB[2][2] - AWB[2][0] * AWB[1][2];
    d3 = AWB[1][0] * AWB[2][1] - AWB[2][0] * AWB[1][1];

    DET = AWB[0][0] * d1 - AWB[0][1] * d2 + AWB[0][2] * d3;

    if (fabs(DET) < 1.e-20)
    {
      printf("DET NULL\n");
    }

    DET1 = 1.0 / DET;

    AA
      = (AWB[0][1] * AWB[1][2] - AWB[1][1] * AWB[0][2]) * PhiS4
        - (AWB[0][0] * AWB[1][2] - AWB[1][0] * AWB[0][2]) * PhiT4
        + (AWB[0][0] * AWB[1][1] - AWB[1][0] * AWB[0][1]) * Phi3;

    Rloc[2] = fmax(0.0 , Rloc[2] - AA * DET1);

    BB
      = -(AWB[0][1] * AWB[2][2] - AWB[2][1] * AWB[0][2]) * PhiS4
        + (AWB[0][0] * AWB[2][2] - AWB[2][0] * AWB[0][2]) * PhiT4
        - (AWB[0][0] * AWB[2][1] - AWB[2][0] * AWB[0][1]) * Phi3;

    Rloc[1] = Rloc[1] - BB * DET1;

    CC = d1 * PhiS4 - d2 * PhiT4 + d3 * Phi3;

    Rloc[0] = Rloc[0] - CC * DET1;

    for (i = 0 ; i < 3 ; ++i) WRloc[i] = W[i][0] * Rloc[0] + W[i][1] * Rloc[1] + W[i][2] * Rloc[2];

    WREF = 0.0;
    ERW  = 0.0;

    for (i = 0 ; i < 3 ; ++i)
    {
      Rerror[i] = Rloci[i] - Rloc[i];
      Verror[i] = WRloci[i] - WRloc[i];
      ERW  += Rerror[i] * Verror[i];
      WREF += WRloc[i] * Rloci[i];
    }

    if (WREF < 1.e-20) WREF = 1.0;
    ERW = ERW / WREF;

    INEWTON = inew + 1;
    if (fabs(ERW) < TOL * TOL)  break;
  }
  if (verbose > 0)
  {
    printf("-----------------------------------    AlartCurnierNewton number of iteration = %i \n", INEWTON);
    printf("-----------------------------------    AlartCurnierNewton error = %.10e \n", ERW);
  }

  dparam[1] = ERW;
  reactionBlock[0] = Rloc[2];
  reactionBlock[1] = Rloc[0];
  reactionBlock[2] = Rloc[1];
  if (fabs(ERW) > TOL * TOL)  return 1;
  else return 0;


}
