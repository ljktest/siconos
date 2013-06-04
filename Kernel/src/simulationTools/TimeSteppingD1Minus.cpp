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

#include "TimeSteppingD1Minus.hpp"
#include "D1MinusLinear.hpp"
#include "TimeDiscretisation.hpp"
#include "Topology.hpp"
//#include "Interaction.hpp"
#include "Interaction.hpp"
#include "LagrangianDS.hpp"
#include "LagrangianR.hpp"
#include "NewtonEulerDS.hpp"
#include "NewtonEulerR.hpp"
//#define DEBUG_STDOUT
//#define DEBUG_MESSAGES
#include "debug.h"

using namespace std;
using namespace RELATION;

void TimeSteppingD1Minus::initOSNS()
{
  // initialize OSNS for InteractionsGraph from Topology
  assert(model()->nonSmoothDynamicalSystem()->topology()->isUpToDate());
  SP::Topology topo =  model()->nonSmoothDynamicalSystem()->topology();
  // SP::InteractionsGraph indexSet0 = topo->indexSet(0);

  // InteractionsGraph::VIterator ui, uiend;
  // for (std11::tie(ui, uiend) = indexSet0->vertices(); ui != uiend; ++ui)
  // {
  //   initializeInteraction(indexSet0->bundle(*ui));
  // }

  // there is at least one OSNP
  if (!_allNSProblems->empty())
  {
    if (_allNSProblems->size() != 2)
      RuntimeException::selfThrow("TimeSteppingD1Minus::initOSNS, TimeSteppingD1Minus simulation must have two OneStepNonsmoothProblems.");

    //update all index sets
    updateIndexSets();

    // set evaluation levels (first is of velocity, second of acceleration type)
    (*_allNSProblems)[SICONOS_OSNSP_TS_VELOCITY]->setLevels(1, 1);
    (*_allNSProblems)[SICONOS_OSNSP_TS_VELOCITY]->initialize(shared_from_this());
    (*_allNSProblems)[SICONOS_OSNSP_TS_VELOCITY + 1]->setLevels(2, 2);
    (*_allNSProblems)[SICONOS_OSNSP_TS_VELOCITY + 1]->initialize(shared_from_this());

    // update output
    for (unsigned int level = _levelMinForOutput; level < _levelMaxForOutput; level++)
      updateOutput(level);
  }
}

// void TimeSteppingD1Minus::initializeInteraction(SP::Interaction inter)
// {
//   DEBUG_PRINT("TimeSteppingD1Minus::initializeInteraction(SP::Interaction inter) starts\n");
//   // RELATION::TYPES pbType = inter->relation()->getType();
  
//   // if (pbType == Lagrangian)
//   // {
//   //   assert(inter->data(LagrangianR::velocity));
    
//   //   ConstDSIterator itDS;
//   //   for (itDS = inter->dynamicalSystemsBegin();
//   //        itDS != inter->dynamicalSystemsEnd();
//   //        ++itDS)
//   //   {
//   //     assert(Type::value(**itDS) == Type::LagrangianDS ||
//   //            Type::value(**itDS) == Type::LagrangianLinearTIDS);
//   //     inter->data(LagrangianR::velocity)->insertPtr(std11::static_pointer_cast<LagrangianDS>(*itDS)->velocity());
//   //   }
//   // }
//   // else if (pbType == NewtonEuler)
//   // {
//   //   assert(inter->data(NewtonEulerR::velocity));
//   //   ConstDSIterator itDS;
//   //   for (itDS = inter->dynamicalSystemsBegin();
//   //        itDS != inter->dynamicalSystemsEnd();
//   //        ++itDS)
//   //   {
//   //     assert(Type::value(**itDS) == Type::NewtonEulerDS);
//   //     inter->data(NewtonEulerR::velocity)->insertPtr(std11::static_pointer_cast<NewtonEulerDS>(*itDS)->velocity());
//   //   }
//   // }
//   // else
//   //   RuntimeException::selfThrow("TimeSteppingD1Minus::initializeInteractions - not implemented for Relation of type " + pbType);
//   DEBUG_PRINT("TimeSteppingD1Minus::initializeInteraction(SP::Interaction inter) ends\n");

// }

TimeSteppingD1Minus::TimeSteppingD1Minus(SP::TimeDiscretisation td, int nb) : Simulation(td), impactOccuredLastTimeStep(false)
{
  (*_allNSProblems).resize(nb);
}

TimeSteppingD1Minus::~TimeSteppingD1Minus()
{
}

