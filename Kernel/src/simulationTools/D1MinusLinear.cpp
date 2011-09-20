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

#include "D1MinusLinear.hpp"
#include "Simulation.hpp"
#include "LagrangianLinearTIDS.hpp"
#include "LagrangianRheonomousR.hpp"
#include "NewtonImpactNSL.hpp"
#include "NewtonImpactFrictionNSL.hpp"

using namespace std;
using namespace RELATION;

struct D1MinusLinear::_NSLEffectOnFreeOutput : public SiconosVisitor
{
  OneStepNSProblem *osnsp;
  SP::UnitaryRelation UR;

  _NSLEffectOnFreeOutput(OneStepNSProblem *p, SP::UnitaryRelation UR) : osnsp(p), UR(UR) {};

  void visit(const NewtonImpactNSL& nslaw)
  {
    double e = nslaw.e();
    Index subCoord(4);
    subCoord[0] = 0;
    subCoord[1] = UR->getNonSmoothLawSize();
    subCoord[2] = 0;
    subCoord[3] = subCoord[1];
    subscal(e, *(UR->y_k(osnsp->levelMin())), *(UR->yp()), subCoord, false);
  }
  void visit(const NewtonImpactFrictionNSL& nslaw)
  {
    double e = nslaw.en();
    (*(UR->yp()))(0) +=  e * (*(UR->y_k(osnsp->levelMin())))(0);
  }

  void visit(const EqualityConditionNSL& nslaw) {}

  void visit(const MixedComplementarityConditionNSL& nslaw) {}
};

// --- constructor from a ds ---
D1MinusLinear::D1MinusLinear(SP::DynamicalSystem newDS) :
  OneStepIntegrator(OSI::D1MINUSLINEAR)
{
  OSIDynamicalSystems->insert(newDS);
}

// --- constructor from a list of ds ---
D1MinusLinear::D1MinusLinear(DynamicalSystemsSet& newDS): OneStepIntegrator(OSI::D1MINUSLINEAR, newDS) {}

const SimpleMatrix D1MinusLinear::getW(SP::DynamicalSystem ds)
{
  assert(ds && "D1MinusLinear::getW(ds) - ds == NULL.");
  assert(WMap[ds] && "D1MinusLinear::getW(ds) - W[ds] == NULL.");
  return *(WMap[ds]); // copy
}

SP::SimpleMatrix D1MinusLinear::W(SP::DynamicalSystem ds)
{
  assert(ds && "D1MinusLinear::W(ds) - ds == NULL.");
  return WMap[ds];
}

void D1MinusLinear::setW(const SiconosMatrix& newValue, SP::DynamicalSystem ds)
{
  // check if ds is in the OSI
  if (!OSIDynamicalSystems->isIn(ds))
    RuntimeException::selfThrow("D1MinusLinear::setW(newVal,ds) - ds does not belong to this Integrator...");

  // check dimensions consistency
  unsigned int line = newValue.size(0);
  unsigned int col  = newValue.size(1);

  if (line != col)
    RuntimeException::selfThrow("D1MinusLinear::setW(newVal,ds) - newVal is not square.");

  if (!ds)
    RuntimeException::selfThrow("D1MinusLinear::setW(newVal,ds) - ds == NULL.");

  unsigned int sizeW = ds->getDim();
  if (line != sizeW)
    RuntimeException::selfThrow("D1MinusLinear::setW(newVal,ds) - inconsistent dimension between newVal and dynamical system to be integrated.");

  // memory allocation for W, if required
  if (!WMap[ds])
    WMap[ds].reset(new SimpleMatrix(newValue));
  else if (line == WMap[ds]->size(0) && col == WMap[ds]->size(1))
    *(WMap[ds]) = newValue;
  else
    RuntimeException::selfThrow("D1MinusLinear::setW(newVal,ds) - inconsistent dimensions with problem size for given input matrix W.");
}

