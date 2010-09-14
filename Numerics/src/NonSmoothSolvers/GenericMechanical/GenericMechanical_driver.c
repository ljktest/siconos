/* Siconos-Numerics, Copyright INRIA 2005-2010.
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
#include <math.h>
#include "LA.h"
#include "NumericsOptions.h"
#include "GenericMechanical_Solvers.h"
#include "NonSmoothDrivers.h"
#include "FrictionContact3D_compute_error.h"
#include "FrictionContact3D_unitary_enumerative.h"
//#define GENERICMECHANICAL_DEBUG
//#define GENERICMECHANICAL_DEBUG2
int GenericMechanical_compute_error(GenericMechanicalProblem* pGMP, double *reaction , double *velocity, double tol, SolverOptions* options, double * err)
{
  listNumericsProblem * curProblem = pGMP->firstListElem;
  SparseBlockStructuredMatrix* m = pGMP->M->matrix1;
  int currentRowNumber = 0;
  int ii;
  int  posInX = 0;
  int curSize = 0;
  *err = 0.0;
  double localError = 0;
  int diagBlockNumber;
#ifdef GENERICMECHANICAL_DEBUG
  printf("GenericMechanical compute_error BEGIN:\n");
#endif
  /*update localProblem->q and compute V = M*R+Q of the GMP */
  while (curProblem)
  {
    if (currentRowNumber)
    {
      posInX = m->blocksize0[currentRowNumber - 1];
    }
    curSize = m->blocksize0[currentRowNumber] - posInX;

    /*localproblem->q <-- GMP->q */
    memcpy(curProblem->q, &(pGMP->q[posInX]), curSize * sizeof(double));

    /*computation of the localproblem->q*/
    rowProdNoDiagSBM(pGMP->size, curSize, currentRowNumber, m, reaction, curProblem->q, 0);

    /*computation of the velocity of the GMP*/
    memcpy(velocity + posInX, curProblem->q, curSize * sizeof(double));
    /*add the missing product to the velocity: the diagonal one*/
    diagBlockNumber = getDiagonalBlockPos(m, currentRowNumber);
    DGEMV(LA_NOTRANS, curSize, curSize, 1.0, m->block[diagBlockNumber], curSize, reaction + posInX, 1, 1.0, velocity + posInX, 1);

    /*next*/
    curProblem = curProblem->nextProblem;
    currentRowNumber++;
  }


  /*For each sub-problem, call the corresponding function computing the error.*/
  posInX = 0;
  currentRowNumber = 0;
  curProblem = pGMP->firstListElem;
  while (curProblem)
  {
    if (currentRowNumber)
      posInX = m->blocksize0[currentRowNumber - 1];
    curSize = m->blocksize0[currentRowNumber] - posInX;
    double * Vl = velocity + posInX;
    double * Rl = reaction + posInX;
    for (ii = 0; ii < curSize; ii++)
      if (isnan(Vl[ii]) || isnan(Rl[ii]))
      {
        *err = 10;
        return 1;
      }
    switch (curProblem->type)
    {
    case SICONOS_NUMERICS_PROBLEM_EQUALITY:
    {
      //  LinearSystemProblem* linearProblem = (LinearSystemProblem*) curProblem->problem;
      double * w = velocity + posInX;
      localError = 0.;
      for (ii = 0; ii < curSize; ii++)
      {
        if (fabs(w[ii]) > *err)
        {
          *err = fabs(w[ii]);
          localError = *err;
        }
      }
#ifdef GENERICMECHANICAL_DEBUG2
      printf("GenericMechanical_driver, localerror of linearSystem: %e\n", localError);
#endif
      break;
    }
    case SICONOS_NUMERICS_PROBLEM_LCP:
    {
      //LinearComplementarityProblem* lcpproblem = (LinearComplementarityProblem*) curProblem->problem;

      lcp_compute_error_only(curSize, reaction + posInX, velocity + posInX, &localError);
      localError = localError / (1 + DNRM2(curSize , curProblem->q , 1));
      if (localError > *err)
        *err = localError ;
#ifdef GENERICMECHANICAL_DEBUG2
      printf("GenericMechanical_driver, localerror of lcp: %e\n", localError);
#endif
      break;
    }
    case SICONOS_NUMERICS_PROBLEM_FC3D:
    {
      FrictionContactProblem * fcProblem = (FrictionContactProblem *)curProblem->problem;
      localError = 0.;
      FrictionContact3D_unitary_compute_and_add_error(reaction + posInX, velocity + posInX, fcProblem->mu[0], &localError);
      localError = sqrt(localError) / (1 + DNRM2(curSize , curProblem->q , 1));
      if (localError > *err)
        *err = localError ;
#ifdef GENERICMECHANICAL_DEBUG2
      printf("GenericMechanical_driver, localerror of FC3D: %e\n", localError);
#endif
      break;
    }
    default:
      printf("Numerics : genericMechanicalProblem_GS unknown problem type %d.\n", curProblem->type);
    }
    /*next*/
    curProblem = curProblem->nextProblem;
    currentRowNumber++;
  }
#ifdef GENERICMECHANICAL_DEBUG
  if (*err > tol)
    printf("GenericMechanical_driver compute_error END:, err>tol: error : %e\n", *err);
  else
    printf("GenericMechanical_driver compute_error END:, err<tol: error : %e\n", *err);
#endif
  if (*err > tol)
    return 1;
  else
    return 0;
}