void TimeSteppingD1Minus::updateIndexSet(unsigned int i)
{
  DEBUG_PRINTF("\nTimeSteppingD1Minus::updateIndexSet(unsigned int i) for i = %i\n", i);
  // To update IndexSet i: add or remove Interactions from
  // this set, depending on y values.

  assert(!_model.expired());
  assert(model()->nonSmoothDynamicalSystem());
  assert(model()->nonSmoothDynamicalSystem()->topology());

  SP::Topology topo = model()->nonSmoothDynamicalSystem()->topology();

  assert(i < topo->indexSetsSize() &&
         "TimeSteppingD1Minus::updateIndexSet(i), indexSets[i] does not exist.");
  // IndexSets[0] must not be updated in simulation, since it belongs to Topology.
  assert(i > 0 &&
         "TimeSteppingD1Minus::updateIndexSet(i=0), indexSets[0] cannot be updated.");

  // For all Interactions in indexSet[i-1], compute y[i-1] and
  // update the indexSet[i].
  SP::InteractionsGraph indexSet0 = topo->indexSet(0); // ALL Interactions : formula (8.30) of Acary2008
  SP::InteractionsGraph indexSet1 = topo->indexSet(1); // ACTIVE Interactions for IMPACTS
  SP::InteractionsGraph indexSet2 = topo->indexSet(2); // ACTIVE Interactions for CONTACTS
  assert(indexSet0);
  assert(indexSet1);
  assert(indexSet2);

  topo->setHasChanged(false); // only with changed topology, OSNS will be forced to update themselves

  DEBUG_PRINTF("\nINDEXSETS BEFORE UPDATE for level = %i\n",i);
  DEBUG_PRINTF("TimeSteppingD1MinusLinear::updateIndexSet(unsigned int i). update indexSets start : indexSet0 size : %ld\n", indexSet0->size());
  DEBUG_PRINTF("TimeSteppingD1MinusLinear::updateIndexSet(unsigned int i). update IndexSets start : indexSet1 size : %ld\n", indexSet1->size());
  DEBUG_PRINTF("TimeSteppingD1MinusLinear::updateIndexSet(unsigned int i). update IndexSets start : indexSet2 size : %ld\n", indexSet2->size());

  // DEBUG_EXPR(indexSet0->display());
  // DEBUG_PRINT("\n");
  // DEBUG_EXPR(indexSet1->display());
  // DEBUG_PRINT("\n");
  // DEBUG_EXPR(indexSet2->display());

  InteractionsGraph::VIterator uipend, uip;
  bool forecastImpact = false;
  for (std11::tie(uip, uipend) = indexSet0->vertices(); uip != uipend; ++uip) // loop over ALL verices in indexSet0
  {
    SP::Interaction inter = indexSet0->bundle(*uip);

    if (i == 1) // ACTIVE FOR IMPACT CALCULATIONS? Contacts which have been closing in the last time step
    {
      impactOccuredLastTimeStep = false;
      DEBUG_PRINT("\nUPDATE INDEXSET 1\n");

      double y = (*(inter->y(0)))(0); // current position
      double yOld = (*(inter->yOld(0)))(0); // old position

      DEBUG_PRINTF("y= %f\n", y);
      DEBUG_PRINTF("yOld= %f\n", yOld);
      if (Type::value(*(inter->nonSmoothLaw())) != Type::EqualityConditionNSL) /* We activate Equality constraints
                                                                                * only if there is an impact.
                                                                                * we add them at the end */
      {
        if (!indexSet1->is_vertex(inter))
        {
          if (y <= DEFAULT_TOL_D1MINUS && yOld > DEFAULT_TOL_D1MINUS)
          {
            // if Interaction has not been active in the previous calculation and xnow becomes active
            indexSet1->copy_vertex(inter, *indexSet0);
            forecastImpact=true;
            topo->setHasChanged(true);
          }
        }
        else
        {
          indexSet1->remove_vertex(inter);
          topo->setHasChanged(true);
          impactOccuredLastTimeStep = true;
          inter->lambda(1)->zero(); // impulse is zero
        }
      }
    }
    else if (i == 2) // ACTIVE FOR CONTACT CALCULATIONS? Contacts which are closed but have not been closing in the last time step
    {
      DEBUG_PRINT("\nUPDATE INDEXSET 2\n");

      double y = (*(inter->y(0)))(0); // current position
      //double yOld = (*(inter->yOld(0)))(0); // old position
      double yDot = (*(inter->y(1)))(0); // current position
  
     
      DEBUG_PRINTF("y= %f\n", y);
      DEBUG_PRINTF("yDot= %f\n", yDot);

      DEBUG_EXPR(std::cout << std::boolalpha << (y <= DEFAULT_TOL_D1MINUS ) <<std::endl;);
      DEBUG_EXPR(std::cout << std::boolalpha << (yDot <= DEFAULT_TOL_D1MINUS ) <<std::endl;);

      if (indexSet2->is_vertex(inter))
      {
        if (Type::value(*(inter->nonSmoothLaw())) != Type::EqualityConditionNSL) /* Equality constraints must always be
                                                                                  * activated et the acceleration level*/
        {
          if ((y > DEFAULT_TOL_D1MINUS ) || (yDot > DEFAULT_TOL_D1MINUS ))

          {
            // if Interaction has been active in the previous calculation and now becomes in-active
            indexSet2->remove_vertex(inter);
            topo->setHasChanged(true);
            inter->lambda(2)->zero(); // force is zero
          }
        }
      }
      else
      {
        bool activate = true;
        if (Type::value(*(inter->nonSmoothLaw())) != Type::EqualityConditionNSL) /* Equality constraints must always be
                                                                                  * activated et the acceleration level*/
        {
          //     if (y <= DEFAULT_TOL_D1MINUS && !indexSet1->is_vertex(inter) && !impactOccuredLastTimeStep)
          if ((y <= DEFAULT_TOL_D1MINUS) && (yDot <= DEFAULT_TOL_D1MINUS ))
          {
            activate=true;
          }
          else
          {
            activate =false;
          }
        }


        if (activate)
        {
          // if Interaction has is active but has not become active recently
          indexSet2->copy_vertex(inter, *indexSet0);
          topo->setHasChanged(true);
        }
      }
    }
    else
      RuntimeException::selfThrow("TimeSteppingD1Minus::updateIndexSet, IndexSet[i > 2] does not exist.");
  }

  if (i==1)
  {
    if (forecastImpact) // we add equality constraints
    {
      for (std11::tie(uip, uipend) = indexSet0->vertices(); uip != uipend; ++uip) // loop over ALL verices in indexSet0
      {
        SP::Interaction inter = indexSet0->bundle(*uip);
        if (!indexSet1->is_vertex(inter))
        {
          if (Type::value(*(inter->nonSmoothLaw())) == Type::EqualityConditionNSL) /* We activate Equality constraintsonly if there is an impact.
                                                                                    * we add them a */
          {
            indexSet1->copy_vertex(inter, *indexSet0);
            topo->setHasChanged(true);
          }
        }
      }
    }
    else
    {
      for (std11::tie(uip, uipend) = indexSet0->vertices(); uip != uipend; ++uip) // loop over ALL verices in indexSet0
      {
        SP::Interaction inter = indexSet0->bundle(*uip);



        if (Type::value(*(inter->nonSmoothLaw())) == Type::EqualityConditionNSL) /* We activate Equality constraintsonly if there is an impact.
                                                                                  * we add them a */
        {
          if (indexSet1->is_vertex(inter))
          {
            indexSet1->remove_vertex(inter);
            topo->setHasChanged(true);
            impactOccuredLastTimeStep = true;
            inter->lambda(1)->zero(); // impulse is zero
          }

        }

      }
    }
  }

  DEBUG_PRINTF("\nINDEXSETS AFTER UPDATE for level i = %i\n", i);
  DEBUG_PRINTF("TimeSteppingD1MinusLinear::updateIndexSet(unsigned int i). update indexSets start : indexSet0 size : %ld\n", indexSet0->size());
  DEBUG_PRINTF("TimeSteppingD1MinusLinear::updateIndexSet(unsigned int i). update IndexSets start : indexSet1 size : %ld\n", indexSet1->size());
  DEBUG_PRINTF("TimeSteppingD1MinusLinear::updateIndexSet(unsigned int i). update IndexSets start : indexSet2 size : %ld\n", indexSet2->size());
  // DEBUG_EXPR(indexSet0->display());
  // DEBUG_PRINT("\n");
  // DEBUG_EXPR(indexSet1->display());
  // DEBUG_PRINT("\n");
  // DEBUG_EXPR(indexSet2->display());
}

