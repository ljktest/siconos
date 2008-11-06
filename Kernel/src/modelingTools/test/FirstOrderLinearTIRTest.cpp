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
#include "FirstOrderLinearTIRTest.h"
using namespace std;

#define CPPUNIT_ASSERT_NOT_EQUAL(message, alpha, omega)      \
            if ((alpha) == (omega)) CPPUNIT_FAIL(message);

// test suite registration
CPPUNIT_TEST_SUITE_REGISTRATION(FirstOrderLinearTIRTest);


void FirstOrderLinearTIRTest::setUp()
{
  C.reset(new SimpleMatrix("matC.dat", true));
  D.reset(new SimpleMatrix("matD.dat", true));
  B.reset(new SimpleMatrix("matB.dat", true));
  F.reset(new SimpleMatrix("matF.dat", true));
  e.reset(new SimpleVector(2));
  (*e)(0) = 0.1;
  (*e)(1) = 0.1;
  // parse xml file:
  xmlDocPtr doc;
  xmlNodePtr cur;
  doc = xmlParseFile("firstOrderLinearTIR_test.xml");
  if (!doc)
    XMLException::selfThrow("Document not parsed successfully");
  cur = xmlDocGetRootElement(doc);
  if (!cur)
  {
    XMLException::selfThrow("empty document");
    xmlFreeDoc(doc);
  }
  // get rootNode

  if (xmlStrcmp(cur->name, (const xmlChar *) "SiconosModel"))
  {
    XMLException::selfThrow("document of the wrong type, root node !=SiconosModel");
    xmlFreeDoc(doc);
  }

  // look for NSDS, Interaction and relation node
  xmlNode* nodetmp = SiconosDOMTreeTools::findNodeChild(cur, "NSDS");
  SP::NonSmoothDynamicalSystemXML nsdsxml(new NonSmoothDynamicalSystemXML(nodetmp));
  nsds.reset(new NonSmoothDynamicalSystem(nsdsxml));
  nodetmp = SiconosDOMTreeTools::findNodeChild(nodetmp, "Interaction_Definition");
  nodetmp = SiconosDOMTreeTools::findNodeChild(nodetmp, "Interaction");
  nodetmp = SiconosDOMTreeTools::findNodeChild(nodetmp, "Interaction_Content");

  // get relation
  node1 = SiconosDOMTreeTools::findNodeChild(nodetmp, "FirstOrderRelation");
  tmpxml1.reset(new LinearRXML(node1));

}

void FirstOrderLinearTIRTest::tearDown()
{}

// xml constructor
void FirstOrderLinearTIRTest::testBuildFirstOrderLinearTIR0()
{
  cout << "==========================================" << endl;
  cout << "==== FirstOrderLinearTIR tests start ...====" << endl;
  cout << "==========================================" << endl;
  SP::FirstOrderLinearTIR folr(new FirstOrderLinearTIR(tmpxml1));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR0 : ", folr->getType() == RELATION::FirstOrder, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR0 : ", folr->getSubType() == RELATION::LinearTIR, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR0 : ", folr->getC() == *C, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR0 : ", folr->getD() == *D, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR0 : ", folr->getF() == *F, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR0 : ", folr->getE() == *e, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR0 : ", folr->getB() == *B, true);
  cout << "--> Constructor xml test ended with success." << endl;
}

// data constructor (1)
void FirstOrderLinearTIRTest::testBuildFirstOrderLinearTIR1()
{
  cout << "--> Test: constructor 1." << endl;
  SP::FirstOrderLinearTIR folr(new FirstOrderLinearTIR(C, B));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR1a : ", folr->getCPtr() == C, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR1b : ", folr->getBPtr() == B, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR1c : ", folr->getType() == RELATION::FirstOrder, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR1d : ", folr->getSubType() == RELATION::LinearTIR, true);
  cout << "--> Constructor 1 test ended with success." << endl;
}

// data constructor (5)
void FirstOrderLinearTIRTest::testBuildFirstOrderLinearTIR2()
{
  cout << "--> Test: constructor 2." << endl;
  SP::FirstOrderLinearTIR folr(new FirstOrderLinearTIR(C, D, F, e, B));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR2a : ", folr->getCPtr() == C, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR2b : ", folr->getDPtr() == D, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR2c : ", folr->getFPtr() == F, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR2d : ", folr->getEPtr() == e, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR2e : ", folr->getBPtr() == B, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR2f : ", folr->getType() == RELATION::FirstOrder, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR2g : ", folr->getSubType() == RELATION::LinearTIR, true);
  cout << "--> Constructor 2 test ended with success." << endl;
}

// data constructor (4)
void FirstOrderLinearTIRTest::testBuildFirstOrderLinearTIR4()
{
  cout << "--> Test: constructor 4." << endl;
  SP::FirstOrderLinearTIR folr(new FirstOrderLinearTIR(*C, *B));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR4a : ", folr->getC() == *C, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR4b : ", folr->getB() == *B, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR4c : ", folr->getType() == RELATION::FirstOrder, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR4d : ", folr->getSubType() == RELATION::LinearTIR, true);
  cout << "--> Constructor 4 test ended with success." << endl;
}

// data constructor (5)
void FirstOrderLinearTIRTest::testBuildFirstOrderLinearTIR5()
{
  cout << "--> Test: constructor 5." << endl;
  SP::FirstOrderLinearTIR folr(new FirstOrderLinearTIR(*C, *D, *F, *e, *B));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR5a : ", folr->getC() == *C, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR5b : ", folr->getD() == *D, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR5c : ", folr->getF() == *F, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR5d : ", folr->getE() == *e, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR5e : ", folr->getB() == *B, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR5f : ", folr->getType() == RELATION::FirstOrder, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildFirstOrderLinearTIR5g : ", folr->getSubType() == RELATION::LinearTIR, true);
  cout << "--> Constructor 5 test ended with success." << endl;
}

