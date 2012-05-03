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

/*! \file SiconosProperties.hpp

  \brief Exterior properties to vertices or edges of a SiconosGraph
  can be attach with Siconos::Properties. These properties are
  referenced with vertices or edges indices. update_vertices_indices()
  or update_edges_indices() must have been done after any vertices or
  edges insertion or deletion.

*/


#ifndef SICONOS_PROPERTIES_HPP
#define SICONOS_PROPERTIES_HPP

#include <boost/property_map/property_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/static_assert.hpp>
#include <vector>

namespace Siconos
{


/* a templated way to access edge or vertex with the help of
 * boost::mpl::if_ */

/** get vertex needed data */
template<typename G>
struct VertexAccess
{
  typedef VertexAccess type;
  typedef typename G::VDescriptor descriptor;
  typedef typename G::OVIndexAccess OldIndexMap;

  std::vector<descriptor>& indexModified(G& g)
  {
    return g.vertex_index_modified();
  }

  size_t size(G& g)
  {
    return g.vertices_number();
  }

};

/** get edge needed data */
template<typename G>
struct EdgeAccess
{
  typedef EdgeAccess type;
  typedef typename G::EDescriptor descriptor;
  typedef typename G::OEIndexAccess OldIndexMap;

  static std::vector<descriptor>& indexModified(G& g)
  {
    return g.edge_index_modified();
  }

  static size_t size(const G& g)
  {
    return g.edges_number();
  }
};


/** choose vertex or edge access according to IndexMap */
template<typename G, typename IndexMap>
struct VertexOrEdge
{
  typedef typename boost::mpl::if_ < boost::is_same<typename G::VIndexAccess, IndexMap>,
          VertexAccess<G> ,
          EdgeAccess<G>  >::type Access;
};


/** the general properties structure, from boost::vector_property_map :
 \param T the property data type
 \param G the graph type
 \param IndexMap the index map, should be either G::VIndexAccess or G:EIndexAccess
*/
template<typename T, typename G, typename IndexMap>
class Properties
{

private:

  G& _g;
  boost::shared_ptr< std::vector<T> > _store;
  int _stamp;

public:
  typedef typename VertexOrEdge<G, IndexMap>::Access Access;

  Access access;

  typedef typename Access::OldIndexMap OldIndexMap;
  typedef typename boost::property_traits<IndexMap>::key_type  key_type;
  typedef T value_type;
  typedef typename std::iterator_traits <
  typename std::vector<T>::iterator >::reference reference;
  typedef boost::lvalue_property_map_tag category;

  /** constructor from a SiconosGraph
      \param g a SiconosGraph
  */

  Properties(G& g)
    : _g(g), _store(new std::vector<T>()), _stamp(0)
  {}


