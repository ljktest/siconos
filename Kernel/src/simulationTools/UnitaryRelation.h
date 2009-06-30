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
/*! \file UnitaryRelation.h

*/
#ifndef UNITARYRELATION_H
#define UNITARYRELATION_H

#include "Tools.hpp"
#include "SiconosAlgebra.hpp"
#include "Interaction.h"
#include "RelationNamespace.hpp"

#include "SiconosPointers.hpp"

/** Interface to single relations from Interactions
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) June 06, 2006
 *
 * Remind that each Interaction is composed with one (pointer to)
 * relation and a non smooth law (size nsLawSize). The number of
 * relations is sizeOfInteraction/nsLawSize. A UnitaryRelation is one
 * of the relations of the Interaction. Actually, this class only
 * provides an interface to handle single relations, this for
 * IndexSets used in Topology.  Each UnitaryRelation has a pointer to
 * its "mother" Interaction and methods to compute y, lambda and so
 * on.
 *
 * - UnitaryBlocks computation: in OneStepNSProblem, some
 * operators/matrices are required to compute the unitaryBlocks
 * matrices (used for example for the assembly of Mlcp matrix).  In
 * the present class, three methods are available to get the required
 * unitaryBlocks: getLeftUnitaryBlockForDS, getRightUnitaryBlockForDS
 * and getExtraUnitaryBlock, with the general model for unitaryBlock
 * computation:
 *
 * unitaryBlock = getExtraUnitaryBlock  +  getLeftUnitaryBlockForDS * W * getRightUnitaryBlockForDS
 *
 * Examples:
 *
 *   => LinearTIR, unitaryBlock = D + h*theta*C*W*B (D != NULL only
 *      for unitaryBlocks on the diagonal of the full-assembled
 *      matrix) and thus getExtraUnitaryBlock = D,
 *      getLeftUnitaryBlockForDS = C, getRightUnitaryBlockForDS = B.
 *
 *   => Lagrangian, unitaryBlock = G* W* transpose(G) (G=H for
 *      xslagrangian linear) ie getExtraUnitaryBlock = NULL,
 *      getLeftUnitaryBlockForDS = getRightUnitaryBlockForDS = G
 *      (transpose is done using matMultTranspose)
 *
 * Moreover, for, G, B, etc ... we only get the part corresponding to
 * a specific DynamicalSystem (which belongs to the UnitaryRelation)
 *
 */
class UnitaryRelation
{

  // === PRIVATE MEMBERS ===

private:

  /** link to Interaction that owns this relation **/
  SP::Interaction mainInteraction;

  /** relative position of the present relation in the Interaction -
   For example if the present relation takes place from index 2 to 4
   in y vector of mainInteraction, the relative position is equal to
   2. */
  unsigned int relativePosition;

  /** number of the relation, ie the number of the corresponding
      unitaryBlock vector in the main Interaction.*/
  unsigned int number;

  /** Absolute position in the "global" vector of constraints (for
      example, the one handled by lsodar) */
  unsigned int absolutePostion;

  /** work vector to save pointers to state-related data of the
      dynamical systems involved in the UR.*/
  SP::SiconosVector workX;
  SP::SiconosVector mWorkXq;
  SP::SiconosVector mXfree;

  /** work vector to save pointers to z data of the dynamical systems
      involved in the UR.*/
  SP::SiconosVector workZ;



  /** default constructor
   */
  UnitaryRelation() {};

  /** copy constructor: private, no copy cor pass-by value
   */
  UnitaryRelation(const UnitaryRelation&);

public:

  /** constructor from a pointer to Interaction
  *  \param SP::Interaction : Interaction object from which a list of
  *  relation will be "extracted"
  *  \param unsigned int: gives the relative position of the relation
  *  inside the y vector of the interaction
  *  \param unsigned int: gives the number of the unitaryBlock in y
  *  vector of the interaction that corresponds to the present
  *  unitary relation.
  */
  UnitaryRelation(SP::Interaction , unsigned int, unsigned int);

  /** destructor
  */
  ~UnitaryRelation();

  /** get main interaction of this unitary relation
  *  \return a pointer to Interaction
  */
  inline SP::Interaction getInteractionPtr()
  {
    assert(mainInteraction);
    return mainInteraction;
  }

  /** get relative position of the Unitary Relation
  *  \return an unsigned int
  */
  inline const unsigned int getRelativePosition() const
  {
    return relativePosition;
  } ;

  /** get id of the parent interaction
   *  \return a string
   */
  inline const std::string getId() const
  {
    return mainInteraction->getId();
  };

  /** get number of the Unitary Relation
   *  \return an unsigned int
   */
  inline const unsigned int getNumber() const
  {
    return number;
  };

  /** get vector of output derivatives
  *  \return a VectorOfVectors
  */
  //  const std::vector< SimpleVector* > getY() const;
  const VectorOfVectors getY() const;

