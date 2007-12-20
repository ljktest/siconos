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
#include "FrictionContact.h"
#include "FrictionContactXML.h"
#include "Topology.h"
#include "Interaction.h"
#include "Simulation.h"
#include "UnitaryRelation.h"
#include "Model.h"
#include "NonSmoothDynamicalSystem.h"
#include "DynamicalSystem.h"
#include "Relation.h"
#include "NewtonImpactFrictionNSL.h"

using namespace std;

// xml constructor
FrictionContact::FrictionContact(const string pbType, OneStepNSProblemXML* osNsPbXml, Simulation* newSimu):
  OneStepNSProblem(pbType, osNsPbXml, newSimu), w(NULL), z(NULL), M(NULL), q(NULL), mu(NULL),
  isWAllocatedIn(false), isZAllocatedIn(false), isMAllocatedIn(false), isQAllocatedIn(false), Mspbl(NULL)
{
  FrictionContactXML * xmllcp = (static_cast<FrictionContactXML*>(osNsPbXml));

  // If both q and M are given in xml file, check if sizes are consistent
  if (xmllcp->hasQ() && xmllcp->hasM() && ((xmllcp->getM()).size(0) != (xmllcp->getQ()).size()))
    RuntimeException::selfThrow("FrictionContact: xml constructor, inconsistent sizes between given q and M");

  // first dimension is given by M matrix size in xml file
  if (xmllcp->hasM())
  {
    sizeOutput = (xmllcp->getM()).size(0);
    M = new SimpleMatrix(xmllcp->getM());
    isMAllocatedIn = true;
  }

  if (xmllcp->hasQ())
  {
    // get sizeOutput if necessary
    if (M == NULL)
      sizeOutput = (xmllcp->getQ()).size();

    q = new SimpleVector(xmllcp->getQ());
    isQAllocatedIn = true;
  }
}

// Constructor from a set of data
FrictionContact::FrictionContact(const string pbType, Simulation* newSimu, const string newId):
  OneStepNSProblem(pbType, newSimu, newId), w(NULL), z(NULL), M(NULL), q(NULL), mu(NULL),
  isWAllocatedIn(false), isZAllocatedIn(false), isMAllocatedIn(false), isQAllocatedIn(false)
{}

// Constructor from a set of data
FrictionContact::FrictionContact(const string pbType, Solver*  newSolver, Simulation* newSimu, const string newId):
  OneStepNSProblem(pbType, newSimu, newId, newSolver), w(NULL), z(NULL), M(NULL), q(NULL), mu(NULL),
  isWAllocatedIn(false), isZAllocatedIn(false), isMAllocatedIn(false), isQAllocatedIn(false), Mspbl(NULL)
{}

// destructor
FrictionContact::~FrictionContact()
{
  if (isWAllocatedIn) delete w;
  w = NULL;
  if (isZAllocatedIn) delete z;
  z = NULL;
  if (isMAllocatedIn)
    if (solver->useBlocks()) freeSpBlMat(Mspbl);
    else delete M;
  M = NULL;
  Mspbl = NULL;
  if (isQAllocatedIn) delete q;
  q = NULL;
  if (mu != NULL) delete mu;
  mu = NULL;
}

// Setters

void FrictionContact::setW(const SimpleVector& newValue)
{
  if (sizeOutput != newValue.size())
    RuntimeException::selfThrow("FrictionContact: setW, inconsistent size between given w size and problem size. You should set sizeOutput before");

  if (w == NULL)
  {
    w = new SimpleVector(sizeOutput);
    isWAllocatedIn = true;
  }
  else if (w->size() != sizeOutput)
    RuntimeException::selfThrow("FrictionContact: setW, w size differs from sizeOutput");

  *w = newValue;
}

void FrictionContact::setWPtr(SimpleVector* newPtr)
{
  if (sizeOutput != newPtr->size())
    RuntimeException::selfThrow("FrictionContact: setWPtr, inconsistent size between given w size and problem size. You should set sizeOutput before");

  if (isWAllocatedIn) delete w;
  w = newPtr;
  isWAllocatedIn = false;
}


void FrictionContact::setZ(const SimpleVector& newValue)
{
  if (sizeOutput != newValue.size())
    RuntimeException::selfThrow("FrictionContact: setZ, inconsistent size between given z size and problem size. You should set sizeOutput before");

  if (z == NULL)
  {
    z = new SimpleVector(sizeOutput);
    isZAllocatedIn = true;
  }

  *z = newValue;
}

void FrictionContact::setZPtr(SimpleVector* newPtr)
{
  if (sizeOutput != newPtr->size())
    RuntimeException::selfThrow("FrictionContact: setZPtr, inconsistent size between given z size and problem size. You should set sizeOutput before");

  if (isZAllocatedIn) delete z;
  z = newPtr;
  isZAllocatedIn = false;
}

