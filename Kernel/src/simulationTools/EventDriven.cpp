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

#include "EventDriven.hpp"
#include "LsodarOSI.hpp"
#include "LCP.hpp"
#include "Model.hpp"
#include "Interaction.hpp"
#include "EventsManager.hpp"
#include "NonSmoothDynamicalSystem.hpp"
#include "Topology.hpp"
#include "DynamicalSystem.hpp"
#include "LagrangianDS.hpp"
#include "EventFactory.hpp"
#include "BlockMatrix.hpp"
#include "NewMarkAlphaOSI.hpp"
#include "Relation.hpp"
#include "NonSmoothLaw.hpp"
#include "NewtonEulerR.hpp"
//#define DEBUG_MESSAGES

#include <debug.h>

using namespace std;
using namespace RELATION;

/** defaut constructor
 *  \param a pointer to a timeDiscretisation (linked to the model that owns this simulation)
 */
EventDriven::EventDriven(SP::TimeDiscretisation td): Simulation(td), _istate(1), _isNewtonConverge(false)
{
  _numberOfOneStepNSproblems = 2;
  (*_allNSProblems).resize(_numberOfOneStepNSproblems);
  _newtonTolerance = DEFAULT_TOL_ED;
  _newtonMaxIteration = 50;
  _newtonNbSteps = 0;
  _newtonResiduDSMax = 0.0;
  _newtonResiduYMax = 0.0;
  _localizeEventMaxIter = 100;
}

EventDriven::EventDriven(SP::TimeDiscretisation td, int nb): Simulation(td), _istate(1), _isNewtonConverge(false)
{
  (*_allNSProblems).resize(nb);
  _numberOfOneStepNSproblems = 0;
  _newtonTolerance = DEFAULT_TOL_ED;
  _newtonMaxIteration = 50;
  _newtonNbSteps = 0;
  _newtonResiduDSMax = 0.0;
  _newtonResiduYMax = 0.0;
  _localizeEventMaxIter = 100;
}

double EventDriven::TOL_ED = DEFAULT_TOL_ED;




void EventDriven::insertIntegrator(SP::OneStepIntegrator osi)
{
  Simulation::insertIntegrator(osi);
  // Determine the number of OneStepNSproblems depending on the OneStepIntegrator type
  OSI::TYPES  osiType = osi->getType();
  if (osiType == OSI::NEWMARKALPHAOSI) // EventDrivent asscociated with NewMarkAlpha
  {
    _numberOfOneStepNSproblems = 3;
    if (_allNSProblems->size() != 3)
    {
      (*_allNSProblems).resize(_numberOfOneStepNSproblems);
    }
  }
}

void EventDriven::updateIndexSet(unsigned int i)
{
  assert(!_model.expired());
  assert(model()->nonSmoothDynamicalSystem());
  assert(model()->nonSmoothDynamicalSystem()->topology());
  SP::Topology topo = model()->nonSmoothDynamicalSystem()->topology();

  assert(i < topo->indexSetsSize() &&
         "EventDriven::updateIndexSet(i), indexSets[i] does not exist.");
  // IndexSets[0] must not be updated in simulation, since it belongs to Topology.
  assert(i > 0  &&
         "EventDriven::updateIndexSet(i=0), indexSets[0] cannot be updated.");

  // For all Interactions in indexSet[i-1], compute y[i-1] and
  // update the indexSet[i].
  SP::InteractionsGraph indexSet0 = topo->indexSet(0);
  SP::InteractionsGraph indexSet1 = topo->indexSet(1);
  SP::InteractionsGraph indexSet2 = topo->indexSet(2);
  assert(indexSet0);
  assert(indexSet1);
  assert(indexSet2);

  // DEBUG_PRINTF("update indexSets start : indexSet0 size : %ld\n", indexSet0->size());
  // DEBUG_PRINTF("update IndexSets start : indexSet1 size : %ld\n", indexSet1->size());
  // DEBUG_PRINTF("update IndexSets start : indexSet2 size : %ld\n", indexSet2->size());

  InteractionsGraph::VIterator uibegin, uipend, uip;
  std11::tie(uibegin, uipend) = indexSet0->vertices();
  // loop over all vertices of the indexSet[i-1]
  for (uip = uibegin; uip != uipend; ++uip)
  {
    SP::Interaction inter = indexSet0->bundle(*uip);
    if (i == 1) // IndexSet[1]
    {
      // if indexSet[1]=>getYRef(0): output y
      // if indexSet[2]=>getYRef(1): output ydot
      double y = inter->getYRef(0); // output to define the IndexSets at this Interaction
      if (y < -TOL_ED) // y[0] < 0
      {
        inter->display();
        cout << "y = " << y << " < -TOL_ED =  "   << -TOL_ED  <<endl;
        RuntimeException::selfThrow("EventDriven::updateIndexSet, output of level 0 must be positive!!! ");
      }
      // 1 - If the Interaction is not yet in the set
      if (!indexSet1->is_vertex(inter)) // Interaction is not yet in the indexSet[i]
      {
        if (fabs(y) <= TOL_ED)
        {
          // vertex and edges insertions
          indexSet1->copy_vertex(inter, *indexSet0);
        }
      }
      else // if the Interaction was already in the set
      {
        if (fabs(y) > TOL_ED)
        {
          indexSet1->remove_vertex(inter); // remove the Interaction from IndexSet[1]
          inter->lambda(1)->zero(); // reset the lambda[1] to zero
        }
      }
    }
    else if (i == 2) // IndexSet[2]
    {
      if (indexSet1->is_vertex(inter)) // Interaction is in the indexSet[1]
      {
        double y = inter->getYRef(1); // output of level 1 at this Interaction
        if (!indexSet2->is_vertex(inter)) // Interaction is not yet in the indexSet[2]
        {
          if (fabs(y) <= TOL_ED)
          {
            // vertex and edges insertions
            indexSet2->copy_vertex(inter, *indexSet0);
          }
        }
        else // if the Interaction was already in the set
        {
          if (fabs(y) > TOL_ED)
          {
            indexSet2->remove_vertex(inter); // remove the Interaction from IndexSet[1]
            inter->lambda(2)->zero(); // reset the lambda[i] to zero
          }
        }
      }
      else // Interaction is not in the indexSet[1]
      {
        if (indexSet2->is_vertex(inter)) // Interaction is in the indexSet[2]
        {
          indexSet2->remove_vertex(inter); // remove the Interaction from IndexSet[2]
          inter->lambda(2)->zero(); // reset the lambda[i] to zero
        }
      }
    }
    else
    {
      RuntimeException::selfThrow("EventDriven::updateIndexSet, IndexSet[i > 2] doesn't exist");
    }
  }

  // DEBUG_PRINTF("update indexSets end : indexSet0 size : %ld\n", indexSet0->size());
  // DEBUG_PRINTF("update IndexSets end : indexSet1 size : %ld\n", indexSet1->size());
  // DEBUG_PRINTF("update IndexSets end : indexSet2 size : %ld\n", indexSet2->size());
}

