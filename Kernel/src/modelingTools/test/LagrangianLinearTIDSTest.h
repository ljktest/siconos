/* Siconos-Kernel version 2.1.0, Copyright INRIA 2005-2006.
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
#ifndef __LagrangianLinearTIDSTest__
#define __LagrangianLinearTIDSTest__

#include <cppunit/extensions/HelperMacros.h>
#include "LagrangianLinearTIDS.h"
#include "LagrangianLinearTIDSXML.h"
#include "RuntimeException.h"
#include "XMLException.h"

class LagrangianLinearTIDSTest : public CppUnit::TestFixture
{

private:

  // Name of the tests suite
  CPPUNIT_TEST_SUITE(LagrangianLinearTIDSTest);

  // tests to be done ...

  CPPUNIT_TEST(testBuildLagrangianLinearTIDS1);
  CPPUNIT_TEST(testBuildLagrangianLinearTIDS2);
  CPPUNIT_TEST(testBuildLagrangianLinearTIDS3);
  CPPUNIT_TEST(testcomputeDS);
  CPPUNIT_TEST(End);

  CPPUNIT_TEST_SUITE_END();

  // \todo exception test

  void testBuildLagrangianLinearTIDS1();
  void testBuildLagrangianLinearTIDS2();
  void testBuildLagrangianLinearTIDS3();
  void testcomputeDS();
  void End();

  // Members

  SimpleVector * q0, *velocity0;
  SiconosMatrix *mass, *K, *C;
  xmlNode * node1;
  LagrangianLinearTIDSXML* tmpxml1;

public:
  void setUp();
  void tearDown();

};

#endif




