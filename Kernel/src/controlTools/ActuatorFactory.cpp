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

#include "ActuatorFactory.hpp"
#include "RuntimeException.hpp"



namespace ActuatorFactory
{

Registry& Registry::get()
{
  static Registry instance;
  return instance;
}

void Registry::add(unsigned int type, object_creator creator)
{
  factory_map[type] = creator;
}

SP::Actuator Registry::instantiate(unsigned int type, SP::ControlSensor sensor)
{
  MapFactoryIt it = factory_map.find(type);

  if (it == factory_map.end())
    RuntimeException::selfThrow("Registry::instantiate (ActuatorFactory) \
        failed, no class numbered: " + type);

  // std::cout <<std::endl << "Factory instance for class" << name <<std::endl ; // for test purposes only
  return (it->second)(sensor) ;  // run our factory
}

Registration::Registration(unsigned int type, object_creator creator)
{
  //  std::cout <<std::endl << "Registration of " << name <<std::endl <<std::endl ;
  // Add creator into the factory of Actuators
  Registry::get().add(type, creator) ;
}

}
