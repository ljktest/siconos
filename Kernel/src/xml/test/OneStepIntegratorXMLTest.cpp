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
#include "OneStepIntegratorXMLTest.hpp"

#define CPPUNIT_ASSERT_NOT_EQUAL(message, alpha, omega)      \
            if ((alpha) == (omega)) CPPUNIT_FAIL(message);

// on place cette classe de test dans le registry
CPPUNIT_TEST_SUITE_REGISTRATION(OneStepIntegratorXMLTest);




void OneStepIntegratorXMLTest::setUp()
{
  try
  {
    modelXML = new SiconosModelXML("OneStepI.xml");
    strategyXML = modelXML->getStrategyXML();
    oneStepIs = strategyXML->getOneStepIntegratorXML();
    std::cout << "%%% setup : size == " << oneStepIs.size() <<std::endl;
  }
  catch (SiconosException e)
  {
    std::cout << "Error in OneStepIntegratorXMLTest : " << e.report() <<std::endl;
    exit(0);
  }
}

void OneStepIntegratorXMLTest::tearDown()
{

}

void OneStepIntegratorXMLTest::testNbOneStepIntegrator()
{
  CPPUNIT_ASSERT_MESSAGE("testNbOneStepIntegrator ", oneStepIs.size() == 3);
  std::cout << "OneStepIntegratorXMLTest >>> testNbOneStepIntegrator ................ OK\n ";
}

void OneStepIntegratorXMLTest::testGetR()
{
  // integrator 0 is a MoreauJeanOSI and doesn't have a attribute r !
  //  CPPUNIT_ASSERT_MESSAGE("testGetR : OneStepIntegator 1", oneStepIs[0]->getR() == 55);
  CPPUNIT_ASSERT_MESSAGE("testGetR : OneStepIntegator 2", oneStepIs[1]->getR() == 0);
  CPPUNIT_ASSERT_MESSAGE("testGetR : OneStepIntegator 3", oneStepIs[2]->getR() == -55);
  std::cout << "OneStepIntegratorXMLTest >>> testGetR ............................... OK\n ";
}


void OneStepIntegratorXMLTest::testGetDSConcerned()
{

  CPPUNIT_ASSERT_MESSAGE("testGetDSConcerned : OneStepIntegator 1 :size", oneStepIs[0]->getDSConcerned().size() == 1);
  CPPUNIT_ASSERT_MESSAGE("testGetDSConcerned : OneStepIntegator 1", oneStepIs[0]->getDSConcerned().at(0) == 2);

  CPPUNIT_ASSERT_MESSAGE("testGetDSConcerned : OneStepIntegator 2 :size", oneStepIs[1]->getDSConcerned().size() == 1);
  CPPUNIT_ASSERT_MESSAGE("testGetDSConcerned : OneStepIntegator 2", oneStepIs[1]->getDSConcerned().at(0) == 2);

  CPPUNIT_ASSERT_MESSAGE("testGetDSConcerned : OneStepIntegator 3 :size", oneStepIs[2]->getDSConcerned().size() == 1);
  CPPUNIT_ASSERT_MESSAGE("testGetDSConcerned : OneStepIntegator 3", oneStepIs[2]->getDSConcerned().at(0) == 1);

  std::cout << "OneStepIntegratorXMLTest >>> testGetDSConcerned ..................... OK\n ";
}



void OneStepIntegratorXMLTest::testGetType()
{
  CPPUNIT_ASSERT_MESSAGE("testGetType : OneStepIntegator 1 : MoreauJeanOSI ", oneStepIs[0]->getType() == "MoreauJeanOSI");
  CPPUNIT_ASSERT_MESSAGE("testGetType : OneStepIntegator 2 : Adams", oneStepIs[1]->getType() == "Adams");
  CPPUNIT_ASSERT_MESSAGE("testGetType : OneStepIntegator 3 : LsodarOSI", oneStepIs[2]->getType() == "LSODAR");

  std::cout << "OneStepIntegratorXMLTest >>> testGetType ............................ OK\n ";
}

void OneStepIntegratorXMLTest::testAdamsXML()
{
  AdamsXML* adams = static_cast<AdamsXML*>(oneStepIs[1]);

  CPPUNIT_ASSERT_MESSAGE("testAdamsXML type ", adams->getType() == "Adams");
  CPPUNIT_ASSERT_MESSAGE("testAdamsXML R", adams->getR() == 0);

  std::cout << "OneStepIntegratorXMLTest >>> testAdamsXML ........................... OK\n ";
}

void OneStepIntegratorXMLTest::testMoreauJeanOSIXML()
{
  MoreauJeanOSIXML* moreau = static_cast<MoreauJeanOSIXML*>(oneStepIs[0]);

  CPPUNIT_ASSERT_MESSAGE("testMoreauJeanOSIXML type ", moreau->getType() == "MoreauJeanOSI");
  //CPPUNIT_ASSERT_MESSAGE("testMoreauJeanOSIXML R", moreau->getR() == 55);

  std::cout << "OneStepIntegratorXMLTest >>> testMoreauJeanOSIXML .......................... OK\n ";
}

void OneStepIntegratorXMLTest::testLsodarOSIXML()
{
  LsodarOSIXML* lsodar = static_cast<LsodarOSIXML*>(oneStepIs[2]);

  CPPUNIT_ASSERT_MESSAGE("testLsodarOSIXML type ", lsodar->getType() == "LSODAR");
  CPPUNIT_ASSERT_MESSAGE("testLsodarOSIXML R", lsodar->getR() == -55);

  std::cout << "OneStepIntegratorXMLTest >>> testLsodarOSIXML .......................... OK\n ";
}
