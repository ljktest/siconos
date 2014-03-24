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
/*! \file LCP.hpp
  \brief Linear Complementarity Problem formulation and solving
*/

#ifndef LCP_H
#define LCP_H

#include "LinearOSNS.hpp"
TYPEDEF_SPTR(LinearComplementarityProblem)

/** Formalization and Resolution of a Linear Complementarity Problem (LCP)
 
   \author SICONOS Development Team - copyright INRIA
   \version 3.0.0.
   \date (Creation) Apr 26, 2004
 
  \section LCPintro Aim of the LCP class
 
  This class is devoted to the formalization and the resolution of the
  Linear Complementarity Problem (LCP) defined by :
    \f[
  w =  q + M z
  \f]
  \f[
  w \geq 0, z \geq 0,  z^{T} w =0
  \f]
  where
     - \f$ w \in R^{n} \f$  and \f$z \in R^{n} \f$ are the unknowns,
     - \f$ M \in R^{n \times n } \f$  and \f$q \in R^{n} \f$
 
   The LCP main components are:
   - a problem (variables M,q and size of the problem), which directly corresponds to the LinearComplementarityProblem structure of Numerics
   - the unknowns z and w

 */
class LCP : public LinearOSNS
{
protected:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(LCP);


  /** Numerics problem to solve */
  SP::LinearComplementarityProblem _numerics_problem;

public:

  /** Constructor with Numerics solver id (default = Lemke)
      \param int id of numerics solver
  */
  LCP(int numericsSolverId = SICONOS_LCP_LEMKE);

  /** destructor
   */
  ~LCP();

  /** Compute the unknown z and w and update the Interaction (y and lambda )
   *  \param double : current time
   *  \return int, information about the solver convergence.
   */
  int compute(double);

  /** initialize
   */
  void initialize(SP::Simulation sim);

  /** print the data to the screen
   */
  void display() const;

  /** visitors hook
  */
  ACCEPT_STD_VISITORS();


};

#endif // LCP_H
