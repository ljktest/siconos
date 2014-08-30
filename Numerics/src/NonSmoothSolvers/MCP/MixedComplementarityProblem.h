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

/*! \page MCProblem Mixed (Non Linear) Complementarity problem (MCP)
  \section mcpIntro  Problem Statement
  Given a sufficiently smooth function \f${F} \colon {{\mathrm{I\!R}}}^{n+m}  \to {{\mathrm{I\!R}}}^{n+m} \f$,
  the Mixed Complementarity problem (MCP) is to find two vectors \f$(z,w \in {{\mathrm{I\!R}}}^{n+m})\f$ such that:\n
  \f{align*}{
  w &= \begin{pmatrix}w_e\\w_i\end{pmatrix} = F(z) \\
  w_e &=0 \\
  0 &\le w_i \perp z_i \ge 0,
  \f}
  where "i" (resp. "e") stands for inequalities (resp. equalities). The vector \f$z\f$ is splitted like \f$w\f$:
  \f{equation*}{z =\begin{pmatrix}z_e\\z_i\end{pmatrix}.\f}
  The vectors \f$z_i,w_i\f$ are of size <tt>sizeEqualities</tt>, the vectors \f$z_e,w_e\f$ are of size <tt>sizeInequalities</tt>
  and \f$F\f$ is a non linear function that must be user-defined.

  A Mixed Complementarity problem (MCP) is a NCP "augmented" with equality constraints.

  \section mcpSolversList Available solvers :
  - mcp_FB(), nonsmooth Newton method based on Fisher-Burmeister function.
*/

/** type for user defined function used to compute Fmcp and its jacobian.
    TODO : set properly the list of arguments for this function, when
    things will be clearer ...
 */
typedef void (*ptrFunctionMCP)(int size , double* z, double * F);
typedef void (*ptrFunctionMCP2)(void* env, int n1, int n2, double* z, double * F);

/** \struct  MixedComplementarityProblem MixedComplementarityProblem.h
 * The structure that defines a Mixed Complementarity problem (MCP) : Find two vectors \f$(z,w \in {{\mathrm{I\!R}}}^{n+m})\f$ such that:\n
  \f{align*}{
  w &= \begin{pmatrix}w_e\\w_i\end{pmatrix} = F(z) \\
  w_e &=0 \\
  0 &\le w_i \perp z_i \ge 0
  \f}
  where "i" (resp. "e") stands for inequalities (resp. equalities). The vector \f$z\f$ is splitted like \f$w\f$:
  \f{equation*}{z =\begin{pmatrix}z_e\\z_i\end{pmatrix}\f}.
  \f$z_i,w_i\f$ are vectors of size <pre>sizeEqualities</pre>, \f$z_e,w_e\f$ vectors of size <pre>sizeInequalities</pre>
  and \f$F\f$ is a non linear function that must be user-defined.
 */
typedef struct
{
  int sizeEqualities; /**< size of equalities $z_e, w_e$ size */
  int sizeInequalities; /**< size of inequalities $z_i,w_i$ size */
  ptrFunctionMCP computeFmcp; /**< pointer to the function to compute F(z) */
  ptrFunctionMCP computeNablaFmcp; /** pointer to the function to compute the jacobian of F(z) */
  /** The value F(z) */
  double* Fmcp ;
  /** jacobian of F(z) */
  double* nablaFmcp ;

} MixedComplementarityProblem;

/** \struct MixedComplementarityProblem2_ MixedComplementarityProblem.h
 * Structure that contains and defines a MixedComplementarityProblem
 */
typedef struct MixedComplementarityProblem2_
{
  int n1; /**< number of equalities constraints */
  int n2; /**< size of complementary variables */
  ptrFunctionMCP2 compute_Fmcp; /**< pointer to the function used to compute \f$F_{mcp}(z) = (G(z), H(z))\f$ */
  ptrFunctionMCP2 compute_nabla_Fmcp; /**< pointer to the function used to compute \f$\nabla_z F_{mcp}\f$ */
  double* nabla_Fmcp; /**< storage for \f$\nabla_z F_{mcp}\f$*/
  void* env; /**< environment for the compute_Fmcp and compute_nabla_Fmcp function.
               When called from Python, it contains an object with compute_Fmcp and compute_nabla_Fmcp as methods.
               When called from C, it can reference a data struct containing variables needed for the computations.*/
} MixedComplementarityProblem2;

#include "NumericsConfig.h"

#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
extern "C"
{
#endif

  /** \fn  void freeMixedComplementarityProblem(MixedComplementarityProblem* problem)
   *  \brief function to delete a MixedComplementarityProblem
   *  \param problem  pointer to a MixedComplementarityProblem to delete
   */
  void freeMixedComplementarityProblem(MixedComplementarityProblem* problem);



#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
}
#endif

#endif
