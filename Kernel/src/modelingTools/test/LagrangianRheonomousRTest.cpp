/* Siconos-Kernel version 2.1.1, Copyright INRIA 2005-2007.
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
#include "LagrangianRheonomousRTest.h"
using namespace std;

#define CPPUNIT_ASSERT_NOT_EQUAL(message, alpha, omega)      \
            if ((alpha) == (omega)) CPPUNIT_FAIL(message);

// test suite registration
CPPUNIT_TEST_SUITE_REGISTRATION(LagrangianRheonomousRTest);


void LagrangianRheonomousRTest::setUp()
{
  // parse xml file:
  xmlDocPtr doc;
  xmlNodePtr cur;
  doc = xmlParseFile("LagrangianRheonomous_test.xml");
  if (doc == NULL)
    XMLException::selfThrow("Document not parsed successfully");
  cur = xmlDocGetRootElement(doc);
  if (cur == NULL)
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

  // look for NSDS node
  node = SiconosDOMTreeTools::findNodeChild(cur, "NSDS");
  xmlNode * nodetmp = SiconosDOMTreeTools::findNodeChild(node, "Interaction");
  nodetmp = SiconosDOMTreeTools::findNodeChild(nodetmp, "Interaction_Content");
  // get relation
  xmlNode * node1 = SiconosDOMTreeTools::findNodeChild(nodetmp, "LagrangianRelation");
  tmpxml1 = new LagrangianRXML(node1);
}

void LagrangianRheonomousRTest::tearDown()
{
  delete tmpxml1;
}

// xml constructor
void LagrangianRheonomousRTest::testBuildLagrangianRheonomousR0()
{
  cout << "==============================================" << endl;
  cout << "==== LagrangianRheonomousR tests start ...==== " << endl;
  cout << "==============================================" << endl;
  LagrangianRheonomousR * R1 = new LagrangianRheonomousR(tmpxml1);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianRheonomousR1a : ", R1->getType() == "Lagrangian", true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianRheonomousR1b : ", R1->getSubType() == "RheonomousR", true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianRheonomousR1c : ", R1->getFunctionName("h") == "TestPlugin:hRheo", true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianRheonomousR1d : ", R1->getFunctionName("G0") == "TestPlugin:G0Rheo", true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianRheonomousR1e : ", R1->getFunctionName("hDot") == "TestPlugin:hDot", true);
  delete R1;
  cout << " xml Constructor (1) LagrangianRheonomousR ok" << endl;
}

// default constructor:
void LagrangianRheonomousRTest::testBuildLagrangianRheonomousR1()
{
  LagrangianRheonomousR * R1 = new LagrangianRheonomousR();
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianRheonomousR2a : ", R1->getType() == "Lagrangian", true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianRheonomousR2b : ", R1->getSubType() == "RheonomousR", true);
  delete R1;
  cout << " default Constructor LagrangianRheonomousR ok" << endl;
}

// data constructor:
void LagrangianRheonomousRTest::testBuildLagrangianRheonomousR2()
{
  LagrangianRheonomousR * R1 = new LagrangianRheonomousR("TestPlugin:hRheo", "TestPlugin:hDot", "TestPlugin:G0Rheo");
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianRheonomousR3a : ", R1->getType() == "Lagrangian", true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianRheonomousR3b : ", R1->getSubType() == "RheonomousR", true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianRheonomousR3c : ", R1->getFunctionName("h") == "TestPlugin:hRheo", true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianRheonomousR3d : ", R1->getFunctionName("G0") == "TestPlugin:G0Rheo", true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianRheonomousR3e : ", R1->getFunctionName("hDot") == "TestPlugin:hDot", true);
  delete R1;
  cout << " data Constructor LagrangianRheonomousR ok" << endl;
}


void LagrangianRheonomousRTest::End()
{
  cout << "================================================" << endl;
  cout << "====== End of LagrangianRheonomousR tests ====== " << endl;
  cout << "================================================" << endl;
}
