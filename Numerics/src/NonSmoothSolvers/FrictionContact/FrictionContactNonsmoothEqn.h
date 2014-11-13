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
#ifndef FRICTIONCONTACT3D_NONSMOOTHEQN_H
#define FRICTIONCONTACT3D_NONSMOOTHEQN_H

/*!\file FrictionContactNonsmoothEqn.h

  \brief Formulation of a FrictionContactProblem as a nonsmooth equation.
  [...]
 */

#include "FrictionContactProblem.h"
#include "SolverOptions.h"

/* Notes:
 - check Olivier Newton_method (only dense)
 - only sparse storage.
 - void parameter *data is unused for the moment.
 - problem_size, mu and rho should be moved outside parameters (inside *data)
*/


/** The nonsmooth function signature.
[...]
 */
typedef void (*FrictionContactNonsmoothEqnFunPtr)(
  void* data,
  unsigned int problem_size,
  double* reaction,
  double* velocity,
  double* mu,
  double* rho,
  double* F,
  double* A,
  double* B);

/** The nonsmooth equation structure.
[...]
 */
typedef struct
{
  FrictionContactProblem* problem;
  void* data;
  FrictionContactNonsmoothEqnFunPtr function;
} FrictionContactNonsmoothEqn;

/** The needed steps before solver usage.
   Note: at the moment this is for MPI initialization.
   \param options the SolverOptions.
 */
void frictionContactNonsmoothEqnInit(SolverOptions* options);

/** Solve the equation. The only implemented method is
    nonsmooth Newton method with a Goldstein Price line search.
    \param equation the nonsmooth equation.
    \param reaction the reaction guess as input and the solution as output.
    \param velocity the velocity guess as input and the solution as output.
    \param info the return info. 0 success, 1 failure.
    \param options the SolverOptions parameter.
 */
void frictionContactNonsmoothEqnSolve(FrictionContactNonsmoothEqn* equation,
                                      double* reaction,
                                      double* velocity,
                                      int* info,
                                      SolverOptions* options);
#endif