void FrictionContact::setM(const SiconosMatrix& newValue)
{
  if (sizeOutput != newValue.size(0) || sizeOutput != newValue.size(1))
    RuntimeException::selfThrow("FrictionContact: setM, inconsistent size between given M size and problem size. You should set sizeOutput before");

  if (M == NULL)
  {
    M = new SimpleMatrix(sizeOutput, sizeOutput);
    isMAllocatedIn = true;
  }

  *M = newValue;
}

void FrictionContact::setMPtr(SiconosMatrix* newPtr)
{
  if (sizeOutput != newPtr->size(0) || sizeOutput != newPtr->size(1))
    RuntimeException::selfThrow("FrictionContact: setMPtr, inconsistent size between given M size and problem size. You should set sizeOutput before");

  if (isMAllocatedIn) delete M;
  M = newPtr;
  isMAllocatedIn = false;
}


void FrictionContact::setQ(const SimpleVector& newValue)
{
  if (sizeOutput != newValue.size())
    RuntimeException::selfThrow("FrictionContact: setQ, inconsistent size between given q size and problem size. You should set sizeOutput before");

  if (q == NULL)
  {
    q = new SimpleVector(sizeOutput);
    isQAllocatedIn = true;
  }

  *q = newValue;
}

void FrictionContact::setQPtr(SimpleVector* newPtr)
{
  if (sizeOutput != newPtr->size())
    RuntimeException::selfThrow("FrictionContact: setQPtr, inconsistent size between given q size and problem size. You should set sizeOutput before");

  if (isQAllocatedIn) delete q;
  q = newPtr;
  isQAllocatedIn = false;
}

void FrictionContact::initialize()
{

  // This function performs all steps that are not time dependant
  // General initialize for OneStepNSProblem
  OneStepNSProblem::initialize();

  // Memory allocation for w, M, z and q
  if (w == NULL)
  {
    w = new SimpleVector(maxSize);
    isWAllocatedIn = true;
  }
  else
  {
    if (w->size() != maxSize)
      w->resize(maxSize);
  }
  if (z == NULL)
  {
    z = new SimpleVector(maxSize);
    isZAllocatedIn = true;
  }
  else
  {
    if (z->size() != maxSize)
      z->resize(maxSize);
  }
  if (M == NULL)
  {
    M = new SimpleMatrix(maxSize, maxSize);
    isMAllocatedIn = true;
  }
  else
  {
    if (M->size(0) != maxSize || M->size(1) != maxSize)
      M->resize(maxSize, maxSize);
  }
  if (q == NULL)
  {
    q = new SimpleVector(maxSize);
    isQAllocatedIn = true;
  }
  else
  {
    if (q->size() != maxSize)
      q->resize(maxSize);
  }
  w->zero();
  z->zero();

  // if all relative degrees are equal to 0 or 1 and if if we do not use a solver block
  // get topology
  Topology * topology = simulation->getModelPtr()->getNonSmoothDynamicalSystemPtr()->getTopologyPtr();

  if (topology->isTimeInvariant() &&   !OSNSInteractions->isEmpty() && !solver->useBlocks())
  {
    // computeSizeOutput and updateBlocks were already done in OneStepNSProblem::initialize
    if (sizeOutput != 0)
      assembleM();
  }

  // Fill vector of friction coefficients
  // For each Unitary relation in index set 0, we get the corresponding non smooth law and its mu.
  UnitaryRelationsSet* I0 = topology->getIndexSet0Ptr();
  ConstUnitaryRelationsIterator itUR;
  unsigned int i = 0;
  mu = new SimpleVector(I0->size());
  for (itUR = I0->begin(); itUR != I0->end(); ++itUR)
    (*mu)(i++) = static_cast<NewtonImpactFrictionNSL*>((*itUR)->getInteractionPtr()->getNonSmoothLawPtr())->getMu();


}

