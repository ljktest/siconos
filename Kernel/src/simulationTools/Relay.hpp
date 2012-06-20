/* Siconos-Kernel, Copyright INRIA 2005-2011.
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
/*! \file Relay.hpp
  \brief Linear Complementarity Problem formulation and solving
*/

#ifndef RELAY_H
#define RELAY_H

#include "LinearOSNS.hpp"
TYPEDEF_SPTR(RelayProblem);
class LinearOSNS;
class Simulation;
/** Formalization and Resolution of a Linear Complementarity Problem (Relay)
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) Apr 26, 2004
 *
 * \section Relayintro Aim of the Relay class
 *
 * This class is devoted to the formalization and the resolution of the
 * Linear Complementarity Problem (Relay) defined by :
 *  * \f[
 * w =  q + M z
 * \f]
 * \f[
 * w \geq 0, z \geq 0,  z^{T} w =0
 * \f]
 * where
 *    - \f$ w \in R^{n} \f$  and \f$z \in R^{n} \f$ are the unknowns,
 *    - \f$ M \in R^{n \times n } \f$  and \f$q \in R^{n} \f$
 *
 *  The Relay main components are:
 *  - a problem (variables M,q and size of the problem), which directly corresponds to the LinearComplementarityProblem structure of Numerics
 *  - the unknowns z and w
 *  - a NonSmoothSolver, used to define a solver and its parameters (connected to SolverOptions structure of Numerics)
 *
 *  A Relay is connected to a simulation that handles a NonSmoothDynamicalSystem and its Topology. \n
 *  IndexSets from simulation are used to know which constraints (Interaction) are active or not. \n
 *
 * \b Construction:
 *   - XML reading (inputs = xml node with tag "OneStepNSProblem" and a SP::Simulation)
 *   - Constructor from data (inputs = SP::Simulation, id, SP::NonSmoothSolver) - The solver is optional.
 * Main functions:
 *
 * \b Main functions:
 *  - formalization of the problem: computes M,q using the set of "active" Interactions from the simulation and \n
 *  the interactionBlock-matrices saved in the field interactionBlocks.\n
 *  Functions: initialize(), computeInteractionBlock(), preCompute()
 *  - solving of the FrictionContact problem: function compute(), used to call solvers from Numerics through \n
 * the lcp_driver() interface of Numerics.
 *  - post-treatment of data: set values of y/lambda variables of the active Interaction (ie Interactions) using \n
 *  ouput results from the solver (z,w); function postCompute().
 *
 *
 * \todo : add "recover" function to start from old values of z and w.
 */
class Relay : public LinearOSNS
{

protected:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(Relay);


  /** contains the vector lb (lower bounds) of a Relay system */
  SP::SiconosVector _lb;

  /** contains the vector ub (upper bounds) of a Relay system */
  SP::SiconosVector _ub;

  /** contains the numerics proble for Relay system */
  SP::RelayProblem _numerics_problem;

  /** nslaw effects : visitors experimentation
   */

  struct _BoundsNSLEffect;
  friend struct _BoundsNSLEffect;



public:

  /** xml constructor
   *  \param SP::OneStepNSProblemXML : the XML linked-object
   */
  Relay(SP::OneStepNSProblemXML onestepnspbxml):
    LinearOSNS(onestepnspbxml, "Relay") {};

  /** constructor from data
   *  \param Solver* pointer to object that contains solver algorithm and formulation \n
   *  (optional, default = NULL => read .opt file in Numerics)
   *  \param String: id of the problem (default = "unamed")
   */
  Relay(const int newNumericsSolverId = SICONOS_RELAY_LEMKE, const std::string& newId = "unamed_relay");

  /** destructor
   */
  ~Relay();
  // --- lb ---
  /** get the value of lb, the   lower bounds of the Relay system
   *  \return SiconosVector
   *  \warning: SiconosVector is an abstract class => can not be an
   *  lvalue => return SiconosVector
   */
  inline const SiconosVector getLb() const
  {
    return *_lb;
  }

  /** get lb, the lower bounds of the Relay system
   *  \return pointer on a SiconosVector
   */
  inline SP::SiconosVector lb() const
  {
    return _lb;
  }

  /** set lb to pointer newPtr
   *  \param SP::SiconosVector  newPtr
   */
  inline void setLb(SP::SiconosVector newPtr)
  {
    _lb = newPtr;
  }


  // --- ub ---
  /** get the value of ub, the  upper bounds of the Relay system
   *  \return SiconosVector
   *  \warning: SiconosVector is an abstract class => can not be an
   *  lvalue => return SiconosVector
   */
  inline const SiconosVector getUb() const
  {
    return *_ub;
  }

  /** get lb, the lower bounds of the Relay system
   *  \return pointer on a SiconosVector
   */
  inline SP::SiconosVector ub() const
  {
    return _ub;
  }

  /** set lb to pointer newPtr
   *  \param SP::SiconosVector  newPtr
   */
  inline void setUb(SP::SiconosVector newPtr)
  {
    _ub = newPtr;
  }

  void initialize(SP::Simulation sim) ;

  /** Compute the unknown z and w and update the Interaction (y and lambda )
   *  \param double : current time
   *  \return int, information about the solver convergence.
   */
  int compute(double);

  /** print the data to the screen
   */
  void display() const;

  /** encapsulates an operation of dynamic casting. Needed by Python interface.
   *  \param OneStepNSProblem* : the one step problem which must be converted
   * \return a pointer on the problem if it is of the right type, NULL otherwise
   */
  static Relay* convert(OneStepNSProblem* osnsp);

};

TYPEDEF_SPTR(Relay);

#endif // RELAY_H
