/* Siconos-Kernel, Copyright INRIA 2005-2014
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
#include "Model.hpp"
#include "EventsManager.hpp"
#include "Event.hpp"
#include "NonSmoothDynamicalSystem.hpp"
#include "SimulationGraphs.hpp"

#include "ControlLinearAdditionalTermsED.hpp"
#include "ControlManager.hpp"
#include "ControlLsodarSimulation.hpp"
#include "ControlSimulation_impl.hpp"
#include "Observer.hpp"
#include "Actuator.hpp"

#include <boost/progress.hpp>
#include <boost/timer.hpp>



ControlLsodarSimulation::ControlLsodarSimulation(double t0, double T, double h):
  ControlSimulation(t0, T, h)
{
  _processIntegrator.reset(new LsodarOSI());
  _processSimulation.reset(new EventDriven(_processTD, 0));
  _processSimulation->setName("plant simulation");
  _processSimulation->insertIntegrator(_processIntegrator);
  std11::static_pointer_cast<LsodarOSI>(_processIntegrator)->setExtraAdditionalTerms(
      std11::shared_ptr<ControlLinearAdditionalTermsED>(new ControlLinearAdditionalTermsED()));

  _DSG0 = _model->nonSmoothDynamicalSystem()->topology()->dSG(0);
  _IG0 = _model->nonSmoothDynamicalSystem()->topology()->indexSet0();

  // Control part
  _CM.reset(new ControlManager(_processSimulation));
}

void ControlLsodarSimulation::run()
{
  EventsManager& eventsManager = *_processSimulation->eventsManager();
  unsigned k = 0;
  boost::progress_display show_progress(_N);
  boost::timer time;
  time.restart();
  EventDriven& sim = static_cast<EventDriven&>(*_processSimulation);

  while (sim.hasNextEvent())
  {
    if (eventsManager.needsIntegration())
    {
      sim.advanceToEvent();
    }
    sim.processEvents();
    Event& currentEvent = *eventsManager.currentEvent();
    if (currentEvent.getType() == ACTUATOR_EVENT)
    {
      // this is necessary since we changed the control input, hence the RHS
      sim.setIstate(1);
    }
    if (sim.hasNextEvent() && eventsManager.nextEvent()->getType() == TD_EVENT) // We store only on TD_EVENT, this should be settable
    {
      (*_dataM)(k, 0) = sim.startingTime();
      storeData(k);
      ++k;
      ++show_progress;
    }
  }

  /* saves last status */
  (*_dataM)(k, 0) = sim.startingTime();
  storeData(k);
  ++k;

  _elapsedTime = time.elapsed();
  _dataM->resize(k, _nDim + 1);
}
