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
/** \file OccR.hpp
    \brief A Siconos Newton Euler 3D friction relation between
    two BRep contact points.
 */

#ifndef OccR_hpp
#define OccR_hpp

#include "MechanicsFwd.hpp"
#include "Geometer.hpp"

#include <SiconosFwd.hpp>
#include <NewtonEulerFrom3DLocalFrameR.hpp>

class OccR : public NewtonEulerFrom3DLocalFrameR
{
public:
  /** Constructor from contact points.
   *  \param contact1 : the first contact.
   *  \param contact2 : the second contact.
   */
  OccR(const ContactPoint& contact1,
       const ContactPoint& contact2,
       const DistanceCalculatorType& distance_calculator = CadmbtbType());

  /** Compute h.
   *  \param time : the time.
   *  \param q0 : the state vector.
   *  \param y : output vector.
   */
  void computeh(double time, BlockVector& q0, SiconosVector& y);

  /** Set offset.
   * \param val : the new value.
   */
  void setOffset(double val) { _offset = val; };

  /** Set normalFromFace1.
   * \param val : the new value.
   */
  void setNormalFromFace1(bool val) { _normalFromFace1 = val; };

  /** Get geometer.
   * \return a SP::Geometer object.
   */
  SP::Geometer geometer() { return _geometer; }

  /** Set geometer.
   * \param geometer the new geometer
   */
  void setGeometer(SP::Geometer geometer) { _geometer = geometer; }

protected:
  const ContactPoint& _contact1;
  const ContactPoint& _contact2;

  SP::Geometer _geometer;

  bool _normalFromFace1;
  bool _offsetp1;
  double _offset;
};

#endif
