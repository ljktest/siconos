/* Siconos-Kernel version 2.1.1, Copyright INRIA 2005-2007.
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
/*! \file DynamicalSystemsSet.h
Set of DynamicalSystem*
*/
#ifndef DSSET_H
#define DSSET_H

#include "DynamicalSystem.h"
#include "SiconosSet.h"

/** A set of pointers to dynamical systems, sorted in a growing order according to their numbers */
typedef SiconosSet<DynamicalSystem, int> DynamicalSystemsSet;
/** Iterator through a set of DS */
typedef std::set<DynamicalSystem*, Cmp<DynamicalSystem, int> >::iterator DSIterator;
/** const Iterator through a set of DS */
typedef std::set<DynamicalSystem*, Cmp<DynamicalSystem, int> >::const_iterator ConstDSIterator;
/** return type value for insert function - bool = false if insertion failed. */
typedef std::pair<DSIterator, bool> CheckInsertDS;

#endif
