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

/*! \file BulletBroadphase.cpp
  \brief Implementation of a Bullet-based broadphase algorithm.
*/

#include "BulletBroadphase.hpp"

#include <map>
#include <stdio.h>

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>

#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>

DEFINE_SPTR(btDefaultCollisionConfiguration);
DEFINE_SPTR(btCollisionDispatcher);
DEFINE_SPTR(btBroadphaseInterface);

DEFINE_SPTR(btCollisionWorld);
DEFINE_SPTR(btCollisionObject);
DEFINE_SPTR(btCollisionShape);
DEFINE_SPTR(btStaticPlaneShape);
DEFINE_SPTR(btSphereShape);
DEFINE_SPTR(btBoxShape);

class BulletBroadphase_impl : public SiconosShapeHandler
{
protected:
  SP::btCollisionWorld _collisionWorld;
  SP::btDefaultCollisionConfiguration _collisionConfiguration;
  SP::btCollisionDispatcher _dispatcher;
  SP::btBroadphaseInterface _broadphase;

  SP::Contactor currentContactor;

  std::vector<SP::SiconosPlane> dirtyPlanes;
  std::vector<SP::SiconosSphere> dirtySpheres;
  std::vector<SP::SiconosBox> dirtyBoxes;

  std::map<SP::SiconosShape, SP::btCollisionObject> objectMap;
  std::map<SP::SiconosPlane, SP::btStaticPlaneShape> planeMap;
  std::map<SP::SiconosSphere, SP::btSphereShape> sphereMap;

public:
  BulletBroadphase_impl() {}
  ~BulletBroadphase_impl() {}

  virtual void onChanged(SP::SiconosPlane plane);
  virtual void onChanged(SP::SiconosSphere sphere);
  virtual void onChanged(SP::SiconosBox box);

  friend class BulletBroadphase;
};

void BulletBroadphase_impl::onChanged(SP::SiconosPlane plane)
{
  dirtyPlanes.push_back(plane);
  printf("pushed plane: %p(%ld)\n",
         &*plane,plane.use_count());
}

void BulletBroadphase_impl::onChanged(SP::SiconosSphere sphere)
{
  dirtySpheres.push_back(sphere);
  printf("pushed sphere: %p(%ld)\n",
         &*sphere,sphere.use_count());
}

void BulletBroadphase_impl::onChanged(SP::SiconosBox box)
{
    dirtyBoxes.push_back(box);
}

BulletBroadphase::BulletBroadphase() {
  impl.reset(new BulletBroadphase_impl());
  impl->_collisionConfiguration.reset(
    new btDefaultCollisionConfiguration());
  impl->_dispatcher.reset(
    new btCollisionDispatcher(&*impl->_collisionConfiguration));
  impl->_broadphase.reset(new btDbvtBroadphase());
  impl->_collisionWorld.reset(
    new btCollisionWorld(&*impl->_dispatcher, &*impl->_broadphase,
                         &*impl->_collisionConfiguration));
}

BulletBroadphase::~BulletBroadphase() {
  // must be the first de-allocated, otherwise segfault
  impl->_collisionWorld.reset();
}

void BulletBroadphase::visit(SP::SiconosSphere sphere)
{
  printf("contactor: %p, ", impl->currentContactor);
  printf("sphere: %p(%ld)\n",
         &*sphere,sphere.use_count());

  // create corresponding Bullet object and shape
  SP::btCollisionObject btobject(new btCollisionObject());

  // track association (and to keep a reference)
  impl->objectMap[sphere] = SP::btCollisionObject(btobject);

  // set radius to 1.0 and use scaling instead of setting radius
  // directly, makes it easier to change during update
  SP::btSphereShape btsphere(new btSphereShape(1.0));

  // associate the shape with the object
  btobject->setCollisionShape(&*btsphere);

  // track association (and to keep a reference)
  impl->sphereMap[sphere] = btsphere;

  // initial update of the sphere properties
  update(sphere);

  // put it in the world
  impl->_collisionWorld->addCollisionObject(&*btobject);

  // install handler for updates
  sphere->setHandler(impl);
}

