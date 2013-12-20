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

/*! \file

Typedef for simulation-related objects
*/

#ifndef SimulationTypedef_H
#define SimulationTypedef_H

#include <vector>
#include <map>
#include <set>

#include "SiconosGraph.hpp"
#include "SiconosPointers.hpp"

#include "SiconosProperties.hpp"
#include "Interaction.hpp"

/** double precision machine */
/*  eq dlmach('e'),  DBL_EPSILON,  fabs(a-b) <  */
const double MACHINE_PREC = std::numeric_limits<double>::epsilon();



// ================== Objects to handle DS ==================

/** Map of SP::SimpleMatrix; key = the number of the related DS*/
typedef std::map<unsigned int, SP::SimpleMatrix> MapOfDSMatrices;

/** Iterator through a map of matrices */
typedef MapOfDSMatrices::iterator MatIterator;

/** Const iterator through a map of matrices */
typedef MapOfDSMatrices::const_iterator ConstMatIterator;

/** Map of SiconosVector; key = the related DS*/
typedef std::map<SP::DynamicalSystem, SP::SiconosVector> DSVectors;

/** Iterator through a map of matrices */
typedef DSVectors::iterator DSVectorsIterator;

/** Const iterator through a map of matrices */
typedef DSVectors::const_iterator ConstDSVectorsIterator;

/** Map of double; key = the related DS */
typedef std::map<SP::DynamicalSystem, double> MapOfDouble;

/** Iterator through a map of double */
typedef MapOfDouble::iterator DoubleIterator;

// ================== Objects that should not exists (used in ZOH) ==================

/** Map of TimeDiscretisation; key = the number of the related DS*/
typedef std::map<unsigned int, SP::TimeDiscretisation> MapOfTD;

/** Map of Model; key = the number of the related DS*/
typedef std::map<unsigned int, SP::Model> MapOfModel;

/** Map of OSI; key = the number of the related DS*/
typedef std::map<unsigned int, SP::OneStepIntegrator> MapOfOSI;

/** Map of Simulation; key = the number of the related DS*/
typedef std::map<unsigned int, SP::Simulation> MapOfSimulation;

/* * Map of DynamicalSystem; key = the number of the related DS*/
typedef std::map<unsigned int, SP::DynamicalSystem> MapOfDS;

typedef std::map<unsigned int, SP::SiconosVector> MapOfVectors;

typedef std::map<unsigned int, SP::Relation> MapOfRelation;

/** Map of boolean; key = the number of the related DS*/
typedef std::map<unsigned int, bool> MapOfBool;

// ================== Objects to handle Interactions ==================

/** Map of SiconosMatrices with a Interactions as a key - Used for diagonal interactionBlock-terms in assembled matrices of LCP etc ...*/
typedef std::map< SP::Interaction, SP::SiconosMatrix>  MapOfInteractionMatrices;

/** Iterator through a MapOfInteractionMatrices */
typedef MapOfInteractionMatrices::iterator InteractionMatrixColumnIterator ;

/** Const iterator through a MapOfInteractionMatrices */
typedef MapOfInteractionMatrices::const_iterator ConstInteractionMatrixColumnIterator;

/** Map of MapOfDSInteractionMatrices with a DynamicalSystem as a key - Used for interactionBlock-terms indexed by a DynamicalSystem and an Interaction in assembled matrices of LCP etc ..*/
typedef std::map< SP::DynamicalSystem , MapOfInteractionMatrices >  MapOfDSMapOfInteractionMatrices;

/** Iterator through a MapOfDSMapOfInteractionMatrices */
typedef MapOfDSMapOfInteractionMatrices::iterator DSInteractionMatrixRowIterator ;

/** Const iterator through a MapOfDSMapOfInteractionMatrices */
typedef MapOfDSMapOfInteractionMatrices::const_iterator ConstDSInteractionMatrixRowIterator ;




