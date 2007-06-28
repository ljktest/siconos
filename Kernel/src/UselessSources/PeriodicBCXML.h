/* Siconos-Kernel version 2.1.0, Copyright INRIA 2005-2006.
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
/*! \file
*/


#ifndef __PERIODICBCXML__
#define __PERIODICBCXML__
#include "BoundaryConditionXML.h"

//!  XML management for  PeriodicBC
/**  \author SICONOS Development Team - copyright INRIA
*   \version 2.1.0.
*   \date 05/25/2004
*
*/
class PeriodicBCXML : public BoundaryConditionXML
{
public:

  PeriodicBCXML();

  /** Build a PeriodicBCXML object from a DOM tree describing a PeriodicBC
  *   \param xmlNode * PeriodicBCNode : the PeriodicBC DOM tree
  */
  PeriodicBCXML(xmlNode * PeriodicBCNode);

  ~PeriodicBCXML();


  /** makes the operations to add a BoundaryCondition to the DynamicalSystemXML
  *  \param xmlNode* : the root node of this BoundaryCondition
  //     *  \param BoundaryCondition* : the BoundaryCondition of the DS
  */
  void updateBoundaryConditionXML(xmlNode* node/*, BoundaryCondition* bc */);
};


#endif
