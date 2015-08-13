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


#include "op3x3.h"
#include "SparseBlockMatrix.h"
#include "FrictionContact3D_Solvers.h"
#include "FrictionContactProblem.h"
#include "FrictionContact3D_compute_error.h"
#include "FischerBurmeisterGenerated.h"
#include "FrictionContactNonsmoothEqn.h"
#include "FrictionContact3D_localFischerBurmeister.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "Friction_cst.h"
#include "SiconosLapack.h"

void frictionContact3D_FischerBurmeisterFunction(
  unsigned int problemSize,
  FischerBurmeisterFun3x3Ptr computeACFun3x3,
  double *reaction,
  double *velocity,
  double *mu,
  double *rho,
  double *result,
  double *A,
  double *B)
{
  assert(reaction);
  assert(velocity);
  assert(rho);
  assert(mu);

  assert(problemSize / 3 > 0);
  assert(problemSize % 3 == 0);

  unsigned int i;
  for (i = 0; i < problemSize; i += 3)
  {

    computeACFun3x3(reaction, velocity, *mu, rho, result, A, B);

    reaction += 3;
    velocity += 3;
    mu++;
    rho += 3;

    if (result)
      result += 3;

    if (A)
      A += 9;

    if (B)
      B += 9;

  }

}


int frictionContact3D_FischerBurmeister_compute_error(
    FrictionContactProblem* problem,
    double *z , double *w, double tolerance,
    SolverOptions * options, double * error)
{

  double *A = NULL;
  double *B = NULL;

  unsigned int problemSize = 3 * problem->numberOfContacts;

  double *rho = (double*) malloc(problemSize*sizeof(double));
  double *F = (double *) malloc(problemSize*sizeof(double));

  FischerBurmeisterFun3x3Ptr computeACFun3x3;

  switch (options->iparam[10])
  {
  case 0:
  {

    computeACFun3x3 = &frictionContact3D_FischerBurmeisterFunctionGenerated;
    break;
  }
  }

  frictionContact3D_FischerBurmeisterFunction(
    problemSize,
    computeACFun3x3,
    z, w,
    problem->mu, rho,
    F, A, B);

  *error=0.;
  for(unsigned int i=0; i<problemSize;
      i+=3)
  {
    *error += sqrt(F[i]*F[i] + F[i+1]*F[i+1] + F[i+2]*F[i+2]);
  }

  *error /= (problem->numberOfContacts + 1);

  free(F);
  free(rho);

  if (*error > tolerance)
  {
    if (verbose > 1)
      printf(" Numerics - FrictionContact3D_compute_error: error = %g > tolerance = %g.\n",
             *error, tolerance);
    return 1;
  }
  else
  {
    return 0;
  }
}

int frictionContact3D_FischerBurmeister_setDefaultSolverOptions(
  SolverOptions* options)
{
  if (verbose > 0)
  {
    printf("Set the default solver options for the LOCALFB Solver\n");
  }

  options->solverId = SICONOS_FRICTION_3D_LOCALFB;
  options->numberOfInternalSolvers = 0;
  options->isSet = 1;
  options->filterOn = 1;
  options->iSize = 14;
  options->dSize = 14;
  options->iparam = (int *) malloc(options->iSize * sizeof(int));
  options->dparam = (double *) malloc(options->dSize * sizeof(double));
  options->dWork = NULL;
  options->iWork = NULL;   options->callback = NULL; options->numericsOptions = NULL;
  for (unsigned int i = 0; i < 14; i++)
  {
    options->iparam[i] = 0;
    options->dparam[i] = 0.0;
  }
  options->iparam[0] = 200;
  options->iparam[1] = 1;
  options->iparam[3] = 100000; /* nzmax*/
  options->iparam[5] = 1;
  options->iparam[7] = 1;      /* erritermax */
  options->dparam[0] = 1e-3;
  options->dparam[3] = 1;      /* default rho */

  options->iparam[8] = -1;     /* mpi com fortran */
  options->iparam[10] = 0;
  options->iparam[11] = 0;     /* 0 GoldsteinPrice line search, 1 FBLSA */
  options->iparam[12] = 100;   /* max iter line search */

  options->iparam[13] = 0;     /* Linear solver used at each Newton iteration. 0: cs_lusol, 1 mumps */

  options->internalSolvers = NULL;

  return 0;
}


typedef struct
{
  FischerBurmeisterFun3x3Ptr computeACFun3x3;
} FischerBurmeisterParams;

void nonsmoothEqnFischerBurmeisterFun(void* arg,
                                      unsigned int problemSize,
                                      double* reaction,
                                      double* velocity,
                                      double* mu,
                                      double* rho,
                                      double* result,
                                      double* A,
                                      double* B);
void nonsmoothEqnFischerBurmeisterFun(void* arg,
                                      unsigned int problemSize,
                                      double* reaction,
                                      double* velocity,
                                      double* mu,
                                      double* rho,
                                      double* result,
                                      double* A,
                                      double* B)
{
  FischerBurmeisterParams* acparams_p = (FischerBurmeisterParams *) arg;

  frictionContact3D_FischerBurmeisterFunction(problemSize,
                                         acparams_p->computeACFun3x3,
                                         reaction,
                                         velocity,
                                         mu,
                                         rho,
                                         result,
                                         A,
                                         B);
}




void frictionContact3D_localFischerBurmeister(
  FrictionContactProblem* problem,
  double *reaction,
  double *velocity,
  int *info,
  SolverOptions *options)
{
  assert(problem);
  assert(reaction);
  assert(velocity);
  assert(info);
  assert(options);

  assert(problem->dimension == 3);

  assert(options->iparam);
  assert(options->dparam);

  assert(problem->q);
  assert(problem->mu);
  assert(problem->M);

  assert(problem->M->matrix0 || problem->M->matrix1);

  assert(!options->iparam[4]); // only host

  FischerBurmeisterParams acparams;

  switch (options->iparam[10])
  {
  case 0:
  {
    acparams.computeACFun3x3 = &frictionContact3D_FischerBurmeisterFunctionGenerated;
    break;
  }
  }

  FrictionContactNonsmoothEqn equation;

  equation.problem = problem;
  equation.data = (void *) &acparams;
  equation.function = &nonsmoothEqnFischerBurmeisterFun;

  frictionContactNonsmoothEqnSolve(&equation, reaction, velocity, info,
                                   options);

}