void EventDriven::updateIndexSetsWithDoubleCondition()
{

  assert(!_model.expired());
  assert(model()->nonSmoothDynamicalSystem());
  assert(model()->nonSmoothDynamicalSystem()->topology());

  // for all Interactions in indexSet[i-1], compute y[i-1] and
  // update the indexSet[i]

  SP::Topology topo = model()->nonSmoothDynamicalSystem()->topology();

  SP::InteractionsGraph indexSet2 = topo->indexSet(2);

  InteractionsGraph::VIterator ui, uiend, vnext;
  std11::tie(ui, uiend) = indexSet2->vertices();

  for (vnext = ui; ui != uiend; ui = vnext)
  {
    ++vnext;

    SP::Interaction inter = indexSet2->bundle(*ui);
    double gamma = inter->getYRef(2);
    double F     = inter->getLambdaRef(2);
    if (fabs(F) < TOL_ED)
      indexSet2->remove_vertex(inter);
    else if ((gamma < -TOL_ED) || (F < -TOL_ED))
      RuntimeException::selfThrow("EventDriven::updateIndexSetsWithDoubleCondition(), output[2] and lambda[2] for Interactionof indexSet[2] must be higher or equal to zero.");
    else if (((fabs(gamma) > TOL_ED) && (fabs(F) > TOL_ED)))
      RuntimeException::selfThrow("EventDriven::updateIndexSetsWithDoubleCondition(), something is wrong for the LCP resolution.");
  }
}

void EventDriven::initOSNS()
{
  assert(!_model.expired());
  assert(model()->nonSmoothDynamicalSystem());
  assert(model()->nonSmoothDynamicalSystem()->topology());
  // for all Interactions in indexSet[i-1], compute y[i-1] and
  // update the indexSet[i]
  // Note that interactions set may be empty.
  InteractionsGraph::VIterator ui, uiend;
  SP::Topology topo = model()->nonSmoothDynamicalSystem()->topology();

  SP::InteractionsGraph indexSet0 = topo->indexSet(0);

  // === update all index sets ===
  updateIndexSets();
  initOSIRhs();

  if (!_allNSProblems->empty()) // ie if at least a non smooth problem has been built.
  {
    OSI::TYPES  osiType = (*_allOSI->begin())->getType();
    if (osiType == OSI::LSODAROSI || osiType == OSI::HEM5OSI) //EventDriven associated with LsodarOSI OSI
    {      
    }
    else if (osiType == OSI::NEWMARKALPHAOSI) // EventDrivent asscociated with NewMarkAlpha
    {
      if (_allNSProblems->size() != 3)
        RuntimeException::selfThrow
        (" EventDriven::initialize, \n an EventDriven simulation associated with NewMarkAlphaOSI must have three non smooth problems.\n Here, there are "
         + _allNSProblems->size());
      // Initialize OSNSP at position level
      (*_allNSProblems)[SICONOS_OSNSP_ED_SMOOTH_POS]->setInputOutputLevel(2);
      (*_allNSProblems)[SICONOS_OSNSP_ED_SMOOTH_POS]->setIndexSetLevel(2);
      (*_allNSProblems)[SICONOS_OSNSP_ED_SMOOTH_POS]->initialize(shared_from_this());
    }
    else
    {
      RuntimeException::selfThrow(" EventDriven::initialize, OSI not yet implemented.");
    }

    if (!((*_allNSProblems)[SICONOS_OSNSP_ED_IMPACT])) // ie if the impact problem does not
      // exist
      RuntimeException::selfThrow
      ("EventDriven::initialize, an EventDriven simulation must have an 'impact' non smooth problem.");
    if (!((*_allNSProblems)[SICONOS_OSNSP_ED_SMOOTH_ACC])) // ie if the acceleration-level problem
      // does not exist
      RuntimeException::selfThrow
      ("EventDriven::initialize, an EventDriven simulation must have an 'acceleration' non smooth problem.");
    // Initialize OSNSP for impact problem and at the acceleration level
    // WARNING: only for Lagrangian systems - To be reviewed for other ones.
    (*_allNSProblems)[SICONOS_OSNSP_ED_IMPACT]->setInputOutputLevel(1);
    (*_allNSProblems)[SICONOS_OSNSP_ED_IMPACT]->setIndexSetLevel(1);

    (*_allNSProblems)[SICONOS_OSNSP_ED_IMPACT]->initialize(shared_from_this());
    (*_allNSProblems)[SICONOS_OSNSP_ED_SMOOTH_ACC]->setInputOutputLevel(2);
    (*_allNSProblems)[SICONOS_OSNSP_ED_SMOOTH_ACC]->setIndexSetLevel(2);
    (*_allNSProblems)[SICONOS_OSNSP_ED_SMOOTH_ACC]->initialize(shared_from_this());
    //
    // Detect NonSmoothEvent at the beginning of the simulation
    if( topo->indexSetsSize() > 1)
    {
      SP::InteractionsGraph indexSet1 = model()->nonSmoothDynamicalSystem()->topology()->indexSet(1);
      if (indexSet1->size() != 0) // There is one non-smooth event to be added
      {
        _eventsManager->scheduleNonSmoothEvent(*this, _eventsManager->startingTime(), false);
      };
    }
  }
}