void genericMechanicalProblem_GS(GenericMechanicalProblem* pGMP, double * reaction, double * velocity, int * info, SolverOptions* options)
{

  listNumericsProblem * curProblem = 0;
  SparseBlockStructuredMatrix* m = pGMP->M->matrix1;
  int iterMax = options->iparam[0];
  int it = 0;
  int currentRowNumber = 0;
  int diagBlockNumber = 0;
  double tol = options->dparam[0];
  double err = 0;
  int tolViolate = 1;
  double * sol = 0;
  double * w = 0;
  int resLocalSolver = 0;
  //printf("genericMechanicalProblem_GS \n");
  //displayGMP(pGMP);
  while (it < iterMax && tolViolate)
  {
    currentRowNumber = 0;
    curProblem =  pGMP->firstListElem;
    int  posInX = 0;
    int curSize = 0;
#ifdef GENERICMECHANICAL_DEBUG
    printf("GS it %d, initial value:\n", it);
    for (int ii = 0; ii < pGMP->size; ii++)
      printf("R[%d]=%e | V[]=%e \n", ii, reaction[ii], velocity[ii]);
#endif
    while (curProblem)
    {
      if (currentRowNumber)
      {
        posInX = m->blocksize0[currentRowNumber - 1];
      }
      curSize = m->blocksize0[currentRowNumber] - posInX;

      /*about the diagonal block:*/
      diagBlockNumber = getDiagonalBlockPos(m, currentRowNumber);
      sol = reaction + posInX;
      w = velocity + posInX;

      switch (curProblem->type)
      {
      case SICONOS_NUMERICS_PROBLEM_EQUALITY:
      {
        /*Mz*/
        LinearSystemProblem* linearProblem = (LinearSystemProblem*) curProblem->problem;
        linearProblem->M->matrix0 = m->block[diagBlockNumber];
        /*about q.*/
        memcpy(linearProblem->q, &(pGMP->q[posInX]), curSize * sizeof(double));
        /* for(ii=0;ii< curSize; ii++){ */
        /*   linearProblem->q[ii] = pGMP->q[posInX+ii]; */
        /* } */
        rowProdNoDiagSBM(pGMP->size, curSize, currentRowNumber, m, reaction, linearProblem->q, 0);

        resLocalSolver = LinearSystem_driver(linearProblem, sol, w, 0);

        break;
      }
      case SICONOS_NUMERICS_PROBLEM_LCP:
      {
        /*Mz*/
        LinearComplementarityProblem* lcpProblem = (LinearComplementarityProblem*) curProblem->problem;
        lcpProblem->M->matrix0 = m->block[diagBlockNumber];
        /*about q.*/
        memcpy(curProblem->q, &(pGMP->q[posInX]), curSize * sizeof(double));
        /* for(ii=0;ii< curSize; ii++){ */
        /*   lcpProblem->q[ii] = pGMP->q[posInX+ii]; */
        /* } */
        rowProdNoDiagSBM(pGMP->size, curSize, currentRowNumber, m, reaction, curProblem->q, 0);
        resLocalSolver = linearComplementarity_driver(lcpProblem, sol, w, options->internalSolvers, 0);

        break;
      }
      case SICONOS_NUMERICS_PROBLEM_FC3D:
      {
        FrictionContactProblem * fcProblem = (FrictionContactProblem *)curProblem->problem;
        fcProblem->M->matrix0 = m->block[diagBlockNumber];
        memcpy(curProblem->q, &(pGMP->q[posInX]), curSize * sizeof(double));
        rowProdNoDiagSBM(pGMP->size, curSize, currentRowNumber, m, reaction, curProblem->q, 0);
        resLocalSolver = frictionContact3D_unitary_enumerative_solve(fcProblem, sol, &options->internalSolvers[1]);
        break;
      }
      default:
        printf("Numerics : genericMechanicalProblem_GS unknown problem type %d.\n", curProblem->type);
      }
#ifdef GENERICMECHANICAL_DEBUG
      printf("GS it %d, the line number is %d:\n", it, currentRowNumber);
      for (int ii = 0; ii < pGMP->size; ii++)
        printf("R[%d]=%e | V[]=%e \n", ii, reaction[ii], velocity[ii]);
      if (resLocalSolver)
        printf("Numerics:GenericMechanical_drivers Local solver failed\n");
#endif
      curProblem = curProblem->nextProblem;
      currentRowNumber++;
    }
    /*compute global error.*/
    tolViolate = GenericMechanical_compute_error(pGMP, reaction, velocity, tol, options, &err);

    /*next GS it*/
    it++;
  }
  if (tolViolate)
    printf("---GenericalMechanical_drivers, FAILED***************************************\n");

  *info = tolViolate;
}



int genericMechanical_driver(GenericMechanicalProblem* problem, double *reaction , double *velocity, SolverOptions* options)
{
  // if (options == NULL )
  //  numericsError("FrictionContact3D_driver", "null input for solver options");

  /* If the options for solver have not been set, read default values in .opt file */

  int info = 0;
  //  display(problem->M);
  //displayGMP(problem);
  genericMechanicalProblem_GS(problem, reaction, velocity, &info, options);

  return info;

}

void genericMechnicalProblem_setDefaultSolverOptions(SolverOptions* options, int id)
{
  options->iSize = 5;
  options->dSize = 5;
  options->numberOfInternalSolvers = 2;
  options->dWork = 0;
  options->iWork = 0;
  options->iparam = (int *)malloc(options->iSize * sizeof(int));
  options->dparam = (double *)malloc(options->dSize * sizeof(double));
  options->iparam[0] = 10000;
  options->dparam[0] = 1e-4;

  options->internalSolvers = (SolverOptions *)malloc(2 * sizeof(SolverOptions));;
  linearComplementarity_setDefaultSolverOptions(0, options->internalSolvers, SICONOS_LCP_LEMKE);
  frictionContact3D_unitary_enumerative_setDefaultSolverOptions(&options->internalSolvers[1]);
  //frictionContact3D_AlartCurnierNewton_setDefaultSolverOptions(&options->internalSolvers[1]);
}

