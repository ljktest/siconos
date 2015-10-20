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
#ifndef FRICTIONCONTACT3DUNITARY_ENUMERATIVE_H
#define FRICTIONCONTACT3DUNITARY_ENUMERATIVE_H

/*!\file fc3d_unitary_enumerative.h
  \brief Typedef and functions declarations related to the quartic solver for 3 dimension frictional contact problems.

  Each solver must have 4 functions in its interface:
  - initialize: link local static variables to the global ones (M,q,...)
  - update: link/fill the local variables corresponding to sub-blocks of the full problem, for a specific contact
  - solve: solve the local problem
  - free

*/
#include "SparseBlockMatrix.h"
#include "SolverOptions.h"

#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
extern "C"
{
#endif
  void fc3d_unitary_enumerative_free(FrictionContactProblem* problem);
  void fc3d_unitary_enumerative_initialize(FrictionContactProblem* problem);
  /*API for the nsgs*/
  int fc3d_unitary_enumerative_solve(FrictionContactProblem* problem, double * reaction, SolverOptions* options);
  int fc3d_unitary_enumerative_solve_sliding(FrictionContactProblem* problem, double * reaction, SolverOptions* options);
  int fc3d_unitary_enumerative_test_non_sliding(FrictionContactProblem* problem, double * reaction, double * velocity, SolverOptions* options);
  int fc3d_unitary_enumerative(FrictionContactProblem* problem, double * reaction, double * velocity, int *info, SolverOptions* options);
  int fc3d_unitary_enumerative_setDefaultSolverOptions(SolverOptions* options);
  int fc3d_unitary_enumerative_solve_poly_nu_sliding(FrictionContactProblem* problem, double * reaction, SolverOptions* options);
#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
}
#endif

#endif