void EventDriven::initOSIs()
{
  for (OSIIterator itosi = _allOSI->begin();  itosi != _allOSI->end(); ++itosi)
  {
    // Initialize the acceleration like for NewMarkAlphaScheme
    if ((*itosi)->getType() == OSI::NEWMARKALPHAOSI)
    {
      SP::NewMarkAlphaOSI osi_NewMark =  std11::static_pointer_cast<NewMarkAlphaOSI>(*itosi);
      for (DSIterator itds = (*itosi)->dynamicalSystems()->begin();
           itds != (*itosi)->dynamicalSystems()->end(); ++itds)
      {
        if ((Type::value(**itds) == Type::LagrangianDS) || (Type::value(**itds) == Type::LagrangianLinearTIDS))
        {
          SP::LagrangianDS d = std11::static_pointer_cast<LagrangianDS>(*itds);
          *(d->workspace(DynamicalSystem::acce_like)) = *(d->acceleration()); // set a0 = ddotq0
          // Allocate the memory to stock coefficients of the polynomial for the dense output
          d->allocateWorkMatrix(LagrangianDS::coeffs_denseoutput, (*itds)->getDim(), (osi_NewMark->getOrderDenseOutput() + 1));
        }
      }
    }
  }
}

void EventDriven::initOSIRhs()
{
  // === initialization for OneStepIntegrators ===
  OSI::TYPES  osiType = (*_allOSI->begin())->getType();
  for (OSIIterator itosi = _allOSI->begin();  itosi != _allOSI->end(); ++itosi)
  {
    //Check whether OSIs used are of the same type
    if ((*itosi)->getType() != osiType)
      RuntimeException::selfThrow("OSIs used must be of the same type");
    for (DSIterator itds = (*itosi)->dynamicalSystems()->begin();
         itds != (*itosi)->dynamicalSystems()->end(); ++itds)
    {
      // Initialize right-hand side
      (*itds)->initRhs(model()->t0());
    }
  }
}

void EventDriven::initialize(SP::Model m, bool withOSI)
{
  // Initialization for Simulation
  Simulation::initialize(m, withOSI);
  // Initialization for all OneStepIntegrators
  initOSIs();
  initOSIRhs();
}

void EventDriven::computef(SP::OneStepIntegrator osi, integer * sizeOfX, doublereal * time, doublereal * x, doublereal * xdot)
{
  // computeF is supposed to fill xdot in, using the definition of the
  // dynamical systems belonging to the osi

  // Check osi type: only lsodar is allowed.
  if (osi->getType() != OSI::LSODAROSI)
    RuntimeException::selfThrow("EventDriven::computef(osi, ...), not yet implemented for a one step integrator of type " + osi->getType());

  LsodarOSI& lsodar = static_cast<LsodarOSI&>(*osi);
  // fill in xWork vector (ie all the x of the ds of this osi) with x
  lsodar.fillXWork(sizeOfX, x);

  double t = *time;
  model()->setCurrentTime(t);
  // Update Jacobian matrices at all interactions
  InteractionsGraph::VIterator ui, uiend;
  SP::InteractionsGraph indexSet0 = model()->nonSmoothDynamicalSystem()->topology()->indexSet(0);
  for (std11::tie(ui, uiend) = indexSet0->vertices(); ui != uiend; ++ui)
  {
    SP::Interaction inter = indexSet0->bundle(*ui);
    inter->relation()->computeJach(t, *inter);
    if (inter->relation()->getType() == NewtonEuler)
    {
      SP::DynamicalSystem ds1 = indexSet0->properties(*ui).source;
      SP::DynamicalSystem ds2 = indexSet0->properties(*ui).target;
      SP::NewtonEulerR ner = (std11::static_pointer_cast<NewtonEulerR>(inter->relation()));
      ner->computeJachqT(*inter, ds1, ds2);
    }
  }

  // solve a LCP at "acceleration" level if required
  if (!_allNSProblems->empty())
  {
    if (((*_allNSProblems)[SICONOS_OSNSP_ED_SMOOTH_ACC]->hasInteractions()))
    {
      // Update the state of the DS
      (*_allNSProblems)[SICONOS_OSNSP_ED_SMOOTH_ACC]->compute(t);
      updateInput(2); // Necessary to compute DS state below
    }
    // Compute the right-hand side ( xdot = f + r in DS) for all the
    //ds, with the new value of input.  lsodar->computeRhs(t);
  }

  // update the DS of the OSI.
  lsodar.computeRhs(t);
  //  for the DS state, ie the ones computed by lsodar (x above)
  // Update Index sets? No !!

  // Get the required value, ie xdot for output.
  DSIterator it;
  unsigned int i = 0;
  for (it = lsodar.dynamicalSystemsBegin(); it != lsodar.dynamicalSystemsEnd(); ++it)
  {
    if (Type::value(**it) == Type::LagrangianDS ||
        Type::value(**it) == Type::LagrangianLinearTIDS)
    {
      LagrangianDS& LDS = *std11::static_pointer_cast<LagrangianDS>(*it);
      SiconosVector& qDotTmp = *LDS.velocity();
      SiconosVector& qDotDotTmp = *LDS.acceleration();
      for (unsigned int j = 0 ; j < (*it)->getDim() ; ++j)
        xdot[i++] = qDotTmp(j);
      for (unsigned int j = 0 ; j < (*it)->getDim() ; ++j)
        xdot[i++] = qDotDotTmp(j);
    }
    else
    {
      SiconosVector& xtmp2 = *(*it)->rhs(); // Pointer link !
      for (unsigned int j = 0 ; j < (*it)->getN() ; ++j) // Warning: getN, not getDim !!!!
        xdot[i++] = xtmp2(j);
    }
  }
}

