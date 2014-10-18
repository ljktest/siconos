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
#ifndef LCP_SOLVERS_H
#define LCP_SOLVERS_H

/*!\file LCP_Solvers.h
  \brief Subroutines for the resolution of Linear Complementarity Problems.\n

  \author siconos-team@lists.gforge.inria.fr
*/

/** \page LCPSolvers Linear Complementarity Problems Solvers
 *  \tableofcontents

This page gives an overview of the available solvers for LCP and their required parameters.

For each solver, the input argument are:
- a LinearComplementarityProblem structure
- the unknowns z and w
- info, the termination value (0: convergence, >0 problem which depends on the solver)
- a SolverOptions structure, which handles iparam and dparam

Remark: when the filterOn parameter (from SolverOptions) is different from 0, lcp_compute_error() is called at the end of the
process to check the validity of the solution. This function needs a tolerance value and returns an error. \n
In that case, tolerance is dparam[0] and error output dparam[1]. Thus, in the following solvers, when dparam[0,1] are omitted, that
means that they are not required inputs, and that if filter is on, some default values will be used.

\section lcpLemke lexicographic Lemke

Direct solver for LCP based on pivoting method principle for degenerated problem.

 function: lcp_lexicolemke() \n
 parameters:
- iparam[0] (in) : max. number of iterations
- iparam[1] (out): number of iterations processed

\section lcpQP QP Solver

quadratic programm formulation for solving a LCP with a symmetric matrix M.

The QP we solve is

  Minimize:
  \f{equation*}
    z^T (M z + q)
  \f}

  subject to:
  \f{equation*}
  Mz  + q  \geq  0
  \f}

  which is the classical reformulation that can be found
  in Cottle, Pang and Stone (2009).

  If the symmetry condition is not fulfilled, use the NSQP Solver

function: lcp_qp() \n
 parameters:
- dparam[0] (in): tolerance

\section lcpNSQP NSQP Solver

non symmetric (and not nonsmooth as one could have thought in a plateform dedicated to nonsmooth problems)
quadratic programm formulation for solving an LCP with a non symmetric matrix.

function: lcp_nsqp() \n
 parameters:
- dparam[0] (in): tolerance

\section lcpCPG CPG Solver
Conjugated Projected Gradient solver for LCP based on quadratic minimization.
Reference: "Conjugate gradient type algorithms for frictional multi-contact problems: applications to granular materials",
M. Renouf, P. Alart. doi:10.1016/j.cma.2004.07.009

function: lcp_cpg() \n
 parameters:
- iparam[0] (in): maximum number of iterations allowed
- iparam[1] (out): number of iterations processed
- dparam[0] (in): tolerance
- dparam[1] (out): resulting error


\section lcpPGS PGS Solver
Projected Gauss-Seidel solver

function: lcp_pgs() \n
 parameters:
- iparam[0] (in): maximum number of iterations allowed
- iparam[1] (out): number of iterations processed
- dparam[0] (in): tolerance
- dparam[1] (out): resulting error

\section lcpRPGS RPGS Solver
Regularized Projected Gauss-Seidel, solver for LCP, able to handle with matrices with null diagonal terms

function: lcp_rpgs() \n
 parameters:
- iparam[0] (in): maximum number of iterations allowed
- iparam[1] (out): number of iterations processed
- dparam[0] (in): tolerance
- dparam[1] (out): resulting error
- dparam[2] (in): rho

\section lcpPSOR PSOR Solver
Projected Succesive over relaxation solver for LCP. See Cottle, Pang and Stone (2009), Chap 5 
function: lcp_psor() \n
 parameters:
- iparam[0] (in): maximum number of iterations allowed
- iparam[1] (out): number of iterations processed
- dparam[0] (in): tolerance
- dparam[1] (out): resulting error
- dparam[2] (in): relaxation parameter

\section lcpNewtonMin  NewtonMin Solver
a nonsmooth Newton method based on the min formulation of the LCP

function: lcp_newton_min() \n
 parameters:
- iparam[0] (in): maximum number of iterations allowed
- iparam[1] (out): number of iterations processed
- iparam[2] (in): if > 0, keep the work vector (reduce the number of memory allocation if the same type of problem is solved multiple times)
- iparam[3] (in): if > 0. use a non-monotone linear search
- iparam[4] (in): if a non-monotone linear search is used, specify the number of merit values to remember
- dparam[0] (in): tolerance
- dparam[1] (out): resulting error

\section lcpNewtonFB  NewtonFB Solver
a nonsmooth Newton method based based on the Fischer-Burmeister NCP function.
It uses a variant of line search algorithm (VFBLSA in Facchinei-Pang 2003).

function: lcp_newton_FB() \n
 parameters:
- iparam[0] (in): maximum number of iterations allowed
- iparam[1] (out): number of iterations processed
- iparam[2] (in): if > 0, keep the work vector (reduce the number of memory allocation if the same type of problem is solved multiple times)
- iparam[3] (in): if > 0. use a non-monotone linear search
- iparam[4] (in): if a non-monotone linear search is used, specify the number of merit values to remember
- dparam[0] (in): tolerance
- dparam[1] (out): resulting error

\section lcpNewtonminFB  Newton min + FB Solver
a nonsmooth Newton method based based on the minFBLSA algorithm : the descent direction is given
by a min reformulation but the linesearch is done with Fischer-Burmeister (and if needed the gradient direction).

function: lcp_newton_minFB() \n
 parameters:
- iparam[0] (in): maximum number of iterations allowed
- iparam[1] (out): number of iterations processed
- iparam[2] (in): if > 0, keep the work vector (reduce the number of memory allocation if the same type of problem is solved multiple times)
- iparam[3] (in): if > 0. use a non-monotone linear search
- iparam[4] (in): if a non-monotone linear search is used, specify the number of merit values to remember
- dparam[0] (in): tolerance
- dparam[1] (out): resulting error


\section lcpPath Path (Ferris) Solver
This solver uses the external PATH solver

 function: lcp_path() \n
 parameters:
- dparam[0] (in): tolerance

\section lcpEnum Enumeratif Solver
A brute-force method to find the solution of the LCP

function: lcp_enum() \n
 parameters:
- iparam[0] (in): search for multiple solutions if 1
- iparam[1] (out): key of the solution
- iparam[1] (out): number of solutions
- iparam[3] (in):  starting key values (seed)
- iparam[4] (in):  use DGELS (1) or DGESV (0).
- dparam[0] (in): tolerance

\section lcpLatin Latin Solver
LArge Time INcrements solver

function: lcp_latin() \n
 parameters:
- iparam[0] (in): maximum number of iterations allowed
- iparam[1] (out): number of iterations processed
- dparam[0] (in): tolerance
- dparam[1] (out): resulting error
- dparam[2] (in): latin parameter

\section lcpLatinW Latin_w Solver
LArge Time INcrements solver with relaxation

function: lcp_latin_w() \n
 parameters:
- iparam[0] (in): maximum number of iterations allowed
- iparam[1] (out): number of iterations processed
- dparam[0] (in): tolerance
- dparam[1] (out): resulting error
- dparam[2] (in): latin parameter
- dparam[3] (in): relaxation parameter

\section lcpGS Block solver (Gauss Seidel)
Gauss-Seidel for Sparse-Block matrices. \n
Matrix M of the LCP must be a SparseBlockStructuredMatrix. \n
This solver first build a local problem for each row of blocks and then call any of the other solvers through lcp_driver().

function: lcp_nsgs_SBM() \n
 parameters:
- iparam[0] (in): maximum number of iterations allowed for GS process
- iparam[1] (out): number of GS iterations processed
- iparam[2] (out): sum of all local number of iterations (if it has sense for the local solver)
- dparam[0] (in): tolerance
- dparam[1] (out): resulting error
- dparam[2] (in): sum of all local error values

*/

