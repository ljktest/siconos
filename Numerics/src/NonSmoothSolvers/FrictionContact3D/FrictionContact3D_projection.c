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
#include <float.h>

/* Static variables */

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
static double qLocal[3];
static double mu_i = 0.0;

void projection_fillMLocal(int contact)
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
    MLocal = MGlobal->matrix1->block[diagPos];
  }
  else
    numericsError("FrictionContact3D_projection -", "unknown storage type for matrix M");
}

void frictionContact3D_projection_initialize(int n0, const NumericsMatrix*const M0, const double*const q0, const double*const mu0)
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
  if (MGlobal->storageType == 0)
  {
    MLocal = (double*)malloc(nLocal * nLocal * sizeof(*MLocal));
    isMAllocatedIn = 1;
  }
  else
    isMAllocatedIn = 0;
}

void frictionContact3D_projection_update(int contact, double* reaction)
{
  /* Build a local problem for a specific contact
     reaction corresponds to the global vector (size n) of the global problem.
  */

  /* Call the update function which depends on the storage for MGlobal/MBGlobal */
  /* Build a local problem for a specific contact
   reaction corresponds to the global vector (size n) of the global problem.
  */

  /* The part of MGlobal which corresponds to the current block is copied into MLocal */
  projection_fillMLocal(contact);

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

void frictionContact3D_projectionWithDiagonalization_solve(int contact, int dimReaction, double* reaction, int* iparam, double* dparam)
{
  /* Current block position */
  int pos = contact * nLocal;

  /* Builds local problem for the current contact */
  frictionContact3D_projection_update(contact, reaction);

  double mrn, num, mu2 = mu_i * mu_i;


  /* projection */
  if (qLocal[0] > 0.)
  {
    reaction[pos] = 0.;
    reaction[pos + 1] = 0.;
    reaction[pos + 2] = 0.;
  }
  else
  {
    if (MLocal[0] < DBL_EPSILON || MLocal[nLocal + 1] < DBL_EPSILON || MLocal[2 * nLocal + 2] < DBL_EPSILON)
    {
      fprintf(stderr, "FrictionContact3D_projection error: null term on MLocal diagonal.\n");
      exit(EXIT_FAILURE);
    }

    reaction[pos] = -qLocal[0] / MLocal[0];
    reaction[pos + 1] = -qLocal[1] / MLocal[nLocal + 1];
    reaction[pos + 2] = -qLocal[2] / MLocal[2 * nLocal + 2];

    mrn = reaction[pos + 1] * reaction[pos + 1] + reaction[pos + 2] * reaction[pos + 2];

    if (mrn > mu2 * reaction[pos]*reaction[pos])
    {
      num = mu_i * reaction[pos] / sqrt(mrn);
      reaction[pos + 1] = reaction[pos + 1] * num;
      reaction[pos + 2] = reaction[pos + 2] * num;
    }
  }
}


void frictionContact3D_projectionOnConeWithLocalIteration_solve(int contact, int dimReaction, double* reaction, int* iparam, double* dparam)
{
  /* Current block position */
  int pos = contact * nLocal;

  /* Builds local problem for the current contact */
  frictionContact3D_projection_update(contact, reaction);

  double  mu2 = mu_i * mu_i;

  /*double an = 1./(MLocal[0]);*/
  /*   double alpha = MLocal[nLocal+1] + MLocal[2*nLocal+2]; */
  /*   double det = MLocal[1*nLocal+1]*MLocal[2*nLocal+2] - MLocal[2*nLocal+1] + MLocal[1*nLocal+2]; */
  /*   double beta = alpha*alpha - 4*det; */
  /*   double at = 2*(alpha - beta)/((alpha + beta)*(alpha + beta)); */

  double an = 1. / (MLocal[0] + mu_i);

  int incx = 1, incy = 1;



  double worktmp[3];
  double normUT;
  double localerror = 1.0;
  //printf ("localerror = %14.7e\n",localerror );
  int localiter = 0;
  while ((localerror > dparam[0]) && (localiter < iparam[0]))
  {
    localiter ++;
    printf("----------------------\n");
    printf("localiter = %i\n", localiter);
    /*    printf ("reaction[pos] = %14.7e\n",reaction[pos]); */
    /*    printf ("reaction[pos+1] = %14.7e\n",reaction[pos+1]); */
    /*    printf ("reaction[pos+2] = %14.7e\n",reaction[pos+2]); */
    DCOPY(nLocal , qLocal, incx , worktmp , incy);
    DGEMV(LA_NOTRANS, nLocal, nLocal, 1.0, MLocal, 3, &reaction[pos], incx, 1.0, worktmp, incy);
    normUT = sqrt(worktmp[1] * worktmp[1] + worktmp[2] * worktmp[2]);
    /*    printf ("qLocal[0] = %14.7e\n",qLocal[0]); */
    /*    printf ("qLocal[1] = %14.7e\n",qLocal[1]); */
    /*    printf ("qLocal[2] = %14.7e\n",qLocal[2]); */
    /*    printf ("MLocal[0] = %14.7e\n",MLocal[0]); */
    /*    printf ("MLocal[1] = %14.7e\n",MLocal[1]); */
    /*    printf ("MLocal[2] = %14.7e\n",MLocal[2]); */
    /*    printf ("MLocal[3] = %14.7e\n",MLocal[3]); */
    /*    printf ("MLocal[4] = %14.7e\n",MLocal[4]); */
    /*    printf ("MLocal[5] = %14.7e\n",MLocal[5]); */
    /*    printf ("MLocal[6] = %14.7e\n",MLocal[6]); */
    /*    printf ("MLocal[7] = %14.7e\n",MLocal[7]); */
    /*    printf ("MLocal[8] = %14.7e\n",MLocal[8]); */
    /*    printf ("velocity[0] = %14.7e\n",worktmp[0]); */
    /*    printf ("velocity[1] = %14.7e\n",worktmp[1]); */
    /*    printf ("velocity[2] = %14.7e\n",worktmp[2]); */
    /*    printf ("normUT = %14.7e\n",normUT); */
    /*    printf ("Modified velocity[0] = %14.7e\n",worktmp[0]+mu_i*normUT); */
    /*    printf ("Modified velocity[1] = %14.7e\n",worktmp[1]); */
    /*    printf ("Modified velocity[2] = %14.7e\n",worktmp[2]); */



    reaction[pos] -= an * (worktmp[0] + mu_i * normUT);
    reaction[pos + 1] -= an * worktmp[1];
    reaction[pos + 2] -= an * worktmp[2];
    worktmp[0] = reaction[pos] + an * (worktmp[0] + mu_i * normUT);
    worktmp[1] = reaction[pos + 1] + an * worktmp[1];
    worktmp[2] = reaction[pos + 2] + an * worktmp[2];


    projectionOnCone(&reaction[pos], mu_i);

    localerror =  sqrt((worktmp[0] - reaction[pos]) * (worktmp[0] - reaction[pos]) +
                       (worktmp[1] - reaction[pos + 1]) * (worktmp[1] - reaction[pos + 1]) +
                       (worktmp[2] - reaction[pos + 2]) * (worktmp[2] - reaction[pos + 2]));
    /*      printf ("localerror = %14.7e\n",localerror );  */
    /*    printf ("worktmp[0] = %14.7e\n",worktmp[0]); */
    /*    printf ("worktmp[1] = %14.7e\n",worktmp[1]); */
    /*    printf ("worktmp[2] = %14.7e\n",worktmp[2]); */
    printf("reaction[pos] = %14.7e\n", reaction[pos]);
    printf("reaction[pos+1] = %14.7e\n", reaction[pos + 1]);
    printf("reaction[pos+2] = %14.7e\n", reaction[pos + 2]);
    /*    DCOPY( nLocal , qLocal, incx , worktmp , incy ); */
    /*    DGEMV(LA_NOTRANS, nLocal, nLocal, 1.0, MLocal, 3, &reaction[pos], incx, 1.0, worktmp, incy ); */
    /*    normUT = sqrt(worktmp[1]*worktmp[1]+worktmp[2]*worktmp[2]); */
    /*    worktmp[0] = reaction[pos] - (worktmp[0]+mu_i*normUT); */
    /*    worktmp[1] = reaction[pos+1] - worktmp[1]; */
    /*    worktmp[2] = reaction[pos+2] - worktmp[2]; */

    /*    projectionOnCone(worktmp, mu_i);   */
    /*    localerror2 =  sqrt( (worktmp[0]-reaction[pos])*(worktmp[0]-reaction[pos]) + */
    /*            (worktmp[1]-reaction[pos+1])*(worktmp[1]-reaction[pos+1])+ */
    /*            (worktmp[2]-reaction[pos+2])*(worktmp[2]-reaction[pos+2]) ); */
    /*      printf ("localerror2 = %14.7e\n",localerror2 );  */


    /* /\*    printf ("velocity[0] = %14.7e\n",worktmp[0]); *\/ */
    /* /\*    printf ("velocity[1] = %14.7e\n",worktmp[1]); *\/ */
    /* /\*    printf ("velocity[2] = %14.7e\n",worktmp[2]); *\/ */
    /* /\*    printf ("normUT = %14.7e\n",normUT); *\/ */
    /* /\*    printf ("Modified velocity[0] = %14.7e\n",worktmp[0]+mu_i*normUT); *\/ */
    /* /\*    printf ("Modified velocity[1] = %14.7e\n",worktmp[1]); *\/ */
    /* /\*    printf ("Modified velocity[2] = %14.7e\n",worktmp[2]); *\/ */






  }
}
void frictionContact3D_projectionOnCone_solve(int contact, int dimReaction, double* reaction, int* iparam, double* dparam)
{
  /* Current block position */
  int pos = contact * nLocal;

  /* Builds local problem for the current contact */
  frictionContact3D_projection_update(contact, reaction);

  double  mu2 = mu_i * mu_i;

  /*double an = 1./(MLocal[0]);*/
  /*   double alpha = MLocal[nLocal+1] + MLocal[2*nLocal+2]; */
  /*   double det = MLocal[1*nLocal+1]*MLocal[2*nLocal+2] - MLocal[2*nLocal+1] + MLocal[1*nLocal+2]; */
  /*   double beta = alpha*alpha - 4*det; */
  /*   double at = 2*(alpha - beta)/((alpha + beta)*(alpha + beta)); */

  double an = 1. / (MLocal[0] + mu_i);

  int incx = 1, incy = 1;
  double worktmp[3];
  double normUT;
  DCOPY(nLocal , qLocal, incx , worktmp , incy);
  DGEMV(LA_NOTRANS, nLocal, nLocal, 1.0, MLocal, 3, &reaction[pos], incx, 1.0, worktmp, incy);
  normUT = sqrt(worktmp[1] * worktmp[1] + worktmp[2] * worktmp[2]);
  reaction[pos] -= an * (worktmp[0] + mu_i * normUT);
  reaction[pos + 1] -= an * worktmp[1];
  reaction[pos + 2] -= an * worktmp[2];

  projectionOnCone(&reaction[pos], mu_i);

}


void frictionContact3D_projection_free()
{
  MGlobal = NULL;
  qGlobal = NULL;
  mu = NULL;
  if (isMAllocatedIn == 1)
    free(MLocal);
  MLocal = NULL;
}

void projectionOnCone(double* r, double  mu)
{
  double normT = sqrt(r[1] * r[1] + r[2] * r[2]);
  double mu2 = mu * mu;
  if (mu * normT <= - r[0])
  {
    r[0] = 0.0;
    r[1] = 0.0;
    r[2] = 0.0;
    return ;
  }
  else if (normT <= mu * r[0])
  {
    return ;
  }
  else
  {
    r[0] = (mu * normT + r[0]) / (mu2 + 1.0);
    r[1] = mu * r[0] * r[1] / normT;
    r[2] = mu * r[0] * r[2] / normT;
    return;
  }
}