void FrictionContact::computeBlock(UnitaryRelation* UR1, UnitaryRelation* UR2)
{

  // Warning: we suppose that at this point, all non linear operators (G for lagrangian relation for example) have been computed through plug-in mechanism.
  // Get DS common between UR1 and UR2
  DynamicalSystemsSet commonDS;
  intersection(*UR1->getDynamicalSystemsPtr(), *UR2->getDynamicalSystemsPtr(), commonDS);

  if (!commonDS.isEmpty()) // Nothing to be done if there are no common DS between the two UR.
  {

    // Get dimension of the NonSmoothLaw (ie dim of the block)
    unsigned int nslawSize1 = UR1->getNonSmoothLawSize();
    unsigned int nslawSize2 = UR2->getNonSmoothLawSize();
    // Check allocation
    if (blocks[UR1][UR2] == NULL)
      blocks[UR1][UR2] = new SimpleMatrix(nslawSize1, nslawSize2);

    // Get DS common between UR1 and UR2
    DSIterator itDS;

    // Get the W and Theta maps of one of the Unitary Relation - Warning: in the current version, if OSI!=Moreau, this fails.
    MapOfMatrices W;
    MapOfDouble Theta;
    getOSIMaps(UR1, W, Theta);

    SiconosMatrix* currentBlock = blocks[UR1][UR2];
    SimpleMatrix *work = NULL;
    currentBlock->zero();
    SiconosMatrix *leftBlock = NULL, *rightBlock = NULL;
    unsigned int sizeDS;
    string relationType1, relationType2;
    bool flagRightBlock = false;

    // General form of the block is :   block = a*extraBlock + b * leftBlock * OP * rightBlock
    // a and b are scalars, OP a matrix depending on the integrator, the simulation type ...
    // left, right and extra depend on the relation type and the non smooth law.

    // loop over the common DS
    for (itDS = commonDS.begin(); itDS != commonDS.end(); itDS++)
    {
      sizeDS = (*itDS)->getDim();
      // get blocks corresponding to the current DS
      // These blocks depends on the relation type.
      leftBlock = new SimpleMatrix(nslawSize1, sizeDS);

      UR1->getLeftBlockForDS(*itDS, leftBlock);
      relationType1 = UR1->getRelationType();
      relationType2 = UR2->getRelationType();
      // Computing depends on relation type -> move this in UnitaryRelation method?
      if (relationType1 == "Lagrangian" || relationType2 == "Lagrangian")
      {
        if (UR1 == UR2)
          rightBlock = leftBlock ;
        else
        {
          rightBlock = new SimpleMatrix(nslawSize2, sizeDS);
          UR2->getLeftBlockForDS(*itDS, rightBlock);
          // Warning: we use getLeft for Right block because right = transpose(left) and because of size checking inside the getBlock function,
          // a getRight call will fail.
          flagRightBlock = true;
        }

        work = new SimpleMatrix(*rightBlock);
        work->trans();
        // W contains a lu-factorized matrix and we solve
        // W * X = rightBlock with PLU
        // Work is a temporary matrix.
        W[*itDS]->PLUForwardBackwardInPlace(*work);
        //*currentBlock +=  *leftBlock * *work; // left = right = G or H
        gemm(CblasNoTrans, CblasNoTrans, 1.0, *leftBlock, *work, 1.0, *currentBlock);
        delete work;
        if (flagRightBlock) delete rightBlock;
      }
      else RuntimeException::selfThrow("FrictionContact::computeBlock not yet implemented for relation of type " + relationType1);
      delete leftBlock;
    }
  }
}

void FrictionContact::assembleM() //
{
  // === Description ===
  // For each Unitary Relation which is active (ie which is in indexSet[1] of the Simulation), named itRow (it corresponds to a row of block in M),
  // and for each Unitary Relation connected to itRow and active, named itCol, we get the block[itRow][itCol] and copy it at the right place in M matrix
  //
  // Note that if a Unitary Relation is connected to another one (ie if they have common DS), the corresponding block must be in blocks map. This is the role of
  // updateBlocks() to ensure this.
  // But the presence of a block in the map does not imply that its corresponding UR are active, since this block may have been computed at a previous time step.
  // That is why we need to check if itCol is in indexSet1.
  //
  // See updateBlocks function for more details.

  if (M->size(0) != sizeOutput || M->size(1) != sizeOutput)
    M->resize(sizeOutput, sizeOutput);
  M->zero();

  // Get index set 1 from Simulation
  UnitaryRelationsSet * indexSet = simulation->getIndexSetPtr(levelMin);
  // === Loop through "active" Unitary Relations (ie present in indexSets[level]) ===

  unsigned int pos = 0, col = 0; // index position used for block copy into M, see below.
  UnitaryRelationsIterator itRow;
  UnitaryMatrixColumnIterator itCol;
  for (itRow = indexSet->begin(); itRow != indexSet->end(); ++itRow)
  {
    for (itCol = blocks[*itRow].begin(); itCol != blocks[*itRow].end(); ++itCol)
    {
      // *itRow and itCol are UnitaryRelation*.

      // Check that itCol is in Index set 1
      if ((indexSet->find((*itCol).first)) != indexSet->end())
      {
        pos = blocksPositions[*itRow];
        col = blocksPositions[(*itCol).first];
        // copy the block into Mlcp - pos/col: position in M (row and column) of first element of the copied block
        static_cast<SimpleMatrix*>(M)->setBlock(pos, col, *(blocks[*itRow][(*itCol).first])); // \todo avoid copy
      }
    }
  }
  // === end of UnitaryRelations loop ===
}

