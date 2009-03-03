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
#include "LinearOSNS.h"
#include "OneStepNSProblemXML.h"
#include "Simulation.h"
#include "Topology.h"
#include "Model.h"
#include "Moreau.h"
#include "Lsodar.h"
#include "FirstOrderLinearR.h"
#include "FirstOrderLinearTIR.h"
#include "LagrangianLinearTIR.h"
#include "NewtonImpactNSL.h"
#include "NewtonImpactFrictionNSL.h"
#include "NonSmoothDynamicalSystem.h"

using namespace std;
using namespace RELATION;

// xml constructor
LinearOSNS::LinearOSNS(SP::OneStepNSProblemXML onestepnspbxml,
                       const string & name) :
  OneStepNSProblem(name, onestepnspbxml), MStorageType(0), keepLambdaAndYState(false)
{
  // Read storage type if given (optional , default = dense)
  if (onestepnspbxml->hasStorageType())
    MStorageType = onestepnspbxml->getStorageType();
}

// Constructor from a set of data
LinearOSNS::LinearOSNS(const string& name, SP::NonSmoothSolver newSolver,
                       const string& newId):
  OneStepNSProblem(name, newId, newSolver), MStorageType(0), keepLambdaAndYState(false)
{}

// Setters

void LinearOSNS::setW(const SiconosVector& newValue)
{
  assert(sizeOutput == newValue.size() &&
         "LinearOSNS: setW, inconsistent size between given velocity size and problem size. You should set sizeOutput before");
  setObject<SimpleVector, SP::SiconosVector, SiconosVector>(_w, newValue);
}

void LinearOSNS::setZ(const SiconosVector& newValue)
{
  assert(sizeOutput == newValue.size() &&
         "LinearOSNS: setZ, inconsistent size between given velocity size and problem size. You should set sizeOutput before");
  setObject<SimpleVector, SP::SiconosVector, SiconosVector>(_z, newValue);
}

void LinearOSNS::setM(const OSNSMatrix& newValue)
{
  // Useless ?
  RuntimeException::selfThrow("LinearOSNS: setM, forbidden operation. Try setMPtr().");
}

void LinearOSNS::setQ(const SiconosVector& newValue)
{
  assert(sizeOutput == newValue.size() &&
         "LinearOSNS: setQ, inconsistent size between given velocity size and problem size. You should set sizeOutput before");
  setObject<SimpleVector, SP::SiconosVector, SiconosVector>(q, newValue);
}

void LinearOSNS::initVectorsMemory()
{
  // Memory allocation for _w, M, z and q.
  // If one of them has already been allocated, nothing is done.
  // We suppose that user has chosen a correct size.
  if (! _w)
    _w.reset(new SimpleVector(maxSize));
  else
  {
    if (_w->size() != maxSize)
      _w->resize(maxSize);
  }

  if (! _z)
    _z.reset(new SimpleVector(maxSize));
  else
  {
    if (_z->size() != maxSize)
      _z->resize(maxSize);
  }

  if (! q)
    q.reset(new SimpleVector(maxSize));
  else
  {
    if (q->size() != maxSize)
      q->resize(maxSize);
  }
}

void LinearOSNS::initialize(SP::Simulation sim)
{
  // - Checks memory allocation for main variables (M,q,_w,z)
  // - Formalizes the problem if the topology is time-invariant

  // This function performs all steps that are time-invariant

  // General initialize for OneStepNSProblem
  OneStepNSProblem::initialize(sim);

  initVectorsMemory();

  // get topology
  SP::Topology topology = simulation->getModelPtr()
                          ->getNonSmoothDynamicalSystemPtr()->getTopologyPtr();

  // Note that unitaryBlocks is up to date since updateUnitaryBlocks
  // has been called during OneStepNSProblem::initialize()

  // If the topology is TimeInvariant ie if M structure does not
  // change during simulation:
  if (topology->isTimeInvariant() &&   !OSNSInteractions->isEmpty())
  {
    updateM();
  }
  else // in that case, M will be updated during preCompute
  {
    // Default size for M = maxSize
    if (! M)
    {
      if (MStorageType == 0)
        M.reset(new OSNSMatrix(maxSize, 0));

      else // if(MStorageType == 1) size = number of unitaryBlocks
        // = number of UR in the largest considered indexSet
        M.reset(new OSNSMatrix(simulation->getIndexSetPtr(levelMin)->size(), 1));
    }
  }
}

