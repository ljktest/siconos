/* Siconos-Numerics, Copyright INRIA 2005-2011.
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

#include "NCP_Path.h"
#include "NonSmoothNewton.h"
#include "FrictionContact3D_Solvers.h"
#include "LA.h"
#include <stdlib.h>
#include <stdio.h>
#include "Friction_cst.h"

/* Pointer to function used to update the solver, to formalize the local problem for example. */
typedef void (*UpdateSolverPtr)(int, double*);


static FuncEvalPtr F = NULL;
static JacEvalPtr jacobianF = NULL;
static UpdateSolverPtr updateSolver = NULL;
static PostSolverPtr postSolver = NULL;
static FreeSolverPtr freeSolver = NULL;
/* size of a block */
static int Fsize;

/** writes \f$ F(z) \f$ using Glocker formulation
 */
int F_GlockerPath(int sizeF, double* reaction, double* FVector)
{
  /* Glocker formulation */
  int up2Date = 0;
  double* FGlocker = NULL;
  computeFGlocker(&FGlocker, up2Date);
  /* Note that FGlocker is a static var. in FrictionContact3D2NCP_Glocker and thus there is no memory allocation in
     the present file.
  */

  /* TMP COPY: review memory management for FGlocker ...*/
  DCOPY(sizeF , FGlocker , 1, FVector , 1);
  FGlocker = NULL;
  return 1;
}

/** writes \f$ \nabla_z F(z) \f$  using Glocker formulation and the Fischer-Burmeister function.
 */
int jacobianF_GlockerPath(int sizeF, int nnz, double* reaction, int* col_start, int* col_len, int* row, double* jacobianFMatrix)
{
  int up2Date = 0;
  /* Glocker formulation */
  double* FGlocker = NULL, *jacobianFGlocker = NULL;
  computeFGlocker(&FGlocker, up2Date);
  computeJacobianFGlocker(&jacobianFGlocker, up2Date);
  /* Note that FGlocker and jacobianFGlocker are static var. in FrictionContact3D2NCP_Glocker and thus there is no memory allocation in
   the present file.
  */

  /* Write jacobianFGlocker in a Path-Sparse format */
  convertToPathSparse(sizeF, sizeF, jacobianFGlocker, col_start, col_len, row, jacobianFMatrix);

  FGlocker = NULL;
  jacobianFGlocker = NULL;
  return 1;
}


void frictionContact3D_Path_initialize(FrictionContactProblem* problem, FrictionContactProblem* localproblem, SolverOptions * localsolver_options)
{

  /*
     Initialize solver (Connect F and its jacobian, set local size ...) according to the chosen formulation.
  */

  /* Glocker formulation */
  if (localsolver_options->solverId == SICONOS_FRICTION_3D_NCPGlockerFBPATH)
  {
    Fsize = 5;
    NCPGlocker_initialize(problem, localproblem);
    F = &F_GlockerPath;
    jacobianF = &jacobianF_GlockerPath;
    /*    updateSolver = &NCPGlocker_update; */
    postSolver = &NCPGlocker_post;
    freeSolver = &NCPGlocker_free;
  }
  else
  {
    fprintf(stderr, "Numerics, FrictionContact3D_Path failed. Unknown formulation type.\n");
    exit(EXIT_FAILURE);
  }
}

int frictionContact3D_Path_solve(FrictionContactProblem * localproblem , double* reaction, SolverOptions * options)
{
  int * iparam = options->iparam;
  double * dparam = options->dparam;


  /*   (*updateSolver)(contact, reaction); */

  double * reactionBlock = reaction;
  int info = NCP_Path(Fsize, reactionBlock, F, jacobianF, iparam, dparam);
  if (info > 0)
  {
    fprintf(stderr, "Numerics, FrictionContact3D_Path failed");
    exit(EXIT_FAILURE);
  }
  return info;
  /*   (*postSolver)(contact,reaction); */
}

void frictionContact3D_Path_free()
{
  F = NULL;
  jacobianF = NULL;
  updateSolver = NULL;
  postSolver = NULL;
  (*freeSolver)();
}

void frictionContact3D_Path_computeError(int n, double* velocity, double*reaction, double * error)
{
  /*   int numberOfContacts = n/3; */
  /*   int sizeGlobal = numberOfContacts*FSize; */
  /*   //  double * FGlobal = (double*)malloc(sizeGlobal*sizeof(*FGlobal));  */
  /*   (*computeFGlobal)(reaction,velocity); */
  /*   int i; */
  /*   double Fz; */
  /*   *error = 0; */
  /*   for(i=0;i<sizeGlobal;++i) */
  /*     { */
  /*       Fz = velocity[i]*reaction[i]; */
  /*       if(Fz>0) */
  /*  *error+=Fz; */
  /*       if(reaction[i]<0) */
  /*  *error+=reaction[i]; */
  /*       if(velocity[i]<0) */
  /*  *error+=velocity[i]; */
  /*     } */

  /*   // (*computeVelocity)(FGlobal); */

  /*   free(FGlobal); */

}
