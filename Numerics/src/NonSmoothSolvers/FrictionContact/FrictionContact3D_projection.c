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


#include "FrictionContact3D_Solvers.h"
#include "projectionOnCone.h"
#include "projectionOnCylinder.h"
#include "FrictionContact3D_compute_error.h"

#include "SiconosBlas.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

/* Static variables */

/* The global problem of size n= 3*nc, nc being the number of contacts, is locally saved in MGlobal and qGlobal */
/* mu corresponds to the vector of friction coefficients */
/* note that either MGlobal or MBGlobal is used, depending on the chosen storage */
/* static int n=0; */
/* static const NumericsMatrix* MGlobal = NULL; */
/* static const double* qGlobal = NULL; */
/* static const double* mu = NULL; */

/* Local problem operators */
/* static const int nLocal = 3; */
/* static double* MLocal; */
/* static int isMAllocatedIn = 0; /\* True if a malloc is done for MLocal, else false *\/ */
/* static double qLocal[3]; */
/* static double mu_i = 0.0; */

#define VERBOSE_DEBUG

void frictionContact3D_projection_initialize(FrictionContactProblem * problem, FrictionContactProblem * localproblem)
{

}

void frictionContact3D_projection_update(int contact, FrictionContactProblem* problem, FrictionContactProblem* localproblem, double* reaction, SolverOptions* options)
{
  /* Build a local problem for a specific contact
     reaction corresponds to the global vector (size n) of the global problem.
  */

  /* Call the update function which depends on the storage for MGlobal/MBGlobal */
  /* Build a local problem for a specific contact
   reaction corresponds to the global vector (size n) of the global problem.
  */

  /* The part of MGlobal which corresponds to the current block is copied into MLocal */
  frictionContact3D_nsgs_fillMLocal(problem, localproblem, contact);

  /****  Computation of qLocal = qBlock + sum over a row of blocks in MGlobal of the products MLocal.reactionBlock,
     excluding the block corresponding to the current contact. ****/
  frictionContact3D_nsgs_computeqLocal(problem, localproblem, reaction, contact);

  /* Friction coefficient for current block*/
  localproblem->mu[0] = problem->mu[contact];

}

void frictionContact3D_projectionWithDiagonalization_update(int contact, FrictionContactProblem* problem, FrictionContactProblem* localproblem,  double* reaction, SolverOptions* options)
{
  /* Build a local problem for a specific contact
     reaction corresponds to the global vector (size n) of the global problem.
  */

  /* Call the update function which depends on the storage for MGlobal/MBGlobal */
  /* Build a local problem for a specific contact
   reaction corresponds to the global vector (size n) of the global problem.
  */

  /* The part of MGlobal which corresponds to the current block is copied into MLocal */
  frictionContact3D_nsgs_fillMLocal(problem, localproblem, contact);

  /****  Computation of qLocal = qBlock + sum over a row of blocks in MGlobal of the products MLocal.reactionBlock,
     excluding the block corresponding to the current contact. ****/

  NumericsMatrix * MGlobal = problem->M;
  double * MLocal =  localproblem->M->matrix0;


  double *qLocal = localproblem->q;
  double * qGlobal = problem->q;
  int n = 3 * problem->numberOfContacts;


  int in = 3 * contact, it = in + 1, is = it + 1;
  /* reaction current block set to zero, to exclude current contact block */
  /*   double rin= reaction[in] ; double rit= reaction[it] ; double ris= reaction[is] ;  */
  /* qLocal computation*/
  qLocal[0] = qGlobal[in];
  qLocal[1] = qGlobal[it];
  qLocal[2] = qGlobal[is];

  if (MGlobal->storageType == 0)
  {
    double * MM = MGlobal->matrix0;
    int incx = n, incy = 1;
    qLocal[0] += cblas_ddot(n , &MM[in] , incx , reaction , incy);
    qLocal[1] += cblas_ddot(n , &MM[it] , incx , reaction , incy);
    qLocal[2] += cblas_ddot(n , &MM[is] , incx , reaction , incy);
    // Substract diagonal term
    qLocal[0] -= MM[in + n * in] * reaction[in];
    qLocal[1] -= MM[it + n * it] * reaction[it];
    qLocal[2] -= MM[is + n * is] * reaction[is];
  }
  else if (MGlobal->storageType == 1)
  {
    /* qLocal += rowMB * reaction
       with rowMB the row of blocks of MGlobal which corresponds to the current contact
    */
    subRowProdSBM(n, 3, contact, MGlobal->matrix1, reaction, qLocal, 0);
    // Substract diagonal term
    qLocal[0] -= MLocal[0] * reaction[in];
    qLocal[1] -= MLocal[4] * reaction[it];
    qLocal[2] -= MLocal[8] * reaction[is];

  }
  /*   reaction[in] = rin; reaction[it] = rit; reaction[is] = ris; */

  /* Friction coefficient for current block*/
  localproblem->mu[0] = problem->mu[contact];
}


