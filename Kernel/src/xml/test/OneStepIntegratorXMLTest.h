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
#ifndef __OneStepIntegratorXMLTest__
#define __OneStepIntegratorXMLTest__

#include <cppunit/extensions/HelperMacros.h>

#include <string>
#include <cstdlib>
#include <libxml/parser.h>
#include "NewSiconosVector.h"
#include "SiconosMatrix.h"
#include "OneStepIntegratorXML.h"
#include "SiconosModelXML.h"

#include "AdamsXML.h"
#include "MoreauXML.h"
#include "LsodarXML.h"

using namespace std;

class OneStepIntegratorXMLTest: public CppUnit::TestFixture
{


private:
  SiconosModelXML* modelXML;
  StrategyXML* strategyXML;
  vector<OneStepIntegratorXML*> oneStepIs;

  // on nomme la suite de tests
  CPPUNIT_TEST_SUITE(OneStepIntegratorXMLTest);

  // on ajoute les tests a effectuer :

  // les tests qui doivent passer
  CPPUNIT_TEST(testNbOneStepIntegrator);
  CPPUNIT_TEST(testGetR);
  CPPUNIT_TEST(testGetDSConcerned);
  CPPUNIT_TEST(testGetType);
  CPPUNIT_TEST(testAdamsXML);
  CPPUNIT_TEST(testMoreauXML);
  CPPUNIT_TEST(testLsodarXML);
  // exceptions
  //CPPUNIT_TEST_EXCEPTION(testGetStringAttributeValueException, XMLException);


  // on termine
  CPPUNIT_TEST_SUITE_END();


  // declaration de fonctions de testvoid OneStepIntegratorXMLTest::testGetR()
  void testNbOneStepIntegrator();
  void testGetR();
  void testGetDSConcerned();
  void testGetType();
  void testAdamsXML();
  void testMoreauXML();
  void testLsodarXML();


public:
  void setUp();
  void tearDown();

};

#endif // __OneStepIntegratorXMLTest__