void D1MinusLinear::setWPtr(SP::SimpleMatrix newPtr, SP::DynamicalSystem ds)
{
  unsigned int line = newPtr->size(0);
  unsigned int col  = newPtr->size(1);
  if (line != col)
    RuntimeException::selfThrow("D1MinusLinear::setWPtr(newVal,ds) - newVal is not square.");

  if (!ds)
    RuntimeException::selfThrow("D1MinusLinear::setWPtr(newVal,ds) - ds == NULL.");

  unsigned int sizeW = ds->getDim();
  if (line != sizeW)
    RuntimeException::selfThrow("D1MinusLinear::setW(newVal,ds) - inconsistent dimension between newVal and dynamical system to be integrated.");

  WMap[ds] = newPtr;
}

void D1MinusLinear::initialize()
{
  OneStepIntegrator::initialize();

  double t0 = simulationLink->model()->t0();

  for (ConstDSIterator itDS = OSIDynamicalSystems->begin(); itDS != OSIDynamicalSystems->end(); ++itDS)
  {
    initW(t0, *itDS);
    (*itDS)->allocateWorkVector(DynamicalSystem::local_buffer, WMap[*itDS]->size(0));
  }
}

void D1MinusLinear::initW(double t, SP::DynamicalSystem ds)
{
  if (!ds)
    RuntimeException::selfThrow("D1MinusLinear::initW(t,ds) - ds == NULL");

  if (!OSIDynamicalSystems->isIn(ds))
    RuntimeException::selfThrow("D1MinusLinear::initW(t,ds) - ds does not belong to the OSI.");

  if (WMap.find(ds) != WMap.end())
    RuntimeException::selfThrow("D1MinusLinear::initW(t,ds) - W(ds) is already in the map and has been initialized.");

  Type::Siconos dsType = Type::value(*ds);

  // Lagrangian Systems
  if (dsType == Type::LagrangianDS || dsType == Type::LagrangianLinearTIDS)
  {
    SP::LagrangianDS d = boost::static_pointer_cast<LagrangianDS> (ds);
    WMap[ds].reset(new SimpleMatrix(*(d->mass())));
    SP::SiconosMatrix W = WMap[ds];
  }
  else RuntimeException::selfThrow("D1MinusLinear::initW(t,ds) - not yet implemented for Dynamical system type: " + dsType);
}

void D1MinusLinear::computeW(double t, SP::DynamicalSystem ds)
{
  assert(ds && "D1MinusLinear::computeW(t,ds) - ds == NULL.");
  assert((WMap.find(ds) != WMap.end()) && "D1MinusLinear::computeW(t,ds) - W(ds) does not exists. Maybe you forget to initialize the OSI?");

  Type::Siconos dsType = Type::value(*ds);
  SP::SiconosMatrix W = WMap[ds];

  // Lagrangian Nonlinear Systems
  if (dsType == Type::LagrangianDS)
  {
    SP::LagrangianDS d = boost::static_pointer_cast<LagrangianDS> (ds);
    d->computeMass();
    *W = *(d->mass());
  }
  // Lagrangian Linear Systems
  else if (dsType == Type::LagrangianLinearTIDS) {}
  else RuntimeException::selfThrow("D1MinusLinear::computeW(t,ds) - not yet implemented for Dynamical system type: " + dsType);
}

