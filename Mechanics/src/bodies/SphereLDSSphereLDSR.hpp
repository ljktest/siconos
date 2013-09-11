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
 * Foundation, Inc., 51 Franklin St, Fifth FLOOR, Boston, MA  02110-1301  USA
 *
 * Contact: Vincent ACARY, siconos-team@lists.gforge.inria.fr
 *
 */

/*! \file SphereLDSSphereLDSR.hpp
  \brief Two spheres relation - Inherits from LagrangianScleronomousR
*/

#ifndef SphereLDSSphereLDSR_h
#define SphereLDSSphereLDSR_h

#include "LagrangianScleronomousR.hpp"

class SphereLDSSphereLDSR : public LagrangianScleronomousR, public std11::enable_shared_from_this<SphereLDSSphereLDSR>
{
private:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(SphereLDSSphereLDSR);

  double r1, r2, r1pr2;

  SphereLDSSphereLDSR() {};

public:

  /** Constructor

  \param disk1 radius
  \param disk2 radius
  */
  SphereLDSSphereLDSR(double, double);

  double distance(double, double, double, double, double, double, double, double);

  void computeh(const double time, Interaction& inter);

  void computeJachq(const double time, Interaction& inter);

  /** visitors hook
   */
  ACCEPT_VISITORS();

};
#endif /* SphereLDSSphereLDSR_h */
