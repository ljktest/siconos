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
#ifndef RELAY_SOLVERS_H
#define RELAY_SOLVERS_H

/*!\file Relay_Solvers.h
  \author Nineb Sheherazade and Dubois Frederic.
  Last Modifications : Mathieu Renouf , Pascal Denoyelle, Franck Perignon and Olivier Huber
  Subroutines for the resolution of relay problems.
*/

/*! \page RelaySolvers Relay Problems Solvers

This page gives an overview of the available solvers for relay problems and their required parameters.

For each solver, the input argument are:
- a RelayProblem
- the unknowns (z,w)
- info, the termination value (0: convergence, >0 problem which depends on the solver)
- a SolverOptions structure, which handles iparam and dparam

\section relayLatin Latin
LArge Time INcrements solver

 function: dr_latin \n
 parameters:
- iparam[0] (in): maximum number of iterations allowed
- iparam[1] (out): number of iterations processed
- dparam[0] (in): tolerance
- dparam[1] (out): resulting error
- dparam[2] (in): latin parameter

\section relayNLGS Non-linear Gauss Seidel
LArge Time INcrements solver

 function: dr_nlgs()\n
 parameters:
- iparam[0] (in): maximum number of iterations allowed
- iparam[1] (out): number of iterations processed
- dparam[0] (in): tolerance
- dparam[1] (out): resulting error

\section relayENUM enumerative solver
The relay problem is reformulated as a LCP and solved with the enumerative solver

 function: relay_enum\n
   parameters:
  - dparam[0] (in): tolerance
  - iparam[0] (in) : search for multiple solutions if 1
  - iparam[1] (out) : key of the solution
  - iparam[3] (in) :  starting key values (seed)
  - iparam[4] (in) :  use DGELS (1) or DGESV (0).

\section relayPATH PATH solver
The relay problem is reformulated as a LCP and solved with the PATH solver

 function: relay_path\n
- dparam[0] (in): tolerance

\section relayLEMKE Lemke solver
The relay problem is reformulated as a LCP and solved with Lemke's method

 function: relay_lemke\n
 parameters:
- iparam[0] (in): maximum number of iterations allowed
- iparam[1] (out): number of iterations processed

\section relayAVI_CaoFerris CaoFerris solver
The relay problem is reformulated as an AVI and solved with the solver proposed by Cao and Ferris

 function: relay_avi_caoferris\n
 parameters:
- iparam[0] (in): maximum number of iterations allowed
- iparam[1] (out): number of iterations processed

*/

#include "RelayProblem.h"
#include "LinearComplementarityProblem.h"
#include "SolverOptions.h"

#ifdef __cplusplus
#undef restrict
#define restrict __restrict
#endif

