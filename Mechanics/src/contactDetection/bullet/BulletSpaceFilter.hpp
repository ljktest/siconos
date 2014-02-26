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

#ifndef BulletSpaceFilter_hpp
#define BulletSpaceFilter_hpp

#include "BulletSiconosFwd.hpp"
#include "SpaceFilter.hpp"
#include <vector>

class BulletSpaceFilter : public SpaceFilter
{

protected:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(BulletSpaceFilter);

  std11::shared_ptr<std::vector<SP::btCollisionObject> > _staticObjects;
  std11::shared_ptr<std::vector<SP::btCollisionShape> > _staticShapes;

  SP::btCollisionWorld _collisionWorld;
  SP::btDefaultCollisionConfiguration _collisionConfiguration;
  SP::btCollisionDispatcher _dispatcher;
  SP::btBroadphaseInterface _broadphase;
  bool _dynamicCollisionsObjectsInserted;
  bool _staticCollisionsObjectsInserted;

  double _closeContactsThreshold;

public:
  BulletSpaceFilter(SP::Model model, SP::NonSmoothLaw nslaw);

  /** get Bullet collision configuration
      \return a pointer on a Bullet collision configuration
  */
  SP::btDefaultCollisionConfiguration collisionConfiguration() const
  {
    return _collisionConfiguration;
  }

  /** get Bullet broadphase
      \return pointer on a BulletBroadPhase
  */
  SP::btBroadphaseInterface broadphase() const
  {
    return _broadphase;
  };

  /** get bullet collision world
      \return pointer on btCollisionWorld
  */
  SP::btCollisionWorld collisionWorld() const
  {
    return _collisionWorld;
  };

  /** get static objects
      \return a std::vector of btCollisionObject
  */
  std11::shared_ptr<std::vector<SP::btCollisionObject> >staticObjects() const
  {
    return _staticObjects;
  };

  /** get static shapes
      \return a std::vector of btCollisionShape
  */

  std11::shared_ptr<std::vector<SP::btCollisionShape> > staticShapes() const
  {
    return _staticShapes;
  };

  /** add a static object
   * \param co a btCollisionObject
   */
  void addStaticObject(SP::btCollisionObject co)
  {
    _staticObjects->push_back(co);
  };

  /** add a static shape
   * \param cs a btCollisionShape
   */
  void addStaticShape(SP::btCollisionShape cs)
  {
    _staticShapes->push_back(cs);
  }

  /** execute the broadphase contact detection and build indexSet0
   */
  void buildInteractions(double);


  /** set close contact parameter
   *  \param threshold double value that will be multiplicated by the
   *         radius of the object bouncing box
  */
  void setCloseContactFilterParam(double threshold)
  {
    _closeContactsThreshold = threshold;
  }

  ACCEPT_STD_VISITORS();

  /** set a new collision configuration
   * \param collisionConfig the new bullet collision configuration
   */
  void setCollisionConfiguration(
    SP::btDefaultCollisionConfiguration collisionConfig);


};


#include <Question.hpp>
#include "BulletR.hpp"
#include "BulletFrom1DLocalFrameR.hpp"

struct ForCollisionWorld : public Question<SP::btCollisionWorld>
{
  using SiconosVisitor::visit;

  ANSWER(BulletSpaceFilter, collisionWorld());
};

struct ForStaticObjects : public Question< std11::shared_ptr<std::vector<SP::btCollisionObject> > >
{
  using SiconosVisitor::visit;

  ANSWER(BulletSpaceFilter, staticObjects());
};

struct ForStaticShapes : public Question< std11::shared_ptr<std::vector<SP::btCollisionShape> > >
{
  using SiconosVisitor::visit;

  ANSWER(BulletSpaceFilter, staticShapes());
};


struct ForContactPoint : public Question<SP::btManifoldPoint>
{
  using SiconosVisitor::visit;

  void visit(const NewtonEulerR&)
  {
  }
  ANSWER(BulletR, contactPoint());
  ANSWER(BulletFrom1DLocalFrameR, contactPoint());
};

#endif
