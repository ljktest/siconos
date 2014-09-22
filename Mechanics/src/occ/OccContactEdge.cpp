#include "OccContactEdge.hpp"
#include "ContactShapeDistance.hpp"
#include "cadmbtb.hpp"

#include <RuntimeException.hpp>

#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>

#include <limits>

OccContactEdge::OccContactEdge(const OccContactShape& reference_shape,
                               unsigned int index) :
  OccContactShape(reference_shape),
  _index(index),
  _edge(reference_shape.edge(index))
{
};

const TopoDS_Edge& OccContactEdge::contact() const
{
  return *this->_edge;
}

void OccContactEdge::computeUVBounds()
{
  TopExp_Explorer Ex1;
  Ex1.Init(this->contact(),TopAbs_EDGE);
  const TopoDS_Edge& edge = TopoDS::Edge(Ex1.Current());
  BRepAdaptor_Curve SC(edge);
  this->binf1[0]=SC.FirstParameter();
  this->bsup1[0]=SC.LastParameter();
  this->binf1[1]=0.;
  this->bsup1[1]=0.;
}

SP::ContactShapeDistance OccContactEdge::distance(
  const OccContactFace& sh2, bool normalFromFace1) const
{

  SP::ContactShapeDistance pdist(new ContactShapeDistance());
  ContactShapeDistance& dist = *pdist;

  dist.value = std::numeric_limits<double>::infinity();

  cadmbtb_distanceFaceEdge(sh2, *this,
                           dist.x1, dist.y1, dist.z1,
                           dist.x2, dist.y2, dist.z2,
                           dist.nx, dist.ny, dist.nz,
                           normalFromFace1,
                           dist.value);

  return pdist;
}

SP::ContactShapeDistance OccContactEdge::distance(
  const OccContactEdge& sh2, bool normalFromFace1) const
{
  RuntimeException::selfThrow(
    "cadmbtb_distance : cannot compute distance between edges");

  return SP::ContactShapeDistance();
}
