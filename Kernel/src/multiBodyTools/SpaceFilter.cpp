#include "SpaceFilter.hpp"


/* hash is done with encapsulation */


class HashedCircle : public Hashed
{
public:
  SP::Circle body;
  HashedCircle(SP::Circle c, int i, int j) : Hashed(c, i, j) {};

};

class HashedDisk : public Hashed
{
public:
  SP::Disk body;
  HashedDisk(SP::Disk d, int i, int j) : Hashed(d, i, j) {};

};

class HashedSphereLDS : public Hashed
{
public:
  SP::SphereLDS body;
  HashedSphereLDS(SP::SphereLDS d, int i, int j, int k) : Hashed(d, i, j, k) {};

};

/* needed by boost hash */
bool operator ==(SP::Hashed const& a, SP::Hashed const& b)
{
  return (a->i == b->i &&
          a->j == b->j &&
          a->k == b->k);
};



std::size_t hash_value(SP::Hashed const& h)
{
  std::size_t seed = 0;
  boost::hash_combine(seed, h->i);
  boost::hash_combine(seed, h->j);
  boost::hash_combine(seed, h->k);
  return seed;
};


TYPEDEF_SPTR(HashedCircle);
TYPEDEF_SPTR(HashedDisk);
TYPEDEF_SPTR(HashedSphereLDS);

/* the hashing is done with a visitor */
class SpaceFilter::_BodyHash : public SiconosVisitor
{
public:
  SP::SpaceFilter parent;
  _BodyHash(SP::SpaceFilter p) : parent(p) {};

  void visit(SP::Disk pds)
  {
    int i, j, imin, imax, jmin, jmax;

    unsigned int _bboxfactor = parent->bboxfactor();
    unsigned int _cellsize = parent->cellsize();

    imin = (int) floor((pds->getQ(0) - _bboxfactor * pds->getRadius()) / _cellsize);
    imax = (int) floor((pds->getQ(0) + _bboxfactor * pds->getRadius()) / _cellsize);

    jmin = (int) floor((pds->getQ(1) - _bboxfactor * pds->getRadius()) / _cellsize);
    jmax = (int) floor((pds->getQ(1) + _bboxfactor * pds->getRadius()) / _cellsize);

    for (i = imin; i <= imax; ++i)
    {
      for (j = jmin; j <= jmax; ++j)
      {
        parent->insert(pds, i, j, 0);
      }
    }
  };

  void visit(SP::Circle pds)
  {
    int i, j, imin, imax, jmin, jmax;

    unsigned int _bboxfactor = parent->bboxfactor();
    unsigned int _cellsize = parent->cellsize();

    imin = (int) floor((pds->getQ(0) - _bboxfactor * pds->getRadius()) / _cellsize);
    imax = (int) floor((pds->getQ(0) + _bboxfactor * pds->getRadius()) / _cellsize);

    jmin = (int) floor((pds->getQ(1) - _bboxfactor * pds->getRadius()) / _cellsize);
    jmax = (int) floor((pds->getQ(1) + _bboxfactor * pds->getRadius()) / _cellsize);

    for (i = imin; i <= imax; ++i)
    {
      for (j = jmin; j <= jmax; ++j)
      {
        parent->insert(pds, i, j, 0);
      }
    }
  }

  void visit(SP::SphereLDS pds)
  {
    int i, j, k, imin, imax, jmin, jmax, kmin, kmax;

    unsigned int _bboxfactor = parent->bboxfactor();
    unsigned int _cellsize = parent->cellsize();

    imin = (int) floor((pds->getQ(0) - _bboxfactor * pds->getRadius()) / _cellsize);
    imax = (int) floor((pds->getQ(0) + _bboxfactor * pds->getRadius()) / _cellsize);

    jmin = (int) floor((pds->getQ(1) - _bboxfactor * pds->getRadius()) / _cellsize);
    jmax = (int) floor((pds->getQ(1) + _bboxfactor * pds->getRadius()) / _cellsize);

    kmin = (int) floor((pds->getQ(2) - _bboxfactor * pds->getRadius()) / _cellsize);
    kmax = (int) floor((pds->getQ(2) + _bboxfactor * pds->getRadius()) / _cellsize);

    for (i = imin; i <= imax; ++i)
    {
      for (j = jmin; j <= jmax; ++j)
      {
        for (k = kmin; k <= kmax; ++k)
        {
          parent->insert(pds, i, j, k);
        }
      }
    }

  }

