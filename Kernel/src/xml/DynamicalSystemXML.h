/* Siconos-Kernel version 3.0.0, Copyright INRIA 2005-2008.
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

/*! \file DynamicalSystemXML.h

*/

#ifndef __DynamicalSystemXML__
#define __DynamicalSystemXML__

#include "SiconosDOMTreeTools.h"
#include "SimpleVector.h"


class SimpleVector;
class SiconosVector;

/** XML management for DynamicalSystem
 *
 *  \author SICONOS Development Team - copyright INRIA
 *   \version 3.0.0.
 *   \date 04/04/2004
 *
 * Reading/Writing of xml data for the DynamicalSystem class and its derived classes.
 *
 */
class DynamicalSystemXML
{

protected:

  xmlNodePtr rootNode;/**< Dynamical System Node  */
  xmlNodePtr stepsInMemoryNode; /**< size of memory */
  xmlNodePtr zNode; /**< z in \f$ M \dot x = f(x,t,z) \f$ */

  /** Default constructor */
  DynamicalSystemXML();

public:

  /** Build a DynamicalSystemXML object from the DynamicalSystem node of the xml DOMtree
   *   \param an xmlNodePtr DynamicalSystemNode
   *   \param bool isBVP : if true, NonSmoothDynamicalSystem is a Boundary value problem
   */
  DynamicalSystemXML(xmlNodePtr DynamicalSystemNode, bool);

  /** Destructor */
  virtual inline ~DynamicalSystemXML() {};

  /** Return the number of the DynamicalSystem
   *   \return an integer
   */
  inline const int getNumber() const
  {
    return SiconosDOMTreeTools::getAttributeValue<int>(rootNode, NUMBER_ATTRIBUTE);
  }

  /** Return the type of the DynamicalSystem
   *   \return a string
   */
  inline const std::string getType() const
  {
    std::string res((char*)rootNode->name);
    return res;
  }

  /** Return the id of the DynamicalSystem
   *   \return The string id of the DynamicalSystem
   */
  inline const std::string getId() const
  {
    return SiconosDOMTreeTools::getStringAttributeValue(rootNode, "Id");
  }

  /** return true if id is given
   *  \return a bool
   */
  inline bool hasId() const
  {
    return (xmlHasProp(rootNode, (xmlChar*)"Id"));
  }

  /** to save the id of the DynamicalSystem
   *   \param a string
   */
  inline void setId(const std::string& s)
  {
    SiconosDOMTreeTools::setStringAttributeValue(rootNode, "Id", s);
  }

  /** Returns the z vector, discret state of the DynamicalSystem
   *  \return SimpleVector
   */
  inline const SimpleVector getZ() const
  {
    return  SiconosDOMTreeTools::getSiconosVectorValue(zNode);
  }

  /** save z of the DynamicalSystem
   *   \param a SiconosVector
   */
  inline void setZ(const SiconosVector& v)
  {
    if (!hasZ())
      zNode = SiconosDOMTreeTools::createVectorNode(rootNode, "z", v);
    else SiconosDOMTreeTools::setSiconosVectorNodeValue(zNode, v);
  }

  /** Returns the steps in memory for the DynamicalSystemXML
   *   \return The integer number of steps in memory for the DynamicalSystemXML
   */
  inline const unsigned int getStepsInMemory() const
  {
    return  SiconosDOMTreeTools::getContentValue<unsigned int>(stepsInMemoryNode);
  }

  /** to save the steps in memory for the DynamicalSystemXML
   *   \param an unsigned int
   */
  void setStepsInMemory(const unsigned int&);

  /** returns true if stepsInMemoryNode is defined
   *  \return a bool
   */
  inline bool hasStepsInMemory() const
  {
    return (stepsInMemoryNode != NULL);
  }

  /** returns true if zNode is defined
   *  \return a bool
   */
  inline bool hasZ() const
  {
    return (zNode != NULL);
  }

};

#endif
