/* Siconos-Kernel, Copyright INRIA 2005-2015
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

/*! \file SiconosGraphs.hpp
 * \brief Definitions of the graphs used in Siconos
 */

#ifndef SimulationGraphs_H
#define SimulationGraphs_H

#include "SiconosGraph.hpp"
#include "SiconosProperties.hpp"
#include "SiconosPointers.hpp"
#include "SimulationTypeDef.hpp"

/** the graph structure :
 *
 * InteractionsGraph = L(DynamicalSystemsGraph)
 *
 * where L is the line graph
 * transformation
 *
 *
 * Properties on graph :
 * --------------------
 *
 * The properties on the graph enable to store the data that are specicic to a simulation
 * strategy. It avoids to burden the modeling classes that should be as independent as possible from
 * the simulation choices.
 *
 * There are mainly two types of properties
 * <ul>
 * <li>  Mandatory properties DynamicalSystemProperties and InteractionProperties .
 *       These properties are always  instanciated for any kind of simulation.
 *       The accesors to the property are illustrated in the followinf example :
 *       For a given SP::DynamicalSystem ds and a given graph SP::DynamicalSystemsGrap DSG
 *
 *       DynamicalSystemsGraph::VDescriptor dsv = DSG->descriptor(ds);
 *       SP::OneStepintegrator osi = DSG->properties(dsv).osi;
 * </li>
 * <li> Optional Properties
 *      They are installed thanks to the macro INSTALL_GRAPH_PROPERTIES.
 *
 *      The accesors to the property are illustrated in the following example :
 *      For a given SP::DynamicalSystem ds and a given graph SP::DynamicalSystemsGrap DSG
 *
 *      DynamicalSystemsGraph::VDescriptor dsv = DSG->descriptor(ds);
 *      DSG->name.insert(dsv, name); // insert the name in the property
 *      const std::string& name =  DSG[*dsv]->name;
 * 
 *
 * </li>
 * </ul>
 */

/** \struct InteractionProperties mandatory properties for an Interaction  */
struct InteractionProperties
{
  SP::SiconosMatrix block;             /**< diagonal block */
  SP::DynamicalSystem source;
  unsigned int source_pos;
  SP::DynamicalSystem target;
  unsigned int target_pos;
  SP::OneStepIntegrator osi;
  bool forControl;                     /**< true if the relation is used to add a control input to a DS */
  SP::VectorOfBlockVectors DSlink;     /**< pointer links to DS variables needed for computation, mostly x (or q), z, r (or p) */
  SP::VectorOfVectors workVectors;     /**< set of SiconosVector, mostly to have continuous memory vectors (not the case with BlockVector in DSlink) */
  SP::VectorOfSMatrices workMatrices;  /**< To store jacobians */


  ACCEPT_SERIALIZATION(InteractionProperties);
};

/** \struct DynamicalSystemProperties mandatory properties for a DynamicalSystems */
struct DynamicalSystemProperties
{
  SP::SiconosMatrix upper_block;          /**< i,j block i<j */
  SP::SiconosMatrix lower_block;          /**< i,j block i>j */
  SP::VectorOfVectors workVectors;        /**< Used for instance in Newton iteration */
  SP::VectorOfMatrices workMatrices;      /**< Mostly for Lagrangian system */
  SP::OneStepIntegrator osi;              /**< Integrator used for the given DynamicalSystem */
  SP::SimpleMatrix W;                    /**< Matrix for integration */
  SP::SimpleMatrix WBoundaryConditions;  /**< Matrix for integration of boundary conditions*/
//  SP::SiconosMemory _xMemory            /**< old value of x, TBD */

  ACCEPT_SERIALIZATION(DynamicalSystemProperties);
};

struct GraphProperties
{
  bool symmetric;

  ACCEPT_SERIALIZATION(GraphProperties);
};



typedef SiconosGraph < std11::shared_ptr<DynamicalSystem>, std11::shared_ptr<Interaction>,
        DynamicalSystemProperties, InteractionProperties,
        GraphProperties > _DynamicalSystemsGraph;


typedef SiconosGraph < std11::shared_ptr<Interaction>, std11::shared_ptr<DynamicalSystem>,
        InteractionProperties, DynamicalSystemProperties,
        GraphProperties > _InteractionsGraph;

struct DynamicalSystemsGraph : public _DynamicalSystemsGraph
{
  /** optional properties : memory is allocated only on first access */
  INSTALL_GRAPH_PROPERTIES(DynamicalSystems,
                           ((VertexSP, MatrixIntegrator, Ad)) // for ZOH Integration
                           ((VertexSP, MatrixIntegrator, AdInt)) // for ZOH Integration
                           ((VertexSP, MatrixIntegrator, Ld)) // For Observer (ZOH Integration)
                           ((VertexSP, MatrixIntegrator, Bd)) // For Controlled System (ZOH Integration)
                           ((VertexSP, SiconosMatrix, B)) // For Controlled System
                           ((VertexSP, SiconosMatrix, L)) // For Observer
                           ((VertexSP, PluggedObject, pluginB)) // For Controlled System
                           ((VertexSP, PluggedObject, pluginL)) // For Observer
                           ((VertexSP, SiconosVector, e)) // For Observer
                           ((VertexSP, SiconosVector, u)) // For Controlled System
                           ((VertexSP, PluggedObject, pluginU)) // For Controlled System (nonlinear w.r.t u)
                           ((VertexSP, PluggedObject, pluginJacgx)) // For Controlled System (nonlinear w.r.t u); compute nabla_x g(x, u)
                           ((VertexSP, SiconosVector, tmpXdot)) // For Controlled System (nonlinear w.r.t u); tmpXdot = g(x, u)
                           ((VertexSP, SimpleMatrix, jacgx)) // For Controlled System (nonlinear w.r.t u); jacgx = nabla_x g(x, u)
                           ((Vertex, std::string, name)) // a name for a dynamical system
                           ((Vertex, unsigned int, groupId))); // For group manipulations (example assign
                                                               // a material id for contact law
                                                               // determination
  // always needed -> DynamicalSystemProperties

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
    pluginU._store->erase(vd);
    pluginJacgx._store->erase(vd);
    tmpXdot._store->erase(vd);
    jacgx._store->erase(vd);
    name._store->erase(vd);
    groupId._store->erase(vd);
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



#endif