  void visit(SP::ExternalBody d)
  {
    d->selfHash(parent);
  }

  /* ... visit other objects */

};



/* proximity detection for circular object */
struct SpaceFilter::_CircularFilter : public SiconosVisitor
{
  SP::CircularDS ds1;
  SP::SpaceFilter parent;

  _CircularFilter(SP::SpaceFilter parent, SP::CircularDS ds1) : parent(parent), ds1(ds1) {};

  void visit_circular(SP::CircularDS ds2)
  {

    SP::CircularR rel;
    SP::DynamicalSystemsGraph DSG0 = parent->_nsds->topology()->dSG(0);

    assert(ds1 != ds2);
    assert(DSG0->bundle(DSG0->descriptor(ds1)) == ds1);
    assert(DSG0->bundle(DSG0->descriptor(ds2)) == ds2);

    double r1 = ds1->getRadius();
    double r2 = ds2->getRadius();
    double tol = r1 + r2;

    double x1 = ds1->getQ(0);
    double y1 = ds1->getQ(1);
    double x2 = ds2->getQ(0);
    double y2 = ds2->getQ(1);
    double rmax = fmax(r1, r2);
    double rmin = fmin(r1, r2);

    double d = hypot(x1 - x2, y1 - y2);

    if (d < rmax)
    {
      // one inside the other
      if (rmax - (d + rmin) < tol)
        rel.reset(new CircleCircleR(r1, r2));
    }
    else
    {
      if (d - (r1 + r2) < tol)
        rel.reset(new DiskDiskR(r1, r2));
    }


    if (rel)
    {
      bool found = false;
      DynamicalSystemsGraph::OEIterator oei, oeiend;
      for (boost::tie(oei, oeiend) = DSG0->out_edges(DSG0->descriptor(ds1));
           oei != oeiend; ++oei)
      {
        if (DSG0->bundle(DSG0->target(*oei)) == ds2)
        {
          found = true;
          break;
        }
      }

      if (!found)
      {
        SP::Interaction inter(new Interaction(2,
                                              parent->_nslaw,
                                              rel, parent->_interID++));
        inter->insert(ds1);
        inter->insert(ds2);
        parent->_nsds->topology()->insertInteraction(inter);
      }
    }
    else
    {
      // is interaction in graph ?
      bool found = false;
      DynamicalSystemsGraph::OEIterator oei, oeiend;
      for (boost::tie(oei, oeiend) = DSG0->out_edges(DSG0->descriptor(ds1));
           oei != oeiend; ++oei)
      {
        if (DSG0->bundle(DSG0->target(*oei)) == ds2)
        {
          found = true;
          break;
        }
      }

      if (found)
      {
        parent->_nsds->topology()->
        removeInteraction(DSG0->bundle(*oei)->interaction());
      }

    }
  }

  void visit(SP::Disk disk)
  {
    visit_circular(disk);
  }

  void visit(SP::Circle circle)
  {
    visit_circular(circle);
  }

  // do nothing (everything must be done in ExternalBody.findInteractions)
  void visit(SP::ExternalBody)
  {}


};

/* proximity detection for sphere objects */
struct SpaceFilter::_SphereLDSFilter : public SiconosVisitor
{
  SP::SpaceFilter parent;
  SP::SphereLDS ds1;

  _SphereLDSFilter(SP::SpaceFilter p, SP::SphereLDS s) : parent(p), ds1(s) {};

