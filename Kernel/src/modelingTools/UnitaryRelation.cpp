/* Siconos-Kernel version 2.1.1, Copyright INRIA 2005-2007.
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
#include "DynamicalSystem.h"
#include "UnitaryRelation.h"
#include "FirstOrderLinearTIR.h"
#include "LagrangianLinearR.h"
#include "NewtonImpactNSL.h"
#include "NewtonImpactFrictionNSL.h"
#include "RuntimeException.h"

using namespace std;

// --- CONSTRUCTORS ---

// Data constructor
UnitaryRelation::UnitaryRelation(Interaction* inter, unsigned int pos, unsigned int num): mainInteraction(inter), relativePosition(pos), number(num)
{}

// --- DESTRUCTOR ---
UnitaryRelation::~UnitaryRelation()
{
  mainInteraction = NULL;
}

const VectorOfVectors UnitaryRelation::getY() const
{
  // A new object of type VectorOfVectors is created but it handles pointers to BlockVectors,
  // thus there is no copy of the "basic" SimpleVectors.

  VectorOfVectors tmp;
  VectorOfVectors interactionBlocks = mainInteraction->getY();

  for (unsigned int i = 0; i < interactionBlocks.size(); ++i)
    tmp[i] = interactionBlocks[i]->getVectorPtr(number);

  return tmp;
}

SiconosVector* UnitaryRelation::getYPtr(unsigned int i) const
{
  // i is the derivative number.
  return ((mainInteraction->getYPtr(i))->getVectorPtr(number));
}

SiconosVector* UnitaryRelation::getYOldPtr(unsigned int i) const
{
  // i is the derivative number.
  return ((mainInteraction->getYOldPtr(i))->getVectorPtr(number));
}

const VectorOfVectors UnitaryRelation::getLambda() const
{
  // A new object of type VectorOfVectors is created but it handles pointers to BlockVectors,
  // thus there is no copy of the "basic" SimpleVectors.
  VectorOfVectors tmp;
  VectorOfVectors interactionBlocks = mainInteraction->getLambda();

  for (unsigned int i = 0; i < interactionBlocks.size(); ++i)
    tmp[i] = interactionBlocks[i]->getVectorPtr(number);

  return tmp;
}

SiconosVector* UnitaryRelation::getLambdaPtr(unsigned int i) const
{
  // i is the derivative number.
  return ((mainInteraction->getLambdaPtr(i))->getVectorPtr(number));
}

const double UnitaryRelation::getYRef(unsigned int i) const
{
  // get the single value used to build indexSets
  // Warning: the relativePosition depends on NsLawSize and/or type.
  // This means that at the time, for the block of y that corresponds to the present relation, the first scalar value is used.
  // For example, for friction, normal part is in first position, followed by the tangential parts.
  return (*getYPtr(i))(0);
}

const double UnitaryRelation::getLambdaRef(unsigned int i) const
{
  // get the single value used to build indexSets
  return (*getLambdaPtr(i))(0);
}

const unsigned int UnitaryRelation::getNonSmoothLawSize() const
{
  return mainInteraction->getNonSmoothLawPtr()->getNsLawSize();
}

const string UnitaryRelation::getNonSmoothLawType() const
{
  return mainInteraction->getNonSmoothLawPtr()->getType();
}

const string UnitaryRelation::getRelationType() const
{
  return mainInteraction->getRelationPtr()->getType();
}

const string UnitaryRelation::getRelationSubType() const
{
  return mainInteraction->getRelationPtr()->getSubType();
}

DynamicalSystemsSet * UnitaryRelation::getDynamicalSystemsPtr()
{
  return mainInteraction->getDynamicalSystemsPtr();
}

void UnitaryRelation::getLeftBlockForDS(DynamicalSystem * ds, SiconosMatrix* Block, unsigned index) const
{
  unsigned int k = 0;
  DSIterator itDS;
  itDS = mainInteraction->dynamicalSystemsBegin();

  // look for ds and its position in G
  while (*itDS != ds && itDS != mainInteraction->dynamicalSystemsEnd())
  {
    k += (*itDS)->getDim();
    itDS++;
  }

  // check dimension (1)
  if ((*itDS)->getDim() != Block->size(1))
    RuntimeException::selfThrow("UnitaryRelation::getLeftBlockForDS(DS, Block, ...): inconsistent sizes between Block and DS");

  SiconosMatrix * originalMatrix = NULL; // Complete matrix, Relation member.
  string relationType = getRelationType();
  string relationSubType = getRelationSubType();

  if (relationType == "FirstOrder")
  {
    if (relationSubType == "Type1R")
      originalMatrix = (static_cast<FirstOrderR*>(mainInteraction->getRelationPtr()))->getJacobianHPtr(0);
    else if (relationSubType == "LinearTIR")
      originalMatrix = (static_cast<FirstOrderLinearTIR*>(mainInteraction->getRelationPtr()))->getCPtr();
    else if (relationSubType == "LinearR")
      originalMatrix = (static_cast<FirstOrderLinearR*>(mainInteraction->getRelationPtr()))->getCPtr();
    else
      RuntimeException::selfThrow("UnitaryRelation::getLeftBlockForDS, not yet implemented for first order relations of subtype " + relationSubType);
  }
  else if (relationType == "Lagrangian")
  {
    if (relationSubType == "LinearR")
      originalMatrix = (static_cast<LagrangianLinearR*>(mainInteraction->getRelationPtr()))->getHPtr();
    else
      originalMatrix = (static_cast<LagrangianR*>(mainInteraction->getRelationPtr()))->getGPtr(index);
    //      else RuntimeException::selfThrow("UnitaryRelation::getLeftBlockForDS, not yet implemented for Lagrangian relation of subtype "+relationSubType);
  }
  else
    RuntimeException::selfThrow("UnitaryRelation::getLeftBlockForDS, not yet implemented for relations of type " + relationType);

  // copy sub-block of originalMatrix into Block
  // dim of the sub-block
  Index subDim(2);
  subDim[0] = Block->size(0);
  subDim[1] = Block->size(1);
  // Position (row,col) of first element to be read in originalMatrix and of first element to be set in Block
  Index subPos(4);
  subPos[0] = relativePosition;
  subPos[1] = k;
  subPos[2] = 0;
  subPos[3] = 0;
  setBlock(originalMatrix, Block, subDim, subPos);
}

void UnitaryRelation::getRightBlockForDS(DynamicalSystem * ds, SiconosMatrix* Block, unsigned index) const
{
  unsigned int k = 0;
  DSIterator itDS;
  itDS = mainInteraction->dynamicalSystemsBegin();

  // look for ds and its position in G
  while (*itDS != ds && itDS != mainInteraction->dynamicalSystemsEnd())
  {
    k += (*itDS)->getDim();
    itDS++;
  }

  // check dimension (1)
  if ((*itDS)->getDim() != Block->size(0))
    RuntimeException::selfThrow("UnitaryRelation::getRightBlockForDS(DS, Block, ...): inconsistent sizes between Block and DS");

  SiconosMatrix * originalMatrix = NULL; // Complete matrix, Relation member.
  string relationType = getRelationType() + getRelationSubType();
  if (relationType == "FirstOrderType1R")
    originalMatrix = (static_cast<FirstOrderR*>(mainInteraction->getRelationPtr()))->getJacobianGPtr(0);
  else if (relationType == "FirstOrderLinearTIR")
    originalMatrix = (static_cast<FirstOrderLinearTIR*>(mainInteraction->getRelationPtr()))->getBPtr();
  else if (relationType == "FirstOrderLinearR")
    originalMatrix = (static_cast<FirstOrderLinearR*>(mainInteraction->getRelationPtr()))->getBPtr();

  //else if( (relationType == LAGRANGIANRELATION || relationType == LAGRANGIANLINEARRELATION))
  // originalMatrix = (static_cast<LagrangianR*>(mainInteraction->getRelationPtr()))->getGPtr(index);
  // For Lagrangian systems, right block = transpose (left block) so we do not need to use the present function.
  else RuntimeException::selfThrow("UnitaryRelation::getRightBlockForDS, not yet implemented for relation of type " + relationType);

  if (originalMatrix == NULL)
    RuntimeException::selfThrow("UnitaryRelation::getRightBlockForDS(DS, Block, ...): the right block is a NULL pointer (matrix B in relation) ");

  // copy sub-block of originalMatrix into Block
  // dim of the sub-block
  Index subDim(2);
  subDim[0] = Block->size(0);
  subDim[1] = Block->size(1);
  // Position (row,col) of first element to be read in originalMatrix and of first element to be set in Block
  Index subPos(4);
  subPos[0] = k;
  subPos[1] = relativePosition;
  subPos[2] = 0;
  subPos[3] = 0;
  setBlock(originalMatrix, Block, subDim, subPos);
}

void UnitaryRelation::getExtraBlock(SiconosMatrix* Block) const
{
  // !!! Warning: we suppose that D is block diagonal, ie that there is no coupling between UnitaryRelation through D !!!
  // Any coupling between relations through D must be taken into account thanks to the nslaw (by "increasing" its dimension).

  if (getRelationType() != "FirstOrder")
    RuntimeException::selfThrow("UnitaryRelation::getExtraBlock, forbidden or not yet implemented for relation of type " + getRelationType());

  string relationSubType = getRelationSubType();
  SiconosMatrix * D = NULL;
  if (relationSubType == "Type1R")
  {
    // nothing, D = NULL
  }
  else if (relationSubType == "LinearTIR")
    D = static_cast<FirstOrderLinearTIR*>(mainInteraction->getRelationPtr())->getDPtr();
  else if (relationSubType == "LinearR")
    D = static_cast<FirstOrderLinearR*>(mainInteraction->getRelationPtr())->getDPtr();
  else
    RuntimeException::selfThrow("UnitaryRelation::getExtraBlock, not yet implemented for first order relations of subtype " + relationSubType);
  if (D == NULL)
    return; //ie no extra block

  // copy sub-block of originalMatrix into Block
  // dim of the sub-block
  Index subDim(2);
  subDim[0] = Block->size(0);
  subDim[1] = Block->size(1);
  // Position (row,col) of first element to be read in originalMatrix and of first element to be set in Block
  Index subPos(4);
  subPos[0] = relativePosition;
  subPos[1] = relativePosition;
  subPos[2] = 0;
  subPos[3] = 0;
  setBlock(D, Block, subDim, subPos);
}

void UnitaryRelation::computeEquivalentY(double time, unsigned int level, const string& simulationType, SiconosVector* yOut)
{

  // Get relation and non smooth law types
  string relationType = getRelationType();
  string nslawType = getNonSmoothLawType();
  // Warning: first version with OneStepNSProblem as an input argument. But this means "inclusion" of simulationTools class into a
  // modelingTools class => no!
  //   string simulationType = osns->getSimulationPtr()->getType();
  // unsigned int level = osns->getLevelMin(); // this corresponds to the derivative order (for y) used to compute yOut.

  if (simulationType == "TimeStepping")
    mainInteraction->computeFreeOutput(time, level);
  else  if (simulationType == "EventDriven")
    mainInteraction->computeOutput(time, level);

  (*yOut) = *(getYPtr(level)); // yOut = yFree

  if (relationType == "Lagrangian")
  {
    double e;

    if (nslawType == NEWTONIMPACTNSLAW)
    {
      e = (static_cast<NewtonImpactNSL*>(mainInteraction->getNonSmoothLawPtr()))->getE();
      if (simulationType == "TimeStepping")
        scal(e, *getYOldPtr(level), *yOut, false);
      //*yOut+=e**getYOldPtr(level);
      //axpy(e,*getYOldPtr(level),*yOut);  // yOut = e*yOld + yOut
      else if (simulationType == "EventDriven")
        *yOut *= (1.0 + e);
      else
        RuntimeException::selfThrow("UnitaryRelation::computeEquivalentY not yet implemented for relation of type " + relationType + " and non smooth law of type " + nslawType + " for a simulaton of type " + simulationType);
    }
    else if (nslawType == NEWTONIMPACTFRICTIONNSLAW)
    {
      e = (static_cast<NewtonImpactFrictionNSL*>(mainInteraction->getNonSmoothLawPtr()))->getEn();
      // Only the normal part is multiplied by e
      if (simulationType == "TimeStepping")
        (*yOut)(0) +=  e * (*getYOldPtr(level))(0);
      else RuntimeException::selfThrow("UnitaryRelation::computeEquivalentY not yet implemented for relation of type " + relationType + " and non smooth law of type " + nslawType + " for a simulaton of type " + simulationType);

    }
    else
      RuntimeException::selfThrow("UnitaryRelation::computeEquivalentY not yet implemented for relation of type " + relationType + " and non smooth law of type " + nslawType);
  }
}


