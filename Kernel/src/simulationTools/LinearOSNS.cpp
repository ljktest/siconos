/* Siconos-Kernel, Copyright INRIA 2005-2010.
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
#include "LinearOSNS.hpp"
#include "OneStepNSProblemXML.hpp"
#include "Simulation.hpp"
#include "Topology.hpp"
#include "Model.hpp"
#include "Moreau.hpp"
#include "Lsodar.hpp"
#include "NewtonEulerR.hpp"
#include "FirstOrderLinearR.hpp"
#include "FirstOrderLinearTIR.hpp"
#include "LagrangianLinearTIR.hpp"
#include "NewtonImpactNSL.hpp"
#include "NewtonImpactFrictionNSL.hpp"
#include "NonSmoothDynamicalSystem.hpp"
#include "LagrangianRheonomousR.hpp"

using namespace std;
using namespace RELATION;
LinearOSNS::LinearOSNS(): _MStorageType(0), _keepLambdaAndYState(false)
{
}
// xml constructor
LinearOSNS::LinearOSNS(SP::OneStepNSProblemXML onestepnspbxml,
                       const string & name) :
  OneStepNSProblem(onestepnspbxml), _MStorageType(0), _keepLambdaAndYState(false)
{
  // Read storage type if given (optional , default = dense)
  if (onestepnspbxml->hasStorageType())
    _MStorageType = onestepnspbxml->getStorageType();
}

// Constructor from a set of data
LinearOSNS::LinearOSNS(const int numericsSolverId,  const string& name,
                       const string& newId):
  OneStepNSProblem(newId, numericsSolverId), _MStorageType(0), _keepLambdaAndYState(false)
{}

LinearOSNS::LinearOSNS(const int numericsSolverId,
                       const std::string& name):
  OneStepNSProblem(numericsSolverId), _MStorageType(0), _keepLambdaAndYState(false)
{}
// Setters

void LinearOSNS::setW(const SiconosVector& newValue)
{
  assert(_sizeOutput == newValue.size() &&
         "LinearOSNS: setW, inconsistent size between given velocity size and problem size. You should set sizeOutput before");
  setObject<SimpleVector, SP::SiconosVector, SiconosVector>(_w, newValue);
}

void LinearOSNS::setz(const SiconosVector& newValue)
{
  assert(_sizeOutput == newValue.size() &&
         "LinearOSNS: setz, inconsistent size between given velocity size and problem size. You should set sizeOutput before");
  setObject<SimpleVector, SP::SiconosVector, SiconosVector>(_z, newValue);
}

void LinearOSNS::setM(const OSNSMatrix& newValue)
{
  // Useless ?
  RuntimeException::selfThrow("LinearOSNS: setM, forbidden operation. Try setMPtr().");
}

void LinearOSNS::initVectorsMemory()
{
  // Memory allocation for _w, M, z and q.
  // If one of them has already been allocated, nothing is done.
  // We suppose that user has chosen a correct size.
  if (! _w)
    _w.reset(new SimpleVector(maxSize()));
  else
  {
    if (_w->size() != maxSize())
      _w->resize(maxSize());
  }

  if (! _z)
    _z.reset(new SimpleVector(maxSize()));
  else
  {
    if (_z->size() != maxSize())
      _z->resize(maxSize());
  }

  if (! _q)
    _q.reset(new SimpleVector(maxSize()));
  else
  {
    if (_q->size() != maxSize())
      _q->resize(maxSize());
  }
}

void LinearOSNS::initialize(SP::Simulation sim)
{
  // - Checks memory allocation for main variables (_M,q,_w,z)
  // - Formalizes the problem if the topology is time-invariant

  // This function performs all steps that are time-invariant

  // General initialize for OneStepNSProblem
  OneStepNSProblem::initialize(sim);

  initVectorsMemory();

  // get topology
  SP::Topology topology = simulation()->model()
                          ->nonSmoothDynamicalSystem()->topology();

  // Note that _unitaryBlocks is up to date since updateUnitaryBlocks
  // has been called during OneStepNSProblem::initialize()

  // If the topology is TimeInvariant ie if M structure does not
  // change during simulation:
  bool b = topology->isTimeInvariant();
  bool isLinear = simulation()->model()->nonSmoothDynamicalSystem()->isLinear();
  if ((b || !isLinear) &&   !interactions()->isEmpty())
  {
    updateM();
  }
  else // in that case, M will be updated during preCompute
  {
    // Default size for M = maxSize()
    if (! _M)
    {
      if (_MStorageType == 0)
        _M.reset(new OSNSMatrix(maxSize(), 0));

      else // if(_MStorageType == 1) size = number of _unitaryBlocks
        // = number of UR in the largest considered indexSet
        _M.reset(new OSNSMatrix(simulation()->indexSet(levelMin())->size(), 1));
    }
  }
}

void LinearOSNS::updateM()
{
  // Get index set from Simulation
  SP::UnitaryRelationsGraph indexSet = simulation()->indexSet(levelMin());
  if (! _M) // Creates and fills M using UR of indexSet
    _M.reset(new OSNSMatrix(indexSet, _unitaryBlocks, _MStorageType));

  else
  {
    _M->setStorageType(_MStorageType);
    _M->fill(indexSet, _unitaryBlocks);
  }
  _sizeOutput = _M->size();
}

void LinearOSNS::computeUnitaryBlock(SP::UnitaryRelation UR1, SP::UnitaryRelation UR2)
{

  // Computes matrix _unitaryBlocks[UR1][UR2] (and allocates memory if
  // necessary) if UR1 and UR2 have commond DynamicalSystem.  How
  // _unitaryBlocks are computed depends explicitely on the type of
  // Relation of each UR.

  // Get DS common between UR1 and UR2
  DynamicalSystemsSet commonDS;
  intersection(*UR1->dynamicalSystems(), *UR2->dynamicalSystems(), commonDS);
  assert(!commonDS.isEmpty()) ;

  // Warning: we suppose that at this point, all non linear
  // operators (G for lagrangian relation for example) have been
  // computed through plug-in mechanism.

  // Get dimension of the NonSmoothLaw (ie dim of the unitaryBlock)
  unsigned int nslawSize1 = UR1->getNonSmoothLawSize();
  unsigned int nslawSize2 = UR2->getNonSmoothLawSize();
  // Check allocation
  if (! _unitaryBlocks[UR1][UR2])
    _unitaryBlocks[UR1][UR2].reset(new SimpleMatrix(nslawSize1, nslawSize2));

  // Get the W and Theta maps of one of the Unitary Relation -
  // Warning: in the current version, if OSI!=Moreau, this fails.
  // If OSI = MOREAU, centralUnitaryBlocks = W if OSI = LSODAR,
  // centralUnitaryBlocks = M (mass matrices)
  MapOfDSMatrices centralUnitaryBlocks;
  getOSIMaps(UR1, centralUnitaryBlocks);

  SP::SiconosMatrix currentUnitaryBlock = _unitaryBlocks[UR1][UR2];

  SP::SiconosMatrix leftUnitaryBlock, rightUnitaryBlock;

  unsigned int sizeDS;
  RELATION::TYPES relationType1, relationType2;
  double h = simulation()->timeDiscretisation()->currentTimeStep();

  // General form of the unitaryBlock is : unitaryBlock =
  // a*extraUnitaryBlock + b * leftUnitaryBlock * centralUnitaryBlocks
  // * rightUnitaryBlock a and b are scalars, centralUnitaryBlocks a
  // matrix depending on the integrator (and on the DS), the
  // simulation type ...  left, right and extra depend on the relation
  // type and the non smooth law.
  relationType1 = UR1->getRelationType();
  relationType2 = UR2->getRelationType();
  // ==== First Order Relations - Specific treatment for diagonal _unitaryBlocks ===
  if (UR1 == UR2)
    UR1->getExtraUnitaryBlock(currentUnitaryBlock);
  else
    currentUnitaryBlock->zero();


  // loop over the common DS
  for (DSIterator itDS = commonDS.begin(); itDS != commonDS.end(); itDS++)
  {
    sizeDS = (*itDS)->getDim();

    // get _unitaryBlocks corresponding to the current DS
    // These _unitaryBlocks depends on the relation type.
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
    else if (relationType1 == Lagrangian || relationType2 == Lagrangian || relationType1 == NewtonEuler || relationType2 == NewtonEuler)
    {
      if (UR1 == UR2)
      {
        SP::SiconosMatrix work(new SimpleMatrix(*leftUnitaryBlock));
        //
        //        cout<<"LinearOSNS : leftUBlock\n";
        //        work->display();
        work->trans();
        //        cout<<"LinearOSNS::computeUnitaryBlock leftUnitaryBlock"<<endl;
        //        leftUnitaryBlock->display();
        centralUnitaryBlocks[*itDS]->PLUForwardBackwardInPlace(*work);
        //*currentUnitaryBlock +=  *leftUnitaryBlock ** work;
        prod(*leftUnitaryBlock, *work, *currentUnitaryBlock, false);
        //      gemm(CblasNoTrans,CblasNoTrans,1.0,*leftUnitaryBlock,*work,1.0,*currentUnitaryBlock);
        //*currentUnitaryBlock *=h;
        //        cout<<"LinearOSNS::computeUnitaryBlock unitaryBlock"<<endl;
        //        currentUnitaryBlock->display();

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


/* Cascading visitors experimentation.
   the dispatch is done on simulationtype and then on nslawtype

   1. Cumbersome. Is it better than if (simulationtype == ... ) { if
   nslawType == ...) ?
   2. Can simulation dispatch be done at top-level ?

*/