  void visit(SP::SphereLDS ds2)
  {
    SP::SphereLDSSphereLDSR rel;
    SP::DynamicalSystemsGraph DSG0 = parent->_nsds->topology()->dSG(0);

    assert(ds1 != ds2);
    assert(DSG0->bundle(DSG0->descriptor(ds1)) == ds1);
    assert(DSG0->bundle(DSG0->descriptor(ds2)) == ds2);

    double r1 = ds1->getRadius();
    double r2 = ds2->getRadius();
    double tol = r1 + r2;

    double x1 = ds1->getQ(0);
    double y1 = ds1->getQ(1);
    double z1 = ds1->getQ(2);
    double x2 = ds2->getQ(0);
    double y2 = ds2->getQ(1);
    double z2 = ds2->getQ(2);
    double rmax = fmax(r1, r2);
    double rmin = fmin(r1, r2);

    double dx = x1 - x2;
    double dy = y1 - y2;
    double dz = z1 - z2;

    double d = sqrt(dx * dx + dy * dy + dz * dz);

    if (d < 2 * tol)
    {
      rel.reset(new SphereLDSSphereLDSR(r1, r2));

      bool found = false;
      DynamicalSystemsGraph::OEIterator oei, oeiend;
      for (boost::tie(oei, oeiend) = DSG0->out_edges(DSG0->descriptor(ds1));
           oei != oeiend; ++oei)
      {
        if (DSG0->bundle(DSG0->target(*oei)) == ds2)
        {
          found = true;
          break;
        }
      }

      if (!found)
      {
        SP::Interaction inter(new Interaction(3,
                                              parent->_nslaw,
                                              rel, parent->_interID++));
        inter->insert(ds1);
        inter->insert(ds2);
        parent->_nsds->topology()->insertInteraction(inter);
      }
    }
    else
    {
      // is interaction in graph ?
      bool found = false;
      DynamicalSystemsGraph::OEIterator oei, oeiend;
      for (boost::tie(oei, oeiend) = DSG0->out_edges(DSG0->descriptor(ds1));
           oei != oeiend; ++oei)
      {
        if (DSG0->bundle(DSG0->target(*oei)) == ds2)
        {
          found = true;
          break;
        }
      }

      if (found)
      {
        parent->_nsds->topology()->
        removeInteraction(DSG0->bundle(*oei)->interaction());
      }

    }
  }
};


/* disk plan relation comparison */
struct SpaceFilter::_IsSameDiskPlanR : public SiconosVisitor
{
  double A, B, C, r, xCenter, yCenter, width;
  bool flag;
  SP::SpaceFilter parent;
  _IsSameDiskPlanR(SP::SpaceFilter p, double A, double B, double C, double r,
                   double xCenter, double yCenter, double width) :
    parent(p), A(A), B(B), C(C), r(r), xCenter(xCenter), yCenter(yCenter), width(width), flag(false) {};


  void visit(SP::DiskDiskR)
  {
    flag = false;
  };

  void visit(SP::CircleCircleR)
  {
    flag = false;
  };

  void visit(SP::DiskMovingPlanR)
  {
    flag = false;
  };


  void visit(SP::DiskPlanR rel)
  {
    flag = rel->equal(A, B, C, r, xCenter, yCenter, width);
  };

};

struct SpaceFilter::_IsSameDiskMovingPlanR : public SiconosVisitor
{
  FTime AF, BF, CF;
  double r;
  bool flag;
  SP::SpaceFilter parent;
  _IsSameDiskMovingPlanR(SP::SpaceFilter p, FTime AF, FTime BF, FTime CF, double r) :
    parent(p), AF(AF), BF(BF), CF(CF), r(r), flag(false) {};


  void visit(SP::DiskDiskR)
  {
    flag = false;
  };

  void visit(SP::CircleCircleR)
  {
    flag = false;
  };

  void visit(SP::DiskPlanR)
  {
    flag = false;
  };

  void visit(SP::DiskMovingPlanR rel)
  {
    flag = rel->equal(AF, BF, CF, r);
  };

};

/* sphere plan relation comparison */
struct SpaceFilter::_IsSameSphereLDSPlanR : public SiconosVisitor
{
  double A, B, C, D, r;
  bool flag;
  SP::SpaceFilter parent;
  _IsSameSphereLDSPlanR(SP::SpaceFilter p, double A, double B, double C, double D, double r):
    parent(p), A(A), B(B), C(C), D(D), r(r), flag(false) {};

  void visit(SP::SphereLDSSphereLDSR)
  {
    flag = false;
  };

  void visit(SP::SphereLDSPlanR rel)
  {
    flag = rel->equal(A, B, C, D, r);
  };
};