double D1MinusLinear::computeResidu()
{
  double t = simulationLink->nextTime(); // end of the time step
  double told = simulationLink->startingTime(); // beginning of the time step
  double h = simulationLink->timeStep(); // time step length
  SP::OneStepNSProblems allOSNS  = simulationLink->oneStepNSProblems(); // all OSNSP

  // -- LEFT SIDE --
  // solve a LCP at acceleration level
  if (!allOSNS->empty())
  {
    if (!((*allOSNS)[SICONOS_OSNSP_TS_VELOCITY + 1]->interactions())->isEmpty())
    {
      (*allOSNS)[SICONOS_OSNSP_TS_VELOCITY + 1]->compute(told);
      simulationLink->updateInput(2);
    }
  }

  // advance to the right side
  for (DSIterator it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    // type of the current DS
    Type::Siconos dsType = Type::value(**it);
    if (dsType != Type::LagrangianDS && dsType != Type::LagrangianLinearTIDS)
      RuntimeException::selfThrow("D1MinusLinear::computeResidu() - not yet implemented for Dynamical system type: " + dsType);
    SP::LagrangianDS d = boost::static_pointer_cast<LagrangianDS> (*it);

    // get left state from memory
    SP::SiconosVector qold = d->qMemory()->getSiconosVector(0);
    SP::SiconosVector vold = d->velocityMemory()->getSiconosVector(0); // right limit
    SP::SiconosMatrix Mold = d->mass();

    // initialize *it->residuFree and predicted right velocity (left limit)
    SP::SiconosVector residuFree = (*it)->residuFree(); // POINTER CONSTRUCTOR
    SP::SiconosVector v = d->velocity(); // POINTER CONSTRUCTOR
    scal(-0.5 * h, *(d->p(2)), *residuFree, true);
    scal(h, *(d->p(2)), *v, true);

    // Lagrangian Nonlinear Systems
    if (dsType == Type::LagrangianDS)
    {
      if (d->fL())
      {
        d->computeFL(told, qold, vold); // left force vector
        scal(-0.5 * h, *(d->fL()), *residuFree, false);
        scal(h, *(d->fL()), *v, false);
      }
    }
    // Lagrangian Linear Systems
    else if (dsType == Type::LagrangianLinearTIDS)
    {
      SP::LagrangianLinearTIDS d = boost::static_pointer_cast<LagrangianLinearTIDS> (*it);

      // get left state from memory
      SP::SiconosMatrix C = d->C(); // constant dissipation
      SP::SiconosMatrix K = d->K(); // constant stiffness
      SP::SiconosVector Fext = d->fExt(); // time dependent force

      if (Fext)
      {
        d->computeFExt(told);
        scal(-0.5 * h, *Fext, *residuFree, false);
        scal(h, *Fext, *v, false);
      }

      if (K)
      {
        SP::SiconosVector dummy(new SimpleVector(qold->size()));
        prod(*K, *qold, *dummy);
        scal(0.5 * h, *dummy, *residuFree, false);
        scal(-h, *dummy, *v, false);
      }

      if (C)
      {
        SP::SiconosVector dummy(new SimpleVector(vold->size()));
        prod(*C, *vold, *dummy);
        scal(0.5 * h, *dummy, *residuFree, false);
        scal(-h, *dummy, *v, false);
      }
    }

    Mold->PLUForwardBackwardInPlace(*residuFree);
    Mold->PLUForwardBackwardInPlace(*v);

    *v += *vold;
    SP::SiconosVector q = d->q(); // POINTER CONSTRUCTOR
    *q = *qold;
    scal(0.5 * h, *vold + *v, *q, false);
  }

  // -- RIGHT SIDE --
  // solve a LCP at acceleration level
  if (!allOSNS->empty())
  {
    for (unsigned int level = simulationLink->levelMinForOutput(); level < simulationLink->levelMaxForOutput(); level++)
      simulationLink->updateOutput(level);

    simulationLink->updateIndexSets();

    if (!((*allOSNS)[SICONOS_OSNSP_TS_VELOCITY + 1]->interactions())->isEmpty())
    {
      (*allOSNS)[SICONOS_OSNSP_TS_VELOCITY + 1]->compute(t);
      simulationLink->updateInput(2);
    }
  }

  // get current information for right side
  for (DSIterator it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    Type::Siconos dsType = Type::value(**it);
    SP::LagrangianDS d = boost::static_pointer_cast<LagrangianDS> (*it);

    SP::SiconosVector q = d->q(); // POINTER CONSTRUCTOR
    SP::SiconosVector v = d->velocity(); // POINTER CONSTRUCTOR
    SP::SiconosVector residuFree = d->residuFree(); // POINTER CONSTRUCTOR
    SP::SiconosVector dummy(new SimpleVector(-0.5 * h * (*(d->p(2)))));

    if (dsType == Type::LagrangianDS)
    {
      d->computeMass();
      if (d->fL())
      {
        d->computeFL(t, q, v);
        scal(-0.5 * h, *(d->fL()), *dummy, false);
      }
    }
    else if (dsType == Type::LagrangianLinearTIDS)
    {
      SP::LagrangianLinearTIDS d = boost::static_pointer_cast<LagrangianLinearTIDS> (*it);

      SP::SiconosMatrix C = d->C();
      SP::SiconosMatrix K = d->K();
      SP::SiconosVector Fext = d->fExt();

      if (Fext)
      {
        d->computeFExt(t);
        scal(-0.5 * h, *Fext, *dummy, false);
      }

      if (C)
      {
        prod(0.5 * h, *C, *v, *dummy, false);
      }

      if (K)
      {
        prod(0.5 * h, *K, *q, *dummy, false);
      }
    }

    SP::SiconosMatrix M = d->mass();
    prod(*M, *residuFree, *dummy, false);

    *residuFree = *dummy;

    *(d->workFree()) = *residuFree; // copy residuFree in workFree
    *(d->workFree()) -= *(d->p(1)); // subtract nonsmooth reaction on impulse level
  }

  return 0.; // there is no Newton iteration and the residuum is assumed to vanish
}