void LinearOSNS::updateM()
{
  // Get index set from Simulation
  SP::UnitaryRelationsGraph indexSet = simulation->getIndexSetPtr(levelMin);
  if (! M) // Creates and fills M using UR of indexSet
    M.reset(new OSNSMatrix(indexSet, unitaryBlocks, MStorageType));

  else
  {
    M->setStorageType(MStorageType);
    M->fill(indexSet, unitaryBlocks);
  }
  sizeOutput = M->size();
}

void LinearOSNS::computeUnitaryBlock(SP::UnitaryRelation UR1, SP::UnitaryRelation UR2)
{

  // Computes matrix unitaryBlocks[UR1][UR2] (and allocates memory if
  // necessary) if UR1 and UR2 have commond DynamicalSystem.  How
  // unitaryBlocks are computed depends explicitely on the type of
  // Relation of each UR.

  // Get DS common between UR1 and UR2
  DynamicalSystemsSet commonDS;
  intersection(*UR1->getDynamicalSystemsPtr(), *UR2->getDynamicalSystemsPtr(), commonDS);
  assert(!commonDS.isEmpty()) ;

  // Warning: we suppose that at this point, all non linear
  // operators (G for lagrangian relation for example) have been
  // computed through plug-in mechanism.

  // Get dimension of the NonSmoothLaw (ie dim of the unitaryBlock)
  unsigned int nslawSize1 = UR1->getNonSmoothLawSize();
  unsigned int nslawSize2 = UR2->getNonSmoothLawSize();
  // Check allocation
  if (! unitaryBlocks[UR1][UR2])
    unitaryBlocks[UR1][UR2].reset(new SimpleMatrix(nslawSize1, nslawSize2));

  // Get the W and Theta maps of one of the Unitary Relation -
  // Warning: in the current version, if OSI!=Moreau, this fails.
  // If OSI = MOREAU, centralUnitaryBlocks = W if OSI = LSODAR,
  // centralUnitaryBlocks = M (mass matrices)
  MapOfDSMatrices centralUnitaryBlocks;
  MapOfDouble Theta; // If OSI = LSODAR, Theta remains empty
  getOSIMaps(UR1, centralUnitaryBlocks, Theta);

  SP::SiconosMatrix currentUnitaryBlock = unitaryBlocks[UR1][UR2];

  SP::SiconosMatrix leftUnitaryBlock, rightUnitaryBlock;

  unsigned int sizeDS;
  RELATION::TYPES relationType1, relationType2;
  double h = simulation->getTimeDiscretisationPtr()->getCurrentTimeStep();

  // General form of the unitaryBlock is : unitaryBlock =
  // a*extraUnitaryBlock + b * leftUnitaryBlock * centralUnitaryBlocks
  // * rightUnitaryBlock a and b are scalars, centralUnitaryBlocks a
  // matrix depending on the integrator (and on the DS), the
  // simulation type ...  left, right and extra depend on the relation
  // type and the non smooth law.
  relationType1 = UR1->getRelationType();
  relationType2 = UR2->getRelationType();
  // ==== First Order Relations - Specific treatment for diagonal unitaryBlocks ===
  if (UR1 == UR2)
    UR1->getExtraUnitaryBlock(currentUnitaryBlock);
  else
    currentUnitaryBlock->zero();


  // loop over the common DS
  for (DSIterator itDS = commonDS.begin(); itDS != commonDS.end(); itDS++)
  {
    sizeDS = (*itDS)->getDim();

    // get unitaryBlocks corresponding to the current DS
    // These unitaryBlocks depends on the relation type.
    leftUnitaryBlock.reset(new SimpleMatrix(nslawSize1, sizeDS));
    UR1->getLeftUnitaryBlockForDS(*itDS, leftUnitaryBlock);

    // Computing depends on relation type -> move this in UnitaryRelation method?
    if (relationType1 == FirstOrder && relationType2 == FirstOrder)
    {

      rightUnitaryBlock.reset(new SimpleMatrix(sizeDS, nslawSize2));

      UR2->getRightUnitaryBlockForDS(*itDS, rightUnitaryBlock);
      // centralUnitaryBlock contains a lu-factorized matrix and we solve
      // centralUnitaryBlock * X = rightUnitaryBlock with PLU
      centralUnitaryBlocks[*itDS]->PLUForwardBackwardInPlace(*rightUnitaryBlock);
      //      integration of r with theta method removed
      //      *currentUnitaryBlock += h *Theta[*itDS]* *leftUnitaryBlock * (*rightUnitaryBlock); //left = C, right = W.B
      //gemm(h,*leftUnitaryBlock,*rightUnitaryBlock,1.0,*currentUnitaryBlock);
      *leftUnitaryBlock *= h;
      prod(*leftUnitaryBlock, *rightUnitaryBlock, *currentUnitaryBlock, false);
      //left = C, right = inv(W).B

    }
    else if (relationType1 == Lagrangian || relationType2 == Lagrangian)
    {
      if (UR1 == UR2)
      {
        SP::SiconosMatrix work(new SimpleMatrix(*leftUnitaryBlock));
        work->trans();
        centralUnitaryBlocks[*itDS]->PLUForwardBackwardInPlace(*work);
        //*currentUnitaryBlock +=  *leftUnitaryBlock ** work;
        prod(*leftUnitaryBlock, *work, *currentUnitaryBlock, false);
        //      gemm(CblasNoTrans,CblasNoTrans,1.0,*leftUnitaryBlock,*work,1.0,*currentUnitaryBlock);
      }
      else
      {
        rightUnitaryBlock.reset(new SimpleMatrix(nslawSize2, sizeDS));
        UR2->getLeftUnitaryBlockForDS(*itDS, rightUnitaryBlock);
        // Warning: we use getLeft for Right unitaryBlock
        // because right = transpose(left) and because of
        // size checking inside the getBlock function, a
        // getRight call will fail.
        rightUnitaryBlock->trans();
        centralUnitaryBlocks[*itDS]->PLUForwardBackwardInPlace(*rightUnitaryBlock);
        //*currentUnitaryBlock +=  *leftUnitaryBlock ** work;
        prod(*leftUnitaryBlock, *rightUnitaryBlock, *currentUnitaryBlock, false);
      }
    }

    else RuntimeException::selfThrow("LinearOSNS::computeUnitaryBlock not yet implemented for relation of type " + relationType1);
  }
}