  /** data access from a SiconosGraph vertex descriptor or edge
      descriptor :
      \param v a SiconosGraph::VDescriptor or
      SiconosGraph::EDescriptor according to IndexMap type */
  reference operator[](const key_type& v)
  {

    /* a stamp is used to know if indices have been updated */
    if (_g.stamp() != _stamp)
    {
      _stamp = _g.stamp();

      int k = access.size(_g);

      /* this new store is used as a fifo */
      typename std::vector<T> new_store(k);

      k--;
      for (typename std::vector<typename Access::descriptor>::iterator vi = access.indexModified(_g).begin();
           vi != access.indexModified(_g).end(); ++vi)
      {
        typename boost::property_traits<OldIndexMap>::value_type oi = _g.old_index(*vi);
        new_store[k--] = (*_store)[oi];
      }
      for (typename std::vector<typename Access::descriptor>::iterator vi = access.indexModified(_g).begin();
           vi != access.indexModified(_g).end(); ++vi)
      {
        typename boost::property_traits<IndexMap>::value_type i = _g.index(*vi);
        (*_store)[i] = new_store.back();
        new_store.pop_back();
      };
    };

    typename boost::property_traits<IndexMap>::value_type i = _g.index(v);
    if (static_cast<unsigned>(i) >= _store->size())
    {
      // _store->resize(i+1, T()); faster with large size
      _store->resize(access.size(_g), T());
    }

    return (*_store)[i];
  };

};


/** vertex property structure:
    \param T the property data type
    \param G the graph type
 */
template<typename T, typename G>
class VertexProperties : public Properties<T, G, typename G::VIndexAccess>
{
public:
  VertexProperties(G& g) : Properties<T, G, typename G::VIndexAccess>(g)
  {};
};

/** edge property structure:
    \param T the property data type
    \param G the graph type
 */
template<typename T, typename G>
class EdgeProperties : public Properties<T, G, typename G::EIndexAccess>
{
public:
  EdgeProperties(G& g) : Properties<T, G, typename G::EIndexAccess>(g)
  {};
};

/** function to build a VertexProperties from one template parameter
  \param g the graph
*/
template<typename T, typename G>
VertexProperties<T, G> vertexProperties(G& g)
{
  return VertexProperties<T, G>(g);
};

/** function to build a EdgeProperties from one template parameter
  \param g the graph
*/
template<typename T, typename G>
EdgeProperties<T, G> edgeProperties(G& g)
{
  return EdgeProperties<T, G>(g);
};


/** global properties : they may be attached to main graph and may be
 * referenced from subgraphs. They are not used for the moment */

template<typename T, typename G, typename IndexMap>
class SubProperties
{
private:
  typedef Properties<T, G, IndexMap> RefProperties;
  RefProperties _properties;
  G& _g;

  typedef typename boost::property_traits<IndexMap>::key_type  key_type;
  typedef T value_type;
  typedef typename std::iterator_traits <
  typename std::vector<T>::iterator >::reference reference;
  typedef boost::lvalue_property_map_tag category;

public:

  SubProperties(RefProperties& p, G& g)
    : _properties(p), _g(g) {}

  reference operator[](const key_type& v)
  {
    return _properties[_g.descriptor0(v)];
  }

};

template<typename T, typename G>
class VertexSubProperties : public SubProperties<T, G, typename G::VIndexAccess>
{
public:
  typedef Properties<T, G, typename G::VIndexAccess> RefProperties;

  VertexSubProperties(RefProperties& p, G& g) : SubProperties<T, G, typename G::VIndexAccess>(p, g)
  {};
};

template<typename T, typename G>
class EdgeSubProperties : public SubProperties<T, G, typename G::EIndexAccess>
{
public:
  typedef Properties<T, G, typename G::EIndexAccess> RefProperties;

  EdgeSubProperties(RefProperties& p, G& g) : SubProperties<T, G, typename G::EIndexAccess>(p, g)
  {};
};


}

/* convenience macro for properties declarations */

#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/cat.hpp>

#define I_DECLARE_MEMBERS(r,gt,p) \
  Siconos:: BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(3,0,p),Properties)< BOOST_PP_TUPLE_ELEM(3,1,p), gt> BOOST_PP_TUPLE_ELEM(3,2,p);

#define I_CONS_MEMBERS(r,gt,p) \
  BOOST_PP_TUPLE_ELEM(3,2,p) (Siconos:: BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(3,0,p),Properties)< BOOST_PP_TUPLE_ELEM(3,1,p), gt>(*this)),


#define INSTALL_GRAPH_PROPERTIES(GraphType, PROPERTIES)                 \
  BOOST_PP_SEQ_FOR_EACH(I_DECLARE_MEMBERS, BOOST_PP_CAT(GraphType, Graph), PROPERTIES); \
  bool dummy;                                                           \
                                                                        \
  BOOST_PP_CAT(GraphType, Graph)() :                                    \
    BOOST_PP_SEQ_FOR_EACH(I_CONS_MEMBERS, BOOST_PP_CAT(GraphType, Graph), PROPERTIES) dummy(true) {}; \
 

#endif