void FrictionContact::computeQ(const double time)
{

  if (q->size() != sizeOutput)
    q->resize(sizeOutput);
  q->zero();

  // === Get index set from Simulation ===
  UnitaryRelationsSet * indexSet = simulation->getIndexSetPtr(levelMin);

  // === Loop through "active" Unitary Relations (ie present in indexSets[level]) ===

  unsigned int pos = 0;
  UnitaryRelationsIterator itCurrent, itLiked;
  string simulationType = simulation->getType();
  for (itCurrent = indexSet->begin(); itCurrent !=  indexSet->end(); ++itCurrent)
  {
    // *itCurrent is a UnitaryRelation*.

    // Compute q, this depends on the type of non smooth problem, on the relation type and on the non smooth law
    pos = blocksPositions[*itCurrent];
    (*itCurrent)->computeEquivalentY(time, levelMin, simulationType, q, pos); // free output is saved in y
  }
}

void FrictionContact::preCompute(const double time)
{
  // compute M and q operators for Friction problem

  computeSizeOutput();
  if (sizeOutput != 0)
  {
    updateBlocks();
    assembleM();
    computeQ(time);
    // check z and w sizes and reset if necessary
    if (z->size() != sizeOutput)
    {
      z->resize(sizeOutput, false);
    }

    if (w->size() != sizeOutput)
    {
      w->resize(sizeOutput);
    }
    w->zero();
    z->zero();
  }
}

void FrictionContact::postCompute()
{
  // === Get index set from Topology ===
  UnitaryRelationsSet * indexSet = simulation->getIndexSetPtr(levelMin);

  // y and lambda vectors
  //  vector< SimpleVector* >  Y, Lambda;
  SiconosVector * y, *lambda;

  // === Loop through "active" Unitary Relations (ie present in indexSets[1]) ===

  unsigned int pos = 0;
  unsigned int nsLawSize;
  UnitaryRelationsIterator itCurrent, itLinked;

  for (itCurrent = indexSet->begin(); itCurrent !=  indexSet->end(); ++itCurrent)
  {
    // *itCurrent is a UnitaryRelation*.
    // size if a block that corresponds to the current UnitaryRelation
    nsLawSize = (*itCurrent)->getNonSmoothLawSize();
    // Get the relative position of UR-block in the vector w or z
    pos = blocksPositions[*itCurrent];

    // Get Y and Lambda for the current Unitary Relation
    y = static_cast<SimpleVector*>((*itCurrent)-> getYPtr(levelMin));
    lambda = static_cast<SimpleVector*>((*itCurrent)->getLambdaPtr(levelMin));

    //      static_cast<SimpleVector*>(w)->getBlock(pos,nsLawSize, *y) ;
    //      static_cast<SimpleVector*>(z)->getBlock(pos,nsLawSize, *lambda) ;

    // Copy w/z values, starting from index pos into y/lambda.

    setBlock(w, y, y->size(), pos, 0);
    setBlock(z, lambda, lambda->size(), pos, 0);

  }
}

void FrictionContact::display() const
{
  cout << "======= FrictionContact display ======" << endl;
  cout << "| sizeOutput : " << sizeOutput << endl;
  cout << "| FrictionContact Matrix M  : " << endl;
  if (M != NULL) M->display();
  else cout << "-> NULL" << endl;
  cout << "| FrictionContact vector q : " << endl;
  if (q != NULL) q->display();
  else cout << "-> NULL" << endl;
  cout << "==========================" << endl;

}

void FrictionContact::saveNSProblemToXML()
{
  OneStepNSProblem::saveNSProblemToXML();
  if (onestepnspbxml != NULL)
  {
    (static_cast<FrictionContactXML*>(onestepnspbxml))->setM(*M);
    (static_cast<FrictionContactXML*>(onestepnspbxml))->setQ(*q);
  }
  else RuntimeException::selfThrow("FrictionContact::saveNSProblemToXML - OneStepNSProblemXML object not exists");
}

void FrictionContact::saveMToXML()
{
  if (onestepnspbxml != NULL)
  {
    (static_cast<FrictionContactXML*>(onestepnspbxml))->setM(*M);
  }
  else RuntimeException::selfThrow("FrictionContact::saveMToXML - OneStepNSProblemXML object not exists");
}

void FrictionContact::saveQToXML()
{
  if (onestepnspbxml != NULL)
  {
    (static_cast<FrictionContactXML*>(onestepnspbxml))->setQ(*q);
  }
  else RuntimeException::selfThrow("FrictionContact::saveQToXML - OneStepNSProblemXML object not exists");
}

FrictionContact* FrictionContact::convert(OneStepNSProblem* osnsp)
{
  FrictionContact* fc2d = dynamic_cast<FrictionContact*>(osnsp);
  return fc2d;
}