void LinearOSNS::computeQBlock(SP::UnitaryRelation UR, unsigned int pos)
{

  // Get relation and non smooth law types
  RELATION::TYPES relationType = UR->getRelationType();
  RELATION::SUBTYPES relationSubType = UR->getRelationSubType();
  string nslawType = UR->getNonSmoothLawType();

  string simulationType = simulation->getType();

  SP::DynamicalSystem ds = *(UR->dynamicalSystemsBegin());
  OSI::TYPES osiType = simulation->getIntegratorOfDSPtr(ds)->getType();

  unsigned int sizeY = UR->getNonSmoothLawSize();
  std::vector<unsigned int> coord(8);

  unsigned int relativePosition = UR->getRelativePosition();
  SP::Interaction mainInteraction = UR->getInteractionPtr();
  coord[0] = relativePosition;
  coord[1] = relativePosition + sizeY;
  coord[2] = 0;
  coord[4] = 0;
  coord[6] = pos;
  coord[7] = pos + sizeY;

  SP::SiconosMatrix  H;
  SP::SiconosVector workX;

  workX = UR->getWorkXPtr();

  if (osiType == OSI::MOREAU || osiType == OSI::LSODAR)
  {
    H = mainInteraction->getRelationPtr()->getJacHPtr(0);
    if (H)
    {

      assert(workX);
      assert(q);

      coord[3] = H->size(1);
      coord[5] = H->size(1);
      subprod(*H, *workX, *q, coord, true);
    }

    if (relationType == FirstOrder && (relationSubType == LinearTIR || relationSubType == LinearR))
    {
      // In the first order linear case it may be required to add e + FZ to q.
      // q = HXfree + e + FZ
      SP::SiconosVector e;
      if (relationSubType == LinearTIR)
      {
        e = boost::static_pointer_cast<FirstOrderLinearTIR>(mainInteraction->getRelationPtr())->getEPtr();
        H = boost::static_pointer_cast<FirstOrderLinearTIR>(mainInteraction->getRelationPtr())->getFPtr();
      }
      else
      {
        e = boost::static_pointer_cast<FirstOrderLinearR>(mainInteraction->getRelationPtr())->getEPtr();
        H = boost::static_pointer_cast<FirstOrderLinearR>(mainInteraction->getRelationPtr())->getFPtr();
      }

      if (e)
        boost::static_pointer_cast<SimpleVector>(q)->addBlock(pos, *e);

      if (H)
      {
        SP::SiconosVector  workZ = UR->getWorkZPtr();
        coord[3] = H->size(1);
        coord[5] = H->size(1);
        subprod(*H, *workZ, *q, coord, false);
      }
    }
  }

  else if (osiType == OSI::MOREAU2)
  {
  }
  else
    RuntimeException::selfThrow("LinearOSNS::computeQBlock not yet implemented for OSI of type " + osiType);

  // Add "non-smooth law effect" on q
  if (UR->getRelationType() == Lagrangian)
  {
    double e;
    if (nslawType == NEWTONIMPACTNSLAW)
    {

      e = (boost::static_pointer_cast<NewtonImpactNSL>(mainInteraction->getNonSmoothLawPtr()))->getE();
      std::vector<unsigned int> subCoord(4);
      if (simulationType == "TimeStepping")
      {
        subCoord[0] = 0;
        subCoord[1] = UR->getNonSmoothLawSize();
        subCoord[2] = pos;
        subCoord[3] = pos + subCoord[1];
        subscal(e, *UR->getYOldPtr(levelMin), *q, subCoord, false);
      }
      else if (simulationType == "EventDriven")
      {
        subCoord[0] = pos;
        subCoord[1] = pos + UR->getNonSmoothLawSize();
        subCoord[2] = pos;
        subCoord[3] = subCoord[1];
        subscal(e, *q, *q, subCoord, false); // q = q + e * q
      }
      else
        RuntimeException::selfThrow("LinearOSNS::computeQBlock not yet implemented for this type of relation and a non smooth law of type " + nslawType + " for a simulaton of type " + simulationType);
    }
    else if (nslawType == NEWTONIMPACTFRICTIONNSLAW)
    {
      e = (boost::static_pointer_cast<NewtonImpactFrictionNSL>(mainInteraction->getNonSmoothLawPtr()))->getEn();
      // Only the normal part is multiplied by e
      if (simulationType == "TimeStepping")
        (*q)(pos) +=  e * (*UR->getYOldPtr(levelMin))(0);

      else RuntimeException::selfThrow("LinearOSNS::computeQBlock not yet implemented for this type of relation and a non smooth law of type " + nslawType + " for a simulaton of type " + simulationType);

    }
    else
      RuntimeException::selfThrow("LinearOSNS::computeQBlock not yet implemented for this type of relation and a non smooth law of type " + nslawType);
  }
}

