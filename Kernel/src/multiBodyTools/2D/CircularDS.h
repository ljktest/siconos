/* Siconos-Kernel version 3.0.0, Copyright INRIA 2005-2008.
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
 * Contact: Vincent ACARY vincent.acary@inrialpes.fr
*/

/*! \file CircularDS
  \brief Definition of a 2D circular shape - Inherits from LagrangianDS
*/


#ifndef CircularDS_h
#define CircularDS_h

#include "LagrangianDS.h"

class CircularDS : public LagrangianDS
{
protected:
  double radius;
  double massValue;

public:

  CircularDS(double, double, const SiconosVector&, const SiconosVector&);

  ~CircularDS();

  inline double getQ(unsigned int pos)
  {
    return (*q[0])(pos);
  };
  inline double getVelocity(unsigned int pos)
  {
    return (*q[1])(pos);
  };

  inline double getMassValue()
  {
    return massValue;
  };

  inline double getRadius()
  {
    return radius;
  };

};

TYPEDEF_SPTR(CircularDS);

#endif /* CircularDS_h */
