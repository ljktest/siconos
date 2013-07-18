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

#include "ControlManager.hpp"
#include "EventsManager.hpp"
#include "Simulation.hpp"
#include "Sensor.hpp"
#include "SensorFactory.hpp"
#include "ActuatorFactory.hpp"
#include "Observer.hpp"
#include "ObserverFactory.hpp"
#include "Simulation.hpp"
#include "TimeDiscretisation.hpp"

#include "SensorEvent.hpp"
#include "ActuatorEvent.hpp"
#include "ObserverEvent.hpp"


ControlManager::ControlManager(SP::Simulation sim): _sim(sim)
{
  if (!_sim)
    RuntimeException::selfThrow("ControlManager::constructor failed. The given Simulation is a NULL pointer.");
}

ControlManager::~ControlManager()
{}

void ControlManager::initialize(const Model& m)
{
  // Initialize all the Sensors and insert their events into the
  // EventsManager of the Simulation.
  for (SensorsIterator itS = _allSensors.begin();
       itS != _allSensors.end(); ++itS)
  {
    (*itS)->initialize(m);
  }
  // Initialize all the Actuators and insert their events into the
  // EventsManager of the Simulation.
  for (ActuatorsIterator itA = _allActuators.begin();
       itA != _allActuators.end(); ++itA)
  {
    (*itA)->initialize(m);
  }

  // Initialize all the Actuators and insert their events into the
  // EventsManager of the Simulation.
  for (ObserversIterator itO = _allObservers.begin();
       itO != _allObservers.end(); ++itO)
  {
    (*itO)->initialize(m);
  }
}

SP::Sensor ControlManager::addSensor(int type, SP::TimeDiscretisation td, SP::DynamicalSystem ds)
{
  SensorFactory::Registry& regSensor(SensorFactory::Registry::get()) ;
  SP::Sensor s = (* (_allSensors.insert(regSensor.instantiate(type, ds))).first);
  linkSensorSimulation(s, td);
  return s;
}

SP::Sensor ControlManager::addAndRecordSensor(int type, SP::TimeDiscretisation td, SP::DynamicalSystem ds)
{
  SensorFactory::Registry& regSensor(SensorFactory::Registry::get()) ;
  SP::Sensor s = *(_allSensors.insert(regSensor.instantiate(type, ds))).first;
  linkSensorSimulation(s, td);
  s->initialize(*_sim->model());
  return s;
}

SP::Actuator ControlManager::addActuator(int type, SP::TimeDiscretisation td, SP::ControlSensor sensor)
{
  ActuatorFactory::Registry& regActuator(ActuatorFactory::Registry::get()) ;
  SP::Actuator act = (* (_allActuators.insert(regActuator.instantiate(type, sensor))).first);
  linkActuatorSimulation(act, td); 
  return act;
}

SP::Actuator ControlManager::addAndRecordActuator(int type, SP::TimeDiscretisation td, SP::ControlSensor sensor)
{
  ActuatorFactory::Registry& regActuator(ActuatorFactory::Registry::get()) ;
  SP::Actuator act = *(_allActuators.insert(regActuator.instantiate(type, sensor))).first;
  linkActuatorSimulation(act, td);
  act->initialize(*_sim->model());
  return act;
}

SP::Observer ControlManager::addObserver(int type, SP::TimeDiscretisation td, SP::ControlSensor sensor, const SiconosVector& xHat0)
{
  ObserverFactory::Registry& regObserver(ObserverFactory::Registry::get()) ;
  SP::Observer obs = (* (_allObservers.insert(regObserver.instantiate(type, sensor, xHat0))).first);
  linkObserverSimulation(obs, td);
  return obs;
}

SP::Observer ControlManager::addAndRecordObserver(int type, SP::TimeDiscretisation td, SP::ControlSensor sensor, const SiconosVector& xHat0)
{
  ObserverFactory::Registry& regObserver(ObserverFactory::Registry::get()) ;
  SP::Observer obs = *(_allObservers.insert(regObserver.instantiate(type, sensor, xHat0))).first;
  linkObserverSimulation(obs, td);
  obs->initialize(*_sim->model());
  return obs;
}

void ControlManager::addSensorPtr(SP::Sensor s, SP::TimeDiscretisation td)
{
  _allSensors.insert(s);
  linkSensorSimulation(s, td);
}

void ControlManager::addAndRecordSensorPtr(SP::Sensor s, SP::TimeDiscretisation td)
{
  _allSensors.insert(s);
  linkSensorSimulation(s, td);
  s->initialize(*_sim->model());
}

void ControlManager::addActuatorPtr(SP::Actuator act, SP::TimeDiscretisation td)
{
  _allActuators.insert(act);
  linkActuatorSimulation(act, td);
}

void ControlManager::addAndRecordActuatorPtr(SP::Actuator act, SP::TimeDiscretisation td)
{
  _allActuators.insert(act);
  linkActuatorSimulation(act, td);
  act->initialize(*_sim->model());
}

void ControlManager::addObserverPtr(SP::Observer obs, SP::TimeDiscretisation td)
{
  _allObservers.insert(obs);
  linkObserverSimulation(obs, td);
}

void ControlManager::addAndRecordObserverPtr(SP::Observer obs, SP::TimeDiscretisation td)
{
  _allObservers.insert(obs);
  linkObserverSimulation(obs, td);
  obs->initialize(*_sim->model());
}

void ControlManager::linkSensorSimulation(SP::Sensor s, SP::TimeDiscretisation td)
{
  Event& ev = _sim->eventsManager()->insertEvent(SENSOR_EVENT, td);
  static_cast<SensorEvent&>(ev).setSensorPtr(s);
  s->setTimeDiscretisation(*td);
}

void ControlManager::linkActuatorSimulation(SP::Actuator act, SP::TimeDiscretisation td)
{
  Event& ev = _sim->eventsManager()->insertEvent(ACTUATOR_EVENT, td);
  static_cast<ActuatorEvent&>(ev).setActuatorPtr(act);
  act->setTimeDiscretisation(*td);
}

void ControlManager::linkObserverSimulation(SP::Observer obs, SP::TimeDiscretisation td)
{
  Event& ev = _sim->eventsManager()->insertEvent(OBSERVER_EVENT, td);
  static_cast<ObserverEvent&>(ev).setObserverPtr(obs);
  obs->setTimeDiscretisation(*td);
}

void ControlManager::display() const
{
  std::cout << "=========> ControlManager " ;
  std::cout << "It handles the following objects: " <<std::endl;
  SensorsIterator itS;
  for (itS = _allSensors.begin(); itS != _allSensors.end(); ++itS)
    (*itS)->display();
  ActuatorsIterator itA;
  for (itA = _allActuators.begin(); itA != _allActuators.end(); ++itA)
    (*itA)->display();
  ObserversIterator itO;
  for (itO = _allObservers.begin(); itO != _allObservers.end(); ++itO)
    (*itO)->display();
  std::cout << "==========" << std::endl;
  std::cout << std::endl;
}