/** Map of MapOfInteractionMapOfDSMatrices with a DynamicalSystem as a key - Used for interactionBlock-terms indexed by a DynamicalSystem and an Interaction in assembled matrices of LCP etc ..*/
typedef std::map< SP::Interaction , MapOfDSMatrices >  MapOfInteractionMapOfDSMatrices;

/** Iterator through a MapOfInteractionMapOfDSMatrices */
typedef MapOfInteractionMapOfDSMatrices::iterator InteractionDSMatrixRowIterator ;

/** Const iterator through a MapOfInteractionMapOfDSMatrices */
typedef MapOfInteractionMapOfDSMatrices::const_iterator ConstInteractionDSMatrixRowIterator ;

/** Map to link SP::Interaction with an int - Used for example in interactionBlocksPositions for OSNSMatrix */
typedef std::map< SP::Interaction , unsigned int > Interaction_int;
TYPEDEF_SPTR(Interaction_int)

/** list of indices */
typedef std::vector<unsigned int> IndexInt;
TYPEDEF_SPTR(IndexInt)


/** properties that are always needed */
struct InteractionProperties
{
  SP::SiconosMatrix block;    // diagonal block
  SP::DynamicalSystem source;
  unsigned int source_pos;
  SP::DynamicalSystem target;
  unsigned int target_pos;
  SP::OneStepIntegrator osi;
  bool forControl;            // true if the relation is used to control the DS

  ACCEPT_SERIALIZATION(InteractionProperties);
};

struct SystemProperties
{
  SP::SiconosMatrix upper_block;   // i,j block i<j
  SP::SiconosMatrix lower_block;   // i,j block i>j

  ACCEPT_SERIALIZATION(SystemProperties);
};

struct GraphProperties
{
  bool symmetric;

  ACCEPT_SERIALIZATION(GraphProperties);
};

TYPEDEF_SPTR(GraphProperties)


/** the graph structure :
 *
 * InteractionsGraph = L(DynamicalSystemsGraph)
 *
 * where L is the line graph
 * transformation */
typedef SiconosGraph < std11::shared_ptr<DynamicalSystem>, std11::shared_ptr<Interaction>,
        SystemProperties, InteractionProperties,
        GraphProperties > _DynamicalSystemsGraph;


typedef SiconosGraph < std11::shared_ptr<Interaction>, std11::shared_ptr<DynamicalSystem>,
        InteractionProperties, SystemProperties,
        GraphProperties > _InteractionsGraph;

struct DynamicalSystemsGraph : public _DynamicalSystemsGraph
{
  /** optional properties : memory is allocated only on first access */
  INSTALL_GRAPH_PROPERTIES(DynamicalSystems,
                           ((Vertex, SP::MatrixIntegrator, Ad)) // for ZOH Integration
                           ((Vertex, SP::MatrixIntegrator, AdInt)) // for ZOH Integration
                           ((Vertex, SP::MatrixIntegrator, Ld)) // For Observer (ZOH Integration)
                           ((Vertex, SP::MatrixIntegrator, Bd)) // For Controlled System (ZOH Integration)
                           ((Vertex, SP::SiconosMatrix, B)) // For Controlled System
                           ((Vertex, SP::SiconosMatrix, L)) // For Observer
                           ((Vertex, SP::PluggedObject, pluginB)) //For Controlled System
                           ((Vertex, SP::PluggedObject, pluginL)) // For Observer
                           ((Vertex, SP::SiconosVector, e)) // For Observer (ZOH Integration)
                           ((Vertex, SP::SiconosVector, u))) // For Controlled System (ZOH Integration)
  // always needed -> SystemProperties
  
  /** serialization hooks */
  ACCEPT_SERIALIZATION(DynamicalSystemsGraph);
  
