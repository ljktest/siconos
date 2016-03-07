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
#include "SiconosVisitorTest.hpp"
class ObjectA;
class ObjectB;

#include "../SiconosVisitables.hpp"
#undef SICONOS_VISITABLES
#define SICONOS_VISITABLES()                    \
  KERNEL_CLASSES()                              \
  REGISTER(ObjectA)                             \
  REGISTER(ObjectB)

#include "TypeName.hpp"
#include "../SiconosVisitor.hpp"



// test suite registration
CPPUNIT_TEST_SUITE_REGISTRATION(SiconosVisitorTest);


void SiconosVisitorTest::setUp()
{
}

void SiconosVisitorTest::tearDown()
{
}

class DynamicalSystem
{
public:
  VIRTUAL_ACCEPT_VISITORS(DynamicalSystem);
  virtual ~DynamicalSystem() {}
};

class LagrangianDS : public DynamicalSystem
{
public:
  ACCEPT_STD_VISITORS();
};

/* TypeOf */
void SiconosVisitorTest::t1()
{
  DynamicalSystem *ds = new LagrangianDS();

  CPPUNIT_ASSERT(Type::value(*ds) == Type::LagrangianDS);

  delete(ds);

}

/* standard visitor */
void SiconosVisitorTest::t2()
{

  struct MyVisitor : public SiconosVisitor
  {
#if !defined(_MSC_VER)
    using SiconosVisitor::visit;
#endif

    void visit(const LagrangianDS& ds)
    {
      ;
    }

  };

  DynamicalSystem *ds = new LagrangianDS();

  try
  {
    MyVisitor myvisitor;

    ds->accept(myvisitor);

    delete(ds);
  }

  catch (...)
  {
    CPPUNIT_ASSERT(false);
    delete(ds);
  }


}

void SiconosVisitorTest::t3()
{

  DynamicalSystem *ds = new LagrangianDS();

  CPPUNIT_ASSERT(Type::name(*ds) == "LagrangianDS");

  delete ds;
}

struct Object {

  VIRTUAL_ACCEPT_VISITORS();

  virtual ~Object() {};

};

struct ObjectA : public Object
{
  int id;
  int dummya;

  ACCEPT_STD_VISITORS();
  virtual ~ObjectA() {};

};

struct ObjectB : public Object
{
  int id;
  int dummyb;

  ACCEPT_STD_VISITORS();
  virtual ~ObjectB() {};

};

#define VISITOR_CLASSES()\
  REGISTER(ObjectA)\
  REGISTER(ObjectB)

#include "VisitorMaker.hpp"
using namespace Experimental;

struct GetId : public SiconosVisitor
{

  int result;

  template<typename T>
  void operator()(const T& obj)
  {
    result = obj.id;
  }
};


void SiconosVisitorTest::t4()
{
  Object *o;

  ObjectA ooa;
  ObjectB oob;

  ooa.id = 0;
  oob.id = 1;


  Visitor < Classes < ObjectA, ObjectB >,
            GetId >::Make visitor;

  o = & ooa;

  o->accept(visitor);

  CPPUNIT_ASSERT(visitor.result == 0);

  o = & oob;

  o->accept(visitor);

  CPPUNIT_ASSERT(visitor.result == 1);

};