/* nslaw dispatch */

struct LinearOSNS::_TimeSteppingNSLEffect : public SiconosVisitor
{
  LinearOSNS *parent;
  unsigned int pos;
  SP::UnitaryRelation UR;

  _TimeSteppingNSLEffect(LinearOSNS *p, SP::UnitaryRelation UR, unsigned int pos) :
    parent(p), UR(UR), pos(pos) {};

  void visit(const NewtonImpactNSL& nslaw)
  {
    double e;
    e = nslaw.e();
    Index subCoord(4);
    subCoord[0] = 0;
    subCoord[1] = UR->getNonSmoothLawSize();
    subCoord[2] = pos;
    subCoord[3] = pos + subCoord[1];
    subscal(e, *UR->yOld(parent->levelMin()), *(parent->_q), subCoord, false);
  }

  void visit(const NewtonImpactFrictionNSL& nslaw)
  {
    double e;
    e = nslaw.en();
    // Only the normal part is multiplied by e
    (*(parent->_q))(pos) +=  e * (*UR->yOld(parent->levelMin()))(0);

  }
  void visit(const EqualityConditionNSL& nslaw)
  {
    ;
  }
  void visit(const MixedComplementarityConditionNSL& nslaw)
  {
    ;
  }
};

struct LinearOSNS::_EventDrivenNSLEffect : public SiconosVisitor
{
  LinearOSNS *parent;
  SP::UnitaryRelation UR;
  unsigned int pos;

