/* Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 *
 * Copyright 2016 INRIA.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/


// #define DEBUG_STDOUT
// #define DEBUG_MESSAGES 1
#include <debug.h>

#include "BulletR.hpp"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#elif !(__INTEL_COMPILER || __APPLE__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#endif

#include <BulletCollision/NarrowPhaseCollision/btManifoldPoint.h>
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>

#include <btBulletCollisionCommon.h>

#if defined(__clang__)
#pragma clang diagnostic pop
#elif !(__INTEL_COMPILER || __APPLE__ )
#pragma GCC diagnostic pop
#endif

#include <Interaction.hpp>

static void copyBtVector3(const btVector3 &from, SiconosVector& to)
{
  to(0) = from.x();
  to(1) = from.y();
  to(2) = from.z();
}

// TODO: sppoint parameter used only by BulletSpaceFilter
BulletR::BulletR(const btManifoldPoint &point,
                 bool flip,
                 double y_correction_A,
                 double y_correction_B,
                 double scaling) :
  NewtonEulerFrom3DLocalFrameR(),
  _y_correction_A(y_correction_A),
  _y_correction_B(y_correction_B),
  _scaling(scaling),
  _flip(flip)
{
  updateContactPoints(point);
}

void BulletR::computeh(double time, BlockVector& q0, SiconosVector& y)
{
  DEBUG_BEGIN("BulletR::computeh(...)\n");

  NewtonEulerR::computeh(time, q0, y);

  DEBUG_PRINT("start of computeh\n");

  // Due to margins we add, objects are reported as closer than they really
  // are, so we correct by a factor.
  double correction = _y_correction_A + _y_correction_B;
  y.setValue(0, _contactDistance*_scaling + correction);

  DEBUG_PRINTF("distance : %g\n",  y.getValue(0));

  DEBUG_PRINTF("position on A : %g,%g,%g\n", (*pc1())(0), (*pc1())(1), (*pc1())(2));
  DEBUG_PRINTF("position on B : %g,%g,%g\n", (*pc2())(0), (*pc2())(1), (*pc2())(2));
  DEBUG_PRINTF("normal on B   : %g,%g,%g\n", (*nc())(0), (*nc())(1), (*nc())(2));

  DEBUG_END("BulletR::computeh(...)\n");
}

void BulletR::updateContactPoints(const btManifoldPoint& point)
{
  // Flip contact points if requested
  btVector3 posa = point.getPositionWorldOnA();
  btVector3 posb = point.getPositionWorldOnB();
  if (_flip) {
    posa = point.getPositionWorldOnB();
    posb = point.getPositionWorldOnA();
  }

  // Store distance
  _contactDistance = point.getDistance();

  // Update normal
  btVector3 n(point.m_normalWorldOnB  * (_flip ? -1 : 1));
  copyBtVector3(n, *_Nc);

  // Adjust contact point positions correspondingly along normal.  TODO: This
  // assumes same distance in each direction, i.e. same margin per object.
  posa = posa * _scaling + n * _y_correction_A;
  posb = posb * _scaling - n * _y_correction_B;

  // Update contact point locations
  copyBtVector3(posa, *_Pc1);
  copyBtVector3(posb, *_Pc2);

  assert(!((*_Nc)(0)==0 && (*_Nc)(1)==0 && (*_Nc)(2)==0)
         && "nc = 0, problems..\n");
}
