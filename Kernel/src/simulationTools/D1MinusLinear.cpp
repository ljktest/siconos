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
#include "LagrangianScleronomousR.hpp"
#include "NewtonImpactNSL.hpp"

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
};

D1MinusLinear::D1MinusLinear(SP::DynamicalSystem newDS) :
  OneStepIntegrator(OSI::D1MINUSLINEAR)
{
  OSIDynamicalSystems->insert(newDS);
}

D1MinusLinear::D1MinusLinear(DynamicalSystemsSet& newDS): OneStepIntegrator(OSI::D1MINUSLINEAR, newDS) {}

void D1MinusLinear::initialize()
{
  for (DSIterator it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    Type::Siconos dsType = Type::value(**it);
    if (dsType != Type::LagrangianDS && dsType != Type::LagrangianLinearTIDS)
      RuntimeException::selfThrow("D1MinusLinear::initialize - not implemented for Dynamical system type: " + dsType);
    SP::LagrangianDS d = boost::static_pointer_cast<LagrangianDS> (*it);
    d->computeMass();
  }
}

double D1MinusLinear::computeResidu()
{
  double t = simulationLink->nextTime(); // end of the time step
  double told = simulationLink->startingTime(); // beginning of the time step
  double h = simulationLink->timeStep(); // time step length
  SP::OneStepNSProblems allOSNS  = simulationLink->oneStepNSProblems(); // all OSNSP
  SP::InteractionsSet allInteractions = simulationLink->model()->nonSmoothDynamicalSystem()->interactions(); // all Interactions

  // -- LEFT SIDE --
  // calculate acceleration without contact force
  for (DSIterator it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    // type of the current DS
    Type::Siconos dsType = Type::value(**it);
    SP::LagrangianDS d = boost::static_pointer_cast<LagrangianDS> (*it);
    SP::SiconosVector workFree = d->workFree(); // POINTER CONSTRUCTOR : contains acceleration without contact force
    workFree->zero();

    // get left state from memory
    SP::SiconosVector qold = d->qMemory()->getSiconosVector(0);
    SP::SiconosVector vold = d->velocityMemory()->getSiconosVector(0); // right limit
    SP::SiconosMatrix Mold = d->mass();

    // Lagrangian Nonlinear Systems
    if (dsType == Type::LagrangianDS)
    {
      if (d->forces())
      {
        d->computeForces(told, qold, vold);
        *workFree += *(d->forces());
      }
    }
    // Lagrangian Linear Systems
    else if (dsType == Type::LagrangianLinearTIDS)
    {
      SP::LagrangianLinearTIDS d = boost::static_pointer_cast<LagrangianLinearTIDS> (*it);

      SP::SiconosMatrix C = d->C(); // constant dissipation
      SP::SiconosMatrix K = d->K(); // constant stiffness
      SP::SiconosVector Fext = d->fExt(); // time dependent force

      if (K)
      {
        prod(*K, *qold, *workFree, false);
      }

      if (C)
      {
        prod(*C, *vold, *workFree, false);
      }

      *workFree *= -1.;

      if (Fext)
      {
        d->computeFExt(told);
        *workFree += *Fext;
      }
    }

    Mold->PLUForwardBackwardInPlace(*workFree); // contains left (right limit) acceleration without contact force
  }

  // solve a LCP at acceleration level
  if (!allOSNS->empty())
  {
    for (InteractionsIterator it = allInteractions->begin(); it != allInteractions->end(); it++)
    {
      (*it)->relation()->computeJach(told);
      (*it)->relation()->computeJacg(told);
    }

    if (simulationLink->model()->nonSmoothDynamicalSystem()->topology()->hasChanged())
    {
      for (OSNSIterator itOsns = allOSNS->begin(); itOsns != allOSNS->end(); ++itOsns)
      {
        (*itOsns)->setHasBeUpdated(false);
      }
    }

    if (!((*allOSNS)[SICONOS_OSNSP_TS_VELOCITY + 1]->interactions())->isEmpty())
    {
      (*allOSNS)[SICONOS_OSNSP_TS_VELOCITY + 1]->compute(told);
      simulationLink->updateInput(2);
    }
  }

  // ADVANCE TO RIGHT
  for (DSIterator it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    // type of the current DS
    SP::LagrangianDS d = boost::static_pointer_cast<LagrangianDS> (*it);
    SP::SiconosVector workFree = d->workFree(); // contains acceleration without contact force

    // get left state from memory
    SP::SiconosVector qold = d->qMemory()->getSiconosVector(0);
    SP::SiconosVector vold = d->velocityMemory()->getSiconosVector(0); // right limit
    SP::SiconosMatrix Mold = d->mass();

    // initialize *it->residuFree and predicted right velocity (left limit)
    SP::SiconosVector residuFree = (*it)->residuFree(); // POINTER CONSTRUCTOR : contains residu without nonsmooth effect
    SP::SiconosVector v = d->velocity(); // POINTER CONSTRUCTOR : contains velocity v_{k+1}^- and not free velocity
    residuFree->zero();
    v->zero();
    if (d->p(2))
    {
      scal(-0.5 * h, *(d->p(2)), *residuFree, false);
      scal(h, *(d->p(2)), *v, false);

      cout << "LEFT FORCE" << endl;
      cout << (*(d->p(2)))(0) << endl;

      Mold->PLUForwardBackwardInPlace(*residuFree);
      Mold->PLUForwardBackwardInPlace(*v);
    }

    *residuFree -= 0.5 * h**workFree;

    *v += h**workFree;
    *v += *vold;

    SP::SiconosVector q = d->q(); // POINTER CONSTRUCTOR : contains position q_{k+1}
    *q = *qold;
    scal(0.5 * h, *vold + *v, *q, false);
  }

  // -- RIGHT SIDE --
  // calculate acceleration without contact force
  for (DSIterator it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    // type of the current DS
    Type::Siconos dsType = Type::value(**it);
    SP::LagrangianDS d = boost::static_pointer_cast<LagrangianDS> (*it);
    SP::SiconosVector workFree = d->workFree(); // POINTER CONSTRUCTOR : contains acceleration without contact force
    workFree->zero();

    // get right state from memory
    SP::SiconosVector q = d->q(); // contains position q_{k+1}
    SP::SiconosVector v = d->velocity(); // contains velocity v_{k+1}^- and not free velocity
    SP::SiconosMatrix M = d->mass(); // POINTER CONSTRUCTOR : contains mass matrix

    // Lagrangian Nonlinear Systems
    if (dsType == Type::LagrangianDS)
    {
      d->computeMass();
      if (d->forces())
      {
        d->computeForces(t, q, v);
        *workFree += *(d->forces());
      }
    }
    // Lagrangian Linear Systems
    else if (dsType == Type::LagrangianLinearTIDS)
    {
      SP::LagrangianLinearTIDS d = boost::static_pointer_cast<LagrangianLinearTIDS> (*it);

      SP::SiconosMatrix C = d->C(); // constant dissipation
      SP::SiconosMatrix K = d->K(); // constant stiffness
      SP::SiconosVector Fext = d->fExt(); // time-dependent force

      if (K)
      {
        prod(*K, *q, *workFree, false);
      }

      if (C)
      {
        prod(*C, *v, *workFree, false);
      }

      *workFree *= -1.;

      if (Fext)
      {
        d->computeFExt(t);
        *workFree += *Fext;
      }
    }

    M->PLUForwardBackwardInPlace(*workFree); // contains right (left limit) acceleration without contact force
  }

  // solve a LCP at acceleration level only for contacts which have been active at the beginning of the time-step
  if (!allOSNS->empty())
  {
    for (unsigned int level = simulationLink->levelMinForOutput(); level < simulationLink->levelMaxForOutput(); level++)
      simulationLink->updateOutput(level);

    simulationLink->updateIndexSet(3); // special update to consider only contacts which have been active at the beginning of the time-step

    for (InteractionsIterator it = allInteractions->begin(); it != allInteractions->end(); it++)
    {
      (*it)->relation()->computeJach(t);
      (*it)->relation()->computeJacg(t);
    }

    if (simulationLink->model()->nonSmoothDynamicalSystem()->topology()->hasChanged())
    {
      for (OSNSIterator itOsns = allOSNS->begin(); itOsns != allOSNS->end(); ++itOsns)
      {
        (*itOsns)->setHasBeUpdated(false);
      }
    }

    if (!((*allOSNS)[SICONOS_OSNSP_TS_VELOCITY + 1]->interactions())->isEmpty())
    {
      (*allOSNS)[SICONOS_OSNSP_TS_VELOCITY + 1]->compute(t);
      simulationLink->updateInput(2);
    }
  }

  // -- RESIDU --
  for (DSIterator it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    // type of the current DS
    SP::LagrangianDS d = boost::static_pointer_cast<LagrangianDS> (*it);
    SP::SiconosVector workFree = d->workFree(); // contains acceleration without contact force

    // get right state from memory
    SP::SiconosMatrix M = d->mass();

    // initialize *it->residuFree
    SP::SiconosVector residuFree = (*it)->residuFree(); // POINTER CONSTRUCTOR : contains residu without nonsmooth effect
    *residuFree -= 0.5 * h**workFree;

    if (d->p(2))
    {
      SP::SiconosVector dummy(new SimpleVector(*(d->p(2)))); // value = contact force
      M->PLUForwardBackwardInPlace(*dummy);
      *residuFree -= 0.5 * h**dummy;

      cout << "RIGHT FORCE" << endl;
      cout << (*(d->p(2)))(0) << endl;
    }

    *residuFree = prod(*M, *residuFree);

    cout << "RESIDU FREE" << endl;
    cout << (*residuFree)(0) << endl;
  }

  return 0.; // there is no Newton iteration and the residuum is assumed to vanish
}

