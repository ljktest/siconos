/* Siconos-Kernel, Copyright INRIA 2005-2012.
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
/*! \file
  Fricton-Contact Non-Smooth Problem
*/
#ifndef GENERICMECHANICAL_H
#define GENERICMECHANICAL_H

#include "LinearOSNS.hpp"
#include "SiconosNumerics.h"
/** Pointer to function of the type used for drivers for GenericMechanical problems in Numerics */

TYPEDEF_SPTR(GenericMechanicalProblem)

/** Formalization and Resolution of a generic mechanical problem: It mixes bilateral equality, complementarity, impact and friction problems.
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) June 23 2010
 *
 * This class is devoted to contains of a set of Non-Smooth Problem.
 *
 * \b Main functions:
 *  - formalization of the problem: computes M,q using the set of "active" Interactions from the simulation and \n
 *  the interactionBlock-matrices saved in the field interactionBlocks.\n
 *  Functions: initialize(), computeInteractionBlock(), preCompute()
 *  - solving of the GenericMechanical problem: function compute(), used to call solvers from Numerics through \n
 * the genericMechanical_driver() interface of Numerics.
 *  - post-treatment of data: set values of y/lambda variables of the active Interaction (ie Interactions) using \n
 *  ouput results from the solver (velocity,reaction); function postCompute().
 *
 */
class GenericMechanical : public LinearOSNS
{
protected:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(GenericMechanical);


  GenericMechanicalProblem * _pnumerics_GMP;

public:

  /** Basic constructor, from numerics solver id.
   *  \param FC3D_Solver_Id int type of FC3D solver
   */
  GenericMechanical(int FC3D_Solver_Id = SICONOS_FRICTION_3D_ONECONTACT_QUARTIC);

  /** destructor
   */
  ~GenericMechanical();

  // GETTERS/SETTERS


  // --- Others functions ---

  /** initialize the GenericMechanical problem(compute topology ...)
   *   \param sim the simulation, owner of this OSNSPB
   */
  void initialize(SP::Simulation sim);

  /** Compute the unknown reaction and velocity and update the Interaction (y and lambda )
   *  \param time double current time
   *  \return int information about the solver convergence (0: ok, >0 problem, see Numerics documentation)
   */
  int compute(double time);

  /** compute extra-diagonal interactionBlock-matrix
    *  \param ed an edge descriptor
    */
  virtual void computeInteractionBlock(const InteractionsGraph::EDescriptor& ed);

  /** compute diagonal Interaction block
   * \param vd  a vertex descriptor
   */
  virtual void computeDiagonalInteractionBlock(const InteractionsGraph::VDescriptor& vd);

  /** print the data to the screen */
  void display() const;
  
  /** compute interactionBlocks if necessary (this depends on the type of
      OSNS, on the indexSets ...)
   */
  void updateInteractionBlocks();

  /** visitors hook
   */
  ACCEPT_STD_VISITORS();



};

#endif // GenericMechanical_H
