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
 * You should have received a copy of the GNU General Public License
 * along with Siconos; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Contact: Vincent ACARY, siconos-team@lists.gforge.inria.fr
 */
#ifndef FRICTIONCONTACTPROBLEM_H
#define FRICTIONCONTACTPROBLEM_H

/*! \page fcProblem Friction-contact problems (2D or 3D)
 *
 * \section fcIntro Problem statement
 *  Given
 * <ul>
 *   <li> a symmetric positive semi--definite  matrix \f${M} \in {{\mathrm{I\!R}}}^{n \times n} \f$ </li>
 *   <li> a vector \f$ {q} \in {{\mathrm{I\!R}}}^n\f$</li>
 *   <li> a vector of coefficients of friction \f$\mu \in{{\mathrm{I\!R}}}^{n_c}\f$</li>
 *</ul>
 * the (reduced or dual) frictional contact problem  is to find two vectors \f$u\in{{\mathrm{I\!R}}}^n\f$,
 * the relative local velocity and \f$r\in {{\mathrm{I\!R}}}^n\f$,
 * the contact forces denoted by \f$\mathrm{FC}(M,q,\mu)\f$  such that
 * \f{eqnarray*}{
 * \begin{cases}
 *   u = M r + q \\
 *    \hat u = u +\left[
 *      \left[\begin{array}{c}
 *          \mu^\alpha \|u^\alpha_{T}\|\\
 *         0 \\
 *         0
 *        \end{array}\right]^T, \alpha = 1 \ldots n_c
 *    \right]^T \\ \                                \
 *    C^\star_{\mu} \ni {\hat u} \perp r \in C_{\mu}
 * \end{cases}
 * \f}
 * and the set \f$C^{\alpha,\star}_{\mu^\alpha}\f$ is its dual.
 *
 * The modified local velocity \f$\widehat u \f$ is not considered as an unknown since it can obtained uniquely
 * from the local velocity \f$u\f$.
 * Coulomb's friction law with Signorini's condition for the unilateral contact written in terms
 * of second order complementarity condition
 * \f{eqnarray}{
 *    C^\star_{\mu} \ni {\hat u} \perp r \in C_{\mu}
 * \f}
 * can be interpreted in a more usual form
 *
 * \f{eqnarray}{
 * \begin{cases}
 *  0 \leq u_{N} \perp r_N \geq 0  \quad\quad\text{ Signorini condition}\\
 *  u_T = 0 \Rightarrow \|r_T\| \leq \mu |r_n|  \quad\quad\text{ Sticking mode} \\
 *  u_T \neq 0 \Rightarrow r_T = - \mu |r_n| \frac{u_T }{\|u_T\|}  \quad\quad\text{ Sliding mode}
 * \end{cases}
 * \f}
 *
 * This problem models any instance of discretized frictional contact problem obtained from
 * <ul>
 * <li>the time-discretization  of dynamics contact problems with event-capturing of event-tracking schemes, </li>
 * <li>the time-discretization  of quasi-static contact problems, </li>
 * <li>the modeling  of static contact problems. In this last case, \f$u\f$ plays the role of the relative displacement at contact  </li>
 * </ul>
 *
 * The problem is stored and given to the solver in Siconos/Numerics thanks to
 *  a C structure FrictionContactProblem .
 *
 *  \section fc3DSolversList Available solvers for Friction Contact 3D (see Friction_cst.h)
 * Use the generic function fc3d_driver() to call one the the specific solvers listed below:
 *
 * <ul>
 *
 * <li> fc3d_nsgs() : non-smooth Gauss-Seidel solver.
 *       SolverId : SICONOS_FRICTION_3D_NSGS =500, </li>
 *
 * <li> fc3d_nsgs_velocity() : non-smooth Gauss-Seidel solver  based on velocity updates
 *       SolverId : SICONOS_FRICTION_3D_NSGSV =501, </li>
 *
 * <li> fc3d_proximal() : Proximal point solver for friction-contact 3D problem
 *       SolverId : SICONOS_FRICTION_3D_PROX =502,</li>
 *
 * <li> fc3d_TrescaFixedPoint() : Fixed point solver for friction-contact
 *      3D problem based on the Tresca problem with fixed friction threshold
 *       SolverId : SICONOS_FRICTION_3D_TFP =503,</li>
 *
 * <li> fc3d_globalAlartCurnier() : Global Alart--Curnier solver
 *       SolverId : SICONOS_FRICTION_3D_NSN_AC =504,</li>
 *
 * <li> fc3d_DeSaxceFixedPoint() : Fixed Point solver for friction-contact 3D problem
 *      based on the De Saxce Formulation
 *        SolverId : SICONOS_FRICTION_3D_DSFP=505, </li>
 *
 * <li> fc3d_ExtraGradient() : Extra Gradient solver for friction-contact 3D problem
 *        based on the De Saxce Formulation</li>
 *        SolverId : SICONOS_FRICTION_3D_EG=506, </li>
 *
 * <li> fc3d_HyperplaneProjection() : Hyperplane Projection solver for friction-contact 3D
 *         problem based on the De Saxce Formulation
 *        SolverId : SICONOS_FRICTION_3D_HP=507, </li>
 *
 * </ul>
 * (see the functions/solvers list in fc3d_Solvers.h)
 *
 * \section fc3DParam Required and optional parameters
 * fc3d problems needs some specific parameters, given to the fc3d_driver()
 * function thanks to a SolverOptions structure. \n
 *
 * \section fc2DSolversList Available solvers for Friction Contact 2D
 * <ul>
 * <li> fc2d_nsgs(), Non Linear Gauss Seidel solver. SolverId SICONOS_FRICTION_2D_NSGS =400,
 * </li>
 * <li> fc2d_cpg(), conjugate projected gradient SolverId SICONOS_FRICTION_2D_CPG =401,
 * </li>
 * <li> fc2d_pgs(), projected Gauss Seidel solver. SolverId SICONOS_FRICTION_2D_PGS =402,
 * </li>
 * <li> fc2d_latin(), latin solver. SolverId SICONOS_FRICTION_2D_LATIN =403, </li>
 * </li>
 * <li> fc2d_lexicolemke(), lemke solver. SolverId SICONOS_FRICTION_2D_LMEKE =404, </li>
 * </ul>
 *
 */


