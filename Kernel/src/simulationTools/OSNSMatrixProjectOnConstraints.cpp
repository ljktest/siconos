/* Siconos-Kernel, Copyright INRIA 2005-2011.
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
#include <assert.h>
#include "OSNSMatrixProjectOnConstraints.hpp"
#include "Tools.hpp"
#include "RelationTypes.hpp"
#include "NewtonEulerR.hpp"
#include "LagrangianR.hpp"

using namespace std;
using namespace RELATION;
using namespace Siconos;
//#define OSNSMPROJ_DEBUG


OSNSMatrixProjectOnConstraints::OSNSMatrixProjectOnConstraints(unsigned int n, unsigned int m, int stor):
  OSNSMatrix(n, m, stor)
{
  convert();
}



// Destructor : pointers are smart
OSNSMatrixProjectOnConstraints::~OSNSMatrixProjectOnConstraints()
{
}

void OSNSMatrixProjectOnConstraints::updateSizeAndPositions(unsigned int& dim,
    SP::UnitaryRelationsGraph indexSet)
{
  // === Description ===

  // For a unitaryBlock (diagonal or extra diagonal) corresponding to
  // a Unitary Relation, we need to know the position of its first
  // element in the full-matrix M. This position depends on the
  // previous unitaryBlocks sizes.
  //
  // positions are saved in a map<SP::UnitaryRelation, unsigned int>,
  // named unitaryBlocksPositions.
  //

  // Computes real size of the current matrix = sum of the dim. of all
  // UR in indexSet
  dim = 0;
  UnitaryRelationsGraph::VIterator vd, vdend;
#ifdef OSNSMPROJ_DEBUG
  std::cout << "indexSet :" << indexSet << std::endl;
  indexSet->display();
#endif
  for (boost::tie(vd, vdend) = indexSet->vertices(); vd != vdend; ++vd)
  {
    assert(indexSet->descriptor(indexSet->bundle(*vd)) == *vd);

    //    (*unitaryBlocksPositions)[indexSet->bundle(*vd)] = dim;
#ifdef OSNSMPROJ_DEBUG
    std::cout << " dim :" << dim << std::endl;
    std::cout << "vd :" << *vd << std::endl;
    assert(indexSet->properties(*vd)->blockProj);
#endif


    indexSet->bundle(*vd)->setAbsolutePositionProj(dim);
    SP::UnitaryRelation UR = indexSet->bundle(*vd);

    unsigned int nslawSize = computeSizeForProjection(UR->interaction());

    dim += nslawSize;
    assert(indexSet->bundle(*vd)->absolutePositionProj() < dim);
  }
}

void OSNSMatrixProjectOnConstraints::fill(SP::UnitaryRelationsGraph indexSet, bool update)
{
  assert(indexSet);

  if (update)
  {
    // Computes dimRow and unitaryBlocksPositions according to indexSet
    updateSizeAndPositions(dimColumn, indexSet);
    dimRow = dimColumn;
  }

  if (storageType == 0)
  {

    // === Memory allocation, if required ===
    // Mem. is allocate only if !M or if its size has changed.
    if (update)
    {
      if (! M1)
        M1.reset(new SimpleMatrix(dimRow, dimColumn));
      else
      {
        if (M1->size(0) != dimRow || M1->size(1) != dimColumn)
          M1->resize(dimRow, dimColumn);
        M1->zero();
      }
    }

    // ======> Aim: find UR1 and UR2 both in indexSet and which have
    // common DynamicalSystems.  Then get the corresponding matrix
    // from map unitaryBlocks, and copy it into M

    unsigned int pos = 0, col = 0; // index position used for
    // unitaryBlock copy into M, see
    // below.
    // === Loop through "active" Unitary Relations (ie present in
    // indexSets[level]) ===
    UnitaryRelationsGraph::VIterator vi, viend;
#ifdef OSNSMPROJ_DEBUG
    std::cout << "indexSet :" << indexSet << std::endl;
    indexSet->display();
#endif
    for (boost::tie(vi, viend) = indexSet->vertices();
         vi != viend; ++vi)
    {
      SP::UnitaryRelation ur = indexSet->bundle(*vi);
      pos = ur->absolutePositionProj();
      assert(properties(*indexSet)->blockProj[*vi]);
      boost::static_pointer_cast<SimpleMatrix>(M1)
      ->setBlock(pos, pos, *(properties(*indexSet)->blockProj[*vi]));
#ifdef OSNSMPROJ_DEBUG
      printf("OSNSMatrix M1: %i %i\n", M1->size(0), M1->size(1));
      printf("OSNSMatrix block: %i %i\n", properties(*indexSet)->blockProj[*vi]->size(0), properties(*indexSet)->blockProj[*vi]->size(1));
#endif
    }


    UnitaryRelationsGraph::EIterator ei, eiend;
    for (boost::tie(ei, eiend) = indexSet->edges();
         ei != eiend; ++ei)
    {
      UnitaryRelationsGraph::VDescriptor vd1 = indexSet->source(*ei);
      UnitaryRelationsGraph::VDescriptor vd2 = indexSet->target(*ei);

      SP::UnitaryRelation ur1 = indexSet->bundle(vd1);
      SP::UnitaryRelation ur2 = indexSet->bundle(vd2);

      pos =  ur1->absolutePositionProj();//(*unitaryBlocksPositions)[ur1];

      assert(indexSet->is_vertex(ur2));

      col =  ur2->absolutePositionProj();//(*unitaryBlocksPositions)[ur2];


      assert(pos < dimRow);
      assert(col < dimColumn);

#ifdef OSNSMPROJ_DEBUG
      printf("OSNSMatrix M1: %i %i\n", M1->size(0), M1->size(1));
      printf("OSNSMatrix upper: %i %i\n", indexSet->properties(*ei)->upper_blockProj->size(0), indexSet->properties(*ei)->upper_blockProj->size(1));
      printf("OSNSMatrix lower: %i %i\n", indexSet->properties(*ei)->lower_blockProj->size(0), indexSet->properties(*ei)->lower_blockProj->size(1));
#endif

      boost::static_pointer_cast<SimpleMatrix>(M1)
      ->setBlock(std::min(pos, col), std::max(pos, col),
                 *(properties(*indexSet)->upper_blockProj[*ei]));

      boost::static_pointer_cast<SimpleMatrix>(M1)
      ->setBlock(std::max(pos, col), std::min(pos, col),
                 *properties(*indexSet)->lower_blockProj[*ei]);
    }

  }
  else // if storageType == 1
  {
    if (! M2)
      M2.reset(new BlockCSRMatrix(indexSet));
    else
      M2->fill(indexSet);
  }
  if (update)
    convert();
}
unsigned int OSNSMatrixProjectOnConstraints::getPositionOfUnitaryBlock(SP::UnitaryRelation UR) const
{
  return UR->absolutePositionProj();
}

unsigned int OSNSMatrixProjectOnConstraints::computeSizeForProjection(SP::Interaction inter)
{
  RELATION::TYPES relationType;
  relationType = inter->relation()->getType();
  unsigned int nslawSize = inter->nonSmoothLaw()->size();

  unsigned int size =  nslawSize;

  if (Type::value(*(inter->nonSmoothLaw())) == Type::NewtonImpactFrictionNSL ||
      Type::value(*(inter->nonSmoothLaw())) == Type::NewtonImpactNSL)
  {
    if (relationType == NewtonEuler)
    {
      // SP::NewtonEulerFrom1DLocalFrameR ri = boost::static_pointer_cast<NewtonEulerFrom1DLocalFrameR> (inter->relation());
      // if(ri->_isOnContact)
      //   equalitySize = 1;
      size = 1;
    }
    else if (relationType == Lagrangian)
    {
      size = 1;
    }
    else
    {
      RuntimeException::selfThrow("MLCPProjectOnConstraints::computeSizeForProjection. relation is not o the right type. neither Lagrangian nor NewtonEuler ");
    }
  }

  return size;

}