void TimeSteppingD1Minus::update(unsigned int levelInput)
{
  // compute input (lambda -> r)
  if (!_allNSProblems->empty())
    updateInput(levelInput);

  // compute state for each dynamical system
  for (OSIIterator itOSI = _allOSI->begin(); itOSI != _allOSI->end(); ++itOSI)
    (*itOSI)->updateState(levelInput);

  // compute output (x -> y)
  if (!_allNSProblems->empty())
  {
    for (unsigned int level = _levelMinForOutput; level < _levelMaxForOutput; level++)
      updateOutput(level);
  }
}

void TimeSteppingD1Minus::run()
{
  unsigned int count = 0;
  cout << " ==== Start of " << Type::name(*this) << " simulation - This may take a while ... ====" << endl;
  while (_eventsManager->hasNextEvent())
  {
    advanceToEvent();

    processEvents();
    count++;
  }
  cout << "===== End of " << Type::name(*this) << "simulation. " << count << " events have been processed. ==== " << endl;
}

void TimeSteppingD1Minus::advanceToEvent()
{
  // we start after initialization (initOSNS) with
  // * initial state (q_0, v_0^+)
  // * updated indexset (I_0^+)
  // * updated  gaps and gap velocities (g_0^+)
  //
  // hence we end this procedure with
  // * state (q_{k+1}, v_{k+1}^+)
  // * updated gaps and gap velocities (g_{k+1}^+)
  // * indexset (I_{k+1}^+)

  // calculate residu without nonsmooth event with OSI
  // * calculate position q_{k+1} in ds->q()
  // * calculate velocity v_{k+1}^- and not free velocity in ds->velocity()
  // * calculate free residu in ds->free()
  computeResidu();

  // calculate state without nonsmooth event with OSI
  // * calculate free velocity and not v_{k+1}^- in ds->velocity
  computeFreeState();

  // event (impulse) calculation only when there has been a topology change (here: closing contact)
  // * calculate gap velocity using free velocity with OSI
  // * calculate local impulse (Lambda_{k+1}^+)
  //
  // Maurice Bremond: indices must be recomputed
  // as we deal with dynamic graphs, vertices and edges are stored
  // in lists for fast add/remove during updateIndexSet(i)
  // we need indices of list elements to build the OSNS Matrix so we
  // need an update if graph has changed
  // this should be done in updateIndexSet(i) for all integrators only
  // if a graph has changed
  //updateIndexSet(1);
  //model()->nonSmoothDynamicalSystem()->topology()->indexSet(1)->update_vertices_indices();
  //model()->nonSmoothDynamicalSystem()->topology()->indexSet(1)->update_edges_indices();

  //if(model()->nonSmoothDynamicalSystem()->topology()->hasChanged())
  //{
  //  for(OSNSIterator itOsns = _allNSProblems->begin(); itOsns != _allNSProblems->end(); ++itOsns)
  //  {
  //    (*itOsns)->setHasBeenUpdated(false);
  //  }
  //}

  if (!_allNSProblems->empty())
    computeOneStepNSProblem(SICONOS_OSNSP_TS_VELOCITY);

  // update on impulse level
  // * calculate global impulse (p_{k+1}^+)
  // * update velocity (v_{k+1}^+) with OSI in ds->velocity
  // * calculate local gaps (g_{k+1}^+)
  update(1);

  // indexset (I_{k+1}^+) is calculated in Simulation::processEvent
}