/* proximity detection between circular object and plans */
void SpaceFilter::_PlanCircularFilter(double A, double B, double C,
                                      double xCenter, double yCenter,
                                      double width, SP::CircularDS ds)
{
  double r = ds->getRadius();

  /* tolerance */
  double tol = r;

  SP::DynamicalSystemsGraph DSG0 = _nsds->topology()->dSG(0);

  boost::shared_ptr<_IsSameDiskPlanR>
  isSameDiskPlanR(new _IsSameDiskPlanR(shared_from_this(), A, B, C, r,
                                       xCenter, yCenter, width));


  // all DS must be in DS graph
  assert(DSG0->bundle(DSG0->descriptor(ds)) == ds);
  SP::DiskPlanR relp(new DiskPlanR(r, A, B, C, xCenter, yCenter, width));

  if (relp->distance(ds->getQ(0),
                     ds->getQ(1),
                     ds->getRadius()) < tol)

  {
    // is interaction in graph ?
    bool found = false;
    DynamicalSystemsGraph::OEIterator oei, oeiend;
    for (boost::tie(oei, oeiend) = DSG0->out_edges(DSG0->descriptor(ds));
         oei != oeiend; ++oei)
    {
      DSG0->bundle(*oei)->interaction()
      ->relation()->acceptSP(isSameDiskPlanR);
      if (DSG0->bundle(DSG0->target(*oei)) == ds
          && isSameDiskPlanR->flag)
      {
        found = true;
        break;
      }
    }
    if (!found)
      // no
    {
      SP::Interaction inter(new Interaction(2,
                                            _nslaw,
                                            relp, _interID++));
      inter->insert(ds);

      _nsds->topology()->insertInteraction(inter);

    }
  }
  else
  {
    // is interaction in graph ?
    DynamicalSystemsGraph::OEIterator oei, oeiend;
    for (boost::tie(oei, oeiend) = DSG0->out_edges(DSG0->descriptor(ds));
         oei != oeiend; ++oei)
    {
      DSG0->bundle(*oei)->interaction()
      ->relation()->acceptSP(isSameDiskPlanR);

      if (DSG0->bundle(DSG0->target(*oei)) == ds
          && isSameDiskPlanR->flag)
      {
        _nsds->topology()->
        removeInteraction(DSG0->bundle(*oei)->interaction());
        break;
      }
    }
  }
};



/* proximity detection between circular object and plans */
void SpaceFilter::_MovingPlanCircularFilter(unsigned int i, SP::CircularDS ds, double time)
{
  double r = ds->getRadius();

  /* tolerance */
  double tol = r;

  SP::DynamicalSystemsGraph DSG0 = _nsds->topology()->dSG(0);

  boost::shared_ptr<_IsSameDiskMovingPlanR>
  isSameDiskMovingPlanR(new _IsSameDiskMovingPlanR(shared_from_this(),
                        (*_moving_plans)(i, 0),
                        (*_moving_plans)(i, 1),
                        (*_moving_plans)(i, 2),
                        r));

  // all DS must be in DS graph
  assert(DSG0->bundle(DSG0->descriptor(ds)) == ds);
  SP::DiskMovingPlanR relp(new DiskMovingPlanR((*_moving_plans)(i, 0), (*_moving_plans)(i, 1), (*_moving_plans)(i, 2),
                           (*_moving_plans)(i, 3), (*_moving_plans)(i, 4), (*_moving_plans)(i, 5), r));

  relp->init(time);

  if (relp->distance(ds->getQ(0),
                     ds->getQ(1),
                     ds->getRadius()) < tol)

  {
    // is interaction in graph ?
    bool found = false;
    DynamicalSystemsGraph::OEIterator oei, oeiend;
    for (boost::tie(oei, oeiend) = DSG0->out_edges(DSG0->descriptor(ds));
         oei != oeiend; ++oei)
    {
      DSG0->bundle(*oei)->interaction()
      ->relation()->acceptSP(isSameDiskMovingPlanR);
      if (DSG0->bundle(DSG0->target(*oei)) == ds
          && isSameDiskMovingPlanR->flag)
      {
        found = true;
        break;
      }
    }
    if (!found)
      // no
    {
      SP::Interaction inter(new Interaction(2,
                                            _nslaw,
                                            relp, _interID++));
      inter->insert(ds);

      _nsds->topology()->insertInteraction(inter);

    }
  }
  else
  {
    // is interaction in graph ?
    DynamicalSystemsGraph::OEIterator oei, oeiend;
    for (boost::tie(oei, oeiend) = DSG0->out_edges(DSG0->descriptor(ds));
         oei != oeiend; ++oei)
    {
      DSG0->bundle(*oei)->interaction()
      ->relation()->acceptSP(isSameDiskMovingPlanR);

      if (DSG0->bundle(DSG0->target(*oei)) == ds
          && isSameDiskMovingPlanR->flag)
      {
        _nsds->topology()->
        removeInteraction(DSG0->bundle(*oei)->interaction());
        break;
      }
    }
  }
};

