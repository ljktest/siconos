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

/*! \file DynamicalSystemFactory.hpp
\brief  Factory to generate user-defined DynamicalSystems
*/

#ifndef DynamicalSystemFactory_H
#define DynamicalSystemFactory_H

#include "SiconosPointers.hpp"
#include "DynamicalSystem.hpp"
#include <string>
#include <map>


/** Namespace for DynamicalSystem factory related objects. */
namespace DynamicalSystemFactory
{

/** A pointer to function, returning a pointer to DynamicalSystem,
    built with its type (related to the class name) and a vector of
    initial conditions. */
typedef SP::DynamicalSystem(*object_creator)(int, const SiconosVector&) ;

/** The type of the factory map */
typedef std::map<int, object_creator> MapFactory;

/** An iterator through the MapFactory */
typedef MapFactory::iterator MapFactoryIt;

/** Template function to return a new object of type SubType
 * \param name
 * \param x0
 * \return SP::DynamicalSystem
 */
template<class SubType> SP::DynamicalSystem factory(int name, const SiconosVector& x0)
{
  SP::DynamicalSystem res(new SubType(name, x0));
  return res;
}

/** Registry Class for sensors.
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) November 2010
 *
 * DynamicalSystem factory.
 * Use:

 *     DynamicalSystemFactory::Registry&
 *        regDynamicalSystem(DynamicalSystemFactory::Registry::get()) ;
 *
 *     SP::DynamicalSystem yourDynamicalSystem =
 *        regDynamicalSystem.instantiate(type, x0);
 */
class Registry
{

private :

  /** map that links a std::string, the type of the class, to a pointer
      to function, used to build the object. */
  MapFactory factory_map;

public :

  /** Access function to the Registry
   * \return static Registry&
   */
  static Registry& get() ;

  /** Add an object_creator into the factory_map, factory_map[name] = object.
   * \param i an int, the name of the object added
   * \param obj an object creator
   */
  void add(int i, object_creator obj);

  /** Function to instantiate a new DynamicalSystem
   * \param i an int, the name of the object added (type name!)
   * \param x0 the initial condition (SP)
   * \return  SP::DynamicalSystem
   */
  SP::DynamicalSystem instantiate(int i, const SiconosVector& x0);
} ;

/** Registration Class for sensors.
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) November 2010
 *
 * Class used for auto-registration of DynamicalSystem-type objects.
 *
 */
class Registration
{
public :

  /** To register some new object into the factory
   * \param i an int, the name of the object to be registered
   * \param obj an object creator
   */
  Registration(int i, object_creator obj) ;
} ;

#define AUTO_REGISTER_DS(class_name,class_type) Registration _registration_## class_type(class_name,&factory<class_type>);
}
// end of namespace DynamicalSystemFactory

#endif