  // to be installed with INSTALL_GRAPH_PROPERTIES
  void eraseProperties(_DynamicalSystemsGraph::VDescriptor vd)
  {
    Ad._store->erase(vd);
    AdInt._store->erase(vd);
    Ld._store->erase(vd);
    Bd._store->erase(vd);
    B._store->erase(vd);
    L._store->erase(vd);
    pluginB._store->erase(vd);
    pluginL._store->erase(vd);
    e._store->erase(vd);
    u._store->erase(vd);
  }
};

struct InteractionsGraph : public _InteractionsGraph
{
  /** optional properties : memory is allocated only on first access */
  INSTALL_GRAPH_PROPERTIES(Interactions,
                           ((Vertex, SP::SimpleMatrix, blockProj))        // ProjectOnConstraint
                           ((Edge, SP::SimpleMatrix, upper_blockProj))    // idem
                           ((Edge, SP::SimpleMatrix, lower_blockProj))  // idem
                           ((Vertex, std::string, name)));

  // to be installed with INSTALL_GRAPH_PROPERTIES
  void eraseProperties(_InteractionsGraph::VDescriptor vd)
  {
    blockProj._store->erase(vd);
    name._store->erase(vd);
  }

  // to be installed with INSTALL_GRAPH_PROPERTIES
  void eraseProperties(_InteractionsGraph::EDescriptor ed)
  {
    upper_blockProj._store->erase(ed);
    lower_blockProj._store->erase(ed);
  }

  /** serialization hooks */
  ACCEPT_SERIALIZATION(InteractionsGraph);
};

TYPEDEF_SPTR(DynamicalSystemsGraph)
TYPEDEF_SPTR(InteractionsGraph)




// ================== Objects to handle OSI ==================


/** Vector of OneStepIntegrator */
typedef std::set<SP::OneStepIntegrator> OSISet;

/** Iterator through vector of OSI*/
typedef OSISet::iterator OSIIterator;

/** Const iterator through vector of OSI*/
typedef OSISet::const_iterator ConstOSIIterator;

/** Return type value for insert function - bool = false if insertion failed. */
typedef std::pair<OSISet::iterator, bool> CheckInsertOSI;

/** A map that links DynamicalSystems and their OneStepIntegrator. */
typedef std::map<SP::DynamicalSystem, SP::OneStepIntegrator> DSOSIMap;

/** Iterator through a DSOSIMap. */
typedef DSOSIMap::iterator DSOSIIterator;

/** Const Iterator through a DSOSIMap. */
typedef DSOSIMap::const_iterator DSOSIConstIterator;

// ================== Objects to handle OSNS ==================

#include "OneStepNSProblem.hpp"
/** Map of OSNS */
//typedef std::map<std::string, SP::OneStepNSProblem > OneStepNSProblems;
typedef std::vector<SP::OneStepNSProblem> OneStepNSProblems;

/** Iterator through OneStepNSProblems */
typedef OneStepNSProblems::iterator OSNSIterator;

/** Const iterator through OneStepNSProblems */
typedef OneStepNSProblems::const_iterator ConstOSNSIterator;

// ================== Misc ==================

/** default tolerance value, used to update index sets */
const double DEFAULT_TOLERANCE = 10 * MACHINE_PREC;

enum SICONOS_OSNSP
{
  SICONOS_OSNSP_DEFAULT = 0
};
enum SICONOS_OSNSP_ED
{
  SICONOS_OSNSP_ED_SMOOTH_ACC,
  SICONOS_OSNSP_ED_IMPACT,
  SICONOS_OSNSP_ED_SMOOTH_POS
};
enum SICONOS_OSNSP_TS
{
  SICONOS_OSNSP_TS_VELOCITY = 0,
  SICONOS_OSNSP_TS_POS = 1
};
const int SICONOS_NB_OSNSP_TS = 1;
const int SICONOS_NB_OSNSP_TSP = 2;

/** Event constants */
#define TD_EVENT 1
#define NS_EVENT 2

TYPEDEF_SPTR(OSISet)
TYPEDEF_SPTR(OneStepNSProblems)

#endif