void LinearOSNS::computeQ(double time)
{
  if (q->size() != sizeOutput)
    q->resize(sizeOutput);
  q->zero();

  // === Get index set from Simulation ===
  SP::UnitaryRelationsGraph indexSet =
    simulation->getIndexSetPtr(levelMin);
  // === Loop through "active" Unitary Relations (ie present in
  // indexSets[level]) ===

  unsigned int pos = 0;
  UnitaryRelationsGraph::VIterator ui, uiend;
  for (boost::tie(ui, uiend) = indexSet->vertices(); ui != uiend; ++ui)
  {
    SP::UnitaryRelation ur = indexSet->bundle(*ui);

    // Compute q, this depends on the type of non smooth problem, on
    // the relation type and on the non smooth law
    pos = M->getPositionOfUnitaryBlock(ur);
    computeQBlock(ur, pos); // free output is saved in y
  }
}

void LinearOSNS::preCompute(double time)
{
  // This function is used to prepare data for the
  // LinearComplementarity_Problem

  // - computation of M and q
  // - set sizeOutput
  // - check dim. for _z,_w

  // If the topology is time-invariant, only q needs to be computed at
  // each time step.  M, sizeOutput have been computed in initialize
  // and are uptodate.

  // Get topology
  SP::Topology topology = simulation->getModelPtr()
                          ->getNonSmoothDynamicalSystemPtr()->getTopologyPtr();

  if (!topology->isTimeInvariant())
  {
    // Computes new unitaryBlocks if required
    updateUnitaryBlocks();

    // Updates matrix M
    SP::UnitaryRelationsGraph indexSet = simulation->getIndexSetPtr(levelMin);
    M->fill(indexSet, unitaryBlocks);
    sizeOutput = M->size();

    // Checks z and _w sizes and reset if necessary
    if (_z->size() != sizeOutput)
    {
      _z->resize(sizeOutput, false);
      _z->zero();
    }

    if (_w->size() != sizeOutput)
    {
      _w->resize(sizeOutput);
      _w->zero();
    }

    // _w and _z <- old values. Note : sizeOuput can be unchanged,
    // but positions may have changed
    if (keepLambdaAndYState)
    {
      UnitaryRelationsGraph::VIterator ui, uiend;
      for (boost::tie(ui, uiend) = indexSet->vertices(); ui != uiend; ++ui)
      {
        SP::UnitaryRelation ur = indexSet->bundle(*ui);
        // Get the relative position of UR-unitaryBlock in the vector w
        // or z
        unsigned int pos = M->getPositionOfUnitaryBlock(ur);

        SPC::SiconosVector yOld = ur->getYOldPtr(levelMin);
        SPC::SiconosVector lambdaOld = ur->
                                       getInteractionPtr()->getLambdaOldPtr(levelMin);

        setBlock(*yOld, _w, yOld->size(), 0, pos);
        setBlock(*lambdaOld, _z, lambdaOld->size(), 0, pos);
      }
    }
  }

  // Computes q of LinearOSNS
  computeQ(time);

}

