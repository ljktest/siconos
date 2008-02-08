/* Siconos-Numerics version 2.1.1, Copyright INRIA 2005-2007.
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
#ifndef FRICTIONCONTACTPROBLEM_H
#define FRICTIONCONTACTPROBLEM_H

/*! \page fcProblem Friction-contact problems (2 or 3-dimensional)
  \section fcIntro The problem
  Find \f$(reaction,velocity)\f$ such that:\n

  \f$
  \left\lbrace
  \begin{array}{l}
  M \ reaction + q = velocity \\
  0 \le reaction_n \perp velocity_n \ge 0 \\
  -velocity_t \in \partial\psi_{[-\mu.reaction_n, \mu.reaction_n]}(reaction_t)\\
  \end{array}
  \right.
  \f$

  \f$ reaction, velocity, q\f$ are vectors of size n and \f$ M \f$ is a nXn matrix, with \f$ n = 3*nc \f$, nc being the number of contacts. \n
  \f$ reaction_n\f$ represents the normal part of the reaction while \f$ reaction_t\f$ is its tangential part.

  \f$ \mu \f$ is the friction coefficient (may be different for each contact).

  \section fc3DSolversList Available solvers for Friction Contact 3D
  Use the generic function frictionContact3DSolvers(), to call one the the specific solvers listed below:

  - frictionContact3D_nsgs() : non-smooth Gauss-Seidel solver
  - frictionContact3D_nsgs_SBS() : non-smooth Gauss-Seidel solver with Sparse Block Storage (SBS) for M

  The structure method, argument of frictionContact3DSolvers(), is used to give the name and parameters of the required solver.

  (see the functions/solvers list in FrictionContact3DSolvers.h)

  \section fc3DParam Required and optional parameters
  FrictionContact3D problems needs some specific parameters, given to the FrictionContact3D_Driver() function thanks to a Solver_Options structure. \n
  They are:\n
     - the name of the solver (ex: NSGS), used to switch to the right solver function
     - iparam[0]: max. number of iterations allowed
     - iparam[1]:
     - dparam[0]: tolerance
     - isStorageSparse: 1 if a SparseBlockStructuredMatrix is used for M, else 0 (double* storage)

*/

/*!\file FrictionContact_Problem.h
  \brief Definition of a structure to handle with friction-contact (2D or 3D) problems.
  \author Franck Perignon.
*/

#include "NumericsMatrix.h"

/** The structure that defines a Friction-Contact (3D or 2D)problem
    \param numberOfContacts, the number of contacts
    \param M matrix (n X n, with n = 2 or 3*numberOfContacts)
    \param q vector (n)
    \param mu, vector of friction coefficients (size: numberOfContacts)
    \param isComplete, equal to 0 if some information is missing or wrong for the problem (M or q = NULL, inconsistent sizes), else equal to 1.
*/
typedef struct
{
  int numberOfContacts;
  NumericsMatrix* M;
  double* q;
  double* mu;
  int isComplete;
} FrictionContact_Problem;

#endif