void BulletBroadphase::update(SP::SiconosSphere sphere)
{
  printf("updating sphere: %p(%ld)\n",
         &*sphere,sphere.use_count());

  SP::btSphereShape btsphere(impl->sphereMap[sphere]);

  // TODO ASSERT btsphere!=null

  btsphere->setLocalScaling(btVector3(sphere->radius(),
                                      sphere->radius(),
                                      sphere->radius()));
}

void BulletBroadphase::visit(SP::SiconosPlane plane)
{
  printf("contactor: %p, ", impl->currentContactor);
  printf("plane: %p(%ld)\n",
         &*plane,plane.use_count());

  // create corresponding Bullet object and shape
  SP::btCollisionObject btobject(new btCollisionObject());

  // track association (and to keep a reference)
  impl->objectMap[plane] = SP::btCollisionObject(btobject);

  // create the initial plane with default parameters
  SP::btStaticPlaneShape btplane(
    new btStaticPlaneShape(btVector3(0, 0, 1), 1.0));

  // associate the shape with the object
  btobject->setCollisionShape(&*btplane);

  // track association (and to keep a reference)
  impl->planeMap[plane] = btplane;

  // initial update of the plane properties
  update(plane);

  // put it in the world
  impl->_collisionWorld->addCollisionObject(&*btobject);

  // install handler for updates
  plane->setHandler(impl);
}

void BulletBroadphase::update(SP::SiconosPlane plane)
{
  printf("updating plane: %p(%ld)\n",
         &*plane,plane.use_count());

  SP::btCollisionObject btobject(impl->objectMap[plane]);

  // TODO ASSERT btobject!=null

  // TODO: orientation
  btTransform tr;
  tr.setIdentity();
  tr.setOrigin(btVector3((*plane->position())(0),
                         (*plane->position())(1),
                         (*plane->position())(2)));
  btobject->setWorldTransform(tr);
}

void BulletBroadphase::visit(SP::SiconosBox box)
{
}

void BulletBroadphase::update(SP::SiconosBox box)
{
}

void BulletBroadphase::visit(SP::Contactor contactor)
{
  impl->currentContactor = contactor;

  std::vector<SP::SiconosShape>::const_iterator it;
  for (it=contactor->shapes().begin();
       it!=contactor->shapes().end();
       it++)
  {
    (*it)->acceptSP(shared_from_this());
  }
}

void BulletBroadphase::buildGraph(SP::Contactor contactor)
{
  contactor->acceptSP(shared_from_this());
}

void BulletBroadphase::updateGraph()
{
  printf("dirty planes:\n");
  {
    std::vector<SP::SiconosPlane>::iterator it;
    for (it=impl->dirtyPlanes.begin();
         it!=impl->dirtyPlanes.end(); it++)
    {
      printf("  %p\n", &**it);
      update(*it);
    }
    impl->dirtyPlanes.clear();
  }

  printf("dirty spheres:\n");
  {
    std::vector<SP::SiconosSphere>::iterator it;
    for (it=impl->dirtySpheres.begin();
         it!=impl->dirtySpheres.end(); it++)
    {
      printf("  %p\n", &**it);
      update(*it);
    }
    impl->dirtySpheres.clear();
  }

  printf("dirty boxes:\n");
  {
    std::vector<SP::SiconosBox>::iterator it;
    for (it=impl->dirtyBoxes.begin();
         it!=impl->dirtyBoxes.end(); it++)
    {
      printf("  %p\n", &**it);
      update(*it);
    }
    impl->dirtyBoxes.clear();
  }
}

void BulletBroadphase::performBroadphase()
{
  impl->_collisionWorld->performDiscreteCollisionDetection();
}