void EventDriven::computeJacobianfx(SP::OneStepIntegrator osi,
                                    integer *sizeOfX,
                                    doublereal *time,
                                    doublereal *x,
                                    doublereal *jacob)
{
  if (osi->getType() != OSI::LSODAROSI)
    RuntimeException::selfThrow("EventDriven::computeJacobianfx(osi, ...), not yet implemented for a one step integrator of type " + osi->getType());

  SP::LsodarOSI lsodar = std11::static_pointer_cast<LsodarOSI>(osi);

  // Remark A: according to DLSODAR doc, each call to jacobian is
  // preceded by a call to f with the same arguments NEQ, T, and Y.
  // Thus to gain some efficiency, intermediate quantities shared by
  // both calculations may be saved in class members?  fill in xWork
  // vector (ie all the x of the ds of this osi) with x fillXWork(x);
  // -> copy
  // Maybe this step is not necessary?  because of
  // remark A above

  // Compute the jacobian of the vector field according to x for the
  // current ds
  double t = *time;
  model()->setCurrentTime(t);
  lsodar->computeJacobianRhs(t);

  // Save jacobianX values from dynamical system into current jacob
  // (in-out parameter)

  DSIterator it;
  unsigned int i = 0;
  for (it = lsodar->dynamicalSystemsBegin(); it != lsodar->dynamicalSystemsEnd(); ++it)
  {
    if (Type::value(**it) == Type::LagrangianDS ||
        Type::value(**it) == Type::LagrangianLinearTIDS)
    {
      LagrangianDS& lds = *std11::static_pointer_cast<LagrangianDS>(*it);
      BlockMatrix& jacotmp = *lds.jacobianRhsx();
      for (unsigned int j = 0; j < (*it)->getN(); ++j)
      {
        for (unsigned int k = 0; k < (*it)->getDim(); ++k)
          jacob[i++] = jacotmp(k, j);
      }
    }
    else
    {
      SiconosMatrix& jacotmp = *(*it)->jacobianRhsx(); // Pointer link !
      for (unsigned int j = 0; j < (*it)->getN(); ++j)
      {
        for (unsigned int k = 0; k < (*it)->getDim(); ++k)
          jacob[i++] = jacotmp(k, j);
      }
    }
  }
}

unsigned int EventDriven::computeSizeOfg()
{
  SP::InteractionsGraph indexSet0 = model()->nonSmoothDynamicalSystem()->topology()->indexSet(0);
  return (indexSet0->size());
}


void EventDriven::computeg(SP::OneStepIntegrator osi,
                           integer * sizeOfX, doublereal* time,
                           doublereal* x, integer * ng,
                           doublereal * gOut)
{
  assert(!_model.expired());
  assert(model()->nonSmoothDynamicalSystem());
  assert(model()->nonSmoothDynamicalSystem()->topology());
  InteractionsGraph::VIterator ui, uiend;
  SP::Topology topo = model()->nonSmoothDynamicalSystem()->topology();
  SP::InteractionsGraph indexSet0 = topo->indexSet(0);
  SP::InteractionsGraph indexSet2 = topo->indexSet(2);
  unsigned int nsLawSize, k = 0 ;
  SP::SiconosVector y, ydot, yddot, lambda;
  SP::LsodarOSI lsodar = std11::static_pointer_cast<LsodarOSI>(osi);
  // Solve LCP at acceleration level to calculate the lambda[2] at Interaction of indexSet[2]
  lsodar->fillXWork(sizeOfX, x);
  //
  double t = *time;
  model()->setCurrentTime(t);
  if (!_allNSProblems->empty())
  {
    if (((*_allNSProblems)[SICONOS_OSNSP_ED_SMOOTH_ACC]->hasInteractions()))
    {
      (*_allNSProblems)[SICONOS_OSNSP_ED_SMOOTH_ACC]->compute(t);
    }
  };
  /*
     double * xdottmp = (double *)malloc(*sizeOfX*sizeof(double));
     computef(osi, sizeOfX,time,x,xdottmp);
     free(xdottmp);
     */
  // Update the output from level 0 to level 1
  updateOutput(0);
  updateOutput(1);
  updateOutput(2);
  //
  for (std11::tie(ui, uiend) = indexSet0->vertices(); ui != uiend; ++ui)
  {
    SP::Interaction inter = indexSet0->bundle(*ui);
    nsLawSize = inter->nonSmoothLaw()->size();
    y = inter->y(0);   // output y at this Interaction
    ydot = inter->y(1); // output of level 1 at this Interaction
    yddot = inter->y(2);
    lambda = inter->lambda(2); // input of level 2 at this Interaction
    if (!(indexSet2->is_vertex(inter))) // if Interaction is not in the indexSet[2]
    {
      for (unsigned int i = 0; i < nsLawSize; ++i)
      {
        if ((*y)(i) > TOL_ED)
        {
          gOut[k] = (*y)(i);
        }
        else
        {
          if ((*ydot)(i) > -TOL_ED)
          {
            gOut[k] = 100 * TOL_ED;
          }
          else
          {
            gOut[k] = (*y)(i);
          }
        }
        k++;
      }
    }
    else // If Interaction is in the indexSet[2]
    {
      for (unsigned int i = 0; i < nsLawSize; ++i)
      {
        if ((*lambda)(i) > TOL_ED)
        {
          gOut[k] = (*lambda)(i); // g = lambda[2]
        }
        else
        {
          if ((*yddot)(i) > TOL_ED)
          {
            gOut[k] = (*lambda)(i);
          }
          else
          {
            gOut[k] = 100 * TOL_ED;
          }
        }
        k++;
      }
    }

  }
}
void EventDriven::updateImpactState()
{
  OSIIterator itOSI;
  // Compute input = R(lambda[1])
  updateInput(1);

  // Compute post-impact velocity
  for (itOSI = _allOSI->begin(); itOSI != _allOSI->end() ; ++itOSI)
    (*itOSI)->updateState(1);
}

