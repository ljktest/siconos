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

//#define GENERICMECHANICAL_DEBUG

int GenericMechanical_compute_error(GenericMechanicalProblem* problem, double *reaction , double *velocity, double tol, SolverOptions* options, double * err)
{
  listNumericsProblem * curProblem = problem->firstListElem;
  SparseBlockStructuredMatrix* m = problem->M->matrix1;
  int currentRowNumber = 0;
  int ii;
  int  posInX = 0;
  int curSize = 0;
  *err = 0.0;
  while (curProblem)
  {
    if (currentRowNumber)
      posInX += m->blocksize0[currentRowNumber - 1];

    switch (curProblem->type)
    {
    case SICONOS_NUMERICS_PROBLEM_EQUALITY:
    {
      LinearSystemProblem* linearProblem = (LinearSystemProblem*) curProblem->problem;
      curSize = linearProblem->size;
      double * w = velocity + posInX;
      for (ii = 0; ii < curSize; ii++)
      {
        if (fabs(w[ii]) > *err)
          *err = fabs(w[ii]);
      }
      break;
    }
    case SICONOS_NUMERICS_PROBLEM_LCP:
      break;
    case SICONOS_NUMERICS_PROBLEM_FC3D:
    default:
      printf("Numerics : genericMechanicalProblem_GS unknown problem type %d.\n", curProblem->type);
    }
    curProblem = curProblem->nextProblem;
    currentRowNumber++;
  }
#ifdef GENERICMECHANICAL_DEBUG
  printf("GenericMechanical_driver, error : %e\n", *err);
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
  int globalSize = pGMP->size;
  double tol = options->dparam[0];
  double err = 0;
  int tolViolate = 1;
  double * sol = 0;
  double * w = 0;
  while (it < iterMax && tolViolate)
  {
    currentRowNumber = 0;
    curProblem =  pGMP->firstListElem;
    int  posInX = 0;
    int ii = 0;
    int curSize = 0;
#ifdef GENERICMECHANICAL_DEBUG
    printf("GS it %d, initial value:\n", it);
    for (ii = 0; ii < globalSize; ii++)
      printf("R[%d]=%e | V[]=%e \n", ii, reaction[ii], velocity[ii]);
#endif
    while (curProblem)
    {
      if (currentRowNumber)
      {
        posInX = m->blocksize0[currentRowNumber - 1];
      }
      /*about the diagonal block:*/
      diagBlockNumber = getDiagonalBlockPos(m, currentRowNumber);
      curSize = m->blocksize0[currentRowNumber] - posInX;
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
        for (ii = 0; ii < curSize; ii++)
        {
          linearProblem->q[ii] = pGMP->q[posInX + ii];
        }
        rowProdNoDiagSBM(pGMP->size, curSize, currentRowNumber, m, reaction, linearProblem->q, 0);

        LinearSystem_driver(linearProblem, sol, w, 0);

        break;
      }
      case SICONOS_NUMERICS_PROBLEM_LCP:
      {
        /*Mz*/
        LinearComplementarityProblem* lcpProblem = (LinearComplementarityProblem*) curProblem->problem;
        lcpProblem->M->matrix0 = m->block[diagBlockNumber];
        /*about q.*/
        for (ii = 0; ii < curSize; ii++)
        {
          lcpProblem->q[ii] = pGMP->q[posInX + ii];
        }
        rowProdNoDiagSBM(pGMP->size, curSize, currentRowNumber, m, reaction, lcpProblem->q, 0);
        linearComplementarity_driver(lcpProblem, sol, w, options->internalSolvers, 0);

        break;
      }
      case SICONOS_NUMERICS_PROBLEM_FC3D:
      default:
        printf("Numerics : genericMechanicalProblem_GS unknown problem type %d.\n", curProblem->type);
      }
#ifdef GENERICMECHANICAL_DEBUG
      printf("GS it %d, the line number is %d:\n", it, currentRowNumber);
      for (ii = 0; ii < globalSize; ii++)
        printf("R[%d]=%e | V[]=%e \n", ii, reaction[ii], velocity[ii]);
#endif
      curProblem = curProblem->nextProblem;
      currentRowNumber++;
    }
    /*computation of V=MR+q*/
    memcpy(velocity, pGMP->q, globalSize * sizeof(double));
    prodSBM(globalSize, globalSize, 1.0, m, reaction, 1.0, velocity);
    tolViolate = GenericMechanical_compute_error(pGMP, reaction, velocity, tol, options, &err);
    it++;
  }

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

void genericMechnicalProblem_setDefaultSolverOptions(GenericMechanicalProblem * pGMP, SolverOptions* options, int id)
{
  options->iSize = 5;
  options->dSize = 5;
  options->numberOfInternalSolvers = 2;
  options->dWork = 0;
  options->iWork = 0;
  options->iparam = (int *)malloc(options->iSize * sizeof(int));
  options->dparam = (double *)malloc(options->dSize * sizeof(double));
  options->iparam[0] = 500;
  options->dparam[0] = 1e-6;

  options->internalSolvers = (SolverOptions *)malloc(2 * sizeof(SolverOptions));;
  linearComplementarity_setDefaultSolverOptions(0, options->internalSolvers, SICONOS_LCP_LEMKE);
  linearComplementarity_setDefaultSolverOptions(0, &options->internalSolvers[1], SICONOS_LCP_LEMKE);
}

