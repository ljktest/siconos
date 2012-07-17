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
#ifndef MCP_PROBLEM_H
#define MCP_PROBLEM_H

/*!\file MixedComplementarityProblem.h
*/

/*! \page MCProblem Mixed (Non Linear) Complementarity problems (MCP)
  \section mcpIntro The problem
  A Mixed Complementarity problems (MCP) is a NCP "augmented" with equality constraints.
  It is defined as

  Find \f$(z,w)\f$ such that:\n
  \f{equation*}{
  \begin{array}{ccc}
  w = \left[\begin{array}{c}
        \ \\ w_e\\ \ \\ w_i\end{array}\right] &=& F(z) \\
  w_e &=&0 \\
  0 \le w_i &\perp& z_i \ge 0
  \end{array}
  \f}
  where "i"(resp. "e") stands for inequalities (resp. equalities).

  \f{equation*}{
  z = \left[\begin{array}{c}
        \ \\ z_e\\ \ \\ z_i\end{array}\right]
        \f}

  $z_i,w_i$ are vectors of size $sizeEqualities$, $z_e,w_e$ vectors of size $sizeInequalities$ and $F$ is a
  non linear function that must be user-defined.

  \section mcpSolversList Available solvers :
  - mcp_FB(), nonsmooth Newton method based on Fisher-Burmeister function.

*/

/** type for user defined function used to compute Fmcp and its jacobian.
    TODO : set properly the list of arguments for this function, when
    things will be clearer ...
 */
typedef void (*ptrFunctionMCP)(double*);

//ptrFunctionMCP Fmcp = NULL;

// Definition of a MCP problem.
typedef struct
{
  // $z_e,w_e$ size
  int sizeEqualities;
  // $z_i,w_i$ size
  int sizeInequalities;
  // A pointer to the function to compute F(z)
  ptrFunctionMCP computeFmcp ;
  // A pointer to the function to compute the jacobian of F(z)
  ptrFunctionMCP computeNablaFmcp ;
  // F(z)
  double * Fmcp ;
  // jacobian of F(z)
  double * nablaFmcp ;

} MixedComplementarityProblem;

#endif
