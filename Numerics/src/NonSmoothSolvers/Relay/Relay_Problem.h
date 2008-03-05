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
#ifndef RELAY_PROBLEM_H
#define RELAY_PROBLEM_H

/*! \page RelayProblem Primal or Dual Relay problems
  \section relayIntro The problem
  Two formulations are available, primal and dual problems.

  Try \f$(z,w)\f$ such that:\n

  <em>primal problem:</em>\n

  \f$
  \left\lbrace
  \begin{array}{l}
  w - M z = q \\
  -w \in \partial\psi_{[-b, a]}(z)\\
  \end{array}
  \right.
  \f$

  or

  <em>dual problem:</em>\n
  \f$
  \left\lbrace
  \begin{array}{l}
  w - M z = q\\
  -z \in \partial\psi_{[-b,a]}(w)\\
  \end{array}
  \right.
  \f$

  here M is an (\f$ n \times n \f$)-matrix, q an n-dimensional vector, z an n-dimensional  vector and w an n-dimensional vector.

  The solvers and their parameters are described in \ref relaySolvers . \n

  \section prSolversList Available solvers for primal case

  Use the generic function pr_solver(), to call one the the specific solvers listed below:

  - pr_latin(), latin solver for primal relay problems.
  - pr_nlgs(), non linear Gauss-Seidel solver for primal relay problems

  \section prSolversList Available solvers for dual case

  Use the generic function dr_solver(), to call one the the specific solvers listed below:

  - dr_latin(), latin (LArge Time INcrement)solver for dual relay problems.
  - dr_nlgs(), non linear Gauss-Seidel solver for dual relay problems

  (see the functions/solvers list in Relay_Solvers.h)

*/

/*!\file Relay_Problem.h
  \brief Structure used to define a Relay (dual or primal) Problem

  \author Franck Perignon
*/

#include "NumericsMatrix.h"

/** Relay Problem elements
    \param size dim of the problem
    \param M matrix of the LCP
    \param q vector
    \param a upper bound
    \param b lower bound
    \param isComplete equal to 0 if some information is missing or wrong for the problem (M or q = NULL, inconsistent sizes), else equal to 1.
 */
typedef struct
{
  int size;
  NumericsMatrix* M;
  double * q;
  double * a;
  double * b;
} Relay_Problem;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif

