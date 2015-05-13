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

/*! \file SphereLDS.hpp
  \brief Definition of a 3D sphere as a LagrangianDS (with Euler
         Angles)
*/


#ifndef SphereLDS_h
#define SphereLDS_h

#include "MechanicsFwd.hpp"
#include "LagrangianDS.hpp"

class SphereLDS : public LagrangianDS, public std11::enable_shared_from_this<SphereLDS>
{
protected:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(SphereLDS);

  double radius;
  double massValue;
  double I;

  SphereLDS() {};

public:

  SphereLDS(double, double, SP::SiconosVector, SP::SiconosVector);

  ~SphereLDS();

  inline double getQ(unsigned int pos)
  {
    assert(pos < _ndof);
    return (*_q[0])(pos);
  };
  inline double getVelocity(unsigned int pos)
  {
    assert(pos < _ndof);
    return (*_q[1])(pos);
  };

  inline double getMassValue() const
  {
    return massValue;
  };

  inline double getRadius() const
  {
    return radius;
  };

  void computeMass();
  void computeMass(SP::SiconosVector)
  {
    RuntimeException::selfThrow("SphereLDS::computeMass(vector) - not implemented");
  }

  void computeFGyr(SP::SiconosVector, SP::SiconosVector);

  void computeFGyr();

  void computeJacobianFGyrq();
  void computeJacobianFGyrqDot();

  void computeJacobianFGyrq(SP::SiconosVector, SP::SiconosVector);
  void computeJacobianFGyrqDot(SP::SiconosVector, SP::SiconosVector);


  /** visitors hook
   */
  ACCEPT_BASE_VISITORS(LagrangianDS);

};
#endif /* SphereLDS_h */
