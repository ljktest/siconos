/* Siconos-Kernel, Copyright INRIA 2005-2011.
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
#include "Topology.hpp"
#include "NonSmoothLaw.hpp"
#include "NonSmoothDynamicalSystem.hpp"
#include "Interaction.hpp"
#include "UnitaryRelation.hpp"
#include "EqualityConditionNSL.hpp"

#include <boost/bind.hpp>
#include <algorithm>
#include <limits>

//#define DEBUG_MESSAGES 1
#include <debug.h>

#define MAX_RELATIVE_DEGREE 999

// --- CONSTRUCTORS/DESTRUCTOR ---

// default
Topology::Topology(): _isTopologyUpToDate(false), _hasChanged(true),
  _numberOfConstraints(0), _symmetric(false)
{
  _URG.resize(1);
  _DSG.resize(1);

  _URG[0].reset(new UnitaryRelationsGraph());
  _DSG[0].reset(new DynamicalSystemsGraph());

  _allInteractions.reset(new InteractionsSet());
}

Topology::Topology(SP::InteractionsSet newInteractions) :
  _isTopologyUpToDate(false), _hasChanged(true),
  _numberOfConstraints(0), _symmetric(false)
{

  _URG.resize(1);
  _DSG.resize(1);

  _URG[0].reset(new UnitaryRelationsGraph());
  _DSG[0].reset(new DynamicalSystemsGraph());

  _allInteractions.reset(new InteractionsSet());

  for (InteractionsIterator it = newInteractions->begin();
       it != newInteractions->end(); ++it)
  {
    insertInteraction(*it);
  }

  _isTopologyUpToDate = false;
}


// a constructor with a DS set : when some DS may not be in interactions
Topology::Topology(SP::DynamicalSystemsSet newDSset, SP::InteractionsSet newInteractions) :
  _isTopologyUpToDate(false), _hasChanged(true),
  _numberOfConstraints(0), _symmetric(false)
{

  _URG.resize(1);
  _DSG.resize(1);

  _URG[0].reset(new UnitaryRelationsGraph());
  _DSG[0].reset(new DynamicalSystemsGraph());

  _allInteractions.reset(new InteractionsSet());

  for (InteractionsIterator it = newInteractions->begin();
       it != newInteractions->end(); ++it)
  {
    insertInteraction(*it);
  }

  for (DSIterator ids = newDSset->begin(); ids != newDSset->end() ; ++ids)
  {
    _DSG[0]->add_vertex(*ids);
  }
  _isTopologyUpToDate = false;
}

// destructor
Topology::~Topology()
{
  clear();
}

void Topology::addInteractionInIndexSet(SP::Interaction inter)
{
  // Private function
  //
  // Creates UnitaryRelations corresponding to inter and add them into
  // _URG

  // First, we get the number of relations in the interaction.  This
  // corresponds to inter->getNumberOfRelations but since Interaction
  // has not been initialized yet, this value is not set and we need
  // to get interaction size and nsLaw size.
  unsigned int nsLawSize = inter->nonSmoothLaw()->size();
  unsigned int m = inter->getSizeOfY() / nsLawSize;

  if (m > 1)
    RuntimeException::selfThrow("Topology::addInteractionInIndexSet - m > 1. Obsolete !");

  // vector of the Interaction
  UnitaryRelationsGraph::EDescriptor ur_current_edge;

  UnitaryRelationsGraph::EDescriptor ds_current_edge;

  SP::DynamicalSystemsSet systems = inter->dynamicalSystems();

  _numberOfConstraints += m * nsLawSize;

  // _DSG is the hyper forest : (vertices : dynamical systems, edges :
  // unitary relations)
  //
  // _URG is the hyper graph : (vertices : unitary relations, edges :
  // dynamical systems)

  // _URG = L(_DSG),  L is the line graph transformation


  // for all couples of ds in the interaction
  DEBUG_PRINTF("addInteractionInIndexSet systems->size() : %d\n", systems->size());


  for (DSIterator i1ds = systems->begin(); i1ds != systems->end(); ++i1ds)
  {
    for (DSIterator i2ds = i1ds; i2ds != systems->end(); ++i2ds)
    {

      // build the unitary relations
      std::vector<SP::UnitaryRelation> current_urs;
      for (unsigned int i = 0, pos = 0; i < m; ++i, pos += nsLawSize)
      {
        SP::UnitaryRelation UR(new UnitaryRelation(inter, pos, i));
        current_urs.push_back(UR);
      };

      // one DS in the interaction is a special case : a self branch
      if ((i1ds == i2ds) && inter->dynamicalSystems()->size() == 1)
      {
        DynamicalSystemsGraph::VDescriptor dsgv;
        dsgv = _DSG[0]->add_vertex(*i1ds);

        // this may be a multi edges graph
        for (std::vector<SP::UnitaryRelation>::iterator uri = current_urs.begin();
             uri != current_urs.end(); ++uri)
        {
          assert(!_DSG[0]->is_edge(dsgv, dsgv, *uri));
          assert(!_URG[0]->is_vertex(*uri));

          DynamicalSystemsGraph::EDescriptor new_ed;
          UnitaryRelationsGraph::VDescriptor urg_new_ve;
          boost::tie(new_ed, urg_new_ve) = _DSG[0]->add_edge(dsgv, dsgv, *uri, *_URG[0]);

          // add self branches in vertex properties
          // note : boost graph SEGFAULT on self branch removal
          // see https://svn.boost.org/trac/boost/ticket/4622
          _URG[0]->properties(urg_new_ve).source = *i1ds;
          _URG[0]->properties(urg_new_ve).target = *i1ds;

          assert(_URG[0]->bundle(urg_new_ve) == *uri);
          assert(_URG[0]->is_vertex(*uri));
          assert(_DSG[0]->is_edge(dsgv, dsgv, *uri));
        }
      }
      else
        // multiples DS in the interaction : no self branch
        if (i1ds != i2ds)
        {
          DynamicalSystemsGraph::VDescriptor dsgv1, dsgv2;

          dsgv1 = _DSG[0]->add_vertex(*i1ds);
          dsgv2 = _DSG[0]->add_vertex(*i2ds);

          // this may be a multi edges graph
          for (std::vector<SP::UnitaryRelation>::iterator uri = current_urs.begin();
               uri != current_urs.end(); ++uri)
          {
            assert(!_DSG[0]->is_edge(dsgv1, dsgv2, *uri));
            assert(!_URG[0]->is_vertex(*uri));

            DynamicalSystemsGraph::EDescriptor new_ed;
            UnitaryRelationsGraph::VDescriptor urg_new_ve;
            boost::tie(new_ed, urg_new_ve) = _DSG[0]->add_edge(dsgv1, dsgv2, *uri, *_URG[0]);

            // add self branches in vertex properties
            // note : boost graph SEGFAULT on self branch removal
            // see https://svn.boost.org/trac/boost/ticket/4622
            _URG[0]->properties(urg_new_ve).source = *i1ds;
            _URG[0]->properties(urg_new_ve).target = *i2ds;

            assert(_URG[0]->bundle(urg_new_ve) == *uri);
            assert(_URG[0]->is_vertex(*uri));
            assert(_DSG[0]->is_edge(dsgv1, dsgv2, *uri));
          }
        }
    }
  }
};

/* an edge is removed from _DSG graph if the corresponding vertex is
   removed from the adjoint graph (_URG)
*/
struct VertexIsRemoved
{
  VertexIsRemoved(SP::Interaction I,
                  SP::DynamicalSystemsGraph sg, SP::UnitaryRelationsGraph asg) :
    _I(I), __DSG(sg), __URG(asg) {};
  bool operator()(DynamicalSystemsGraph::EDescriptor ed)
  {

    if (__URG->is_vertex(__DSG->bundle(ed)))
    {
      UnitaryRelationsGraph::VDescriptor uvd = __URG->descriptor(__DSG->bundle(ed));

      if (__URG->bundle(uvd)->interaction() == _I)
      {
        __URG->remove_vertex(__DSG->bundle(ed));

        assert(__URG->size() == __DSG->edges_number() - 1);

        return true;
      }
      else
      {
        return false;
      }
    }
    else
    {
      return true;
    }
  }
  SP::Interaction _I;
  SP::DynamicalSystemsGraph __DSG;
  SP::UnitaryRelationsGraph __URG;
};


