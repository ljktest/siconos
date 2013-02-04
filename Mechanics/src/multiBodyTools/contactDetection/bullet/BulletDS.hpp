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

#ifndef BulletDS_hpp
#define BulletDS_hpp

#include "SiconosPointers.hpp"
#include "BulletSiconos.hpp"
#include "BulletWeightedShape.hpp"

#include "NewtonEulerDS.hpp"

class BulletDS : public NewtonEulerDS, public std11::enable_shared_from_this<BulletDS>
{

private:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(BulletDS);

  SP::BulletWeightedShape _weightedShape;
  SP::btCollisionObject _collisionObject;

public:

  /** Constructor from a BulletBodyShape
   */
  BulletDS(SP::BulletWeightedShape weightedShape,
           SP::SiconosVector position,
           SP::SiconosVector velocity);

  /** get the collision object
  **/
  const SP::btCollisionObject& collisionObject() const
  {
    return _collisionObject;
  };

  /** get the shape
  **/
  SP::BulletWeightedShape weightedShape() const
  {
    return _weightedShape;
  };

  /** get a shared_ptr from this
   */
  std11::shared_ptr<BulletDS> shared_ptr()
  {
    return shared_from_this();
  };

  /** update Bullet positions
   */
  void updateCollisionObject() const;

  /** visitor hook
   */
  ACCEPT_STD_VISITORS();
};

struct ForCollisionObject : public Question<SP::btCollisionObject>
{
  using SiconosVisitor::visit;

  ANSWER(BulletDS, collisionObject());
};

struct ForWeightedShape : public Question<SP::BulletWeightedShape>
{
  using SiconosVisitor::visit;

  ANSWER(BulletDS, weightedShape());
};

struct UpdateCollisionObject : public SiconosVisitor
{
  using SiconosVisitor::visit;

  void visit(const BulletDS& bds)
  {
    bds.updateCollisionObject();
  }
};


TYPEDEF_SPTR(BulletDS)

#endif

