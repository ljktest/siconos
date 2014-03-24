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

/*! \file NonSmoothLaw.hpp
	\brief Base (abstract) class for a nonsmooth law
*/

#ifndef NSLAW_H
#define NSLAW_H

#include "SiconosConst.hpp"
#include "RuntimeException.hpp"

#include "SiconosPointers.hpp"

#include "SiconosFwd.hpp"

#include "SiconosVisitor.hpp"

/** Non Smooth Laws (NSL) Base Class
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) May 05, 2004
 *
 * This class is the base class for all nonsmooth laws in Siconos.
 * A nonsmooth law characterize the (nonsmooth) relationship between 2 variables,
 * usually designated by \f$y\f$ and \f$\lambda\f$. \f$y\f$ is most of time seen as the
 * "input" from DynamicalSystems and is given by a Relation linked to this nonsmoothlaw.
 * \f$\lambda\f$ is then the "output" and through the same Relation feed back to one or more
 * DynamicalSystem.
 *
 * classical examples of nonsmooth law include:
 * - RelayNSL: \f$y \in -\mathcal{N}_{[-1,1]}(\lambda)\quad \Longleftrightarrow \lambda \in -\mbox{sgn} (y)\f$
 * - ComplementarityConditionNSL: \f$0\leq y \perp \lambda \geq 0\f$
 * - NewtonImpactNSL and NewtonImpactFrictionNSL for impact, with possible friction
 * - MultipleImpactNSL for a multiple impact law
 * - MixedComplementarityConditionNSL
 *
 * The computation of both \f$y\f$ and \f$\lambda\f$ is carried on by a solver in Numerics
 * through a OneStepNSProblem object.
 */
class NonSmoothLaw
{
protected:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(NonSmoothLaw);


  /** "size" of the NonSmoothLaw */
  unsigned int _size;

  /** size of the NonSmoothLaw to project on the constraints. It is not equal to _size in the case of friction.*/
  unsigned int _sizeProjectOnConstraints;


  /** default constructor
   */
  NonSmoothLaw() {};

  /** copy constructor (private=> no copy nor pass-by value allowed)
   * \param notUsed notused
   */
  NonSmoothLaw(const NonSmoothLaw& notUsed) {};

public:
  /** basic constructor
  * \param size the nonsmooth law size
  */
  NonSmoothLaw(unsigned int size);

  /** destructor
  */
  virtual ~NonSmoothLaw();

  /** check if the NS law is verified
  *  \return a boolean value which determines if the NS Law is verified
  */
  virtual bool isVerified() const
  {
    RuntimeException::selfThrow("NonSmoothLaw::isVerified, not yet implemented!");
    return false;
  }

  /** to get the size
  *  \return the size of the NS law
  */
  inline unsigned int size() const
  {
    return _size;
  }
  /** to get the significant size for the projection on constraints
  *  \return the significant size for the projection on constraints
  */
  inline unsigned int sizeProjectOnConstraints() const
  {
    return _sizeProjectOnConstraints;
  }

  /** display the data of the NonSmoothLaw on the standard output
  *
  */
  virtual void display() const = 0;

  /** visitors hook
   */
  VIRTUAL_ACCEPT_VISITORS(NonSmoothLaw);

};

#endif