void frictionContact3D_projection_initialize_with_regularization(FrictionContactProblem * problem, FrictionContactProblem * localproblem)
{
  if (!localproblem->M->matrix0)
    localproblem->M->matrix0 = (double*)malloc(9 * sizeof(double));
}

void frictionContact3D_projection_update_with_regularization(int contact, FrictionContactProblem * problem, FrictionContactProblem * localproblem, double* reaction, SolverOptions* options)
{


  /* Build a local problem for a specific contact
     reaction corresponds to the global vector (size n) of the global problem.
  */

  /* Call the update function which depends on the storage for MGlobal/MBGlobal */
  /* Build a local problem for a specific contact
   reaction corresponds to the global vector (size n) of the global problem.
  */

  /* The part of MGlobal which corresponds to the current block is copied into MLocal */

  NumericsMatrix * MGlobal = problem->M;

  int n = 3 * problem->numberOfContacts;


  int storageType = MGlobal->storageType;
  if (storageType == 0)
    // Dense storage
  {
    int in = 3 * contact, it = in + 1, is = it + 1;
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
  else if (storageType == 1)
  {
    int diagPos = getDiagonalBlockPos(MGlobal->matrix1, contact);
    /*     for (int i =0 ; i< 3*3 ; i++) localproblem->M->matrix0[i] = MGlobal->matrix1->block[diagPos][i] ; */
    cblas_dcopy(9, MGlobal->matrix1->block[diagPos], 1, localproblem->M->matrix0 , 1);

  }
  else
    numericsError("FrictionContact3D_projection -", "unknown storage type for matrix M");

  /****  Computation of qLocal = qBlock + sum over a row of blocks in MGlobal of the products MLocal.reactionBlock,
     excluding the block corresponding to the current contact. ****/
  frictionContact3D_nsgs_computeqLocal(problem, localproblem, reaction, contact);

  double rho = options->dparam[3];
  for (int i = 0 ; i < 3 ; i++) localproblem->M->matrix0[i + 3 * i] += rho ;

  double *qLocal = localproblem->q;
  int in = 3 * contact, it = in + 1, is = it + 1;

  /* qLocal computation*/
  qLocal[0] -= rho * reaction[in];
  qLocal[1] -= rho * reaction[it];
  qLocal[2] -= rho * reaction[is];

  /* Friction coefficient for current block*/
  localproblem->mu[0] = problem->mu[contact];


}

int frictionContact3D_projectionWithDiagonalization_solve(FrictionContactProblem* localproblem, double* reaction, SolverOptions * options)
{



  /* Current block position */

  /* Builds local problem for the current contact */
  /*  frictionContact3D_projection_update(contact, reaction); */
  /*  frictionContact3D_projectionWithDiagonalization_update(contact, reaction);  */


  double * MLocal = localproblem->M->matrix0;
  double * qLocal = localproblem->q;
  double mu_i = localproblem->mu[0];
  int nLocal = 3;

  double mrn, num, mu2 = mu_i * mu_i;


  /* projection */
  if (qLocal[0] > 0.)
  {
    reaction[0] = 0.;
    reaction[1] = 0.;
    reaction[2] = 0.;
  }
  else
  {
    if (MLocal[0] < DBL_EPSILON || MLocal[nLocal + 1] < DBL_EPSILON || MLocal[2 * nLocal + 2] < DBL_EPSILON)
    {
      fprintf(stderr, "FrictionContact3D_projection error: null term on MLocal diagonal.\n");
      exit(EXIT_FAILURE);
    }

    reaction[0] = -qLocal[0] / MLocal[0];
    reaction[1] = -qLocal[1] / MLocal[nLocal + 1];
    reaction[2] = -qLocal[2] / MLocal[2 * nLocal + 2];

    mrn = reaction[1] * reaction[1] + reaction[2] * reaction[2];

    if (mrn > mu2 * reaction[0]*reaction[0])
    {
      num = mu_i * reaction[0] / sqrt(mrn);
      reaction[1] = reaction[1] * num;
      reaction[2] = reaction[2] * num;
    }
  }
  return 0;
}

void frictionContact3D_projectionOnConeWithLocalIteration_initialize(FrictionContactProblem * problem, FrictionContactProblem * localproblem, SolverOptions* localsolver_options )
{
  int nc = problem->numberOfContacts;
  /* printf("frictionContact3D_projectionOnConeWithLocalIteration_initialize. Allocation of dwork\n"); */
  localsolver_options->dWork = (double *)malloc(nc * sizeof(double));
  for (int i = 0; i < nc; i++)
  {
    localsolver_options->dWork[i]=1.0;
  }
}

void frictionContact3D_projectionOnConeWithLocalIteration_free(FrictionContactProblem * problem, FrictionContactProblem * localproblem, SolverOptions* localsolver_options )
{
  free(localsolver_options->dWork);
  localsolver_options->dWork=NULL;
}

int frictionContact3D_projectionOnConeWithLocalIteration_solve(FrictionContactProblem* localproblem, double* reaction, SolverOptions* options)
{
  /* int and double parameters */
  int* iparam = options->iparam;
  double* dparam = options->dparam;

  double * MLocal = localproblem->M->matrix0;
  double * qLocal = localproblem->q;
  double mu_i = localproblem->mu[0];
  int nLocal = 3;


  /*   /\* Builds local problem for the current contact *\/ */
  /*   frictionContact3D_projection_update(localproblem, reaction); */


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
  double rho=   options->dWork[options->iparam[4]] , rho_k;
  /* printf ("saved rho = %14.7e\n",rho ); */
  /* printf ("options->iparam[4] = %i\n",options->iparam[4] ); */

  

  int incx = 1, incy = 1;



  double velocity[3],velocity_k[3],reaction_k[3];
  double normUT;
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
  while ((localerror > localtolerance) && (localiter < iparam[0]))
  {
    localiter ++;

    /*    printf ("reaction[0] = %14.7e\n",reaction[0]); */
    /*    printf ("reaction[1] = %14.7e\n",reaction[1]); */
    /*    printf ("reaction[2] = %14.7e\n",reaction[2]); */

    /* Store the error */
    localerror_k = localerror;

    /* store the reaction at the beginning of the iteration */
    cblas_dcopy(nLocal , reaction , 1 , reaction_k, 1);
    


    /* velocity_k <- q  */
    cblas_dcopy(nLocal , qLocal , 1 , velocity_k, 1);
    
    /* velocity_k <- q + M * reaction  */
    cblas_dgemv(CblasColMajor,CblasNoTrans, nLocal, nLocal, 1.0, MLocal, 3, reaction, incx, 1.0, velocity_k, incy);
    

    ls_iter = 0 ;
    success =0;
    
    normUT = sqrt(velocity_k[1] * velocity_k[1] + velocity_k[2] * velocity_k[2]);
    while (!success && (ls_iter < ls_itermax))
    {
      
      rho_k = rho * pow(tau,ls_iter);
      reaction[0] = reaction_k[0] -  rho_k * (velocity_k[0] + mu_i * normUT);
      reaction[1] = reaction_k[1] - rho_k * velocity_k[1];
      reaction[2] = reaction_k[2] - rho_k * velocity_k[2];
      
      projectionOnCone(&reaction[0], mu_i);

      /* velocity <- q  */
      cblas_dcopy(nLocal , qLocal , 1 , velocity, 1);
      
      /* velocity <- q + M * reaction  */
      cblas_dgemv(CblasColMajor,CblasNoTrans, nLocal, nLocal, 1.0, MLocal, 3, reaction, incx, 1.0, velocity, incy);

      a1 = sqrt((velocity_k[0] - velocity[0]) * (velocity_k[0] - velocity[0]) +
                (velocity_k[1] - velocity[1]) * (velocity_k[1] - velocity[1]) +
                (velocity_k[2] - velocity[2]) * (velocity_k[2] - velocity[2]));
 
      a2 = sqrt((reaction_k[0] - reaction[0]) * (reaction_k[0] - reaction[0]) +
                (reaction_k[1] - reaction[1]) * (reaction_k[1] - reaction[1]) +
                (reaction_k[2] - reaction[2]) * (reaction_k[2] - reaction[2]));
 
      
      
      success = (rho_k*a1 <= L * a2)?1:0;

      /* printf("rho_k = %12.8e\t", rho_k); */
      /* printf("a1 = %12.8e\t", a1); */
      /* printf("a2 = %12.8e\t", a2); */
      /* printf("norm reaction = %12.8e\t",sqrt(( reaction[0]) * (reaction[0]) + */
      /*           ( reaction[1]) *  reaction[1]) + */
      /*           ( reaction[2]) * ( reaction[2])); */
      /* printf("success = %i\n", success); */
      
      ls_iter++;
    }
    
    /* printf("----------------------  localiter = %i\t, rho= %.10e\t, error = %.10e \n", localiter, rho, localerror); */

    /* compute local error */ 
    localerror =0.0;
    FrictionContact3D_unitary_compute_and_add_error(reaction , velocity, mu_i, &localerror);


    /*Update rho*/
      if ((rho_k*a1 < Lmin * a2) && (localerror < localerror_k))
      {
        rho =rho_k/taumin;
      }
      else
        rho =rho_k;
      
    if (verbose > 1)
    {
      printf("----------------------  localiter = %i\t, rho= %.10e\t, error = %.10e \n", localiter, rho, localerror);

    }
    
    options->dWork[options->iparam[4]] =rho;
    
  }
  
  if (localerror > localtolerance)
    return 1;
  return 0;

}
void frictionContact3D_projectionOnCylinder_update(int contact, FrictionContactProblem* problem, FrictionContactProblem* localproblem, double* reaction, SolverOptions* options)
{
  /* Build a local problem for a specific contact
     reaction corresponds to the global vector (size n) of the global problem.
  */

  /* Call the update function which depends on the storage for MGlobal/MBGlobal */
  /* Build a local problem for a specific contact
   reaction corresponds to the global vector (size n) of the global problem.
  */

  /* The part of MGlobal which corresponds to the current block is copied into MLocal */
  frictionContact3D_nsgs_fillMLocal(problem, localproblem, contact);

  /****  Computation of qLocal = qBlock + sum over a row of blocks in MGlobal of the products MLocal.reactionBlock,
     excluding the block corresponding to the current contact. ****/
  frictionContact3D_nsgs_computeqLocal(problem, localproblem, reaction, contact);

  /* Friction coefficient for current block*/
  localproblem->mu[0] = (options->dWork[contact]);

}


int frictionContact3D_projectionOnCone_solve(FrictionContactProblem* localproblem, double* reaction, SolverOptions * options)
{


  /*  /\* Builds local problem for the current contact *\/ */
  /*   frictionContact3D_projection_update(contact, reaction); */



  double * MLocal = localproblem->M->matrix0;
  double * qLocal = localproblem->q;
  double mu_i = localproblem->mu[0];
  int nLocal = 3;

  /* this part is critical for the success of the projection */
  /*double an = 1./(MLocal[0]);*/
  /*   double alpha = MLocal[nLocal+1] + MLocal[2*nLocal+2]; */
  /*   double det = MLocal[1*nLocal+1]*MLocal[2*nLocal+2] - MLocal[2*nLocal+1] + MLocal[1*nLocal+2]; */
  /*   double beta = alpha*alpha - 4*det; */
  /*   double at = 2*(alpha - beta)/((alpha + beta)*(alpha + beta)); */

  //double an = 1./(MLocal[0]+mu_i);
  double an = 1. / (MLocal[0]);
  
  
  int incx = 1, incy = 1;
  double worktmp[3];
  double normUT;
  cblas_dcopy(nLocal , qLocal, incx , worktmp , incy);

  cblas_dgemv(CblasColMajor,CblasNoTrans, nLocal, nLocal, 1.0, MLocal, 3, reaction, incx, 1.0, worktmp, incy);
  normUT = sqrt(worktmp[1] * worktmp[1] + worktmp[2] * worktmp[2]);
  reaction[0] -= an * (worktmp[0] + mu_i * normUT);
  reaction[1] -= an * worktmp[1];
  reaction[2] -= an * worktmp[2];


  projectionOnCone(reaction, mu_i);
  return 0;

}



void frictionContact3D_projection_free(FrictionContactProblem * problem, FrictionContactProblem * localproblem, SolverOptions* localsolver_options )
{
}

void frictionContact3D_projection_with_regularization_free(FrictionContactProblem * problem, FrictionContactProblem * localproblem, SolverOptions* localsolver_options )
{
  free(localproblem->M->matrix0);
  localproblem->M->matrix0 = NULL;
}



int frictionContact3D_projectionOnCone_velocity_solve(FrictionContactProblem* localproblem, double* velocity,  SolverOptions* options)
{
  /* int and double parameters */
  /*     int* iparam = options->iparam; */
  /*     double* dparam = options->dparam; */
  /* Current block position */

  /* Builds local problem for the current contact */
  /*   frictionContact3D_projection_update(contact, velocity); */

  double * MLocal = localproblem->M->matrix0;
  double * qLocal = localproblem->q;
  double mu_i = localproblem->mu[0];
  int nLocal = 3;


  /*double an = 1./(MLocal[0]);*/
  /*   double alpha = MLocal[nLocal+1] + MLocal[2*nLocal+2]; */
  /*   double det = MLocal[1*nLocal+1]*MLocal[2*nLocal+2] - MLocal[2*nLocal+1] + MLocal[1*nLocal+2]; */
  /*   double beta = alpha*alpha - 4*det; */
  /*   double at = 2*(alpha - beta)/((alpha + beta)*(alpha + beta)); */

  double an = 1. / (MLocal[0]);
  int incx = 1, incy = 1;
  double worktmp[3];
  double normUT;

  cblas_dcopy(nLocal , qLocal, incx , worktmp , incy);
  cblas_dgemv(CblasColMajor,CblasNoTrans, nLocal, nLocal, 1.0, MLocal, 3, velocity, incx, 1.0, worktmp, incy);
  normUT = sqrt(velocity[1] * velocity[1] + velocity[2] * velocity[2]);
  velocity[0] -=  - mu_i * normUT + an * (worktmp[0]);
  velocity[1] -= an * worktmp[1];
  velocity[2] -= an * worktmp[2];
  double invmui = 1.0 / mu_i;
  projectionOnCone(velocity, invmui);

  normUT = sqrt(velocity[1] * velocity[1] + velocity[2] * velocity[2]);
  velocity[0] -= mu_i * normUT;
  return 0;
}



int frictionContact3D_projectionOnCylinder_solve(FrictionContactProblem *localproblem , double* reaction, SolverOptions* options)
{
  /* int and double parameters */
  /*   int* iparam = options->iparam; */
  /*   double* dparam = options->dparam; */
  double * MLocal = localproblem->M->matrix0;
  double * qLocal = localproblem->q;
  int nLocal = 3;


  /* Builds local problem for the current contact */
  /*   frictionContact3D_projection_update(contact, reaction); */


  /*double an = 1./(MLocal[0]);*/
  /*   double alpha = MLocal[nLocal+1] + MLocal[2*nLocal+2]; */
  /*   double det = MLocal[1*nLocal+1]*MLocal[2*nLocal+2] - MLocal[2*nLocal+1] + MLocal[1*nLocal+2]; */
  /*   double beta = alpha*alpha - 4*det; */
  /*   double at = 2*(alpha - beta)/((alpha + beta)*(alpha + beta)); */

  double an = 1. / (MLocal[0]);

  int incx = 1, incy = 1;
  double worktmp[3];

  double R  = localproblem->mu[0];
  cblas_dcopy(nLocal , qLocal, incx , worktmp , incy);
  cblas_dgemv(CblasColMajor,CblasNoTrans, nLocal, nLocal, 1.0, MLocal, 3, reaction, incx, 1.0, worktmp, incy);
  reaction[0]   -= an * worktmp[0];
  reaction[1] -= an * worktmp[1];
  reaction[2] -= an * worktmp[2];

  projectionOnCylinder(reaction, R);
  return 0;

}