void D1MinusLinear::computeFreeState()
{
  for (DSIterator it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    Type::Siconos dsType = Type::value(**it); // type of the current DS
    SP::SiconosMatrix W = WMap[*it]; // W iteration matrix of the current DS

    // Lagrangian Systems
    if (dsType != Type::LagrangianDS && dsType != Type::LagrangianLinearTIDS)
      RuntimeException::selfThrow("D1MinusLinear::computeFreeState - not yet implemented for Dynamical system type: " + dsType);

    SP::LagrangianDS d = boost::static_pointer_cast<LagrangianDS> (*it);

    // get left state from memory
    SP::SiconosVector vold = d->velocityMemory()->getSiconosVector(0); // right limit

    // get right information
    SP::SiconosMatrix M = d->mass();
    SP::SiconosVector vfree = d->workFree(); // POINTER CONSTRUCTOR
    (*vfree) = *(d->residuFree());

    M->PLUForwardBackwardInPlace(*vfree);
    *vfree *= -1.;
    *vfree += *vold;
  }
}

void D1MinusLinear::computeFreeOutput(SP::UnitaryRelation UR, OneStepNSProblem* osnsp)
{
  SP::OneStepNSProblems allOSNS  = simulationLink->oneStepNSProblems(); // all OSNSP

  // get relation and non smooth law information
  RELATION::TYPES relationType = UR->getRelationType(); // relation
  RELATION::SUBTYPES relationSubType = UR->getRelationSubType();
  unsigned int relativePosition = UR->getRelativePosition();
  unsigned int sizeY = UR->getNonSmoothLawSize(); // related NSL
  SP::DynamicalSystem ds = *(UR->interaction()->dynamicalSystemsBegin()); // related DS

  Index coord(8);
  coord[0] = relativePosition;
  coord[1] = relativePosition + sizeY;
  coord[2] = 0;
  coord[4] = 0;
  coord[6] = 0;
  coord[7] = sizeY;
  SP::SiconosMatrix C;
  SP::SiconosVector Xq = UR->xq();
  SP::SiconosVector Yp = UR->yp();
  SP::SiconosVector Xfree = UR->workFree();

  assert(Xfree);

  SP::Interaction mainInteraction = UR->interaction();
  assert(mainInteraction);
  assert(mainInteraction->relation());

  // Lagrangian Systems
  C = mainInteraction->relation()->C();

  if (C)
  {
    assert(Xfree);
    assert(Yp);
    assert(Xq);

    coord[3] = C->size(1);
    coord[5] = C->size(1);
    subprod(*C, *Xfree, *Yp, coord, true);
  }
  if (relationType == Lagrangian)
  {
    C = (boost::static_pointer_cast<LagrangianR>(mainInteraction->relation()))->jachq();
    SP::SiconosMatrix C2 = mainInteraction->relation()->C();

    if (C)
    {
      assert(Xfree);
      assert(Yp);
      assert(Xq);

      coord[3] = C->size(1);
      coord[5] = C->size(1);
      subprod(*C, *Xfree, *Yp, coord, true);
    }

    SP::SiconosMatrix ID(new SimpleMatrix(sizeY, sizeY));
    ID->eye();

    Index xcoord(8);
    xcoord[0] = 0;
    xcoord[1] = sizeY;
    xcoord[2] = 0;
    xcoord[3] = sizeY;
    xcoord[4] = 0;
    xcoord[5] = sizeY;
    xcoord[6] = 0;
    xcoord[7] = sizeY;

    if (relationSubType == RheonomousR)
    {
      if (((*allOSNS)[SICONOS_OSNSP_TS_VELOCITY]).get() == osnsp)
      {
        boost::static_pointer_cast<LagrangianRheonomousR>(UR->interaction()->relation())->computehDot(simulation()->getTkp1());
        subprod(*ID, *(boost::static_pointer_cast<LagrangianRheonomousR>(UR->interaction()->relation())->hDot()), *Yp, xcoord, false);
      }
      else
        RuntimeException::selfThrow("D1MinusLinear::computeFreeOutput(ur,osnsp) - not yet implemented for SICONOS_OSNSP.");
    }
    if (relationSubType == ScleronomousR) {}
  }

  // Lagrangian Systems
  if (UR->getRelationType() == Lagrangian)
  {
    SP::SiconosVisitor nslEffectOnFreeOutput(new _NSLEffectOnFreeOutput(osnsp, UR));
    UR->interaction()->nonSmoothLaw()->accept(*nslEffectOnFreeOutput);
  }
}