  _EventDrivenNSLEffect(LinearOSNS *p, SP::UnitaryRelation UR, unsigned int pos) :
    parent(p), UR(UR), pos(pos) {};

  void visit(const NewtonImpactNSL& nslaw)
  {
    double e;
    e = nslaw.e();
    Index subCoord(4);
    subCoord[0] = pos;
    subCoord[1] = pos + UR->getNonSmoothLawSize();
    subCoord[2] = pos;
    subCoord[3] = subCoord[1];
    subscal(e, *(parent->_q), *(parent->_q), subCoord, false); // q = q + e * q
  }

  // note : no NewtonImpactFrictionNSL
};



/* Simulation dispatch */
class TimeStepping;
class EventDriven;

struct LinearOSNS::_NSLEffectOnSim : public SiconosVisitor
{

  LinearOSNS *parent;
  SP::UnitaryRelation UR;
  unsigned int pos;

  _NSLEffectOnSim(LinearOSNS *p, SP::UnitaryRelation UR, unsigned int pos) :
    parent(p), UR(UR), pos(pos) {};

  void visit(const TimeStepping& sim)
  {
    SP::SiconosVisitor NSLEffect(new _TimeSteppingNSLEffect(parent, UR, pos));
    UR->interaction()->nonSmoothLaw()->accept(*NSLEffect);
  }

  void visit(const EventDriven& sim)
  {
    SP::SiconosVisitor NSLEffect(new _EventDrivenNSLEffect(parent, UR, pos));
    UR->interaction()->nonSmoothLaw()->accept(*NSLEffect);
  }

};

