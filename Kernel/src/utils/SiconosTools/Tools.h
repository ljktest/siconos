/* Siconos-Kernel version 2.0.1, Copyright INRIA 2005-2006.
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
 * Contact: Vincent ACARY vincent.acary@inrialpes.fr
 */

/*! \file Tools.h
  Various useful functions and typedef.
*/

#ifndef TOOLS
#define TOOLS

#include<string>
#include <sstream>
#include <map>
#include <deque>

/** A map to link string to bool (for plug-in flags)  */
typedef std::map<std::string, bool> BoolMap;

/** Type used for inside-class allocation checking */
typedef std::deque<bool> AllocationFlags;

/** Map used to save the list of plug-in names. */
typedef std::map<std::string, std::string> NamesList;

/** Iterator through a list of names. */
typedef NamesList::iterator NamesIterator;

/** const Iterator through a list of names. */
typedef NamesList::const_iterator NamesConstIterator;

/** A function to convert any type to string*/
template <class T> std::string toString(const T& obj)
{
  static std::ostringstream o;
  o.str("");
  o << obj ;
  return o.str();
}

#endif
