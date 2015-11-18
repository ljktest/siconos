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
#ifndef fc2dSolvers_H
#define fc2dSolvers_H

/*!\file fc2d_Solvers.h
  \brief Subroutines for the resolution of contact problems with friction (2-dimensional case).\n
  \author Nineb Sheherazade and Dubois Frederic.
  Last Modifications : Mathieu Renouf , Pascal Denoyelle, Franck Perignon
*/

#include "FrictionContactProblem.h"
#include "NumericsOptions.h"
#include "SolverOptions.h"
#include "Friction_cst.h"
#include "LinearComplementarityProblem.h"
#include "SiconosCompat.h"

#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
extern "C"
{
#endif
  /** General interface to solvers for friction-contact 2D problem
   *  \param[in] problem the structure which handles the Friction-Contact problem
   *  \param[in,out] reaction global vector (n)
   *  \param[in,out] velocity global vector (n)
   *  \param[in,out] options structure used to define the solver(s) and their parameters
   *  \param[in] global_options for Numerics (verbose mode ...)
   *  \return result (0 if successful otherwise 1).
   */
  int fc2d_driver(FrictionContactProblem* problem, double *reaction , double *velocity, SolverOptions* options, NumericsOptions* global_options);


  /** set the default solver parameters and perform memory allocation for fc3d
      \param options   the pointer to the options to set
      \param solverId  the identifier of the solver
  */
  int fc2d_setDefaultSolverOptions(SolverOptions* options, int solverId);

  /**  cpg (conjugated projected gradient) solver for global contact problems with friction (2D)
       \param[in]  problem the friction-contact problem
       \param[out] reaction vector
       \param[out] velocity vector
       \param[in,out] info termination value
       \param[in,out] options structure for options
  */
  void fc2d_cpg(FrictionContactProblem* problem , double *reaction , double *velocity , int *info, SolverOptions* options);

  /** set the default solver parameters and perform memory allocation for CPG
   \param  options SolverOptions * the pointer to the options to set
   */
  int fc2d_cpg_setDefaultSolverOptions(SolverOptions* options);
  /**  Non Linear Gauss Seidel solver for global contact problem with friction in 2D case.
       \param[in] problem the friction-contact problem
       \param[out] reaction vector
       \param[out] velocity vector
       \param[in,out] info termination value
       \param[in,out] options structure
  */
  void fc2d_nsgs(FrictionContactProblem* problem , double *reaction , double *velocity , int *info, SolverOptions* options);

  /** set the default solver parameters and perform memory allocation for LATIN
  \param options  the pointer to the options to set
  */
  int fc2d_nsgs_setDefaultSolverOptions(SolverOptions* options);

  /**  latin solver for global contact problem with friction in the 2D case.
       \param[in] problem the friction-contact problem
       \param[out] reaction global vector
       \param[out] velocity global vector
       \param[in,out] info termination value
       \param[in,out] options SolverOptions structure
       \author Nineb Sheherazade.
  */
  void fc2d_latin(FrictionContactProblem* problem , double *reaction , double *velocity , int *info, SolverOptions* options);

  /** set the default solver parameters and perform memory allocation for LATIN
  \param  options the pointer to the options to set
  */
  int fc2d_latin_setDefaultSolverOptions(SolverOptions* options);

  /** fc2d_projc is a specific projection operator related to CPG (conjugated projected gradient) algorithm for global contact problem with friction.\n
   *
   *
   * \param[in] xi  the intermediate iterate which goes to be projected (projc1).
   * \param[in] n   the dimension of the system.
   * \param[in] statusi  a vector which contains the initial status.
   * \param[in] p       a vector which contains the components of the descent direction.
   * \param[in] fric a vector which contains the friction coefficient.
   * \param[out] reaction the corrected iterate.
   * \param[out] status  the new status.
   *
   */
  void fc2d_projc(double xi[], int *n, int statusi[], double p[], double fric[], double *reaction, int *status);

  /** fc2d_projf is a specific projection operator related to CPG (conjugated projected gradient) algorithm
   *              for global contact problem with friction.\n
   *
   *
   * \param[in] etat  parameter which represents the status vector.
   * \param[in] n      parameter which represents the dimension of the system.
   * \param[in] y    parameter which contains the components of the residue or descent direction vector.
   * \param[in] fric   parameter which contains the friction coefficient.
   * \param[out] projf1 parameter which contains the projected residue or descent direction.
   *
   */
  void fc2d_projf(int etat[], int *n, double y[], double fric[], double projf1[]);

  /** */
  void fc2d_sparse_nsgs(FrictionContactProblem* problem, double *z, double *w, int *info, SolverOptions* options) ;

  /** set the default solver parameters and perform memory allocation for NSGS
  \param options the pointer to the options to set
  */
  int fc2d_sparse_nsgs_setDefaultSolverOptions(SolverOptions* options);



  /** fc2d_lexicolemke is a Lemke solver for  frictionContact2D problems.\n
     * \param[in] problem structure that represents the fc2d (M, q...)
     * \param[in,out] reaction a n-vector of doubles which contains the initial solution and returns the solution of the problem.
     * \param[in,out] velocity a n-vector of doubles which returns the solution of the problem.
     * \param options
     * \param global_options
     * \param[out] info an integer which returns the termination value:\n
     0 = convergence,\n
     1 = no convergence,\n
     2 = Null diagonal term\n
     \author V. Acary
    */
  void fc2d_lexicolemke(FrictionContactProblem* problem, double *reaction, double *velocity, int *info, SolverOptions* options,  NumericsOptions* global_options);


  /** set the default solver parameters and perform memory allocation for Lemke
      \param options the pointer to options to set
  */
  int fc2d_lexicolemke_setDefaultSolverOptions(SolverOptions* options);


  /** This function transform a FrictionContactProblem (2D) into a LinearComplementarityProblem
   * \param[in] problem A pointer to a FrictionContactProblem to transform
   * \param[out] lcp_problem A pointer to a LinearComplementarity_problem resulting from the reformulation
   * \author Vincent Acary
   */

  int fc2d_tolcp(FrictionContactProblem* problem, LinearComplementarityProblem * lcp_problem);

  /** set the default solver parameters and perform memory allocation for enum solver
      \param options the pointer to options to set
  */
  int fc2d_enum_setDefaultSolverOptions(SolverOptions* options);

  /** fc2d_enum solver for  frictionContact2D problems.\n
     * \param[in] problem structure that represents the fc2d (M, q...)
     * \param[in,out] reaction a n-vector of doubles which contains the initial solution and returns the solution of the problem.
     * \param[in,out] velocity a n-vector of doubles which returns the solution of the problem.
     * \param options
     * \param global_options
     * \param[out] info an integer which returns the termination value:\n
     0 = convergence,\n
     1 = no convergence,\n
     2 = Null diagonal term\n
     \author V. Acary
    */
  void fc2d_enum(FrictionContactProblem* problem, double *reaction, double *velocity, int *info, SolverOptions* options,  NumericsOptions* global_options);



#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
}
#endif

#endif
