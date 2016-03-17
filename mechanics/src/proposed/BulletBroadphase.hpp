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

/*! \file BulletBroadphase.hpp
  \brief Definition of a Bullet-based broadphase algorithm.
*/

#ifndef BulletBroadphase_h
#define BulletBroadphase_h

#include <MechanicsFwd.hpp>

#include <SiconosBroadphase.hpp>
#include <SiconosShape.hpp>
#include <Contactor.hpp>

#include <map>

DEFINE_SPTR(BulletBroadphase_impl);

class BulletBroadphase : public SiconosBroadphase, public std11::enable_shared_from_this<BulletBroadphase>
{
protected:
  SP::BulletBroadphase_impl impl;

public:
  BulletBroadphase();
  ~BulletBroadphase();

protected:
  virtual void visit(SP::SiconosPlane plane);
  virtual void visit(SP::SiconosSphere sphere);
  virtual void visit(SP::SiconosBox box);
  virtual void visit(SP::Contactor contactor);

  virtual void update(SP::SiconosPlane plane);
  virtual void update(SP::SiconosSphere sphere);
  virtual void update(SP::SiconosBox box);

public:
  virtual void buildGraph(SP::Contactor contactor);
  virtual void updateGraph();
  virtual void performBroadphase();
};

#endif /* BulletBroadphase.hpp */
