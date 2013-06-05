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
#include "DynamicalSystemXMLTest.hpp"


#define CPPUNIT_ASSERT_NOT_EQUAL(message, alpha, omega)      \
            if ((alpha) == (omega)) CPPUNIT_FAIL(message);

// on place cette classe de test dans le registry
CPPUNIT_TEST_SUITE_REGISTRATION(DynamicalSystemXMLTest);




void DynamicalSystemXMLTest::setUp()
{

  try
  {
    this->doc = xmlParseFile("DynamicalSystem.xml");
    this->root = xmlDocGetRootElement(doc);
    this->ds = DynamicalSystemXML(root, false);

    vector<double> v(6);
    v.at(0) = 1.0;
    v.at(1) = 0.0;
    v.at(2) = 0.0;
    v.at(3) = 0.0;
    v.at(4) = 0.0;
    v.at(5) = 0.0;
    vectorRef = /*SiconosVector*/SiconosVector(v);

    matrixRef = SiconosMatrix("matrix.dat", true);
    //vectorRef = SiconosVector("vector.dat", true);
  }
  catch (SiconosException e)
  {
    std::cout << "Error in DynamicalSystemXMLTest : " << e.report() <<std::endl;
    exit(0);
  }

}

void DynamicalSystemXMLTest::tearDown()
{
  //xmlFreeDoc(doc);
  xmlCleanupParser();
}


//______________________________________________________________________________

void DynamicalSystemXMLTest::testGetNumber()
{
  CPPUNIT_ASSERT_MESSAGE("testGetNumber : ", ds.number() == 120);
  std::cout << "DynamicalSystemXMLTest >>> testGetNumber ............................ OK\n ";
}

void DynamicalSystemXMLTest::testGetType()
{
  CPPUNIT_ASSERT_MESSAGE("testGetType : ", ds.getType() == "LagrangianDS");
  std::cout << "DynamicalSystemXMLTest >>> testGetType .............................. OK\n ";
}

void DynamicalSystemXMLTest::testGetId()
{
  CPPUNIT_ASSERT_MESSAGE("testGetId : ", ds.getId() == "Ball");
  std::cout << "DynamicalSystemXMLTest >>> testGetId ................................ OK\n ";
}

void DynamicalSystemXMLTest::testGetN()
{
  CPPUNIT_ASSERT_MESSAGE("testGetN : ", ds.getN() == 6);
  std::cout << "DynamicalSystemXMLTest >>> testGetN ................................. OK\n ";
}

void DynamicalSystemXMLTest::testGetStepsInMemory()
{
  CPPUNIT_ASSERT_MESSAGE("testGetStepsInMemory : ", ds.getStepsInMemory() == 3);
  std::cout << "DynamicalSystemXMLTest >>> testGetStepsInMemory ..................... OK\n ";
}

void DynamicalSystemXMLTest::testGetx()
{

  CPPUNIT_ASSERT_MESSAGE("testGetX : ", ds.getx() == vectorRef);
  CPPUNIT_ASSERT_MESSAGE("testGetX0 : ", ds.getX0() == vectorRef);
  CPPUNIT_ASSERT_MESSAGE("testGetXDot : ", ds.getXDot() == vectorRef);

  //  vector<SiconosVector*> XMem;
  //  XMem = ds.getXMemory();
  //
  //  CPPUNIT_ASSERT_MESSAGE("testGetXDot : ", *XMem[1]==vectorRef);

  std::cout << "DynamicalSystemXMLTest >>> testGetX ................................. OK\n ";
}


