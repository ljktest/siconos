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
/*! \file InteractionXML.hpp
*/

#ifndef __INTERACTIONXML__
#define __INTERACTIONXML__

#include "SiconosDOMTreeTools.hpp"
#include "SiconosVector.hpp"

class DynamicalSystem;
class Interaction;
class NonSmoothLawXML;
class RelationXML;

/**  XML management for object Interaction
 *
 *  \author SICONOS Development Team - copyright INRIA
 *   \version 3.0.0.
 *   \date (Creation) 04/12/2004
 *
 */
class InteractionXML
{
private:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(InteractionXML);


  /** root node -> named "Interaction"*/
  xmlNodePtr  rootNode;

  /** size of the interaction (ie of y and lambda) */
  xmlNodePtr  sizeNode;

  /** y[0] */
  xmlNodePtr  yNode;

  /** lambda[0] */
  xmlNodePtr  lambdaNode;

  /** list of DS handled by the interaction */
  xmlNodePtr  DSConcernedNode;

  /** Relation node */
  xmlNodePtr  relationNode;

  /** Non smooth law*/
  xmlNodePtr nsLawNode;

  /** Relation xml object */
  SP::RelationXML relationXML;

  /** Non smooth law xml object*/
  SP::NonSmoothLawXML nSLawXML;

  /** Flags to know if pointers have been allocated inside constructors or not */

public:

  /** Default constructor
  */
  InteractionXML();

  /** Build a InteractionXML object from a DOM tree describing a Interaction
  *   \param xmlNodePtr  interactionNode : the Interaction DOM tree
  */
  InteractionXML(xmlNodePtr);

  /** Destructor
  */
  ~InteractionXML();

  /** Return the root node of the InteractionXML
  *   \return xmlNodePtr
  */
  inline xmlNodePtr getRootNode() const
  {
    return rootNode;
  }

  /** return true if id attribute is present
  *  \return a bool
  */
  inline bool hasId() const
  {
    return SiconosDOMTreeTools::hasAttributeValue(rootNode, ID_ATTRIBUTE);
  };

  /** return true if number attribute is present
  *  \return a bool
  */
  inline bool hasNumber() const
  {
    return SiconosDOMTreeTools::hasAttributeValue(rootNode, NUMBER_ATTRIBUTE);
  };

  /** Return the id of the Interaction (rootNode attribute)
  *   \return a std::string
  */
  inline const std::string getId() const
  {
    if (!hasId())
      XMLException::selfThrow("InteractionXML::getId(): id attribute is unset.");
    return SiconosDOMTreeTools::getStringAttributeValue(rootNode, ID_ATTRIBUTE);
  }

  /** to save the id of the Interaction (rootNode attribute)
  *   \param a std::string
  */
  inline void setId(const std::string  newId)
  {
    SiconosDOMTreeTools::setStringAttributeValue(rootNode, ID_ATTRIBUTE, newId);
  }

  /** Return the number of the Interaction (rootNode attribute)
  *   \return an int
  */
  inline int number() const
  {
    if (!hasNumber())
      XMLException::selfThrow("InteractionXML::number(): number attribute is unset.");
    return SiconosDOMTreeTools::getAttributeValue<int>(rootNode, NUMBER_ATTRIBUTE);
  }

  /** to save the number of the Interaction (rootNode attribute)
  *  \param an int
  */
  inline void setNumber(const int i)
  {
    SiconosDOMTreeTools::setIntegerAttributeValue(rootNode, NUMBER_ATTRIBUTE, i);
  }

  /** return true if size node is present
  *  \return a bool
  */
  bool hasSize() const
  {
    return (sizeNode);
  };

  /** Return the size of the InteractionXML
  *   \return an unsigned int
  */
  inline unsigned int getSize() const
  {
    if (!hasSize())
      XMLException::selfThrow("InteractionXML::getSize() : sizeNode == NULL");
    return SiconosDOMTreeTools::getContentValue<int>(sizeNode);
  }

