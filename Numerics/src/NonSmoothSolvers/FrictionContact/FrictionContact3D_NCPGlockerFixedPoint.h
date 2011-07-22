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
#ifndef FRICTIONCONTACT3DNCPGlockerFixedPoint_H
#define FRICTIONCONTACT3DNCPGlockerFixedPoint_H

/*!\file FrictionContact3D_FixedP.h
  \brief Typedef and functions declarations related to NCP-Fixed Point solver for 3 dimension frictional contact problems.
  \author Houari Khenous
  Each solver must have 4 functions in its interface:
  - initialize: link local static variables to the global ones (M,q,...)
  - update: link/fill the local variables corresponding to sub-blocks of the full problem, for a specific contact
  - solve: solve the local problem
  - free

*/
#include "NumericsMatrix.h"
#include "SolverOptions.h"
#include "FrictionContactProblem.h"

#ifdef __cplusplus
extern "C"
{
#endif

  void F_GlockerFixedP(int sizeF, double* reaction, double* FVector, int up2Date);

  /** Initialize friction-contact 3D Fixed Point solver
      \param dim. of the global problem
      \param matrix M of the global problem
      \param vector q of the global problem
      \param vector of the friction coefficients
      \param  SolverOptions * options of the solver
  */
  void frictionContact3D_FixedP_initialize(FrictionContactProblem * problem, FrictionContactProblem * localproblem, SolverOptions *);

  /** solve friction-contact 3D problem with Fixed Point
      \param number (position in global matrix) of the considered contact
      \param dim. of the global problem
      \param global reaction (only the block corresponding to the current contact will be modified,
      \param  SolverOptions * options of the solver
      return 0 iff successful
   */
  int frictionContact3D_FixedP_solve(FrictionContactProblem * localproblem , double* , SolverOptions *);

  /** free memory for friction contact 3D Fixed Point solver */
  void frictionContact3D_FixedP_free();

  /** solve friction-contact 3D problem with Fixed Point
      \param dimension of the global problem
      \param velocity vector (in-out)
      \param global reaction vector
      \param output error
  */
  void frictionContact3D_Path_computeError(int, double*, double*, double *);

#ifdef __cplusplus
}
#endif

#endif
