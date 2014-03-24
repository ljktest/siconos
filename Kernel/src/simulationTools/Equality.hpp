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
/*! \file Equality.hpp
  \brief Linear Complementarity Problem formulation and solving
*/

#ifndef Equality_H
#define Equality_H

#include "LinearOSNS.hpp"

/** Formalization and Resolution of a Linear Complementarity Problem (Equality)
 
   \author SICONOS Development Team - copyright INRIA
   \version 3.0.0.
   \date (Creation) Apr 26, 2004
 
  \section Equalityintro Aim of the Equality class
 
  This class is devoted to the formalization and the resolution of the
  Linear system (Equality) defined by :
   \f[
  0 = w =  q + M z
  \f]
  where
     - \f$ w \in R^{n} \f$  and \f$z \in R^{n} \f$ are the unknowns,
     - \f$ M \in R^{n \times n } \f$  and \f$q \in R^{n} \f$
 
   The Equality main components are:
   - a problem (variables M,q and size of the problem), which directly corresponds to the LinearComplementarityProblem structure of Numerics
   - the unknowns z and w
 
*/
class Equality : public LinearOSNS
{
private:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(Equality);

  LinearSystemProblem _numerics_problem;
public:

  /** constructor
      \param int id of numerics solver
  */
  Equality(int numericsSolverId = 0): LinearOSNS(numericsSolverId) {};
  
  /** destructor
   */
  ~Equality() {};

  /** initialize
   */
  void initialize(SP::Simulation sim);

  /** Compute the unknown z and w and update the Interaction (y and lambda )
   *  \param double : current time
   *  \return int, information about the solver convergence.
   */
  int compute(double);

  /** Build or reinit M and the NumericsProblem*/
  virtual void updateM();


  /** print the data to the screen
   */
  void display() const;
};

#endif // Equality_H