void LinearOSNS::postCompute()
{
  // This function is used to set y/lambda values using output from
  // lcp_driver (w,z).  Only UnitaryRelations (ie Interactions) of
  // indexSet(leveMin) are concerned.

  // === Get index set from Topology ===
  SP::UnitaryRelationsGraph indexSet = simulation->getIndexSetPtr(levelMin);

  // y and lambda vectors
  SP::SiconosVector lambda;
  SP::SiconosVector y;

  // === Loop through "active" Unitary Relations (ie present in
  // indexSets[1]) ===

  unsigned int pos = 0;

  UnitaryRelationsGraph::VIterator ui, uiend;
  for (boost::tie(ui, uiend) = indexSet->vertices(); ui != uiend; ++ui)
  {
    SP::UnitaryRelation ur = indexSet->bundle(*ui);
    // Get the relative position of UR-unitaryBlock in the vector w
    // or z
    pos = M->getPositionOfUnitaryBlock(ur);

    // Get Y and Lambda for the current Unitary Relation
    y = ur->getYPtr(levelMin);
    lambda = ur->getLambdaPtr(levelMin);
    // Copy _w/_z values, starting from index pos into y/lambda.

    setBlock(*_w, y, y->size(), pos, 0);// Warning: yEquivalent is
    // saved in y !!
    setBlock(*_z, lambda, lambda->size(), pos, 0);
  }

}

void LinearOSNS::display() const
{
  cout << "M  ";
  if (M) M->display();
  else cout << "-> NULL" << endl;
  cout << endl << " q : " ;
  if (q) q->display();
  else cout << "-> NULL" << endl;
  cout << "w  ";
  if (_w) _w->display();
  else cout << "-> NULL" << endl;
  cout << endl << "z : " ;
  if (_z) _z->display();
  else cout << "-> NULL" << endl;
  cout << "==========================" << endl;
}

void LinearOSNS::saveNSProblemToXML()
{
  //   if(onestepnspbxml != NULL)
  //     {
  // //       (boost::static_pointer_cast<LinearOSNSXML>(onestepnspbxml))->setM(*M);
  //       (boost::static_pointer_cast<LinearOSNSXML>(onestepnspbxml))->setQ(*q);
  //     }
  //   else RuntimeException::selfThrow("LinearOSNS::saveNSProblemToXML - OneStepNSProblemXML object not exists");
  RuntimeException::selfThrow("LinearOSNS::saveNSProblemToXML - Not yet implemented.");
}

