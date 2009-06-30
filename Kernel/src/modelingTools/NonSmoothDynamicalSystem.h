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
/*! \file NonSmoothDynamicalSystem.h
 */
#ifndef NSDS_H
#define NSDS_H

/** Available Dynamical Systems types*/
enum dynamicalsystem {LAGRANGIANNLDS, LAGRANGIANTIDS, LINEARTIDS};

#include "SiconosPointers.hpp"
#include "InteractionsSet.hpp"
#include "DynamicalSystemsSet.hpp"
#include "Topology.h"

class Interaction;
class DynamicalSystem;
class Topology;


/** the Non Smooth Dynamical System composed with dynamical systems that interact alltogether.
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) Apr 23, 2004
 *
 */
class NonSmoothDynamicalSystem
{
private:

  /** TRUE if the NonSmoothDynamicalSystem is a boundary value problem*/
  bool BVP;

  /** contains all the Dynamic Systems of the simulation */
  SP::DynamicalSystemsSet allDS;

  /** to build the interactions set */
  SP::InteractionsSet allInteractionsLocal;

  /** the topology of the system */
  SP::Topology topology;

  /** the XML object linked to the NonSmoothDynamicalSystem to read XML data */
  SP::NonSmoothDynamicalSystemXML nsdsxml;

  /** default constructor
   */
  NonSmoothDynamicalSystem(): BVP(false) {};

  /** copy constructor => private: no copy nor pass-by value.
   */
  NonSmoothDynamicalSystem(const NonSmoothDynamicalSystem&);

  /**
   * False is one of the interaction is non-linear.
   */
  bool mIsLinear;

public:

  /** xml constructor
   *  \param: the XML object corresponding to the NonSmoothDynamicalSystem
   */
  NonSmoothDynamicalSystem(SP::NonSmoothDynamicalSystemXML);

  /** constructor from minimum data.
   *  \param: a pointer to DynamicalSystem
   *  \param: a pointer to Interaction
   *  \param: a bool
   */
  NonSmoothDynamicalSystem(SP::DynamicalSystem, SP::Interaction = SP::Interaction(), const bool& = false);

  /** constructor from data - Warning: DS and Interactions are not copied, but links are created
   *  between pointers of the two sets.
   *  \param: a set of DS
   *  \param: a set of Interactions
   *  \param: a bool
   */
  NonSmoothDynamicalSystem(DynamicalSystemsSet&, InteractionsSet&, const bool& = false);

  /** constructor from data (only DS, no Interactions)
   *  between pointers of the two sets.
   *  \param: a set of DS
   *  \param: a bool
   */
  NonSmoothDynamicalSystem(DynamicalSystemsSet&, const bool& = false);

  /** destructor
   */
  ~NonSmoothDynamicalSystem();

  // --- GETTERS/SETTERS ---

  /** get problem type (true if BVP)
   *  \return a bool
   */
  inline const bool isBVP() const
  {
    return BVP;
  }

  /** get problem type (true if IVP)
   *  \return a bool
   */
  inline const bool isIVP() const
  {
    return !BVP;
  }

  /** set the NonSmoothDynamicalSystem to BVP, else it is IVP
   *  \param bool : true if BVP, false otherwise
   */
  inline void setBVP(const bool& newBvp)
  {
    BVP = newBvp;
  }

  // === DynamicalSystems management ===

  /** get the number of Dynamical Systems present in the NSDS (ie in allDS set)
   *  \return an unsigned int
   */
  inline const unsigned int getNumberOfDS() const
  {
    return allDS->size();
  };

  /** get all the DynamicalSystem of the NonSmoothDynamicalSystem problem (saved in a set)
   *  \return a DynamicalSystemsSet *
   */
  inline const SP::DynamicalSystemsSet getDynamicalSystems() const
  {
    return allDS;
  }

  /** get all the DynamicalSystem of the NonSmoothDynamicalSystem problem (saved in a set)
   *  \return a DynamicalSystemsSet *
   */
  inline SP::DynamicalSystemsSet getDynamicalSystems()
  {
    return allDS;
  }

  /** iterator equal to the first element of setOfDS
   *  \return a DSIterator
   */
  inline DSIterator dynamicalSystemsBegin()
  {
    return allDS->begin();
  };

  /** iterator equal to allDS->end()
   *  \return a DSIterator
   */
  inline DSIterator dynamicalSystemsEnd()
  {
    return allDS->end();
  }

  /** const iterator equal to the first element of allDS
   *  \return a ConstDSIterator
   */
  inline ConstDSIterator dynamicalSystemsBegin() const
  {
    return allDS->begin();
  };

  /** const iterator equal to allDS->end()
   *  \return a ConstDSIterator
   */
  inline ConstDSIterator dynamicalSystemsEnd() const
  {
    return allDS->end();
  }