/*!\file FrictionContactProblem.h
  \brief Definition of a structure to handle with friction-contact (2D or 3D) problems.
*/

#include "NumericsMatrix.h"

/** \struct FrictionContactProblem FrictionContactProblem.h
 *  The structure that defines a (reduced or dual) Friction-Contact (3D or 2D) problem
 *  \f$\mathrm{FC}(M,q,\mu)\f$  such that
 * \f{eqnarray*}{
 * \begin{cases}
 *   u = M r + q \\
 *    \hat u = u +\left[
 *      \left[\begin{array}{c}
 *          \mu^\alpha \|u^\alpha_{T}\|\\
 *         0 \\
 *         0
 *        \end{array}\right]^T, \alpha = 1 \ldots n_c
 *    \right]^T \\ \                                \
 *    C^\star_{\mu} \ni {\hat u} \perp r \in C_{\mu}
 * \end{cases}
 * \f}
 *   \param dimension dimension \f$d=2\f$ or \f$d=3\f$ of the contact space (3D or 2D )
 *   \param numberOfContacts the number of contacts \f$ n_c \f$
 *   \param M \f${M} \in {{\mathrm{I\!R}}}^{n \times n} \f$,
 *    a matrix with \f$ n = d  n_c\f$ stored in NumericsMatrix structure
 *   \param q  \f${q} \in {{\mathrm{I\!R}}}^{n} \f$,
 *   \param mu \f${\mu} \in {{\mathrm{I\!R}}}^{n_c} \f$, vector of friction coefficients
 *      (\f$ n_c =\f$ numberOfContacts)
*/
typedef struct
{
  /** dimension \f$d=2\f$ or \f$d=3\f$ of the contact space (3D or 2D ) */
  int dimension;
  /** the number of contacts \f$ n_c \f$ */
  int numberOfContacts;
  /** M \f${M} \in {{\mathrm{I\!R}}}^{n \times n} \f$,
     a matrix with \f$ n = d  n_c\f$ stored in NumericsMatrix structure */
  NumericsMatrix* M;
  /** \f${q} \in {{\mathrm{I\!R}}}^{n} \f$ */
  double* q;
  /** mu \f${\mu} \in {{\mathrm{I\!R}}}^{n_c} \f$, vector of friction coefficients
      (\f$ n_c =\f$ numberOfContacts) */
  double* mu;
} FrictionContactProblem;


#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
extern "C"
{
#endif
  /** display a FrictionContactProblem
   * \param problem the problem to display
   */
  void frictionContact_display(FrictionContactProblem*  problem);

  /** print a FrictionContactProblem in a file (numerics .dat format)
   * \param problem the problem to print out
   * \param file the dest file
   * \return 0 if successfull
   */
  int frictionContact_printInFile(FrictionContactProblem*  problem, FILE* file);

  /** print a FrictionContactProblem in a file (numerics .dat format) from its filename
   * \param problem the problem to print out
   * \param filename the dest file
   * \return 0 if successfull
   */
  int frictionContact_printInFilename(FrictionContactProblem*  problem, char * filename);

  /** read a FrictionContactProblem in a file (numerics .dat format)
   * \param problem the problem to read
   * \param file the target file
   * \return 0 if successfull
   */
  int frictionContact_newFromFile(FrictionContactProblem*  problem, FILE* file);
  
  /** read a FrictionContactProblem in a file (numerics .dat format) from its filename
   * \param problem the problem to read
   * \param filename the name of the target file
   * \return 0 if successfull
   */
  int frictionContact_newFromFilename(FrictionContactProblem*  problem, char * filename);

  /** free a FrictionContactProblem
   * \param problem the problem to free
   */
  void freeFrictionContactProblem(FrictionContactProblem* problem);


  /** new FrictionContactProblem from minimal set of data
   * \param[in] dim the problem dimension
   * \param[in] nc the number of contact
   * \param[in] M the NumericsMatrix
   * \param[in] q the q vector
   * \param[in] mu the mu vector
   * \return a pointer to a FrictionContactProblem structure
   */
  FrictionContactProblem* frictionContactProblem_new(int dim, int nc,
      NumericsMatrix* M, double* q, double* mu);



#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
}
#endif

#endif