#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
extern "C"
{
#endif

  /** General interface to solver for relay problems
      \param[in] problem the RelayProblem structure which handles the problem (M,q)
      \param[in,out] z a n-vector of doubles which contains the solution of the problem.
      \param[in,out] w a n-vector of doubles which contains the solution of the problem.
      \param[in,out] options structure used to define the solver(s) and its (their) parameters
      \param[in,out] global_options non solver-specific options
      \return info termination value
      - 0 : successful\n
      - >0 : otherwise see each solver for more information about the log info
   * \author Nineb Sheherazade.
   */
  int relay_driver(RelayProblem* problem, double *z , double *w, SolverOptions* options,  NumericsOptions* global_options);

  /** set the default solver parameters and perform memory allocation for Relay
      \param[in] problem the RelayProblem structure which handles the problem (M,q)
      \param options the pointer to options to set
      \param solverId the identifier of the solver
  */
  int relay_setDefaultSolverOptions(RelayProblem* problem, SolverOptions* options, int solverId);


  /** relay_pgs is a projected Gauss-Seidel solver for relay problems.\n
   * \param[in] problem structure that represents the Relay (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param options the pointer to options to set
   * \param[out] info an integer which returns the termination value:\n
   0 = convergence,\n
   1 = no convergence,\n
   2 = Null diagonal term\n
   \author V. Acary
  */
  void relay_pgs(RelayProblem* problem, double *z, double *w, int *info, SolverOptions* options);

  /** set the default solver parameters and perform memory allocation for PGS
      \param options SolverOptions * the pointer to options to set
  */
  int relay_pgs_setDefaultSolverOptions(SolverOptions* options);

  /** relay_lexicolemke is a Lemke solver for  relay problems.\n
     * \param[in] problem structure that represents the Relay (M, q...)
     * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
     * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
     * \param options struct used to define the solver(s) and its (their) parameters
     * \param global_options non solver-specific options
     * \param[out] info an integer which returns the termination value:\n
     0 = convergence,\n
     1 = no convergence,\n
     2 = Null diagonal term\n
     \author V. Acary
    */
  void relay_lexicolemke(RelayProblem* problem, double *z, double *w, int *info, SolverOptions* options, NumericsOptions* global_options);

  /** set the default solver parameters and perform memory allocation for Lemke
      \param options the pointer to options to set
  */
  int relay_lexicolemke_setDefaultSolverOptions(SolverOptions* options);

  /** relay_enum is enum solver for  relay problems.\n
     * \param[in] problem structure that represents the Relay (M, q...)
     * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
     * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
     * \param options struct used to define the solver(s) and its (their) parameters
     * \param global_options non solver-specific options
     * \param[out] info an integer which returns the termination value:\n
     0 = convergence,\n
     1 = no convergence,\n
     2 = Null diagonal term\n
     \author V. Acary
    */
  void relay_enum(RelayProblem* problem, double *z, double *w, int *info, SolverOptions* options, NumericsOptions* global_options);

  /** set the default solver parameters and perform memory allocation for ENUM
   * \param[in] problem structure that represents the Relay (M, q...)
   * \param options SolverOptions * the pointer to options to set
  */
  int relay_enum_setDefaultSolverOptions(RelayProblem* problem, SolverOptions* options);

  /** relay_path is a resolution of the Relay with its inherent MCP formulation and using path.\n
   * \param[in] problem structure that represents the Relay (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param options struct used to define the solver(s) and its (their) parameters
   * \param[out] info an integer which returns the termination value:\n
   *  0 = convergence,\n
   *  1 = no convergence,\n
   *  2 = Nul diagonal term\n
   * \author V. acary
   */
  void relay_path(RelayProblem* problem, double *z, double *w, int *info, SolverOptions* options);

  /** set the default solver parameters and perform memory allocation for PATH
      \param options the pointer to options to set
  */
  int relay_path_setDefaultSolverOptions(SolverOptions* options);

  /** Solve a Relay problem using the AVI framework and the solver by Cao
   * and Ferris.
   * \param[in] problem structure that represents the Relay (M, q, ...)
   * \param[in,out] z vector which on call is the initial point and on exit is the solution of the problem.
   * \param[in,out] w vector for computations
   * \param options struct used to define the solver(s) and its (their) parameters
   * \param[out] info an integer which returns the termination value:\n
   *  0 = convergence,\n
   *  1 = no convergence,\n
   * \author Olivier Huber
   */
  void relay_avi_caoferris(RelayProblem* problem, double* restrict z, double* restrict w, int *info, SolverOptions* options);

  /** set the default solver parameters and perform memory allocation for
   * AVI_CAOFERRIS
   * \param options struct used to define the solver(s) and its (their) parameters
   */
  int relay_avi_caoferris_setDefaultSolverOptions(SolverOptions* options);

  /** dr_latin is a specific latin (LArge Time INcrement)solver for dual relay problems.\n
   * \param[in] problem structure that represents the Relay (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param[out] info an integer which returns the termination value:\n
   0 = convergence,\n
   1 = no convergence,\n
   2 = Cholesky factorization failed,\n
   3 = Nul diagonal term\n
   * \param options struct used to define the solver(s) and its (their) parameters
   \author Nineb Sheherazade.
  */
  void dr_latin(RelayProblem* problem, double *z, double *w, int *info, SolverOptions* options)  ;

  /**  dr_nlgs is a specific nlgs (Non Linear Gauss Seidel) solver for dual relay problems.\n
   * \param[in] problem structure that represents the Relay (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param[out] info an integer which returns the termination value:\n
   0 = convergence,\n
   1 = no convergence,\n
   2 = Nul diagonal term\n
   * \param options struct used to define the solver(s) and its (their) parameters
  */
  void dr_nlgs(RelayProblem* problem, double *z, double *w, int *info, SolverOptions* options);

  /** This function computes the input vector \f$ w = Mz + q \f$ and checks the validity of the vector z as a solution \n
     * of the LCP : \n
     * \f$
     *   -(Mz + q) \in  N_{[lb,ub]}(z)
     * \f$
     * The criterion is based on \f$ error = \|z- proj_{[lb,ub]}(z - \rho * (M*z+q)) \|, \rho >0\f$ \n
     * This error is divided by \f$ \|q\| \f$ and then compared to tol.
     * \param[in] problem structure that represents the Relay (M, q...)
     * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
     * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
     * \param[in] tolerance threshold used to validate the solution: if the error is less than this value, the solution is accepted
     * \param[in,out] error the actual error of the solution with respect to the problem
     * \return status: 0 : convergence, 1: error > tolerance
     * \author Vincent Acary
     */
  int relay_compute_error(RelayProblem* problem, double* restrict z , double* restrict w, double tolerance, double* restrict error);


  /** This function computes the projection on the boxr \f$ [lb,ub]\f$ of the vector  \f$z\f$  \n
     * \param[in,out] z a n-vector of doubles which returns the projection
     * \param[in,out] ub a n-vector of doubles which contains the upper bounds
     * \param[in,out] lb a n-vector of doubles which contains the lower bounds
     * \param[in,out] n size of the a n-vector
     * \author Vincent Acary
     */
  void project_on_box(int n, double* restrict z , double* restrict lb, double* restrict ub);

  /** This function transform a RelayProblem into a LinearComplementarityProblem
     * \param[in] problem A pointer to a Relay_problem to transform
     * \param[out] lcp_problem A pointer to a LinearComplementarity_problem resulting from the reformulation
     * \author Vincent Acary
     */
  void relay_to_lcp(RelayProblem* problem, LinearComplementarityProblem* lcp_problem);

#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
}
#endif

#endif