  /** get Dynamical system number I
   *  \param the identifier of the DynamicalSystem to get
   *  \return a pointer on DynamicalSystem
   */
  SP::DynamicalSystem getDynamicalSystemPtrNumber(int) const ;

  /** to set allDS
   *  \param a DynamicalSystemsSet
   */
  void setDynamicalSystems(const DynamicalSystemsSet&) ;

  /** check if DynamicalSystem number N exists
   *  \param the identifier of the DynamicalSystem to get
   *  \return bool
   */
  const bool hasDynamicalSystemNumber(const int&) const ;

  /** check if DynamicalSystem ds is in the set
   *  \param a pointer to DynamicalSystem
   *  \return a bool
   */
  const bool hasDynamicalSystem(SP::DynamicalSystem) const;

  // === Interactions management ===

  /** get the number of Interactions present in the NSDS (ie in allInteractions set)
   *  \return an unsigned int
   */
  inline const unsigned int getNumberOfInteractions() const
  {
    return topology->getInteractionsPtr()->size();
  };

  /** get all the Interactions of the NonSmoothDynamicalSystem problem (saved in a set)
   *  \return an InteractionsSet *
   */
  inline const SP::InteractionsSet getInteractionsPtr() const
  {
    return topology->getInteractionsPtr();
  }

  /** iterator equal to the first element of the set of Interactions
   *  \return an InteractionsIterator
   */
  inline InteractionsIterator interactionsBegin()
  {
    return topology->getInteractionsPtr()->begin();
  };

  /** iterator equal to allInteractions->end()
   *  \return an InteractionsIterator
   */
  inline InteractionsIterator interactionsEnd()
  {
    return topology->getInteractionsPtr()->end();
  }

  /** const iterator equal to the first element of allInteractions
   *  \return a ConstInteractionsIterator
   */
  inline ConstInteractionsIterator interactionsBegin() const
  {
    return topology->getInteractionsPtr()->begin();
  };

  /** const iterator equal to allInteractions->end()
   *  \return a ConstInteractionsIterator
   */
  inline ConstInteractionsIterator interactionsEnd() const
  {
    return topology->getInteractionsPtr()->end();
  }

  /** get Interaction at indix position in the set
   *  \param an int
   *  \return a pointer on Interaction
   */
  SP::Interaction getInteractionPtr(const int&) const ;

  /** get Interaction number I
   *  \param the id-number of the Interaction to get
   *  \return a pointer on Interaction
   */
  SP::Interaction getInteractionPtrNumber(const int&) const ;

  /** to set allInteractions
   *  \param an InteractionsSet
   */
  void setInteractions(const InteractionsSet&) ;

  /** check if Interaction number N exists
   *  \param the identifier of the Interaction to get
   *  \return bool
   */
  const bool hasInteractionNumber(const int&) const;

  /** check if Interaction inter is in the set
   *  \param a pointer to Interaction
   *  \return a bool
   */
  const bool hasInteraction(SP::Interaction) const;

  /** get the topology of the system
   *  \return a pointer on Topology
   */
  inline SP::Topology getTopologyPtr() const
  {
    return topology;
  }

  /** get the xml linked object
   *  \return a pointer on NonSmoothDynamicalSystemXML
   */
  inline SP::NonSmoothDynamicalSystemXML getNonSmoothDynamicalSystemXMLPtr() const
  {
    return nsdsxml;
  }

  /** set the xml linked object
   *  \param NonSmoothDynamicalSystemXML* : a pointer on NonSmoothDynamicalSystemXML* to link
   */
  inline void setNonSmoothDynamicalSystemXMLPtr(SP::NonSmoothDynamicalSystemXML newNsdsxml)
  {
    nsdsxml = newNsdsxml;
  }

  // --- OTHER FUNCTIONS ---

  /** copy the data of the NonSmoothDynamicalSystem to the XML tree
   *  \exception RuntimeException
   */
  void saveNSDSToXML();

  /** display the data of the Non Smooth Dynamical System
   */
  void display() const;

  /** add a DynamicalSystem into the NonSmoothDynamicalSystem (pointer link, no copy!)
   *  \param SP::DynamicalSystem : the DynamicalSystem to add
   */
  void addDynamicalSystemPtr(SP::DynamicalSystem);

  /** add an Interaction into the NonSmoothDynamicalSystem (pointer link, no copy!)
   *  \param Interaction : the Interaction to add
   */
  void addInteractionPtr(SP::Interaction);

  /** calculate an indicator that gives convergence information for the DSs
   *  \return a double
   */
  double nsdsConvergenceIndicator() ;

  /**
   * return false is one of the interations is not linear.
   *        else return true.
   *   \return a bool
   */
  bool isLinear();

  void clear();

};

#endif