void D1MinusLinear::updateState(unsigned int level)
{
  for (DSIterator it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    SP::SiconosMatrix W = WMap[*it]; // W iteration matrix of the current DS
    Type::Siconos dsType = Type::value(**it);

    // Lagrangian Systems
    if (dsType != Type::LagrangianDS && dsType != Type::LagrangianLinearTIDS)
      RuntimeException::selfThrow("D1MinusLinear::updateState(level) - not yet implemented for Dynamical system type: " + dsType);

    SP::LagrangianDS d = boost::static_pointer_cast<LagrangianDS> (*it);
    SP::SiconosVector v = d->velocity(); // pointer constructor
    if (level != LEVELMAX)
    {
      *v = *(d->p(level)); // value = nonsmooth impulse
      W->PLUForwardBackwardInPlace(*v); // solution for its velocity equivalent
      *v += *(d->workFree()); // add free velocity
    }
    else
    {
      *v = *(d->workFree());
    }
  }
}

void D1MinusLinear::display()
{
  OneStepIntegrator::display();

  cout << "====== D1MinusLinear OSI display ======" << endl;
  for (DSIterator it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    cout << "--------------------------------" << endl;
    cout << "--> W of dynamical system number " << (*it)->number() << ": " << endl;
    if (WMap[*it]) WMap[*it]->display();
    else cout << "-> NULL" << endl;
  }
  cout << "================================" << endl;
}

void D1MinusLinear::insertDynamicalSystem(SP::DynamicalSystem ds)
{
  OSIDynamicalSystems->insert(ds);
}

D1MinusLinear* D1MinusLinear::convert(OneStepIntegrator* osi)
{
  return dynamic_cast<D1MinusLinear*>(osi);
}