/* proximity detection between circular object and plans */
void SpaceFilter::_PlanSphereLDSFilter(double A, double B, double C, double D, SP::SphereLDS ds)
{
  double r = ds->getRadius();

  /* tolerance */
  double tol = r;

  SP::DynamicalSystemsGraph DSG0 = _nsds->topology()->dSG(0);

  boost::shared_ptr<_IsSameSphereLDSPlanR>
  isSameSphereLDSPlanR(new _IsSameSphereLDSPlanR(shared_from_this(), A, B, C, D, r));


  // all DS must be in DS graph
  assert(DSG0->bundle(DSG0->descriptor(ds)) == ds);
  SP::SphereLDSPlanR relp(new SphereLDSPlanR(r, A, B, C, D));
  if (relp->distance(ds->getQ(0),
                     ds->getQ(1),
                     ds->getQ(2),
                     ds->getRadius()) < tol)

  {
    // is interaction in graph ?
    bool found = false;
    DynamicalSystemsGraph::OEIterator oei, oeiend;
    for (boost::tie(oei, oeiend) = DSG0->out_edges(DSG0->descriptor(ds));
         oei != oeiend; ++oei)
    {
      DSG0->bundle(*oei)->interaction()
      ->relation()->acceptSP(isSameSphereLDSPlanR);
      if (DSG0->bundle(DSG0->target(*oei)) == ds
          && isSameSphereLDSPlanR->flag)
      {
        found = true;
        break;
      }
    }
    if (!found)
      // no
    {
      SP::Interaction inter(new Interaction(3,
                                            _nslaw,
                                            relp, _interID++));
      inter->insert(ds);

      _nsds->topology()->insertInteraction(inter);

    }
  }
  else
  {
    // is interaction in graph ?
    DynamicalSystemsGraph::OEIterator oei, oeiend;
    for (boost::tie(oei, oeiend) = DSG0->out_edges(DSG0->descriptor(ds));
         oei != oeiend; ++oei)
    {
      DSG0->bundle(*oei)->interaction()
      ->relation()->acceptSP(isSameSphereLDSPlanR);

      if (DSG0->bundle(DSG0->target(*oei)) == ds
          && isSameSphereLDSPlanR->flag)
      {
        _nsds->topology()->
        removeInteraction(DSG0->bundle(*oei)->interaction());
        break;
      }
    }
  }
};


/* insertion */
void SpaceFilter::insert(SP::Disk ds, int i, int j, int k)
{

  SP::Hashed hashed(new HashedDisk(ds, i, j));
  _hash_table.insert(hashed);
};

void SpaceFilter::insert(SP::Circle ds, int i, int j, int k)
{

  SP::Hashed hashed(new HashedCircle(ds, i, j));
  _hash_table.insert(hashed);
};

void SpaceFilter::insert(SP::SphereLDS ds, int i, int j, int k)
{

  SP::Hashed hashed(new HashedSphereLDS(ds, i, j, k));
  _hash_table.insert(hashed);
};

void SpaceFilter::insert(SP::Hashed hashed)
{
  _hash_table.insert(hashed);
};

/* insert other objects */



/* dynamical systems proximity detection */
typedef std::pair<int, int> interPair;

bool operator ==(interPair const& a, interPair const& b)
{
  return ((a.first == b.first) && (a.second == b.second));
};

struct SpaceFilter::_FindInteractions : public SiconosVisitor
{

  typedef std::tr1::unordered_multiset < interPair,
          boost::hash<interPair> > interPairs;

  SP::SpaceFilter parent;
  double time;
  _FindInteractions(SP::SpaceFilter p, double time) : parent(p), time(time) {};

