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
#include "FrictionContact3D_local_nonsmooth_Newton_solvers.h"
#include "FrictionContact3D_Path.h"
#include "FrictionContact3D_NCPGlockerFixedPoint.h"
#include "FrictionContact3D_projection.h"
#include "FrictionContact3D_unitary_enumerative.h"
#include "FrictionContact3D_compute_error.h"
#include "NCP_Solvers.h"
#include "SiconosBlas.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#pragma GCC diagnostic ignored "-Wmissing-prototypes"

void fake_compute_error_nsgs(FrictionContactProblem* problem, double *reaction, double *velocity, double tolerance, SolverOptions  *options,  double* error)
{
  int n = 3 * problem->numberOfContacts;
  *error = 0.;
  int i, m;
  m = 5 * n / 3;
  double err = INFINITY;
  for (i = 0 ; i < m ; ++i)
  {
    *error += Compute_NCP_error1(i, err);
  }
}
void frictionContact3D_nsgs_update(int contact, FrictionContactProblem* problem, FrictionContactProblem* localproblem, double * reaction, SolverOptions* options)
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
void initializeLocalSolver_nsgs(SolverPtr* solve, UpdatePtr* update, FreeSolverNSGSPtr* freeSolver, ComputeErrorPtr* computeError,
                                FrictionContactProblem* problem, FrictionContactProblem* localproblem,
                                SolverOptions * options, SolverOptions * localsolver_options)
{

  /** Connect to local solver */
  switch (localsolver_options->solverId)
  {
    /* Projection */
  case SICONOS_FRICTION_3D_ProjectionOnConeWithDiagonalization:
  {
    *solve = &frictionContact3D_projectionWithDiagonalization_solve;
    *update = &frictionContact3D_projectionWithDiagonalization_update;
    *freeSolver = (FreeSolverNSGSPtr)&frictionContact3D_projection_free;
    *computeError = (ComputeErrorPtr)&FrictionContact3D_compute_error;
    frictionContact3D_projection_initialize(problem, localproblem);
    break;
  }
  case SICONOS_FRICTION_3D_ProjectionOnCone:
  {
    *solve = &frictionContact3D_projectionOnCone_solve;
    *update = &frictionContact3D_projection_update;
    *freeSolver = (FreeSolverNSGSPtr)&frictionContact3D_projection_free;
    *computeError = (ComputeErrorPtr)&FrictionContact3D_compute_error;
    frictionContact3D_projection_initialize(problem, localproblem);
    break;
  }
  case SICONOS_FRICTION_3D_ProjectionOnConeWithLocalIteration:
  {
    *solve = &frictionContact3D_projectionOnConeWithLocalIteration_solve;
    *update = &frictionContact3D_projection_update;
    *freeSolver = (FreeSolverNSGSPtr)&frictionContact3D_projectionOnConeWithLocalIteration_free;
    *computeError = (ComputeErrorPtr)&FrictionContact3D_compute_error;
    frictionContact3D_projectionOnConeWithLocalIteration_initialize(problem, localproblem,localsolver_options );
    break;
  }
  case SICONOS_FRICTION_3D_projectionOnConeWithRegularization:
  {
    *solve = &frictionContact3D_projectionOnCone_solve;
    *update = &frictionContact3D_projection_update_with_regularization;
    *freeSolver = (FreeSolverNSGSPtr)&frictionContact3D_projection_with_regularization_free;
    *computeError = (ComputeErrorPtr)&FrictionContact3D_compute_error;
    frictionContact3D_projection_initialize_with_regularization(problem, localproblem);
    break;
  }
  /* Newton solver (Alart-Curnier) */
  case SICONOS_FRICTION_3D_AlartCurnierNewton:
  {
    *solve = &frictionContact3D_local_nonsmooth_Newton_solvers_solve;
    *update = &frictionContact3D_AC_update;
    *freeSolver = (FreeSolverNSGSPtr)&frictionContact3D_local_nonsmooth_Newton_solvers_free;
    *computeError = (ComputeErrorPtr)&FrictionContact3D_compute_error;
    frictionContact3D_local_nonsmooth_Newton_solvers_initialize(problem, localproblem, localsolver_options);
    break;
  }
  case SICONOS_FRICTION_3D_DampedAlartCurnierNewton:
  {
    *solve = &frictionContact3D_local_nonsmooth_Newton_solvers_solve;
    *update = &frictionContact3D_AC_update;
    *freeSolver = (FreeSolverNSGSPtr)&frictionContact3D_local_nonsmooth_Newton_solvers_free;
    *computeError = (ComputeErrorPtr)&FrictionContact3D_compute_error;
    frictionContact3D_local_nonsmooth_Newton_solvers_initialize(problem, localproblem, localsolver_options);
    break;
  }
  /* Newton solver (Glocker-Fischer-Burmeister)*/
  case SICONOS_FRICTION_3D_NCPGlockerFBNewton:
  {
    *solve = &frictionContact3D_local_nonsmooth_Newton_solvers_solve;
    *update = &NCPGlocker_update;
    *freeSolver = (FreeSolverNSGSPtr)&frictionContact3D_local_nonsmooth_Newton_solvers_free;
    *computeError = (ComputeErrorPtr)&FrictionContact3D_compute_error;
    // *computeError = &fake_compute_error;
    frictionContact3D_local_nonsmooth_Newton_solvers_initialize(problem, localproblem, localsolver_options);
    break;
  }
  /* Path solver (Glocker Formulation) */
  case SICONOS_FRICTION_3D_NCPGlockerFBPATH:
  {
    *solve = &frictionContact3D_Path_solve;
    *freeSolver = (FreeSolverNSGSPtr)&frictionContact3D_Path_free;
    *update = &NCPGlocker_update;
    *computeError = (ComputeErrorPtr)&FrictionContact3D_compute_error;
    // *computeError = &fake_compute_error;
    frictionContact3D_Path_initialize(problem, localproblem, localsolver_options);
    break;
  }

  /* Fixed Point solver (Glocker Formulation) */
  case SICONOS_FRICTION_3D_NCPGlockerFBFixedPoint:
  {
    *solve = &frictionContact3D_FixedP_solve;
    *update = &NCPGlocker_update;
    *freeSolver = (FreeSolverNSGSPtr)&frictionContact3D_FixedP_free;
    *computeError = &fake_compute_error_nsgs;
    frictionContact3D_FixedP_initialize(problem, localproblem, localsolver_options);
    break;
  }
  /*iparam[4] > 10 are reserved for Tresca resolution */
  case SICONOS_FRICTION_3D_projectionOnCylinder:
  {
    *solve = &frictionContact3D_projectionOnCylinder_solve;
    *update = &frictionContact3D_projectionOnCylinder_update;
    *freeSolver = (FreeSolverNSGSPtr)&frictionContact3D_projectionOnCylinder_free;
    *computeError = (ComputeErrorPtr)&FrictionContact3D_Tresca_compute_error;
    frictionContact3D_projectionOnCylinder_initialize(problem, localproblem, options );
    break;
  }
    /*iparam[4] > 10 are reserved for Tresca resolution */
  case SICONOS_FRICTION_3D_projectionOnCylinderWithLocalIteration:
  {
    *solve = &frictionContact3D_projectionOnCylinderWithLocalIteration_solve;
    *update = &frictionContact3D_projectionOnCylinder_update;
    *freeSolver = (FreeSolverNSGSPtr)&frictionContact3D_projectionOnCylinderWithLocalIteration_free;
    *computeError = (ComputeErrorPtr)&FrictionContact3D_Tresca_compute_error;
    frictionContact3D_projectionOnCylinderWithLocalIteration_initialize(problem, localproblem, options, localsolver_options );
    break;
  }
  case SICONOS_FRICTION_3D_QUARTIC:
  {
    *solve = &frictionContact3D_unitary_enumerative_solve;
    *update = &frictionContact3D_nsgs_update;
    *freeSolver = (FreeSolverNSGSPtr)&frictionContact3D_unitary_enumerative_free;
    *computeError = (ComputeErrorPtr)&FrictionContact3D_compute_error;
    frictionContact3D_unitary_enumerative_initialize(localproblem);
    break;
  }
  case SICONOS_FRICTION_3D_QUARTIC_NU:
  {
    *solve = &frictionContact3D_unitary_enumerative_solve;
    *update = &frictionContact3D_nsgs_update;
    *freeSolver = (FreeSolverNSGSPtr)&frictionContact3D_unitary_enumerative_free;
    *computeError = (ComputeErrorPtr)&FrictionContact3D_compute_error;
    frictionContact3D_unitary_enumerative_initialize(localproblem);
    break;
  }
  default:
  {
    fprintf(stderr, "Numerics, FrictionContact3D_nsgs failed. Unknown internal solver : %s.\n", idToName(localsolver_options->solverId));
    exit(EXIT_FAILURE);
  }
  }
}
void frictionContact3D_nsgs_computeqLocal(FrictionContactProblem * problem, FrictionContactProblem * localproblem, double *reaction, int contact)
{

  double *qLocal = localproblem->q;
  int n = 3 * problem->numberOfContacts;


  int in = 3 * contact, it = in + 1, is = it + 1;
  /* reaction current block set to zero, to exclude current contact block */
  double rin = reaction[in] ;
  double rit = reaction[it] ;
  double ris = reaction[is] ;
  reaction[in] = 0.0;
  reaction[it] = 0.0;
  reaction[is] = 0.0;
  /* qLocal computation*/



  qLocal[0] = problem->q[in];
  qLocal[1] =  problem->q[it];
  qLocal[2] =  problem->q[is];

  if (problem->M->storageType == 0)
  {
    double * MM = problem->M->matrix0;
    int incx = n, incy = 1;
    qLocal[0] += cblas_ddot(n , &MM[in] , incx , reaction , incy);
    qLocal[1] += cblas_ddot(n , &MM[it] , incx , reaction , incy);
    qLocal[2] += cblas_ddot(n , &MM[is] , incx , reaction , incy);
  }
  else if (problem->M->storageType == 1)
  {
    /* qLocal += rowMB * reaction
    with rowMB the row of blocks of MGlobal which corresponds to the current contact
    */
    rowProdNoDiagSBM(n, 3, contact, problem->M->matrix1, reaction, qLocal, 0);
  }
  reaction[in] = rin;
  reaction[it] = rit;
  reaction[is] = ris;


}