void D1MinusLinear::computeFreeState()
{
  for (DSIterator it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    // Lagrangian Systems
    SP::LagrangianDS d = boost::static_pointer_cast<LagrangianDS> (*it);

    // get left state from memory
    SP::SiconosVector vold = d->velocityMemory()->getSiconosVector(0); // right limit

    // get right information
    SP::SiconosMatrix M = d->mass();
    SP::SiconosVector vfree = d->velocity(); // POINTER CONSTRUCTOR : contains free velocity
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

  Index coord(8);
  coord[0] = relativePosition;
  coord[1] = relativePosition + sizeY;
  coord[2] = 0;
  coord[3] = 0;
  coord[4] = 0;
  coord[5] = 0;
  coord[6] = 0;
  coord[7] = sizeY;
  SP::SiconosMatrix C; // Jacobian of Relation with respect to degree of freedom
  SP::SiconosVector Xfree; // free degree of freedom
  SP::SiconosVector Yp = UR->yp(); // POINTER CONSTRUCTOR : contains output

  // define Xfree for velocity and acceleration level
  if (((*allOSNS)[SICONOS_OSNSP_TS_VELOCITY]).get() == osnsp)
  {
    Xfree = UR->workx();
  }
  else if (((*allOSNS)[SICONOS_OSNSP_TS_VELOCITY + 1]).get() == osnsp)
  {
    Xfree  = UR->workFree();
  }
  else
    RuntimeException::selfThrow("D1MinusLinear::computeFreeOutput - OSNSP neither on velocity nor on acceleration level.");

  // calculate data of interaction
  SP::Interaction mainInteraction = UR->interaction();
  assert(mainInteraction);
  assert(mainInteraction->relation());

  // only Lagrangian Systems
  if (relationType == Lagrangian)
  {
    // in Yp the linear part of velocity or acceleration relation will be saved
    C = mainInteraction->relation()->C();

    if (C)
    {
      assert(Xfree);
      assert(Yp);

      coord[3] = C->size(1);
      coord[5] = C->size(1);
      subprod(*C, *Xfree, *Yp, coord, true);
    }

    // in Yp corrections have to be added
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

    if (relationSubType == RheonomousR) // explicit time dependence -> partial time derivative has to be added
    {
      if (((*allOSNS)[SICONOS_OSNSP_TS_VELOCITY]).get() == osnsp)
      {
        boost::static_pointer_cast<LagrangianRheonomousR>(UR->interaction()->relation())->computehDot(simulation()->getTkp1());
        subprod(*ID, *(boost::static_pointer_cast<LagrangianRheonomousR>(UR->interaction()->relation())->hDot()), *Yp, xcoord, false);
      }
      else
        RuntimeException::selfThrow("D1MinusLinear::computeFreeOutput is only implemented  at velocity level for LagrangianRheonomousR.");
    }
    if (relationSubType == ScleronomousR) // acceleration term involving Hesse matrix of Relation with respect to degree is added
    {
      if (((*allOSNS)[SICONOS_OSNSP_TS_VELOCITY + 1]).get() == osnsp)
      {
        boost::static_pointer_cast<LagrangianScleronomousR>(UR->interaction()->relation())->computeNonLinearH2dot(simulation()->getTkp1());
        subprod(*ID, *(boost::static_pointer_cast<LagrangianScleronomousR>(UR->interaction()->relation())->Nonlinearh2dot()), *Yp, xcoord, false);
      }
    }
    if (((*allOSNS)[SICONOS_OSNSP_TS_VELOCITY]).get() == osnsp) // impact terms are added
    {
      SP::SiconosVisitor nslEffectOnFreeOutput(new _NSLEffectOnFreeOutput(osnsp, UR));
      UR->interaction()->nonSmoothLaw()->accept(*nslEffectOnFreeOutput);
    }
  }
  else
    RuntimeException::selfThrow("D1MinusLinear::computeFreeOutput - not implemented for Relation of type " + relationType);
}

void D1MinusLinear::updateState(unsigned int level)
{
  for (DSIterator it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    // Lagrangian Systems
    SP::LagrangianDS d = boost::static_pointer_cast<LagrangianDS> (*it);
    SP::SiconosMatrix M = d->mass();
    SP::SiconosVector v = d->velocity(); // POINTER CONSTRUCTOR : contains new velocity

    if (d->p(1))
    {
      SP::SiconosVector dummy(new SimpleVector(*(d->p(1)))); // value = nonsmooth impulse
      M->PLUForwardBackwardInPlace(*dummy); // solution for its velocity equivalent
      *v += *dummy; // add free velocity
      cout << "RIGHT IMPULSE" << endl;
      cout << (*(d->p(1)))(0) << endl;
    }
  }
}

void D1MinusLinear::insertDynamicalSystem(SP::DynamicalSystem ds)
{
  OSIDynamicalSystems->insert(ds);
}

D1MinusLinear* D1MinusLinear::convert(OneStepIntegrator* osi)
{
  return dynamic_cast<D1MinusLinear*>(osi);
}