void LinearOSNS::computeqBlock(SP::UnitaryRelation UR, unsigned int pos)
{

  // Get relation and non smooth law types
  RELATION::TYPES relationType = UR->getRelationType();
  RELATION::SUBTYPES relationSubType = UR->getRelationSubType();

  SP::DynamicalSystem ds = *(UR->dynamicalSystemsBegin());
  OSI::TYPES osiType = simulation()->integratorOfDS(ds)->getType();

  unsigned int sizeY = UR->getNonSmoothLawSize();
  Index coord(8);

  unsigned int relativePosition = UR->getRelativePosition();
  SP::Interaction mainInteraction = UR->interaction();
  coord[0] = relativePosition;
  coord[1] = relativePosition + sizeY;
  coord[2] = 0;
  coord[4] = 0;
  coord[6] = pos;
  coord[7] = pos + sizeY;

  SP::SiconosMatrix  C;
  SP::SiconosMatrix  D;
  SP::SiconosMatrix  F;
  SP::SiconosVector Xq;
  SP::SiconosVector lambda;
  SP::SiconosVector H_alpha;

  Xq = UR->xq();
  lambda = UR->interaction()->lambda(0);
  H_alpha = UR->interaction()->relation()->Halpha();


  // ??? cf svn r 1456 Xfree = UR->workx()
  // ==> Event driven does not work without this

  SP::SiconosVector Xfree;
  if (osiType == OSI::MOREAU)
    Xfree = UR->workFree();
  else if (osiType == OSI::LSODAR)
    Xfree = UR->workx();

  if (osiType == OSI::MOREAU || osiType == OSI::LSODAR)
  {

    if (relationType == FirstOrder && relationSubType == Type2R)
    {
      C = mainInteraction->relation()->C();
      D = mainInteraction->relation()->D();
      if (D)
      {
        coord[3] = D->size(1);
        coord[5] = D->size(1);
        subprod(*D, *lambda, *_q, coord, true);
        for (int i = 0; i < sizeY; i++)
          _q->setValue(relativePosition + i, -1 * _q->getValue(relativePosition + i));

      }
      if (C)
      {
        coord[3] = C->size(1);
        coord[5] = C->size(1);
        subprod(*C, *Xq, *_q, coord, false);

      }
      for (int i = 0; i < sizeY; i++)
        _q->setValue(relativePosition + i, _q->getValue(relativePosition + i) + H_alpha->getValue(relativePosition + i));
    }
    else if (relationType == NewtonEuler)
    {
      SP::SiconosMatrix CT =  boost::static_pointer_cast<NewtonEulerR>(mainInteraction->relation())->jachqT();

      if (CT)
      {

        assert(Xfree);
        assert(_q);

        coord[3] = CT->size(1);
        coord[5] = CT->size(1);

        subprod(*CT, *Xfree, *_q, coord, true);
      }

    }
    else
    {
      C = mainInteraction->relation()->C();

      if (C)
      {

        assert(Xfree);
        assert(_q);

        coord[3] = C->size(1);
        coord[5] = C->size(1);

        subprod(*C, *Xfree, *_q, coord, true);
      }

      if (relationType == Lagrangian && (relationSubType == RheonomousR))
      {
        SP::SiconosMatrix ID(new SimpleMatrix(sizeY, sizeY));
        ID->eye();

        Index xcoord(8);
        xcoord[0] = 0;
        xcoord[1] = sizeY;
        xcoord[2] = 0;
        xcoord[3] = sizeY;
        xcoord[4] = 0;
        xcoord[5] = sizeY;
        xcoord[6] = pos;
        xcoord[7] = pos + sizeY;

        boost::static_pointer_cast<LagrangianRheonomousR>
        (UR->interaction()->relation())->computehDot(simulation()->getTkp1());
        subprod(*ID,
                *(boost::static_pointer_cast<LagrangianRheonomousR>
                  (UR->interaction()->relation())->hDot()),
                *_q,
                xcoord,
                false); // y += hDot
      }
      if (relationType == FirstOrder && (relationSubType == LinearTIR || relationSubType == LinearR))
      {
        // In the first order linear case it may be required to add e + FZ to q.
        // q = HXfree + e + FZ
        SP::SiconosVector e;
        if (relationSubType == LinearTIR)
        {
          e = boost::static_pointer_cast<FirstOrderLinearTIR>(mainInteraction->relation())->e();
          F = boost::static_pointer_cast<FirstOrderLinearTIR>(mainInteraction->relation())->F();
        }
        else
        {
          e = boost::static_pointer_cast<FirstOrderLinearR>(mainInteraction->relation())->e();
          F = boost::static_pointer_cast<FirstOrderLinearR>(mainInteraction->relation())->F();
        }

        if (e)
          boost::static_pointer_cast<SimpleVector>(_q)->addBlock(pos, *e);

        if (F)
        {
          SP::SiconosVector  workZ = UR->workz();
          coord[3] = F->size(1);
          coord[5] = F->size(1);
          subprod(*F, *workZ, *_q, coord, false);
        }
      }

    }
  }

  else if (osiType == OSI::MOREAU2)
  {
  }
  else
    RuntimeException::selfThrow("LinearOSNS::computeqBlock not yet implemented for OSI of type " + osiType);

  // Add "non-smooth law effect" on q
  if (UR->getRelationType() == Lagrangian || UR->getRelationType() == NewtonEuler)
  {
    SP::SiconosVisitor nslEffectOnSim(new _NSLEffectOnSim(this, UR, pos));
    simulation()->accept(*nslEffectOnSim);
  }



}