void frictionContact3D_nsgs_fillMLocal(FrictionContactProblem * problem, FrictionContactProblem * localproblem, int contact)
{

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
    localproblem->M->matrix0 = MGlobal->matrix1->block[diagPos];

  }
  else
    numericsError("FrictionContact3D_projection -", "unknown storage type for matrix M");
}


/* swap two indices */
void uint_swap (unsigned int *a, unsigned int *b)
{
    unsigned int temp = *a;
    *a = *b;
    *b = temp;
}

/* shuffle an unsigned array */
void uint_shuffle (unsigned int *a, unsigned int n) {

  for (unsigned int i = 0; i < n - 1; i++)
  {
    uint_swap  (&a[i], &a[i + rand()%(n - i)]);
  }
}



void frictionContact3D_nsgs(FrictionContactProblem* problem, double *reaction, double *velocity, int* info, SolverOptions* options)
{
  /* int and double parameters */
  int* iparam = options->iparam;
  double* dparam = options->dparam;
  /* Number of contacts */
  unsigned int nc = problem->numberOfContacts;
  /* Maximum number of iterations */
  int itermax = iparam[0];
  /* Tolerance */
  double tolerance = dparam[0];

  if (*info == 0)
    return;

  if (options->numberOfInternalSolvers < 1)
  {
    numericsError("frictionContact3D_nsgs", "The NSGS method needs options for the internal solvers, options[0].numberOfInternalSolvers should be >1");
  }
  assert(&options[1]);

  SolverOptions * localsolver_options = options->internalSolvers;


  SolverPtr local_solver = NULL;
  UpdatePtr update_localproblem = NULL;
  FreeSolverNSGSPtr freeSolver = NULL;
  ComputeErrorPtr computeError = NULL;

  /* Connect local solver and local problem*/
  FrictionContactProblem* localproblem = (FrictionContactProblem*)malloc(sizeof(FrictionContactProblem));
  localproblem->numberOfContacts = 1;
  localproblem->dimension = 3;
  localproblem->q = (double*)malloc(3 * sizeof(double));
  localproblem->mu = (double*)malloc(sizeof(double));


  if (problem->M->storageType == NM_DENSE)
  {
    localproblem->M = createNumericsMatrixFromData(NM_DENSE, 3, 3,
                                           malloc(9 * sizeof(double)));
  }
  else
  {
    localproblem->M = createNumericsMatrixFromData(NM_DENSE, 3, 3, NULL);
  }


  initializeLocalSolver_nsgs(&local_solver, &update_localproblem,
                             (FreeSolverNSGSPtr *)&freeSolver, &computeError,
                             problem , localproblem,
                             options, localsolver_options);

  /*****  NSGS Iterations *****/
  int iter = 0; /* Current iteration number */
  double error = 1.; /* Current error */
  int hasNotConverged = 1;
  unsigned int contact; /* Number of the current row of blocks in M */

  unsigned int *scontacts = NULL;
  if (iparam[5]) /* shuffle */
  {
    if (iparam[6] >0)
    {
      srand((unsigned int)iparam[6]);
    }
    else
      srand(1);
    scontacts = (unsigned int *) malloc(nc * sizeof(unsigned int));
    for (unsigned int i = 0; i<nc ; ++i)
    {
      scontacts[i] = i;
    }
    uint_shuffle(scontacts, nc);
    /* printf("scontacts :\t"); */
    /* for (unsigned int i = 0; i<nc ; ++i) printf("%i\t", scontacts[i]); */
    /* printf("\n"); */
  }

  /*  dparam[0]= dparam[2]; // set the tolerance for the local solver */
  if (iparam[1] == 1 || iparam[1] == 2)
  {
    double reactionold[3];
    while ((iter < itermax) && (hasNotConverged > 0))
    {

      ++iter;
      /* Loop through the contact points */
      //cblas_dcopy( n , q , incx , velocity , incy );
      error = 0.0;

      for (unsigned int i= 0 ; i < nc ; ++i)
      {
        if (iparam[5])
        {
          contact = scontacts[i];
        }
        else
        {
          contact = i;
        }


        reactionold[0] = reaction[3 * contact];
        reactionold[1] = reaction[3 * contact + 1];
        reactionold[2] = reaction[3 * contact + 2];
        if (verbose > 1) printf("----------------------------------- Contact Number %i\n", contact);
        (*update_localproblem)(contact, problem, localproblem, reaction, localsolver_options);


        localsolver_options->iparam[4] = contact;
        (*local_solver)(localproblem, &(reaction[3 * contact]) , localsolver_options);

        error += pow(reaction[3 * contact] - reactionold[0], 2) +
                 pow(reaction[3 * contact + 1] - reactionold[1], 2) +
                 pow(reaction[3 * contact + 2] - reactionold[2], 2);

      }


      /* **** Criterium convergence **** */
      error = sqrt(error);
      if (verbose > 0)
        printf("----------------------------------- FC3D - NSGS - Iteration %i Error = %14.7e <= %7.3e\n", iter, error, options->dparam[0]);
      if (error < tolerance) hasNotConverged = 0;
      *info = hasNotConverged;
    }

    if (iparam[1] == 1) /* Full criterium */
    {
      double absolute_error;
      (*computeError)(problem, reaction , velocity, tolerance, options, &absolute_error);
      if (verbose > 0)
      {
        if (absolute_error > error)
        {
          printf("----------------------------------- FC3D - NSGS - Warning absolute Error = %14.7e is larger than incremental error = %14.7e\n", absolute_error, error);
        }
      }
    }
  }
  else
  {
    if (iparam[5] == 1) /* shuffle */
    {
      int withRelaxation=iparam[4];
      if (withRelaxation)
      {
        double reactionold[3];

        double omega = dparam[8];
        while ((iter < itermax) && (hasNotConverged > 0))
        {
          ++iter;
          /* Loop through the contact points */
          //cblas_dcopy( n , q , incx , velocity , incy );
          for (unsigned int i= 0 ; i < nc ; ++i)
          {

            contact = scontacts[i];

            reactionold[0] = reaction[3 * contact];
            reactionold[1] = reaction[3 * contact + 1];
            reactionold[2] = reaction[3 * contact + 2];

            if (verbose > 1) printf("----------------------------------- Contact Number %i\n", contact);
            (*update_localproblem)(contact, problem, localproblem, reaction, localsolver_options);
            localsolver_options->iparam[4] = contact;
            (*local_solver)(localproblem, &(reaction[3 * contact]), localsolver_options);

            reaction[3 * contact] = omega*reaction[3 * contact]+(1.0-omega)*reactionold[0];
            reaction[3 * contact+1] = omega*reaction[3 * contact+1]+(1.0-omega)*reactionold[1];
            reaction[3 * contact+2] = omega*reaction[3 * contact+2]+(1.0-omega)*reactionold[2];
          }

          /* **** Criterium convergence **** */
          (*computeError)(problem, reaction , velocity, tolerance, options, &error);

          if (verbose > 0)
            printf("----------------------------------- FC3D - NSGS - Iteration %i Error = %14.7e <= %7.3e\n", iter, error, options->dparam[0]);

          if (error < tolerance) hasNotConverged = 0;
          *info = hasNotConverged;

          if (options->callback)
          {
            options->callback->collectStatsIteration(options->callback->env, 3 * nc,
                                                     reaction, velocity,
                                                     error, NULL);
          }
        } /* end while loop */
      }
      else /* without relaxation but shuffle */
      {
        while ((iter < itermax) && (hasNotConverged > 0))
        {
          ++iter;
          /* Loop through the contact points */
          //cblas_dcopy( n , q , incx , velocity , incy );

          for (unsigned int i= 0 ; i < nc ; ++i)
          {
            contact = scontacts[i];

            if (verbose > 1) printf("----------------------------------- Contact Number %i\n", contact);
            (*update_localproblem)(contact, problem, localproblem, reaction, localsolver_options);
            localsolver_options->iparam[4] = contact;
            (*local_solver)(localproblem, &(reaction[3 * contact]), localsolver_options);

          }

          /* **** Criterium convergence **** */
          (*computeError)(problem, reaction , velocity, tolerance, options, &error);

          if (verbose > 0)
            printf("----------------------------------- FC3D - NSGS - Iteration %i Error = %14.7e <= %7.3e\n", iter, error, options->dparam[0]);
          if (error < tolerance) hasNotConverged = 0;
          *info = hasNotConverged;

          if (options->callback)
          {
            options->callback->collectStatsIteration(options->callback->env, 3 * nc,
                                                     reaction, velocity,
                                                     error, NULL);
          }
        }
      }
    }
    else if (iparam[5] == 2) /* shuffle in each loop */
    {
      int withRelaxation=iparam[4];
      if (withRelaxation)
      {
        double reactionold[3];

        double omega = dparam[8];
        while ((iter < itermax) && (hasNotConverged > 0))
        {
          ++iter;
          uint_shuffle(scontacts, nc);

          /* Loop through the contact points */
          //cblas_dcopy( n , q , incx , velocity , incy );

          for (unsigned int i= 0 ; i < nc ; ++i)
          {

            contact = scontacts[i];

            reactionold[0] = reaction[3 * contact];
            reactionold[1] = reaction[3 * contact + 1];
            reactionold[2] = reaction[3 * contact + 2];

            if (verbose > 1) printf("----------------------------------- Contact Number %i\n", contact);
            (*update_localproblem)(contact, problem, localproblem, reaction, localsolver_options);

            localsolver_options->iparam[4] = contact; /* We write in dWork always with respect to the initial index i*/

            (*local_solver)(localproblem, &(reaction[3 * contact]), localsolver_options);

            reaction[3 * contact] = omega*reaction[3 * contact]+(1.0-omega)*reactionold[0];
            reaction[3 * contact+1] = omega*reaction[3 * contact+1]+(1.0-omega)*reactionold[1];
            reaction[3 * contact+2] = omega*reaction[3 * contact+2]+(1.0-omega)*reactionold[2];
          }

          /* **** Criterium convergence **** */
          (*computeError)(problem, reaction , velocity, tolerance, options, &error);

          if (verbose > 0)
            printf("----------------------------------- FC3D - NSGS - Iteration %i Error = %14.7e <= %7.3e\n", iter, error, options->dparam[0]);

          if (error < tolerance) hasNotConverged = 0;
          *info = hasNotConverged;

          if (options->callback)
          {
            options->callback->collectStatsIteration(options->callback->env, 3 * nc,
                                                     reaction, velocity,
                                                     error, NULL);
          }
        }

      }
      else
      {
        while ((iter < itermax) && (hasNotConverged > 0))
        {
          ++iter;
          uint_shuffle(scontacts, nc);
          /* Loop through the contact points */
          //cblas_dcopy( n , q , incx , velocity , incy );
          for (unsigned int i= 0 ; i < nc ; ++i)
          {
            contact = scontacts[i];
            if (verbose > 1) printf("----------------------------------- Contact Number %i\n", contact);
            (*update_localproblem)(contact, problem, localproblem, reaction, localsolver_options);
            localsolver_options->iparam[4] = contact; /* We write in dWork always with respect to the initial index i*/
            (*local_solver)(localproblem, &(reaction[3 * contact]), localsolver_options);
          }
          /* **** Criterium convergence **** */
          (*computeError)(problem, reaction , velocity, tolerance, options, &error);

          if (verbose > 0)
            printf("----------------------------------- FC3D - NSGS - Iteration %i Error = %14.7e <= %7.3e\n", iter, error, options->dparam[0]);

          if (error < tolerance) hasNotConverged = 0;
          *info = hasNotConverged;

          if (options->callback)
          {
            options->callback->collectStatsIteration(options->callback->env, 3 * nc,
                                                     reaction, velocity,
                                                     error, NULL);
          }
        }
      }
    }
    else
    {
      int withRelaxation=iparam[4];
      if(withRelaxation)
      {
        double reactionold[3];
        double omega = dparam[8];
        while ((iter < itermax) && (hasNotConverged > 0))
        {
          ++iter;
          /* Loop through the contact points */
          //cblas_dcopy( n , q , incx , velocity , incy );
          for (contact= 0 ; contact < nc ; ++contact)
          {
            if (withRelaxation)
            {
              reactionold[0] = reaction[3 * contact];
              reactionold[1] = reaction[3 * contact + 1];
              reactionold[2] = reaction[3 * contact + 2];
            }

            if (verbose > 1) printf("----------------------------------- Contact Number %i\n", contact);
            (*update_localproblem)(contact, problem, localproblem, reaction, localsolver_options);
            localsolver_options->iparam[4] = contact;
            (*local_solver)(localproblem, &(reaction[3 * contact]), localsolver_options);
            if(withRelaxation)
            {
              reaction[3 * contact] = omega*reaction[3 * contact]+(1.0-omega)*reactionold[0];
              reaction[3 * contact+1] = omega*reaction[3 * contact+1]+(1.0-omega)*reactionold[1];
              reaction[3 * contact+2] = omega*reaction[3 * contact+2]+(1.0-omega)*reactionold[2];
            }
          }

          /* **** Criterium convergence **** */
          (*computeError)(problem, reaction , velocity, tolerance, options, &error);

          if (verbose > 0)
            printf("----------------------------------- FC3D - NSGS - Iteration %i Error = %14.7e <= %7.3e\n", iter, error, options->dparam[0]);

          if (error < tolerance) hasNotConverged = 0;
          *info = hasNotConverged;

          if (options->callback)
          {
            options->callback->collectStatsIteration(options->callback->env, 3 * nc,
                                                     reaction, velocity,
                                                     error, NULL);
          }
        }

      }
      else
      {
        while ((iter < itermax) && (hasNotConverged > 0))
        {
          ++iter;
          /* Loop through the contact points */
          //cblas_dcopy( n , q , incx , velocity , incy );

          for (contact= 0 ; contact < nc ; ++contact)
          {


            if (verbose > 1) printf("----------------------------------- Contact Number %i\n", contact);
            (*update_localproblem)(contact, problem, localproblem, reaction, localsolver_options);
            localsolver_options->iparam[4] = contact;
            (*local_solver)(localproblem, &(reaction[3 * contact]), localsolver_options);

          }

          /* **** Criterium convergence **** */
          (*computeError)(problem, reaction , velocity, tolerance, options, &error);

          if (verbose > 0)
            printf("----------------------------------- FC3D - NSGS - Iteration %i Error = %14.7e <= %7.3e\n", iter, error, options->dparam[0]);
          if (error < tolerance) hasNotConverged = 0;
          *info = hasNotConverged;

          if (options->callback)
          {
            options->callback->collectStatsIteration(options->callback->env, 3 * nc,
                                                     reaction, velocity,
                                                     error, NULL);
          }
        }
      }
    }
  }
  dparam[0] = tolerance;
  dparam[1] = error;
  iparam[7] = iter;

  /***** Free memory *****/
  (*freeSolver)(problem,localproblem,localsolver_options);
  if (problem->M->storageType == 0 && localproblem->M->matrix0 != NULL)
  {
    free(localproblem->M->matrix0);
  }
  localproblem->M->matrix0 = NULL;
  freeFrictionContactProblem(localproblem);

  if (scontacts) /* shuffle */
  {
    free(scontacts);
  }

}

int frictionContact3D_nsgs_setDefaultSolverOptions(SolverOptions* options)
{
  int i;
  if (verbose > 0)
  {
    printf("Set the Default SolverOptions for the NSGS Solver\n");
  }

  /*  strcpy(options->solverName,"NSGS");*/
  options->solverId = SICONOS_FRICTION_3D_NSGS;
  options->numberOfInternalSolvers = 1;
  options->isSet = 1;
  options->filterOn = 1;
  options->iSize = 10;
  options->dSize = 10;
  options->iparam = (int *)malloc(options->iSize * sizeof(int));
  options->dparam = (double *)malloc(options->dSize * sizeof(double));
  options->dWork = NULL;
  null_SolverOptions(options);
  for (i = 0; i < 10; i++)
  {
    options->iparam[i] = 0;
    options->dparam[i] = 0.0;
  }
  options->iparam[0] = 1000;
  options->dparam[0] = 1e-4;
  options->internalSolvers = (SolverOptions *)malloc(sizeof(SolverOptions));
  frictionContact3D_AlartCurnierNewton_setDefaultSolverOptions(options->internalSolvers);

  return 0;
}
