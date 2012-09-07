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
#include "NonSmoothEvent.hpp"
#include "EventFactory.hpp"
#include "EventDriven.hpp"
#include "OneStepIntegrator.hpp"
#include "LagrangianDS.hpp"

using namespace std;
using namespace EventFactory;

// Default constructor
NonSmoothEvent::NonSmoothEvent(): Event(0.0, NS_EVENT)
{}

NonSmoothEvent::NonSmoothEvent(double time, int): Event(time, NS_EVENT)
{}

NonSmoothEvent::~NonSmoothEvent()
{}

void NonSmoothEvent::process(SP::Simulation simulation)
{
  if (Type::value(*simulation) != Type::EventDriven)
    RuntimeException::selfThrow("NonSmoothEvent::process failed; Simulation is not of EventDriven type.");

  if (!(simulation->oneStepNSProblems()->empty()))
  {
    SP::EventDriven eventDriven = boost::static_pointer_cast<EventDriven>(simulation);

    // Compute y[0], y[1] and update index sets. => already done
    // during advance to event ...

    //       simulation->updateOutput(0, 1);

    //       simulation->updateIndexSets();

    // Get the required index sets ...
    SP::InteractionsGraph indexSet0 = simulation->indexSet(0);
    SP::DynamicalSystemsGraph dsG = simulation->model()->nonSmoothDynamicalSystem()->topology()->dSG(0);
    DynamicalSystemsGraph::VIterator vi, viend;

    //
    SP::InteractionsGraph indexSet1 = simulation->indexSet(1);
    SP::InteractionsGraph indexSet2 = simulation->indexSet(2);
    bool found = true;
    InteractionsGraph::VIterator ui, uiend;
    for (boost::tie(ui, uiend) = indexSet1->vertices(); ui != uiend; ++ui)
    {
      found = indexSet2->is_vertex(indexSet1->bundle(*ui));
      if (!found) break;
    }
    /*
    // Display the variable before processing NSEvent
    cout<< "-------Before processing NS events---------" << endl;
    for (boost::tie(ui, uiend)=indexSet0->vertices(); ui != uiend; ++ui)
      {
        SP::Interaction inter = indexSet0->bundle(*ui);
        cout << "Velocity at this Interaction: " << (*inter->y(1))(0) << endl;
      }

    for (boost::tie(vi, viend) = dsG->vertices(); vi != viend; ++vi)
      {
        SP::DynamicalSystem ds = dsG->bundle(*vi);
        SP::LagrangianDS  Lag_ds = boost::static_pointer_cast<LagrangianDS>(ds);
        cout << "Velocity of DS: " << (*Lag_ds->velocity())(0) << endl;
      }
    */
    // ---> solve impact LCP if IndexSet[1]\IndexSet[2] is not empty.
    if (!found)
    {
      // For Event-Driven algo., memories vectors are of size 2
      // (ie 2 interactionBlocks).  First interactionBlock (pos 0, last
      // in) for post-event values and last interactionBlock (pos 1,
      // first in) for pre-event values.

      simulation->saveInMemory();  // To save pre-impact values

      // solve the LCP-impact => y[1],lambda[1]
      eventDriven->computeOneStepNSProblem(SICONOS_OSNSP_ED_IMPACT); // solveLCPImpact();
    }

    // compute p[1], post-impact velocity, y[1] and indexSet[2]
    simulation->update(1);
    // Update the corresponding index set ...
    eventDriven->updateIndexSets();
    /*
    // Display the variable after processing NSEvent
    cout<< "-------After processing NS events---------" << endl;
    for (boost::tie(ui, uiend)=indexSet0->vertices(); ui != uiend; ++ui)
      {
        SP::Interaction inter = indexSet0->bundle(*ui);
        cout << "Velocity at this Interaction: " << (*inter->y(1))(0) << endl;
      }

    for (boost::tie(vi, viend) = dsG->vertices(); vi != viend; ++vi)
      {
        SP::DynamicalSystem ds = dsG->bundle(*vi);
        SP::LagrangianDS Lag_ds = boost::static_pointer_cast<LagrangianDS>(ds);
        cout << "Velocity of DS: " << (*Lag_ds->velocity())(0) << endl;
      }
    //
    */
    // check that IndexSet[1]-IndexSet[2] is now empty if(
    //    !((*indexSet1-*indexSet2).isEmpty()))
    //    RuntimeException::selfThrow("NonSmoothEvent::process,
    //    error after impact-LCP solving."); ---> solve
    //    acceleration LCP if IndexSet[2] is not empty
    if (indexSet2->size() > 0)
    {
      // Update the state of the DS
      OSIIterator itOSI;
      for (itOSI = simulation->oneStepIntegrators()->begin();
           itOSI != simulation->oneStepIntegrators()->end() ;
           ++itOSI)
        (*itOSI)->updateState(2);

      // solve LCP-acceleration
      eventDriven->computeOneStepNSProblem(SICONOS_OSNSP_ED_ACCELERATION); // solveLCPAcceleration();

      // for all index in IndexSets[2], update the index set according to y[2] and/or lambda[2] sign.
      eventDriven->updateIndexSetsWithDoubleCondition();
    }

    // Save results in memory
    simulation->saveInMemory();
  }
}

AUTO_REGISTER_EVENT(NS_EVENT, NonSmoothEvent)
