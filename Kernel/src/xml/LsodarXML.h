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
/*! \file
*/

#ifndef __LsodarXML__
#define __LsodarXML__

#include "OneStepIntegratorXML.h"

/** XML management for Lsodar
 *
 *  \author SICONOS Development Team - copyright INRIA
 *   \version 2.1.1.
 *   \date 05/17/2004
 *
 *
 *
 * LsodarXML allows to manage data of a Lsodar DOM tree.
 */
class LsodarXML : public OneStepIntegratorXML
{
public:

  LsodarXML();

  /** Build a LsodarXML object from a DOM tree describing Lsodar OneStepIntegrator
  *   \param LsodarNode : the Lsodar DOM tree
  *   \param map<int, bool> definedDSNumbers : to know if DS numbers are not used by another OneStepIntegrator
  */
  LsodarXML(xmlNode * LsodarNode);
private:
};


#endif
