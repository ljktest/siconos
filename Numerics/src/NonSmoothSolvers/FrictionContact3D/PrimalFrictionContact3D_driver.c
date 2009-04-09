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
#include "LA.h"
#include "Numerics_Options.h"
#include "PrimalFrictionContact3D_Solvers.h"
#include "NonSmoothDrivers.h"
#include "FrictionContact3D_Solvers.h"
int reformulationIntoLocalProblem(PrimalFrictionContact_Problem* problem, FrictionContact_Problem* localproblem)
{
  int info = -1;
  NumericsMatrix *M = problem->M;
  NumericsMatrix *H = problem->H;

  localproblem = (FrictionContact_Problem *) malloc(sizeof(FrictionContact_Problem));
  localproblem->numberOfContacts = problem->numberOfContacts;
  localproblem->isComplete = 0;
  localproblem->mu =  problem->mu;



  if (H->storageType != M->storageType)
  {
    //      if(verbose==1)
    printf(" ->storageType != M->storageType :This case is not taken into account\n");
    return info;
  }

  if (M->storageType = 0)
  {


    int n = M->size0;
    int m = H->size1;
    int nm = n * m;
    int infoDGESV;
    int* ipiv = (int *)malloc(n * sizeof(*ipiv));
    double *Htmp = (double*)malloc(nm * sizeof(double));

    // compute W = H^T M^-1 H
    //Copy Htmp <- H
    DCOPY(nm,  H->matrix0 , 1, Htmp, 1);
    //Compute Htmp   <- M^-1 Htmp
    DGESV(n, 1, M->matrix0, n, ipiv, Htmp, m, infoDGESV);

    double *W = (double*)malloc(m * m * sizeof(double));
    // Compute W <-  H^T M^1 H
    DGEMM(LA_TRANS, LA_NOTRANS, m, m, n, 1.0, H->matrix0, n, Htmp, m, 0.0, W, m);
    NumericsMatrix *Wnum = (NumericsMatrix *)malloc(sizeof(NumericsMatrix));
    Wnum->storageType = 0;
    Wnum-> size0 = m;
    Wnum-> size1 = m;
    Wnum->matrix0 = W;
    // compute q = H^T q +b
    localproblem->q = (double*)malloc(m * sizeof(double));



    //



  }

  else
  {
  }


  return info;
}
int computeGlobalVelocity(PrimalFrictionContact_Problem* problem, FrictionContact_Problem* localproblem, double * globalVelocity)
{
  int info = -1;

  return info;
}

int freeLocalProblem(FrictionContact_Problem* localproblem)
{
  int info = -1;

  return info;
}



int primalfrictionContact3D_driver(PrimalFrictionContact_Problem* problem, double *reaction , double *velocity, double* globalVelocity,  Solver_Options* options, Numerics_Options* global_options)
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
  FrictionContact_Problem* localproblem;
  reformulationIntoLocalProblem(problem, localproblem);



  /* Non Smooth Gauss Seidel (NSGS) */
  if (strcmp(name, "NSGS") == 0)
  {
    if (verbose == 1)
      printf(" ========================== Call NSGS solver for Friction-Contact 3D problem ==========================\n");

    frictionContact3D_nsgs(localproblem, reaction , velocity , &info , options);
  }

  computeGlobalVelocity(problem, localproblem, globalVelocity);
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

