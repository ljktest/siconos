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

#ifndef BulletR_hpp
#define BulletR_hpp

#include "BulletSiconos.hpp"
#include "NewtonEulerFrom3DLocalFrameR.hpp"

class BulletR : public NewtonEulerFrom3DLocalFrameR
{
private:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(BulletR);

  unsigned int _contact_num;

  SP::btPersistentManifold _contactManifold;

public:
  BulletR(unsigned int contact_num, SP::btPersistentManifold);
  
  SP::btPersistentManifold contactManifold() const
  {
    return _contactManifold;
  };

  unsigned int contactNum() const
  {
    return _contact_num;
  }

  void computeh(const double time, Interaction& inter);

  ACCEPT_STD_VISITORS();
};

TYPEDEF_SPTR(BulletR)

#endif