void LinearOSNS::computeq(double time)
{
  if (_q->size() != _sizeOutput)
    _q->resize(_sizeOutput);
  _q->zero();

  // === Get index set from Simulation ===
  SP::UnitaryRelationsGraph indexSet =
    simulation()->indexSet(levelMin());
  // === Loop through "active" Unitary Relations (ie present in
  // indexSets[level]) ===

  unsigned int pos = 0;
  UnitaryRelationsGraph::VIterator ui, uiend;
  for (boost::tie(ui, uiend) = indexSet->vertices(); ui != uiend; ++ui)
  {
    SP::UnitaryRelation ur = indexSet->bundle(*ui);

    // Compute q, this depends on the type of non smooth problem, on
    // the relation type and on the non smooth law
    pos = _M->getPositionOfUnitaryBlock(ur);
    computeqBlock(ur, pos); // free output is saved in y
  }
}




void LinearOSNS::preCompute(double time)
{
  // This function is used to prepare data for the
  // LinearComplementarityProblem

  // - computation of M and q
  // - set _sizeOutput
  // - check dim. for _z,_w

  // If the topology is time-invariant, only q needs to be computed at
  // each time step.  M, _sizeOutput have been computed in initialize
  // and are uptodate.

  // Get topology
  SP::Topology topology = simulation()->model()
                          ->nonSmoothDynamicalSystem()->topology();
  bool b = topology->isTimeInvariant();
  bool isLinear = simulation()->model()->nonSmoothDynamicalSystem()->isLinear();

  if (!b || !isLinear)
  {
    // Computes new _unitaryBlocks if required
    updateUnitaryBlocks();

    // Updates matrix M
    SP::UnitaryRelationsGraph indexSet = simulation()->indexSet(levelMin());
    _M->fill(indexSet, _unitaryBlocks);
    _sizeOutput = _M->size();

    // Checks z and _w sizes and reset if necessary
    if (_z->size() != _sizeOutput)
    {
      _z->resize(_sizeOutput, false);
      _z->zero();
    }

    if (_w->size() != _sizeOutput)
    {
      _w->resize(_sizeOutput);
      _w->zero();
    }

    // _w and _z <- old values. Note : sizeOuput can be unchanged,
    // but positions may have changed
    if (_keepLambdaAndYState)
    {
      UnitaryRelationsGraph::VIterator ui, uiend;
      for (boost::tie(ui, uiend) = indexSet->vertices(); ui != uiend; ++ui)
      {
        SP::UnitaryRelation ur = indexSet->bundle(*ui);
        // Get the relative position of UR-unitaryBlock in the vector w
        // or z
        unsigned int pos = _M->getPositionOfUnitaryBlock(ur);

        SPC::SiconosVector yOld = ur->yOld(levelMin());
        SPC::SiconosVector lambdaOld = ur->
                                       interaction()->lambdaOld(levelMin());

        setBlock(*yOld, _w, yOld->size(), 0, pos);
        setBlock(*lambdaOld, _z, lambdaOld->size(), 0, pos);
      }
    }
  }

  // Computes q of LinearOSNS
  computeq(time);

}

void LinearOSNS::postCompute()
{
  // This function is used to set y/lambda values using output from
  // lcp_driver (w,z).  Only UnitaryRelations (ie Interactions) of
  // indexSet(leveMin) are concerned.

  // === Get index set from Topology ===
  SP::UnitaryRelationsGraph indexSet = simulation()->indexSet(levelMin());

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
    pos = _M->getPositionOfUnitaryBlock(ur);

    // Get Y and Lambda for the current Unitary Relation
    y = ur->y(levelMin());
    lambda = ur->lambda(levelMin());
    // Copy _w/_z values, starting from index pos into y/lambda.

    setBlock(*_w, y, y->size(), pos, 0);// Warning: yEquivalent is
    // saved in y !!
    setBlock(*_z, lambda, lambda->size(), pos, 0);
  }

}

void LinearOSNS::display() const
{
  cout << "_M  ";
  if (_M) _M->display();
  else cout << "-> NULL" << endl;
  cout << endl << " q : " ;
  if (_q) _q->display();
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
  // //       (boost::static_pointer_cast<LinearOSNSXML>(onestepnspbxml))->setM(*_M);
  //       (boost::static_pointer_cast<LinearOSNSXML>(onestepnspbxml))->setQ(*q);
  //     }
  //   else RuntimeException::selfThrow("LinearOSNS::saveNSProblemToXML - OneStepNSProblemXML object not exists");
  RuntimeException::selfThrow("LinearOSNS::saveNSProblemToXML - Not yet implemented.");
}