// void TimeSteppingD1Minus::updateInput(unsigned int level)
// {
//   //  assert(level>=0);

//   double time = model()->currentTime();
//   SP::Topology topology = model()->nonSmoothDynamicalSystem()->topology();
//   InteractionsIterator it;

//   // // set dynamical systems non-smooth part to zero.
//   // for (OSIIterator itOSI = _allOSI->begin(); itOSI != _allOSI->end(); ++itOSI)
//   // {
//   //   for (DSIterator itDS = (*itOSI)->dynamicalSystems()->begin(); itDS != (*itOSI)->dynamicalSystems()->end(); ++itDS)
//   //   {
//   //     Type::Siconos dsType = Type::value(**itDS);
//   //     if (dsType != Type::LagrangianDS && dsType != Type::LagrangianLinearTIDS)
//   //       RuntimeException::selfThrow("TimeSteppingD1Minus::updateInput - not implemented for Dynamical system type: " + dsType);
//   //     else
//   //     {
//   //       SP::LagrangianDS d = std11::static_pointer_cast<LagrangianDS> (*itDS);
//   //       if (d->p(level)) d->p(level)->zero();
//   //     }
//   //   }
//   // }
  
//   // Set dynamical systems non-smooth part to zero.
//   reset(level);


//   // we compute input using lambda(level).
//   for (it = topology->interactions()->begin(); it != topology->interactions()->end(); it++)
//   {
//     assert((*it)->lowerLevelForInput() <= level);
//     assert((*it)->upperLevelForInput() >= level);
//     (*it)->computeInput(time, level);
//   }
// }

void TimeSteppingD1Minus::computeResidu()
{
  for (OSIIterator it = _allOSI->begin(); it != _allOSI->end() ; ++it)
    (*it)->computeResidu();
}

void TimeSteppingD1Minus::computeFreeState()
{
  for_each(_allOSI->begin(), _allOSI->end(), std11::bind(&OneStepIntegrator::computeFreeState, _1));
}

TimeSteppingD1Minus* TimeSteppingD1Minus::convert(Simulation *str)
{
  return dynamic_cast<TimeSteppingD1Minus*>(str);
}
