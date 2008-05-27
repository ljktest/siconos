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
#ifndef FRICTIONCONTACT3DNewton_H
#define FRICTIONCONTACT3DNewton_H

/*!\file FrictionContact3D_Newton.h
  \brief Typedef and functions declarations related to Newton solver for 3 dimension frictional contact problems.
  \author Houari Khenous, Franck Perignon

  Each solver must have 4 functions in its interface:
  - initialize: link local static variables to the global ones (M,q,...)
  - update: link/fill the local variables corresponding to sub-blocks of the full problem, for a specific contact
  - solve: solve the local problem
  - free

*/
#include "SparseBlockMatrix.h"

#ifdef __cplusplus
extern "C" {
#endif
  /** Initialize friction-contact 3D Newton solver
      \param dim. of the global problem
      \param matrix M of the global problem
      \param vector q of the global problem
      \param vector of the friction coefficients
      \param vector of int parameters
   */
  void frictionContact3D_Newton_initialize(int, const double*const, const double*const, const double*const, int*);

  /** Initialize friction-contact 3D Newton solver
      \param dim. of the global problem
      \param matrix M of the global problem, saved as a SparseBlockStructuredMatrix
      \param vector q of the global problem
      \param vector of the friction coefficients
      \param vector of int parameters
   */
  void frictionContact3D_Newton_initialize_SBS(int, const SparseBlockStructuredMatrix *const, const double*const, const double*const, int*);

  /** solve friction-contact 3D problem with Newton
      \param number (position in global matrix) of the considered contact
      \param dim. of the global problem
      \param global reaction (only the block corresponding to the current contact will be modified,
      \param vector of int parameters (max iteration numnber ...)
      \param vector of double parameters (tolerance ...)
   */
  void frictionContact3D_Newton_solve(int, int, double*, int*, double*);

  /** free memory for friction contact 3D Newton solver */
  void frictionContact3D_Newton_free();

  /** solve friction-contact 3D problem with Newton
      \param dimension of the global problem
      \param velocity vector (in-out)
      \param global reaction vector
      \param output error
  */
  void frictionContact3D_Newton_computeError(int, double*, double*, double *);

#ifdef __cplusplus
}
#endif

#endif