// set C as a matrix and then plug it
void FirstOrderLinearTIRTest::testSetC()
{
  cout << "--> Test: setC." << endl;
  SP::SiconosMatrix tmp(new SimpleMatrix(*C));
  tmp->zero();
  SP::FirstOrderLinearTIR folr(new FirstOrderLinearTIR(*tmp, *B));
  folr->setC(*C);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSetC : ", folr->getC() == *C, true);
  cout << "--> setC test ended with success." << endl;
}

// setCPtr
void FirstOrderLinearTIRTest::testSetCPtr()
{
  cout << "--> Test: setCPtr." << endl;
  SP::SiconosMatrix tmp(new SimpleMatrix(*C));
  tmp->zero();
  SP::FirstOrderLinearTIR folr(new FirstOrderLinearTIR(*tmp, *B));
  folr->setCPtr(C);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSetCPtr : ", folr->getCPtr() == C, true);
  cout << "--> setCPtr test ended with success." << endl;
}

// set D
void FirstOrderLinearTIRTest::testSetD()
{
  cout << "--> Test: setD." << endl;
  SP::FirstOrderLinearTIR folr(new FirstOrderLinearTIR(*C, *B));
  folr->setD(*D);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSetD: ", folr->getD() == *D, true);
  cout << "--> setD test ended with success." << endl;
}

// setDPtr
void FirstOrderLinearTIRTest::testSetDPtr()
{
  cout << "--> Test: setDPtr." << endl;
  SP::FirstOrderLinearTIR folr(new FirstOrderLinearTIR(*C, *B));
  folr->setDPtr(D);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSetDPtr: ", folr->getDPtr() == D, true);
  cout << "--> setDPtr test ended with success." << endl;
}

// set F
void FirstOrderLinearTIRTest::testSetF()
{
  cout << "--> Test: setF." << endl;
  SP::FirstOrderLinearTIR folr(new FirstOrderLinearTIR(*C, *B));
  folr->setF(*F);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSetF: ", folr->getF() == *F, true);
  cout << "--> setF test ended with success." << endl;
}

// setFPtr
void FirstOrderLinearTIRTest::testSetFPtr()
{
  cout << "--> Test: setFPtr." << endl;
  SP::FirstOrderLinearTIR folr(new FirstOrderLinearTIR(*C, *B));
  folr->setFPtr(F);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSetFPtr: ", folr->getFPtr() == F, true);
  cout << "--> setFPtr test ended with success." << endl;
}

// set E
void FirstOrderLinearTIRTest::testSetE()
{
  cout << "--> Test: setE." << endl;
  SP::FirstOrderLinearTIR folr(new FirstOrderLinearTIR(*C, *B));
  folr->setE(*e);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSetE: ", folr->getE() == *e, true);
  cout << "--> setE test ended with success." << endl;
}

// setEPtr
void FirstOrderLinearTIRTest::testSetEPtr()
{
  cout << "--> Test: setEPtr." << endl;
  SP::FirstOrderLinearTIR folr(new FirstOrderLinearTIR(*C, *B));
  folr->setEPtr(e);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSetEPtr: ", folr->getEPtr() == e, true);
  cout << "--> setEPtr test ended with success." << endl;
}

// set B
void FirstOrderLinearTIRTest::testSetB()
{
  cout << "--> Test: setB." << endl;
  SP::SiconosMatrix tmp(new SimpleMatrix(*B));
  tmp->zero();
  SP::FirstOrderLinearTIR folr(new FirstOrderLinearTIR(*C, *tmp));
  folr->setB(*B);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSetB: ", folr->getB() == *B, true);
  cout << "--> setB test ended with success." << endl;
}

// setBPtr
void FirstOrderLinearTIRTest::testSetBPtr()
{
  cout << "--> Test: setBPtr." << endl;
  SP::SiconosMatrix tmp(new SimpleMatrix(*B));
  tmp->zero();
  SP::FirstOrderLinearTIR folr(new FirstOrderLinearTIR(*C, *tmp));
  folr->setBPtr(B);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSetBPtr: ", folr->getBPtr() == B, true);
  cout << "--> setBPtr test ended with success." << endl;
}

void FirstOrderLinearTIRTest::testGetJac()
{
  cout << "--> Test: getJacPtr." << endl;
  SP::FirstOrderLinearTIR folr(new FirstOrderLinearTIR(*C, *B));
  folr->setDPtr(D);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testGetJacH: ", folr->getJacH(0) == *C, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testGetJacH: ", folr->getJacH(1) == *D, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testGetJacH: ", folr->getJacG(0) == *B, true);

  cout << "--> setBPtr test ended with success." << endl;
}

void FirstOrderLinearTIRTest::testGetJacPtr()
{
  cout << "--> Test: getJacPtr." << endl;
  SP::FirstOrderLinearTIR folr(new FirstOrderLinearTIR(C, B));
  folr->setDPtr(D);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testGetJacH: ", folr->getJacHPtr(0) == C, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testGetJacH: ", folr->getJacHPtr(1) == D, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testGetJacH: ", folr->getJacGPtr(0) == B, true);

  cout << "--> setBPtr test ended with success." << endl;
}

void FirstOrderLinearTIRTest::End()
{
  cout << "===========================================" << endl;
  cout << " ===== End of FirstOrderLinearTIR Tests ===== " << endl;
  cout << "=========================================== " << endl;
}