void EventDriven::updateSmoothState()
{
  // Update input of level 2
  updateInput(2);
  OSIIterator itOSI;
  // Compute acceleration
  for (itOSI = _allOSI->begin(); itOSI != _allOSI->end() ; ++itOSI)
    (*itOSI)->updateState(2);
}


void EventDriven::update(unsigned int levelInput)
{
  assert(levelInput <= 2);
  if (levelInput == 1)
  {
    updateImpactState();
  }
  else
  {
    updateSmoothState();
  }
  // Update output (y)
  updateOutput(levelInput);
  // Warning: index sets are not updated in this function !!
}

void EventDriven::advanceToEvent()
{
  _tinit = _eventsManager->startingTime();
  _tend =  _eventsManager->nextTime();
  _tout = _tend;
  bool isNewEventOccur = false;  // set to true if a new event occur during integration
  OSI::TYPES  osiType = (*_allOSI->begin())->getType(); // Type of OSIs
  double _minConstraint = 0.0;

  // Initialize lambdas of all interactions.
  SP::InteractionsGraph indexSet0 = model()->nonSmoothDynamicalSystem()->
                                    topology()->indexSet(0);
  InteractionsGraph::VIterator ui, uiend, vnext;
  std11::tie(ui, uiend) = indexSet0->vertices();
  for (vnext = ui; ui != uiend; ui = vnext)
  {
    ++vnext;
    indexSet0->bundle(*ui)->resetAllLambda();
  }

  if (osiType == OSI::NEWMARKALPHAOSI)
  {
    newtonSolve(_newtonTolerance, _newtonMaxIteration);
    // Update after Newton iteration
    // Update input of level 2 >>> has already been done in newtonSolve
    // Update state of all Dynamicall Systems >>>  has already been done in newtonSolve
    // Update outputs of levels 0, 1, 2
    updateOutput(0);
    updateOutput(1);
    updateOutput(2);
    // Detect whether or not some events occur during the integration step
    _minConstraint = detectEvents();
    //
#ifdef DEBUG_MESSAGES
    cout << "========== EventDriven::advanceToEvent =============" <<endl;
    cout.precision(15);
    cout << "Istate: " << _istate <<endl;
    cout << "Maximum value of constraint functions: " << _minConstraint <<endl;
#endif
    //
    if (_istate != 2) //some events occur
    {
      cout << "In EventDriven::advanceToEvent, some events are detected!!!" <<endl;
      if (std::abs(_minConstraint) < TOL_ED) // events occur at the end of the integration step
      {
        isNewEventOccur = true;
      }
      else // events need to be localized
      {
        isNewEventOccur = true;
        LocalizeFirstEvent();
      }
      // add new event to the list to be handled
      cout << "A new event occurs at time: " << _tout <<endl;
      _eventsManager->scheduleNonSmoothEvent(*this, _tout);
      model()->setCurrentTime(_tout);
    }
  }
  else if (osiType == OSI::LSODAROSI || osiType == OSI::HEM5OSI)
  {
    // WARNING: this is supposed to work for only one OSI, including all
    // the DS.  To be reviewed for multiple OSI case (if it has sense?).

    // ---> Step 1: integrate the smooth dynamics from current event to
    // next event; Starting event = last accessed event.  Next event =
    // next time step or first root of the 'g' function found by
    // integrator (LsodarOSI)

    // if _istate == 1 => first call. It this case we suppose that _tinit
    // and _tend have been initialized before
    // if(_istate == 2 || _istate == 3)
    //  {
    //    _tinit = _eventsManager->startingTime();
    //    _tend =  _eventsManager->nextTime();
    //  }
    // _tout = _tend;
    // call integrate method for each OSI, between _tinit and _tend.
    OSIIterator it;
    for (it = _allOSI->begin(); it != _allOSI->end(); ++it)
    {
      (*it)->resetNonSmoothPart();
      //====================================================================================
      //     cout << " Start of LsodarOSI integration" << endl;
      (*it)->integrate(_tinit, _tend, _tout, _istate); // integrate must

      //  cout << " End of LsodarOSI integration" << endl;
      // SP::LsodarOSI lsodar = std11::static_pointer_cast<LsodarOSI>(*it);
      // SA::integer iwork = lsodar->getIwork();
      // SA::doublereal rwork = lsodar->getRwork();
      //  cout << "Number of steps used: " << iwork[10] <<endl;
      //  cout << "Method order last used: " << iwork[13] <<endl;
      //  cout << "Step size last used: " << rwork[10] <<endl;
      // return a flag (_istate) telling if _tend has been  reached or not.
      //====================================================================================

      if (_printStat)
      {
        statOut << " =================> Results after advanceToEvent <================= " <<endl;
        statOut << " Starting time: " << _tinit <<endl;
        statOut << " _istate " << _istate <<endl;
      }
      if (_istate == 3) // ie if _tout is not equal to _tend: one or more roots have been found.
      {
        isNewEventOccur = true;
        // Add an event into the events manager list
        _eventsManager->scheduleNonSmoothEvent(*this, _tout);
        if (_printStat)
          statOut << " -----------> New non-smooth event at time " << _tout <<endl;
      }
      // if(_printStat)
      //   {
      //     SP::LsodarOSI lsodar = std11::static_pointer_cast<LsodarOSI>(*it);
      //     statOut << "Results at time " << _tout << ":" <<endl;
      //     SA::integer iwork = lsodar->getIwork();
      //     SA::doublereal Rwork = lsodar->getRwork();
      //     statOut << "Number of steps: " << iwork[10] << ", number of f evaluations: " << iwork[11] << ", number of jacobianF eval.: " << iwork[12] << "." <<endl;
      //   }
    }
    // Set model time to _tout
    model()->setCurrentTime(_tout);
    //update output[0], output[1], output[2]
    updateOutput(0);
    updateOutput(1);
    updateOutput(2);
    //update lambda[2], input[2] and indexSet[2] with double consitions for the case there is no new event added during time integration, otherwise, this
    // update is done when the new event is processed
    if (!isNewEventOccur)
    {
      if (!_allNSProblems->empty())
      {
        // Solve LCP at acceleration level
        if (((*_allNSProblems)[SICONOS_OSNSP_ED_SMOOTH_ACC]->hasInteractions()))
        {
          SP::InteractionsGraph indexSet2 = model()->nonSmoothDynamicalSystem()->topology()->indexSet(2);
          if (indexSet2->size() != 0)
          {
            (*_allNSProblems)[SICONOS_OSNSP_ED_SMOOTH_ACC]->compute(_tout);
            updateInput(2);
            // update indexSet[2] with double condition
            //updateIndexSetsWithDoubleCondition();
          }
        }
      }
    }
  }
  else
  {
    RuntimeException::selfThrow("In EventDriven::advanceToEvent, this type of OneStepIntegrator does not exist for Event-Driven scheme!!!");
  }
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
double EventDriven::computeResiduConstraints()
{
  // Make sure that the state of all Dynamical Systems was updated
  double t = nextTime(); // time at the end of the step
  SP::InteractionsGraph indexSet2 = model()->nonSmoothDynamicalSystem()->topology()->indexSet(2);
  double _y;
  // Loop over all interactions of indexSet2
  InteractionsGraph::VIterator ui, uiend;
  double _maxResiduGap = 0.0;
  for (OSIIterator itosi = _allOSI->begin(); itosi != _allOSI->end(); ++itosi)
  {
    if ((*itosi)->getType() == OSI::NEWMARKALPHAOSI)
    {
      SP::NewMarkAlphaOSI osi_NewMark = std11::static_pointer_cast<NewMarkAlphaOSI>(*itosi);
      bool _flag = osi_NewMark->getFlagVelocityLevel();
      for (std11::tie(ui, uiend) = indexSet2->vertices(); ui != uiend; ++ui)
      {
        SP::Interaction inter = indexSet2->bundle(*ui);
        if (!_flag) // constraints at the position level
        {
          inter->computeOutput(t, 0); // compute y[0] for the interaction at the end time
          _y = (*inter->y(0))(0);
        }
        else // constraints at the velocity level
        {
          inter->computeOutput(t, 1); // compute y[1] for the interaction at the end time
          _y = (*inter->y(1))(0);
        }

        if (_maxResiduGap < abs(_y))
        {
          _maxResiduGap = abs(_y);
        }
        //
#ifdef DEBUG_MESSAGES
        cout << "Contraint residu: " << _y <<endl;
#endif
        //
      }
    }
    else
    {
      RuntimeException::selfThrow("In EventDriven::predictionNewtonIteration, the current OSI must be NewMarkAlpha scheme!!!");
    }
  }
  //
#ifdef DEBUG_MESSAGES
  cout << "Maximum constraint residu: " << _maxResiduGap <<endl;
#endif
  //
  return _maxResiduGap;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void EventDriven::prepareNewtonIteration()
{
  // At this stage, we do
  // (1) compute iteration matrix W for all DSs belonging to all OSIs
  // (2) compute free residu for all DSs belonging to all OSIs and get maximum residu
  // (3) compute free state for all DSs belonging to all OSIs
  // (4) compute maximum gap residu over all interactions of indexSet 2
  _newtonResiduDSMax = 0.0;
  _newtonResiduYMax = 0.0;
  double _maxResidu;
  // Update input of level 2
  updateInput(2);
  // Loop over all OSIs
  OSI::TYPES  osiType;
  for (OSIIterator itosi = _allOSI->begin(); itosi != _allOSI->end(); ++itosi)
  {
    osiType = (*itosi)->getType();
    if (osiType != OSI::NEWMARKALPHAOSI)
    {
      RuntimeException::selfThrow("In EventDriven::prepareNewtonIteration, the current OSI is not NewMarkAlpha scheme!!!");
    }
    // Compute iteration matrix W
    (*itosi)->prepareNewtonIteration(nextTime()); // preparation for each OSI
    // Compute free residus, maximum residu
    _maxResidu = (*itosi)->computeResidu();
    if (_newtonResiduDSMax < _maxResidu)
    {
      _newtonResiduDSMax = _maxResidu;
    }
    // Compute free state
    (*itosi)->computeFreeState();
  }
  // Compute maximum gap residu
  _newtonResiduYMax = computeResiduConstraints();
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool EventDriven::newtonCheckConvergence(double _tol)
{
  bool checkConvergence = true;
  if ((_newtonResiduDSMax > _tol) || (_newtonResiduYMax > _tol))
  {
    checkConvergence = false;
  }
  return checkConvergence;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void EventDriven::predictionNewtonIteration()
{
  // Prediction of the state for all Dynamical Systems before Newton iteration
  for (OSIIterator itosi = _allOSI->begin(); itosi != _allOSI->end(); ++itosi)
  {
    if ((*itosi)->getType() == OSI::NEWMARKALPHAOSI)
    {
      SP::NewMarkAlphaOSI osi_NewMark = std11::static_pointer_cast<NewMarkAlphaOSI>(*itosi);
      osi_NewMark->prediction();
    }
    else
    {
      RuntimeException::selfThrow("In EventDriven::predictionNewtonIteration, the current OSI must be NewMarkAlpha scheme!!!");
    }
  }
  // Prediction of the output and lambda for all Interactions before Newton iteration
  double t = nextTime(); // time at the end of the step
  SP::InteractionsGraph indexSet0 = model()->nonSmoothDynamicalSystem()->topology()->indexSet(0);
  // Loop over all interactions
  InteractionsGraph::VIterator ui, uiend;
  for (std11::tie(ui, uiend) = indexSet0->vertices(); ui != uiend; ++ui)
  {
    SP::Interaction inter = indexSet0->bundle(*ui);
    inter->computeOutput(t, 0); // compute y[0] for the interaction at the end time with the state predicted for Dynamical Systems
    inter->lambda(2)->zero(); // reset lambda[2] to zero
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void EventDriven::correctionNewtonIteration()
{
  //Update the input of level 2 for all Dynamical Systems after each iteration
  updateInput(2);
  // Correction
  for (OSIIterator itosi = _allOSI->begin(); itosi != _allOSI->end(); ++itosi)
  {
    if ((*itosi)->getType() == OSI::NEWMARKALPHAOSI)
    {
      SP::NewMarkAlphaOSI osi_NewMark = std11::static_pointer_cast<NewMarkAlphaOSI>(*itosi);
      osi_NewMark->correction();
    }
    else
    {
      RuntimeException::selfThrow("In EventDriven::correctionNewtonIteration, the current OSI must be NewMarkAlpha scheme!!!");
    }
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void EventDriven::newtonSolve(double criterion, unsigned int maxStep)
{
  _isNewtonConverge = false;
  _newtonNbSteps = 0; // number of Newton iterations
  int info = 0;
  _istate = 1; // beginning of time integration
  // Prediction
  predictionNewtonIteration();
  while (1 != 0)
  {
    _newtonNbSteps++;
    // Prepare for iteration
    prepareNewtonIteration();
    // Check convergence
    _isNewtonConverge = newtonCheckConvergence(_newtonTolerance);
    //
#ifdef DEBUG_MESSAGES
    cout << "Iteration: " << _newtonNbSteps <<endl;
    cout << "Convergence: " << _isNewtonConverge <<endl;
#endif
    //
    if (_isNewtonConverge)
    {
      break;
    }
    if (_newtonNbSteps >  maxStep)
    {
      cout << "Warning!!!In EventDriven::newtonSolve: Number of iterations is greater than the maximum value " << maxStep <<endl;
    }
    // If no convergence, proceed iteration
    SP::InteractionsGraph indexSet2 = model()->nonSmoothDynamicalSystem()->topology()->indexSet(2);
    if (indexSet2->size() != 0) // if indexSet2 is not empty, solve LCP to determine contact forces
    {
      info = computeOneStepNSProblem(SICONOS_OSNSP_ED_SMOOTH_POS);
      if (info != 0)
      {
        cout << "Warning!!!In EventDriven::newtonSolve: LCP solver may fail" <<endl;
      }
    }
    // Correction of the state of all Dynamical Systems
    correctionNewtonIteration();
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
double EventDriven::detectEvents(bool _IsUpdateIstate)
{
  double _minResiduOutput = 0.0; // maximum of g_i with i running over all activated or deactivated contacts
  // Loop over all interactions to detect whether some constraints are activated or deactivated
  bool _IsContactClosed = false;
  bool _IsContactOpened = false;
  bool _IsFirstTime = true;
  InteractionsGraph::VIterator ui, uiend;
  SP::SiconosVector y, ydot, lambda;
  SP::Topology topo = model()->nonSmoothDynamicalSystem()->topology();
  SP::InteractionsGraph indexSet0 = topo->indexSet(0);
  SP::InteractionsGraph indexSet2 = topo->indexSet(2);
  //
#ifdef DEBUG_MESSAGES
  cout << "======== In EventDriven::detectEvents =========" <<endl;
#endif
  for (std11::tie(ui, uiend) = indexSet0->vertices(); ui != uiend; ++ui)
  {
    SP::Interaction inter = indexSet0->bundle(*ui);
    double nsLawSize = inter->nonSmoothLaw()->size();
    if (nsLawSize != 1)
    {
      RuntimeException::selfThrow("In EventDriven::detectEvents, the interaction size > 1 has not been implemented yet!!!");
    }
    y = inter->y(0);   // output y at this Interaction
    ydot = inter->y(1); // output of level 1 at this Interaction
    lambda = inter->lambda(2); // input of level 2 at this Interaction
    if (!(indexSet2->is_vertex(inter))) // if Interaction is not in the indexSet[2]
    {
      if ((*y)(0) < TOL_ED) // gap at the current interaction <= 0
      {
        _IsContactClosed = true;
      }

      if (_IsFirstTime)
      {
        _minResiduOutput = (*y)(0);
        _IsFirstTime = false;
      }
      else
      {
        if (_minResiduOutput > (*y)(0))
        {
          _minResiduOutput = (*y)(0);
        }
      }
    }
    else // If interaction is in the indexSet[2]
    {
      if ((*lambda)(0) < TOL_ED) // normal force at the current interaction <= 0
      {
        _IsContactOpened = true;
      }

      if (_IsFirstTime)
      {
        _minResiduOutput = (*lambda)(0);
        _IsFirstTime = false;
      }
      else
      {
        if (_minResiduOutput > (*lambda)(0))
        {
          _minResiduOutput = (*lambda)(0);
        }
      }
    }
    //
#ifdef DEBUG_MESSAGES
    cout.precision(15);
    cout << "Contact number: " << inter->number() <<endl;
    cout << "Contact gap: " << (*y)(0) <<endl;
    cout << "Contact force: " << (*lambda)(0) <<endl;
    cout << "Is contact is closed: " << _IsContactClosed <<endl;
    cout << "Is contact is opened: " << _IsContactOpened <<endl;
#endif
    //
  }
  //
  if (_IsUpdateIstate)
  {
    if ((!_IsContactClosed) && (!_IsContactOpened))
    {
      _istate = 2; //no event is detected
    }
    else if ((_IsContactClosed) && (!_IsContactOpened))
    {
      _istate = 3; // Only some contacts are closed
    }
    else if ((!_IsContactClosed) && (_IsContactOpened))
    {
      _istate = 4; // Only some contacts are opened
    }
    else
    {
      _istate = 5; // Some contacts are closed AND some contacts are opened
    }
  }
  //
  return  _minResiduOutput;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void EventDriven::LocalizeFirstEvent()
{
  // We localize the first event occuring during the integration step when the flag _istate = 3 or 4
  // Compute the coefficients of the dense output polynomial for all DSs
  for (OSIIterator itosi = _allOSI->begin(); itosi != _allOSI->end(); ++itosi)
  {
    if ((*itosi)->getType() == OSI::NEWMARKALPHAOSI)
    {
      SP::NewMarkAlphaOSI osi_NewMark = std11::static_pointer_cast<NewMarkAlphaOSI>(*itosi);
      osi_NewMark->prepareEventLocalization();
    }
    else
    {
      RuntimeException::selfThrow("In EventDriven::LocalizeFirstEvent, the current OSI must be NewMarkAlpha scheme!!!");
    }
  }
  //
  double t_a = startingTime();
  double t_b = nextTime();
  double _minConstraint = 0.0;
  bool found = false;
  bool _IsupdateIstate = false;
  unsigned int _numIter = 0;
  while (!found)
  {
    _numIter++;
    double t_i = (t_b + t_a) / 2.0; // mid-time of the current interval
    // set t_i as the current time
    model()->setCurrentTime(t_i);
    // Generate dense output for all DSs at the time t_i
    for (OSIIterator itosi = _allOSI->begin(); itosi != _allOSI->end(); ++itosi)
    {
      if ((*itosi)->getType() == OSI::NEWMARKALPHAOSI)
      {
        SP::NewMarkAlphaOSI osi_NewMark = std11::static_pointer_cast<NewMarkAlphaOSI>(*itosi);
        osi_NewMark->DenseOutputallDSs(t_i);
      }
    }
    // If _istate = 3 or 5, i.e. some contacts are closed, we need to compute y[0] for all interactions
    if ((_istate == 3) || (_istate == 5)) // some contacts are closed
    {
      updateOutput(0);
    }
    // If _istate = 4 or 5, i.e. some contacts are detached, we need to solve LCP at the acceleration level to compute contact forces
    if ((_istate == 4) || (_istate == 5)) // some contacts are opened
    {
      if (!_allNSProblems->empty())
      {
        (*_allNSProblems)[SICONOS_OSNSP_ED_SMOOTH_ACC]->compute(t_i);
      }
    }
    // Check whether or not some events occur in the interval [t_a, t_i]
    _minConstraint = detectEvents(_IsupdateIstate);
    if (std::abs(_minConstraint) < TOL_ED) // first event is found
    {
      _tout = t_i;
      found = true;
    }
    // if some events are detected in the interval [t_a, t_i] (if _istate != 2), set t_b = t_i
    if (_minConstraint < -TOL_ED)
    {
      t_b = t_i;
    }
    else // if no event is detected in [t_a, t_i], then we have to detect events in the interval [t_i, t_b]
    {
      t_a = t_i;
    }
    //
    if (_numIter > _localizeEventMaxIter)
    {
      RuntimeException::selfThrow("In EventDriven::LocalizeFirstEvent, the numbner of iterations performed is too large!!!");
    }
  }
}