/* remove an interaction : remove edges (unitary relation) from _DSG if
   corresponding vertices are removed from _URG */
void Topology::removeInteractionFromIndexSet(SP::Interaction inter)
{

  for (DSIterator ids = inter->dynamicalSystems()->begin();
       ids != inter->dynamicalSystems()->end();
       ++ids)
  {
    _DSG[0]->remove_out_edge_if
    (_DSG[0]->descriptor(*ids),
     VertexIsRemoved(inter, _DSG[0], _URG[0]));
  };
};


void Topology::insertDynamicalSystem(SP::DynamicalSystem ds)
{
  _DSG[0]->add_vertex(ds);
};


void Topology::insertInteraction(SP::Interaction inter)
{
  assert(_allInteractions);
  assert(_DSG[0]->edges_number() == _URG[0]->size());

  _allInteractions->insert(inter);
  addInteractionInIndexSet(inter);

  assert(_DSG[0]->edges_number() == _URG[0]->size());

};

void Topology::removeInteraction(SP::Interaction inter)
{
  assert(_allInteractions);
  assert(_DSG[0]->edges_number() == _URG[0]->size());

  _allInteractions->erase(inter);
  removeInteractionFromIndexSet(inter);

  assert(_DSG[0]->edges_number() == _URG[0]->size());
};

void Topology::removeDynamicalSystem(SP::DynamicalSystem ds)
{
  RuntimeException::selfThrow("remove dynamical system not implemented");
};


void Topology::link(SP::Interaction inter, SP::DynamicalSystem ds)
{
  // interactions should not know linked dynamical systems in the
  // future
  InteractionsIterator it = _allInteractions->find(inter);
  if (it != _allInteractions->end())
  {
    _allInteractions->erase(*it);
    removeInteractionFromIndexSet(inter);
  }

  inter->insert(ds);
  insertInteraction(inter);

};




bool Topology::hasInteraction(SP::Interaction inter) const
{
  return _allInteractions->isIn(inter);
}

void Topology::initialize()
{


  _isTopologyUpToDate = true;

}

void Topology::setProperties()
{
  for (unsigned int i = 0; i < _URG.size(); ++i)
  {
    _URG[i]->properties().reset(new UnitaryRelationsGraphProperties(_URG[i]));
    _URG[i]->properties()->symmetric = _symmetric;
  }
  _DSG[0]->properties().reset(new DynamicalSystemsGraphProperties(_DSG[0]));
  _DSG[0]->properties()->symmetric = _symmetric;
}

void Topology::clear()
{
  _allInteractions->clear();

  _URG.clear();
  _DSG.clear();

  _isTopologyUpToDate = false;
}