  void visit_circular(SP::CircularDS  ds1)
  {
    assert(parent->_plans->size(0) > 0);

    // interactions with plans

    if (parent->_plans)
    {
      for (unsigned int i = 0; i < parent->_plans->size(0); ++i)
      {
        parent->_PlanCircularFilter((*parent->_plans)(i, 0),
                                    (*parent->_plans)(i, 1),
                                    (*parent->_plans)(i, 2),
                                    (*parent->_plans)(i, 3),
                                    (*parent->_plans)(i, 4),
                                    (*parent->_plans)(i, 5),
                                    ds1);
      }
    }

    if (parent->_moving_plans)
    {
      for (unsigned int i = 0; i < parent->_moving_plans->size1(); ++i)
      {
        parent->_MovingPlanCircularFilter(i, ds1, time);
      }
    }

    SP::SiconosVector Q1 = ds1->q();

    double x1 = Q1->getValue(0);
    double y1 = Q1->getValue(1);
    SP::Hashed hds1(new Hashed(ds1, (int) floor(x1 / parent->_cellsize),
                               (int) floor(y1 / parent->_cellsize)));

    // find all other systems that are in the same cells
    std::pair<space_hash::iterator, space_hash::iterator>
    neighbours = parent->_hash_table.equal_range(hds1);

    unsigned int j;
    interPairs declaredInteractions;
    boost::shared_ptr<_CircularFilter>
    circularFilter(new _CircularFilter(parent, ds1));

    for (j = 0; neighbours.first != neighbours.second; ++neighbours.first, ++j)
    {
      SP::LagrangianDS ds2 = (*neighbours.first)->body;
      int ids1 = ds1->number();
      int ids2 = ds2->number();
      int imax = (std::max)(ids1, ids2);
      int imin = (std::min)(ids1, ids2);
      if (ids1 != ids2)
      {
        // is interaction already treated ?
        interPair interpair;
        interpair = std::pair<int, int>(imin, imax);

        if (declaredInteractions.find(interpair)
            == declaredInteractions.end())
        {
          // no, check proximity
          declaredInteractions.insert(interpair);
          ds2->acceptSP(circularFilter);
        }

      }
    }

  };

  void visit(SP::Circle circle)
  {
    visit_circular(circle);
  };


  void visit(SP::Disk disk)
  {
    visit_circular(disk);
  };

  void visit(SP::SphereLDS ds1)
  {
    // interactions with plans
    for (unsigned int i = 0; i < parent->_plans->size(0); ++i)
    {
      parent->_PlanSphereLDSFilter((*parent->_plans)(i, 0),
                                   (*parent->_plans)(i, 1),
                                   (*parent->_plans)(i, 2),
                                   (*parent->_plans)(i, 3), ds1);
    }

    SP::SiconosVector Q1 = ds1->q();

    double x1 = Q1->getValue(0);
    double y1 = Q1->getValue(1);
    double z1 = Q1->getValue(2);
    SP::Hashed hds1(new Hashed(ds1, (int) floor(x1 / parent->_cellsize),
                               (int) floor(y1 / parent->_cellsize),
                               (int) floor(z1 / parent->_cellsize)));

    // find all other systems that are in the same cells
    std::pair<space_hash::iterator, space_hash::iterator>
    neighbours = parent->_hash_table.equal_range(hds1);

    unsigned int j;
    interPairs declaredInteractions;

    boost::shared_ptr<_SphereLDSFilter> sphereFilter(new _SphereLDSFilter(parent, ds1));

    for (j = 0; neighbours.first != neighbours.second; ++neighbours.first, ++j)
    {
      SP::LagrangianDS ds2 = (*neighbours.first)->body;
      int ids1 = ds1->number();
      int ids2 = ds2->number();
      int imax = (std::max)(ids1, ids2);
      int imin = (std::min)(ids1, ids2);
      if (ids1 != ids2)
      {
        // is interaction already treated ?
        interPair interpair;
        interpair = std::pair<int, int>(imin, imax);

        if (declaredInteractions.find(interpair)
            == declaredInteractions.end())
        {
          // no, check proximity
          declaredInteractions.insert(interpair);
          ds2->acceptSP(sphereFilter);
        }

      }
    }
  }

  void visit(SP::ExternalBody d)
  {
    d->selfFindInteractions(parent);
  }


};


/* general proximity detection */
void SpaceFilter::buildInteractions(double time)
{

  DSIterator it1;

  SP::DynamicalSystemsGraph
  DSG0 = _nsds->topology()->dSG(0);

  boost::shared_ptr<_BodyHash>
  hasher(new _BodyHash(shared_from_this()));
  boost::shared_ptr<_FindInteractions>
  findInteractions(new _FindInteractions(shared_from_this(), time));


  _hash_table.clear();

  // 1: rehash DS
  DynamicalSystemsGraph::VIterator vi, viend;
  for (boost::tie(vi, viend) = DSG0->vertices();
       vi != viend; ++vi)
  {
    // to avoid cast see dual dispatch, visitor pattern
    DSG0->bundle(*vi)->acceptSP(hasher);
  }

  // 2: prox detection
  for (boost::tie(vi, viend) = DSG0->vertices();
       vi != viend; ++vi)
  {
    DSG0->bundle(*vi)->acceptSP(findInteractions);
  }

  // should be done on the fly. Linear time anyway.
  _nsds->topology()->computeRelativeDegrees();

}