  /** to save the size of the Interaction
  *  \return an unsigned int
  */
  void setSize(const unsigned int);

  /** return true if yNode is defined
  *  \return a bool
  */
  inline bool hasY() const
  {
    return (yNode);
  }

  /** Return y vector of the InteractionXML
  *  \return a SiconosVector
  */
  inline SiconosVector getY() const
  {
    if (!hasY())
      XMLException::selfThrow("InteractionXML::getY() : yNode == NULL");
    return SiconosDOMTreeTools::getSiconosVectorValue(yNode);
  }

  /** to save y[0] of the Interaction
  *   \param a SiconosVector*
  */
  void setY(const SiconosVector&);

  /** return true if lambdaNode is defined
  *  \return true if lambdaNode is defined
  */
  inline bool hasLambda() const
  {
    return (lambdaNode);
  };

  /** Return lambda[0] of the Interaction
  *   \return a SiconosVector
  */
  inline SiconosVector getLambda() const
  {
    if (!hasLambda())
      XMLException::selfThrow("InteractionXML::getLambda() : lambdaNode == NULL");
    return SiconosDOMTreeTools::getSiconosVectorValue(lambdaNode);
  }

  /** to save lambda[0]
  *   \return a SiconosVector*
  */
  void setLambda(const SiconosVector&);

  /** true if tag DSConcerned is present
  *  \return a bool
  */
  inline bool hasDSList() const
  {
    return DSConcernedNode;
  }

  /** get the DSConcerned node
  *  \return xmlNodePtr
  */
  inline xmlNodePtr getDSConcernedNode() const
  {
    return DSConcernedNode;
  }

  /** attribute of the DSConcerned tag - True if all DS of the NSDS are concerned
  *  \return a bool
  */
  inline bool hasAllDS() const
  {
    if (SiconosDOMTreeTools::hasAttributeValue(DSConcernedNode, ALL_ATTRIBUTE))
      return SiconosDOMTreeTools::getAttributeValue<bool>(DSConcernedNode, ALL_ATTRIBUTE);
    else return false;
  }

  /** return a vector<int> of ds numbers related to the Interaction
  *  \param in-out vector<int>
  */
  void getDSNumbers(std::vector<int>&);

  /** set the DSs concerned by the InteractionXML
  *   \param a SiconosVector which contains a list of DS number
  */
  //\todo inline SiconosVector setDSConcernedVector(){};

  /** true if tag relation is present
  *  \return a bool
  */
  inline bool hasRelation() const
  {
    return relationNode;
  };

  /** get the relation node
  *  \return xmlNodePtr
  */
  inline xmlNodePtr getRelation() const
  {
    return relationNode;
  }

  /** true if tag nonSmoothLaw is present
  *  \return a bool
  */
  inline bool hasNonSmoothLaw() const
  {
    return nsLawNode;
  };

  /** get the nsLawNode  node
  *  \return xmlNodePtr
  */
  inline xmlNodePtr getNonSmoothLaw() const
  {
    return nsLawNode;
  }

  /** Return the relationXML of the InteractionXML
  *   \return The relationXML of the InteractionXML
  */
  inline SP::RelationXML getRelationXML()
  {
    return relationXML;
  }

  /** Return the NonSmoothLawXML of the InteractionXML
  *   \return The NonSmoothLawXML of the InteractionXML
  */
  inline SP::NonSmoothLawXML getNonSmoothLawXML()
  {
    return nSLawXML;
  }

  /** makes the operations to add an Interaction to the NSDS
  *   \param xmlNodePtr  : the root node of the InteractionXML
  *   \param SP::Interaction : the Interaction of this InteractionXML
  */
  void updateInteractionXML(xmlNodePtr  node, SP::Interaction inter);

  /** loads the data of the Interaction into the InteractionXML (in the DOM tree)
  *   \param NSDS* : the Interaction of this InteractionXML
  */
  void loadInteraction(SP::Interaction);
};

#endif