#include "NumericsOptions.h"
#include "LinearComplementarityProblem.h"
#include "SolverOptions.h"
#include "lcp_cst.h"
#include "SiconosCompat.h"



#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
extern "C"
{
#endif

  /** \fn   int linearComplementarity_driver(LinearComplementarityProblem* problem, double *z , double *w, SolverOptions* options,  NumericsOptions* global_options)
   *
   General interface to solvers for Linear Complementarity Problems
    \param[in] problem the LinearComplementarityProblem structure which handles the problem (M,q)
    \param[in,out] z a n-vector of doubles which contains the solution of the problem.
    \param[in,out] w a n-vector of doubles which contains the solution of the problem.
    \param[in,out] options structure used to define the solver(s) and their parameters
    \param[in]  global_options the global options of Numerics
    \return info termination value
    - 0 : successful\n
    - >0 : otherwise see each solver for more information about the log info
    \author Franck Perignon
  */
  int linearComplementarity_driver(LinearComplementarityProblem* problem, double *z , double *w, SolverOptions* options,  NumericsOptions* global_options);

  /** set the default solver parameters and perform memory allocation for LinearComplementarity
      \param[in] problem the LinearComplementarityProblem structure which handles the problem (M,q)
      \param options the pointer to the array of options to set
      \return info termination value
  */
  int linearComplementarity_setDefaultSolverOptions(LinearComplementarityProblem* problem, SolverOptions* options, int);




  /** lcp_qp uses a quadratic programm formulation for solving a LCP
   * \param[in] problem structure that represents the LCP (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param[out] info an integer which returns the termination value:\n
   *                0 : convergence  / minimization sucessfull\n
   *                1 : Too Many iterations\n
   *              2 : Accuracy insuficient to satisfy convergence criterion\n
   *                5 : Length of working array insufficient\n
   *                Other : The constraints are inconstent\n
   * \param[in,out] options structure used to define the solver and its parameters.
   *
   * \author Vincent Acary
   */
  void lcp_qp(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options);

  /** set the default solver parameters and perform memory allocation for LinearComplementarity
      \param options the pointer to the array of options to set
  */
  int linearComplementarity_qp_setDefaultSolverOptions(SolverOptions* options);

  /** lcp_cpg is a CPG (Conjugated Projected Gradient) solver for LCP based on quadratic minimization.
   * \param[in] problem structure that represents the LCP (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param[out] info an integer which returns the termination value:\n
   0: convergence\n
   1: iter = itermax\n
   2: negative diagonal term\n
   3: pWp nul
   * \param[in,out] options structure used to define the solver and its parameters.
   *
   \author Mathieu Renouf.
  */
  void lcp_cpg(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options);

  /** set the default solver parameters and perform memory allocation for LinearComplementarity
    \param options the pointer to the array of options to set
  */
  int linearComplementarity_cpg_setDefaultSolverOptions(SolverOptions* options);


  /** lcp_pgs (Projected Gauss-Seidel) is a basic Projected Gauss-Seidel solver for LCP.\n
   * \param[in] problem structure that represents the LCP (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param[out] info an integer which returns the termination value:\n
   0 : convergence\n
   1 : iter = itermax\n
   2 : negative diagonal term
   \param[in,out] options structure used to define the solver and its parameters.
  */
  void lcp_pgs(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options);

  /** set the default solver parameters and perform memory allocation for LinearComplementarity
      \param options the pointer to the array of options to set
  */
  int linearComplementarity_pgs_setDefaultSolverOptions(SolverOptions* options);

  /** lcp_rpgs (Regularized Projected Gauss-Seidel ) is a solver for LCP, able to handle matrices with null diagonal terms.\n
   * \param[in] problem structure that represents the LCP (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param[out] info an integer which returns the termination value:\n
   0 : convergence\n
   1 : iter = itermax\n
   2 : negative diagonal term
   * \param[in,out] options structure used to define the solver and its parameters.
   *

   \author Mathieu Renouf & Pascal Denoyelle
   \todo Sizing the regularization paramter and apply it only on null diagnal term

  */
  void lcp_rpgs(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options);

  /** set the default solver parameters and perform memory allocation for LinearComplementarity
      \param options the pointer to the array of options to set
  */
  int linearComplementarity_rpgs_setDefaultSolverOptions(SolverOptions* options);


  /** lcp_psor Projected Succesive over relaxation solver for LCP. See cottle, Pang Stone Chap 5
   * \param[in] problem structure that represents the LCP (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param[out] info an integer which returns the termination value:\n
   0 : convergence\n
   1 : iter = itermax\n
   2 : negative diagonal term
   * \param[in,out] options structure used to define the solver and its parameters.
   \todo use the relax parameter
   \todo add test
   \todo add a vector of relaxation parameter wtith an auto sizing (see SOR algorithm for linear solver.)

  */
  void lcp_psor(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options);

  /** set the default solver parameters and perform memory allocation for LinearComplementarity
      \param options the pointer to the array of options to set
  */
  int linearComplementarity_psor_setDefaultSolverOptions(SolverOptions* options);


  /** lcp_nsqp use a quadratic programm formulation for solving an non symmetric LCP
   * \param[in] problem structure that represents the LCP (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param[out] info an integer which returns the termination value:\n
   *                0 : convergence  / minimization sucessfull\n
   *                1 : Too Many iterations\n
   *            2 : Accuracy insuficient to satisfy convergence criterion\n
   *                5 : Length of working array insufficient\n
   *                Other : The constraints are inconstent\n
   * \param[in,out] options structure used to define the solver and its parameters.
   *
   * \author Vincent Acary
   *
   */
  void lcp_nsqp(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options);

  /** set the default solver parameters and perform memory allocation for LinearComplementarity
      \param options the pointer to the array of options to set
  */
  int linearComplementarity_nsqp_setDefaultSolverOptions(SolverOptions* options);


  /** lcp_latin (LArge Time INcrements) is a basic latin solver for LCP.
   * \param[in] problem structure that represents the LCP (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param[out] info an integer which returns the termination value:\n
   0 : convergence\n
   1 : iter = itermax\n
   2 : Cholesky Factorization failed \n
   3 : nul diagonal term\n
   * \param[in,out] options structure used to define the solver and its parameters.

   \author Nineb Sheherazade.
  */
  void lcp_latin(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options);
  /** set the default solver parameters and perform memory allocation for LinearComplementarity
      \param options  the pointer to the array of options to set
  */
  int linearComplementarity_latin_setDefaultSolverOptions(SolverOptions* options);


  /** lcp_latin_w (LArge Time INcrements) is a basic latin solver with relaxation for LCP.\n
   * \param[in] problem structure that represents the LCP (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param[out] info an integer which returns the termination value:\n
   0 : convergence\n
   1 : iter = itermax\n
   2 : Cholesky Factorization failed \n
   3 : nul diagonal term\n
   * \param[in,out] options structure used to define the solver and its parameters.
   *

   \author Nineb Sheherazade.
  */
  void lcp_latin_w(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options);

  /** set the default solver parameters and perform memory allocation for LinearComplementarity
      \param options the pointer to the array of options to set
  */
  int linearComplementarity_latin_w_setDefaultSolverOptions(SolverOptions* options);


  /** lcp_lexicolemke is a direct solver for LCP based on pivoting method principle for degenerate problem \n
   * Choice of pivot variable is performed via lexicographic ordering \n
   *  Ref: "The Linear Complementarity Problem" Cottle, Pang, Stone (1992)\n
   * \param[in] problem structure that represents the LCP (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param[out] info an integer which returns the termination value:\n
   * 0 : convergence\n
   * 1 : iter = itermax\n
   * 2 : negative diagonal term\n
   * \param[in,out] options structure used to define the solver and its parameters.
   *
   *\author Mathieu Renouf
   */
  void lcp_lexicolemke(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options);

  /** set the default solver parameters and perform memory allocation for LinearComplementarity
      \param options the pointer to the array of options to set
  */
  int linearComplementarity_lexicolemke_setDefaultSolverOptions(SolverOptions* options);


  /** lcp_newton_min uses a nonsmooth Newton method based on the min formulation  (or max formulation) of the LCP
      \f$
      0 \le z \perp w \ge 0 \Longrightarrow \min(w,\rho z)=0 \Longrightarrow w = \max(0,w - \rho z)
      \f$

      \f$
      H(z) = H(\left[ \begin{array}{c} z \\ w \end{array}\right])= \left[ \begin{array}{c} w-Mz-q \\ min(w,\rho z) \end{array}\right] =0\\
      \f$


      References: Alart & Curnier 1990, Pang 1990
      * \param[in] problem structure that represents the LCP (M, q...)
      * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
      * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
      * \param[out] info an integer which returns the termination value:\n
      0 : convergence\n
      1 : iter = itermax\n
      2 : Problem in resolution in DGESV\n
      *                0 : convergence  / minimization sucessfull\n
      *                1 : Too Many iterations\n
      *              2 : Accuracy insuficient to satisfy convergence criterion\n
      *                5 : Length of working array insufficient\n
      *                Other : The constraints are inconstent\n
      * \param[in,out] options structure used to define the solver and its parameters.
      *
      \author Vincent Acary

      \todo Optimizing the memory allocation (Try to avoid the copy of JacH into A)
      \todo Add rules for the computation of the penalization rho
      \todo Add a globalization strategy based on a decrease of a merit function. (Nonmonotone LCP) Reference in Ferris Kanzow 2002
  */
  void lcp_newton_min(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options);

  /** set the default solver parameters and perform memory allocation for LinearComplementarity
      \param options the pointer to the array of options to set
  */
  int linearComplementarity_newton_min_setDefaultSolverOptions(SolverOptions* options);


  /** lcp_newton_FB use a nonsmooth newton method based on the Fischer-Bursmeister convex function
   *
   *
   * \f$
   *   0 \le z \perp w \ge 0 \Longrightarrow \phi(z,w)=\sqrt{z^2+w^2}-(z+w)=0
   * \f$

   * \f$
   *   \Phi(z) = \left[ \begin{array}{c}  \phi(z_1,w_1) \\ \phi(z_1,w_1) \\ \vdots \\  \phi(z_n,w_n)  \end{array}\right] =0\\
   * \f$
   *
   *
   * References: Alart & Curnier 1990, Pang 1990
   *
   * \param[in] problem structure that represents the LCP (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param[out] info an integer which returns the termination value:\n
   *                0 - convergence\n
   *                1 - iter = itermax\n
   *                2 - failure in the descent direction search (in LAPACK) \n
   *
   * \param[in,out] options structure used to define the solver and its parameters.
   * \author Vincent Acary and Olivier Huber
   *
   * \todo Optimizing the memory allocation (Try to avoid the copy of JacH into A)
   * \todo Add rules for the computation of the penalization rho
   * \todo Add a globalization strategy based on a decrease of a merit function. (Nonmonotone LCP) Reference in Ferris Kanzow 2002
   */

  void lcp_newton_FB(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options);
  /** set the default solver parameters and perform memory allocation for LinearComplementarity
      \param options the pointer to the array of options to set
  */
  int linearComplementarity_newton_FB_setDefaultSolverOptions(SolverOptions* options);

  /** lcp_newton_minFB use a nonsmooth newton method based on both a min and Fischer-Bursmeister reformulation
   * References: Facchinei--Pang (2003)
   *
   * \param[in] problem structure that represents the LCP (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param[out] info an integer which returns the termination value:\n
   *                0 - convergence\n
   *                1 - iter = itermax\n
   *                2 - failure in the descent direction search (in LAPACK) \n
   *
   * \param[in,out] options structure used to define the solver and its parameters.
   * \author Olivier Huber
   */
  void lcp_newton_minFB(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options);

  /**
   * \param[in] problem structure that represents the LCP (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param[out] info an integer which returns the termination value:\n
   0 : convergence\n
   1 : iter = itermax\n
   2 : negative diagonal term
   * \param[in,out] options structure used to define the solver and its parameters.

   \author Olivier Bonnefon
  */
  void lcp_path(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options);
  /** set the default solver parameters and perform memory allocation for LinearComplementarity
      \param options the pointer to the array of options to set
  */
  int linearComplementarity_path_setDefaultSolverOptions(SolverOptions* options);


  /** enumerative solver
  * \param[in] problem structure that represents the LCP (M, q...)
  * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
  * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
  * \param[out] info an integer which returns the termination value:\n
  0 : success\n
  1 : failed\n
  * \param[in,out] options structure used to define the solver and its parameters.

  \author Olivier Bonnefon
  */
  void lcp_enum(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options);

  /**
  * \param[in] problem structure that represents the LCP (M, q...)
  * \param[in,out] options structure used to define the solver and its parameters.
  * \param[in] withMemAlloc If it is not 0, then the necessary work memory is allocated.
  */
  void lcp_enum_init(LinearComplementarityProblem* problem, SolverOptions* options, int withMemAlloc);
  /**
  * \param[in] problem structure that represents the LCP (M, q...)
  * \param[in,out] options structure used to define the solver and its parameters.
  * \param[in]  withMemAlloc If it  is not 0, then the work memory is free.
  */
  void lcp_enum_reset(LinearComplementarityProblem* problem, SolverOptions* options, int withMemAlloc);

  /** set the default solver parameters and perform memory allocation for LinearComplementarity
      \param problem structure that represents the LCP (M, q...)
      \param  options  the pointer to the array of options to set
  */
  int linearComplementarity_enum_setDefaultSolverOptions(LinearComplementarityProblem* problem, SolverOptions* options);

  /** lcp_avi_caoferris is a direct solver for LCP based on an Affine Variational Inequalities (AVI) reformulation\n
   * The AVI solver is here the one from Cao and Ferris \n
   *  Ref: "A Pivotal Method for Affine Variational Inequalities" Menglin Cao et Michael Ferris (1996)\n
   * \param[in] problem structure that represents the LCP (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param[out] info an integer which returns the termination value:\n
   * 0 : convergence\n
   * 1 : iter = itermax\n
   * \param[in,out] options structure used to define the solver and its parameters.
   *
   *\author Olivier Huber
   */
  void lcp_avi_caoferris(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options);

  /** set the default solver parameters and perform memory allocation for LinearComplementarity
      \param options the pointer to the array of options to set
  */
  int linearComplementarity_avi_caoferris_setDefaultSolverOptions(SolverOptions* options);

  /** lcp_pivot is a direct solver for LCP based on a pivoting method\n
   * It can currently use Bard, Murty's least-index or Lemke rule for choosing
   * the pivot. The default one is Lemke and it cam be changed by setting
   * iparam[2]. The list of choices are in the enum LCP_PIVOT (see lcp_cst.h).
   * \param[in] problem structure that represents the LCP (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param[out] info an integer which returns the termination value:\n
   * 0 : convergence\n
   * 1 : iter = itermax\n
   * \param[in,out] options structure used to define the solver and its parameters.
   *
   *\author Olivier Huber
   */
  void lcp_pivot(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options);
  void lcp_pivot_covering_vector(LinearComplementarityProblem* problem, double* restrict u , double* restrict s, int *info , SolverOptions* options, double* restrict cov_vec);

  /** lcp_pathsearch is a direct solver for LCP based on the pathsearch algorithm\n
   * \warning this solver is available for testing purposes only! consider
   * using lcp_pivot() if you are looking for simular solvers
   * \param[in] problem structure that represents the LCP (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param[out] info an integer which returns the termination value:\n
   * 0 : convergence\n
   * 1 : iter = itermax\n
   * \param[in,out] options structure used to define the solver and its parameters.
   *
   *\author Olivier Huber
   */
  void lcp_pathsearch(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options);

  /** set the default solver parameters and perform memory allocation for LinearComplementarity
      \param options the pointer to the array of options to set
  */
  int linearComplementarity_pivot_setDefaultSolverOptions(SolverOptions* options);

  /** generic interface used to call any LCP solver applied on a Sparse-Block structured matrix M, with a Gauss-Seidel process
   * to solve the global problem (formulation/solving of local problems for each row of blocks)
   * \param[in] problem structure that represents the LCP (M, q...). M must be a SparseBlockStructuredMatrix
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param info an integer which returns the termination value:\n
   0 : convergence\n
   >0 : failed, depends on local solver
   * \param[in,out] options structure used to define the solver and its parameters.
   * \author Mathieu Renouf, Pascal Denoyelle, Franck Perignon
   */
  void lcp_nsgs_SBM(LinearComplementarityProblem* problem, double *z, double *w, int* info, SolverOptions* options);
  /** set the default solver parameters and perform memory allocation for LinearComplementarity
      \param options the pointer to  the array of options to set
  */
  int linearComplementarity_nsgs_SBM_setDefaultSolverOptions(SolverOptions* options);

  /** Construct local problem from a "global" one
   * \param rowNumber index of the local problem
   * \param blmat matrix containing the problem
   * \param local_problem problem to fill
   * \param q big q
   * \param z big z
   */
  void lcp_nsgs_SBM_buildLocalProblem(int rowNumber, const SparseBlockStructuredMatrix* const blmat, LinearComplementarityProblem* local_problem, double* q, double* z);

  /** This function computes the input vector \f$ w = Mz + q \f$ and checks the validity of the vector z as a solution \n
   * of the LCP : \n
   * \f$
   *    0 \le z \perp Mz + q \ge 0
   * \f$
   * The criterion is based on \f$ \sum [ (z[i]*(Mz+q)[i])_{pos} + (z[i])_{neg} + (Mz+q)[i])_{neg} ] \f$ \n
   * with \f$ x_{pos} = max(0,x) \f$ and \f$ xneg = max(0,-x)\f$. \n
   * This sum is divided by \f$ \|q\| \f$ and then compared to tol.\n
   * \param[in] problem structure that represents the LCP (M, q...)
   * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
   * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
   * \param[in] tolerance threshold used to validate the solution: if the error is less than this value, the solution is accepted
   * \param[out] error the actual error of the solution with respect to the problem
   * \return status: 0 : convergence, 1: error > tolerance
   * \author Pascal Denoyelle, Franck Perignon
   */
  int lcp_compute_error(LinearComplementarityProblem* problem, double *z , double *w, double tolerance, double* error);

  /** This function computes the input vector \f$ w = Mz + q \f$ and checks the validity of the vector z as a solution \n
  * of the LCP : \n
  * \f$
  *    0 \le z \perp Mz + q \ge 0
  * \f$
  * The criterion is based on \f$ \sum [ (z[i]*(Mz+q)[i])_{pos} + (z[i])_{neg} + (Mz+q)[i])_{neg} ] \f$ \n
  * with \f$ x_{pos} = max(0,x) \f$ and \f$ xneg = max(0,-x)\f$. \n
  * This sum is divided by \f$ \|q\| \f$ and then compared to tol.\n
  * \param[in] n size of the LCP
  * \param[in,out] z a n-vector of doubles which contains the initial solution and returns the solution of the problem.
  * \param[in,out] w a n-vector of doubles which returns the solution of the problem.
  * \param[out] error the result of the computation
  * \author Pascal Denoyelle, Franck Perignon
  */
  void lcp_compute_error_only(unsigned int n,  double *z , double *w, double * error);

  /*   /\** Function used to extract from LCP matrix the part which corresponds to non null z */
  /*    *\/ */
  /*   int extractLCP( NumericsMatrix* MGlobal, double *z , int *indic, int *indicop, double *submatlcp , double *submatlcpop, */
  /*     int *ipiv , int *sizesublcp , int *sizesublcpop); */

  /*   /\** Function used to solve the problem with sub-matrices from extractLCP */
  /*    *\/ */
  /*   int predictLCP(int sizeLCP, double* q, double *z , double *w , double tol, */
  /*   int *indic , int *indicop , double *submatlcp , double *submatlcpop , */
  /*    int *ipiv , int *sizesublcp , int *sizesublcpop , double *subq , double *bufz , double *newz); */

  /** Interface to solvers for Linear Complementarity Problems, dedicated to dense matrix storage
      \param[in] problem the LinearComplementarityProblem structure which handles the problem (M,q)
      \param[in,out] z a n-vector of doubles which contains the solution of the problem.
      \param[in,out] w a n-vector of doubles which contains the solution of the problem.
      \param[in,out] options structure used to define the solver(s) and their parameters
      \return info termination value
      - 0 : successful\n
      - >0 : otherwise see each solver for more information about the log info
      \author Nineb Sheherazade, Mathieu Renouf, Franck Perignon
  */
  int lcp_driver_DenseMatrix(LinearComplementarityProblem* problem, double *z , double *w, SolverOptions* options);

#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
}
#endif

#endif

