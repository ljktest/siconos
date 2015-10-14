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
#ifndef __LagrangianDSTest__
#define __LagrangianDSTest__

#include <cppunit/extensions/HelperMacros.h>
#include "LagrangianDS.hpp"
#include "RuntimeException.hpp"

class LagrangianDSTest : public CppUnit::TestFixture
{

private:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(LagrangianDSTest);


  // Name of the tests suite
  CPPUNIT_TEST_SUITE(LagrangianDSTest);

  // tests to be done ...

  CPPUNIT_TEST(testBuildLagrangianDS1);
  CPPUNIT_TEST(testBuildLagrangianDS2);
  CPPUNIT_TEST(testBuildLagrangianDS3);
  CPPUNIT_TEST(testBuildLagrangianDS4);
  CPPUNIT_TEST(testBuildLagrangianDS5);
  CPPUNIT_TEST(testcomputeDS);
  CPPUNIT_TEST(End);

  CPPUNIT_TEST_SUITE_END();

  // \todo exception test

  void testBuildLagrangianDS1();
  void testBuildLagrangianDS2();
  void testBuildLagrangianDS3();
  void testBuildLagrangianDS4();
  void testBuildLagrangianDS5();
  void testcomputeDS();
  void End();

  // Members

  SP::SiconosVector q0, velocity0, u0;
  SP::SiconosMatrix mass;

public:
  void setUp();
  void tearDown();

};

#endif