  /** get y[i], derivative number i of output
  *  \return pointer on a SimpleVector
  */
  SP::SiconosVector getYPtr(unsigned int) const;

  /** get yOld[i], derivative number i of output
  *  \return pointer on a SimpleVector
  */
  SP::SiconosVector getYOldPtr(unsigned int) const;

  /** get vector of input derivatives
  *  \return a VectorOfVectors
  */
  const  VectorOfVectors getLambda() const;

  /** get lambda[i], derivative number i of input
  *  \return pointer on a SimpleVector
  */
  SP::SiconosVector getLambdaPtr(unsigned int) const;

  /** get y[i], derivative number i of output, value used to compute indexSets
  *  \return a double
  */
  const double getYRef(unsigned int) const;

  /** get lambda[i], derivative number i of output, value used to compute indexSets
  *  \return a double
  */
  const double getLambdaRef(unsigned int) const;

  /** returns the size of the embedded non smooth law
  *  \return an unsigned int
  */
  const unsigned int getNonSmoothLawSize() const;

  /** returns the type of the embedded non smooth law
  *  \return a string
  */
  const std::string getNonSmoothLawType() const;

  /** returns the type of the embedded relation.
   */
  const RELATION::TYPES getRelationType() const;

  /** returns the subtype of the embedded relation.
   */
  const RELATION::SUBTYPES getRelationSubType() const;

  /** function used to sort UnitaryRelation in SiconosSet<SP::UnitaryRelation>
   *  \return an int
   */
  inline double* const getSort() const
  {
    return (double*)this;
  }

  /** gets an iterator to the first element of the DynamicalSystems set.
   *  \return a DSIterator.
   */
  inline DSIterator dynamicalSystemsBegin()
  {
    return mainInteraction->dynamicalSystemsBegin();
  };

  /** gets an iterator equal to DynamicalSystems.end().
   *  \return a DSIterator.
   */
  inline DSIterator dynamicalSystemsEnd()
  {
    return mainInteraction->dynamicalSystemsEnd();
  };

  /** gets a const iterator to the first element of the  DynamicalSystems set.
   *  \return a ConstDSIterator.
   */
  inline ConstDSIterator dynamicalSystemsBegin() const
  {
    return mainInteraction->dynamicalSystemsBegin();
  };

  /** gets a const iterator equal to DynamicalSystems.end().
   *  \return a ConstDSIterator.
   */
  inline ConstDSIterator dynamicalSystemsEnd() const
  {
    return mainInteraction->dynamicalSystemsEnd();
  };

  /** gets a pointer to the DynamicalSystemsSet
   *  \return a DynamicalSystemsSet*
   */
  SP::DynamicalSystemsSet getDynamicalSystemsPtr() ;

  /** To initialize the UR: mainly to set work vectors.
   */
  void initialize(const std::string&);

  /* to set workX content.
   \param a SP::SiconosVector to be inserted into workX
  */
  inline void insertInWorkX(SP::SiconosVector newX)
  {
    assert(workX) ;
    workX->insertPtr(newX);
  };

  /** Get a pointer to workX */
  inline SP::SiconosVector getWorkXPtr()
  {
    return workX;
  };
  inline SP::SiconosVector getXqPtr()
  {
    return mWorkXq;
  };
  inline SP::SiconosVector getXfreePtr()
  {
    return mXfree;
  };


  /** Get a pointer to workX */
  inline SP::SiconosVector getWorkZPtr()
  {
    return workZ;
  };

  /** gets the matrix used in unitaryBlock computation, (left * W * rigth), depends on the relation type (ex, LinearTIR, left = C, right = B).
   *         We get only the part corresponding to ds.
   *  \param a pointer to a dynamical system
   *  \param a pointer to SiconosMatrix (in-out parameter): the resulting unitaryBlock matrix
   *  \param unsigned int (optional): gradient index (see relations for details)
   */
  void getLeftUnitaryBlockForDS(SP::DynamicalSystem, SP::SiconosMatrix, unsigned int = 0) const;

  /** gets the matrix used in unitaryBlock computation, (left * W * rigth), depends on the relation type (ex, LinearTIR, left = C, right = B).
   *         We get only the part corresponding to ds.
   *  \param a pointer to a dynamical system
   *  \param a pointer to SiconosMatrix (in-out parameter): the resulting unitaryBlock matrix
   *  \param unsigned int (optional): gradient index (see relations for details)
   */
  void getRightUnitaryBlockForDS(SP::DynamicalSystem, SP::SiconosMatrix, unsigned int = 0) const;

  /** gets extra unitaryBlock corresponding to the present UR (see the top of this files for extra unitaryBlock meaning)
   *  \param a pointer to a SiconosMatrix (in-out parameter)
   */
  void getExtraUnitaryBlock(SP::SiconosMatrix) const;

};

TYPEDEF_SPTR(UnitaryRelation);
#endif // UNITARYRELATION_H
