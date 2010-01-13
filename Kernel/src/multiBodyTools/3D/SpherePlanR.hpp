/* Siconos-Example version 3.0.0, Copyright INRIA 2005-2010.
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

/*! \file SpherePlanR.h
  \brief Sphere relation with a plan - Inherits from LagrangianScleronomousR
*/

#ifndef SpherePlanR_h
#define SpherePlanR_h

#include "CircularR.hpp"

class SpherePlanR : public LagrangianScleronomousR, public boost::enable_shared_from_this<SpherePlanR>
{
private:

  /* Ax + By + Cz + D = 0 */
  double r, A, B, C, D, nN, nU;
  /* u ^ v  = n */
  double u1, u2, u3, v1, v2, v3, n1, n2, n3, ru1, ru2, ru3, rv1, rv2, rv3;

  SpherePlanR();

public:

  /** Constructor

  \param disk radius
  \param A
  \param B
  \param C
  */
  SpherePlanR(double, double, double, double, double);

  double distance(double, double, double, double);

  void computeh(double);

  void computeJachq(double);

  bool equal(double _A, double _B, double _C, double _D, double _r)
  {
    return (A == _A && B == _B && C == _C && D == _D && r == _r) ;
  }

  /** visitors hook
   */
  ACCEPT_VISITORS();

};

TYPEDEF_SPTR(SpherePlanR);

#endif /* SpherePlanR_h */
