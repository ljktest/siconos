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

/*! \file Topology.h

*/
#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include "SiconosConst.h"
#include "InteractionsSet.hpp"
#include "UnitaryRelationsSet.hpp"

class NonSmoothDynamicalSystem;
class Interaction;
class SiconosMatrix;
class UnitaryRelation;


/** map that links a SP::UnitaryRelation to an int - Used for Relative degrees */
typedef std::map< SP::UnitaryRelation, unsigned int > UnitaryRelationsIntMap;

/** iterator through UnitaryRelationsIntMap */
typedef UnitaryRelationsIntMap::iterator IteratorForRelativeDegrees;
/** const iterator through UnitaryRelationsIntMap */
typedef UnitaryRelationsIntMap::const_iterator ConstIteratorForRelativeDegrees;

/**  This class describes the topology of the non-smooth dynamical system. It holds all the "potential" Unitary Relations and their Relative Degrees.
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) July 20, 2005
 *
 *  Topology is built in NSDS constructors but initialized in Simulation->initialize(), ie when all Interactions have been clearly defined.
 *
 * Note that indexSet0 holds all the possible relations (declared by user) not only those which are "actives".
 *
 * Construction consists in:
 *    - link with the NSDS that owns the topology.
 *
 * Initialization consists in:
 *    - scan of all the interactions of the NSDS
 *    - initialization of each interaction
 *    - insertion of the relations of all the Interaction into indexSet0
 *    - computation of all relative degrees
 *
 * Insertion of an Interaction into the set indexSet0: addInteractionInIndexSet(SP::Interaction inter)
 *   for each relation of the interaction, it creates a new UnitaryRelation and inserts it into indexSet0
 *   It also counts the total number of "constraints" in the system.
 *
 * Relative degrees computation: at the time, depends only on the non-smooth law type of each UnitaryRelation.
 * Degrees are saved in a map that links each UnitaryRelation to an int, its relative degree.
 *
 * Rule: no Unitary Relation can be created outside of the present class (ie the only calls to new UnitaryRelation(...) are part of Topology methods)
 * Thus, no flags for inside-class allocation - All UnitaryRelation pointers are cleared during Topology destructor call.
 *
 */
class Topology
{

private:

  // --- MEMBERS ---

  /** the set of all the interactions of the system */
  SP::InteractionsSet allInteractions;

  /** index set I0, ie a set of all the Unitary Relations - This corresponds to indexSets[0] of the Simulation */
  SP::UnitaryRelationsSet indexSet0;

  /** map that links UnitaryRelations with their relative degrees */
  UnitaryRelationsIntMap relativeDegrees;

  /** check if topology has been updated since nsds modifications occur */
  bool isTopologyUpToDate;

  /** check if topology is static (all relative degrees = 0 or 1) or not */
  bool isTopologyTimeInvariant;

  /** Total number of (scalar) constraints in the problem, ie sum of all nslaw sizes of Unitary Relations of IndexSet0.*/
  unsigned int numberOfConstraints;

  // === PRIVATE FUNCTIONS ===

  /** schedules the relations of Interaction inter into IndexSet0 (ie creates the corresponding UnitaryRelations)
  * \param: a pointer to Interaction
  */
  const bool addInteractionInIndexSet(SP::Interaction);

  /** compute the  RelativeDegrees Map
  */
  void computeRelativeDegrees();

  /** default constructor
  */
  Topology();

public:

  // --- CONSTRUCTORS/DESTRUCTOR ---

  /** constructor from InteractionSet
  * \param: a SP::InteractionSet
  */
  Topology(SP::InteractionsSet) ;

  /** destructor */
  ~Topology();

  // === GETTERS/SETTERS ===

  /** iterator equal to the first element of allInteractions
  *  \return a InteractionsIterator
  */
  inline InteractionsIterator interactionsBegin()
  {
    return allInteractions->begin();
  };

  /** iterator equal to allInteractions->end()
  *  \return a InteractionsIterator
  */
  inline InteractionsIterator interactionsEnd()
  {
    return allInteractions->end();
  };

  /** const iterator equal to the first element of allInteractions
  *  \return a ConstInteractionsIterator
  */
  inline ConstInteractionsIterator interactionsBegin() const
  {
    return allInteractions->begin();
  };

  /** const iterator equal to allInteractions->end()
  *  \return a ConstInteractionsIterator
  */
  inline ConstInteractionsIterator interactionsEnd() const
  {
    return allInteractions->end();
  }

  /** get all the Interactions of the Topology problem (saved in a set)
  *  \return an InteractionsSet
  */
  inline const SP::InteractionsSet getInteractions() const
  {
    return allInteractions;
  }

  /** set the Interactions of the Topology problem (saved in a set)
   */
  inline void setInteractionsPtr(SP::InteractionsSet newInteractions)
  {
    allInteractions->clear() ;
    allInteractions = newInteractions;
  }


  /** check if Interaction inter is in the set
  *  \param a pointer to Interaction
  *  \return a bool
  */



  const bool hasInteraction(SP::Interaction) const;

  /** get a pointer to the index set of all Unitary Relations.
   *  \return a UnitaryRelationsSet*
   */
  inline SP::UnitaryRelationsSet getIndexSet0Ptr()
  {
    return indexSet0;
  }

  // --- relativeDegreesMap ---

  /** get the relativeDegrees Map of this topology
  *  \return a UnitaryRelationsIntMap
  */
  inline const UnitaryRelationsIntMap getRelativeDegrees() const
  {
    return relativeDegrees;
  }

  /** get the relativeDegree vector of a specific UnitaryRelation
  *  \param a pointer to UnitaryRelation
  *  \return an unsigned int
  */
  inline const unsigned int getRelativeDegree(SP::UnitaryRelation UR)
  {
    return relativeDegrees[UR];
  }

  /** for all relative degrees (one per Unitary Relation), find the maximum value.
  *  \return an unsigned int
  */
  const unsigned int getMaxRelativeDegree();

  /** for all relative degrees (one per Unitary Relation), find the minimum value.
  *  \return an unsigned int
  */
  const unsigned int getMinRelativeDegree();

  // --- isTopologyUpToDate ---

  /** set isTopologyUpToDate to val
  *  \param a bool
  */
  inline void setUpToDate(const bool val)
  {
    isTopologyUpToDate = val;
  }

  /** check if topology has been updated since modifications occurs on nsds
  *  \return a bool
  */
  inline bool isUpToDate()
  {
    return isTopologyUpToDate;
  }

  // --- isTopologyTimeInvariant ---

  /** set isTopologyTimeInvariant to val
  *  \param a bool
  */
  inline void setTimeInvariant(const bool val)
  {
    isTopologyTimeInvariant = val;
  }

  /** check if all relative degrees are equal to 0 or 1
  *  \return a bool
  */
  inline bool isTimeInvariant()
  {
    return isTopologyTimeInvariant;
  }

  /** get the total number of scalar constraints
  *  \return an unsigned int
  */
  inline const unsigned int getNumberOfConstraints()
  {
    return numberOfConstraints;
  };

  /** initializes the topology (called in Simulation->initialize)
  */
  void initialize();

  void clear();
};

#endif // TOPOLOGY_H
