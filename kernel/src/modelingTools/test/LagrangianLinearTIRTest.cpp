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
#include "LagrangianLinearTIRTest.hpp"


#define CPPUNIT_ASSERT_NOT_EQUAL(message, alpha, omega)      \
            if ((alpha) == (omega)) CPPUNIT_FAIL(message);

// test suite registration
CPPUNIT_TEST_SUITE_REGISTRATION(LagrangianLinearTIRTest);


void LagrangianLinearTIRTest::setUp()
{
  C.reset(new SimpleMatrix("matC.dat", true));
  D.reset(new SimpleMatrix("matD.dat", true));
  F.reset(new SimpleMatrix("matF.dat", true));
  e.reset(new SiconosVector(1));
  (*e)(0) = 0.1;
}

void LagrangianLinearTIRTest::tearDown()
{}

// data constructor (1)
void LagrangianLinearTIRTest::testBuildLagrangianLinearTIR1()
{
  std::cout << "--> Test: constructor 1." <<std::endl;
  SP::LagrangianLinearTIR folr(new LagrangianLinearTIR(C));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianLinearTIR1a : ", folr->C() == C, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianLinearTIR1c : ", folr->getType() == RELATION::Lagrangian, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianLinearTIR1d : ", folr->getSubType() == RELATION::LinearTIR, true);
  std::cout << "--> Constructor 1 test ended with success." <<std::endl;
}

// data constructor (5)
void LagrangianLinearTIRTest::testBuildLagrangianLinearTIR2()
{
  std::cout << "--> Test: constructor 2." <<std::endl;
  SP::LagrangianLinearTIR folr(new LagrangianLinearTIR(C, D, F, e));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianLinearTIR2f : ", folr->getType() == RELATION::Lagrangian, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianLinearTIR2g : ", folr->getSubType() == RELATION::LinearTIR, true);
  std::cout << "--> Constructor 2 test ended with success." <<std::endl;
}

// data constructor (5)
void LagrangianLinearTIRTest::testBuildLagrangianLinearTIR3()
{
  std::cout << "--> Test: constructor 3." <<std::endl;
  SP::LagrangianLinearTIR folr(new LagrangianLinearTIR(C, e));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianLinearTIR3a : ", folr->C() == C, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianLinearTIR3d : ", folr->e() == e, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianLinearTIR3f : ", folr->getType() == RELATION::Lagrangian, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianLinearTIR3g : ", folr->getSubType() == RELATION::LinearTIR, true);
  std::cout << "--> Constructor 3 test ended with success." <<std::endl;
}

// data constructor (4)
void LagrangianLinearTIRTest::testBuildLagrangianLinearTIR4()
{
  std::cout << "--> Test: constructor 4." <<std::endl;
  SP::LagrangianLinearTIR folr(new LagrangianLinearTIR(C));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianLinearTIR4c : ", folr->getType() == RELATION::Lagrangian, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildLagrangianLinearTIR4d : ", folr->getSubType() == RELATION::LinearTIR, true);
  std::cout << "--> Constructor 4 test ended with success." <<std::endl;
}
// set C as a matrix and then plug it


// setCPtr
void LagrangianLinearTIRTest::testSetCPtr()
{
  std::cout << "--> Test: setCPtr." <<std::endl;
  SP::SimpleMatrix tmp(new SimpleMatrix(*C));
  tmp->zero();
  SP::LagrangianLinearTIR folr(new LagrangianLinearTIR(tmp));
  folr->setCPtr(C);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSetCPtr : ", folr->C() == C, true);
  std::cout << "--> setCPtr test ended with success." <<std::endl;
}

// set D

// setDPtr
void LagrangianLinearTIRTest::testSetDPtr()
{
  std::cout << "--> Test: setDPtr." <<std::endl;
  SP::LagrangianLinearTIR folr(new LagrangianLinearTIR(C));
  folr->setDPtr(D);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSetDPtr: ", folr->D() == D, true);
  std::cout << "--> setDPtr test ended with success." <<std::endl;
}

// set F

// setFPtr
void LagrangianLinearTIRTest::testSetFPtr()
{
  std::cout << "--> Test: setFPtr." <<std::endl;
  SP::LagrangianLinearTIR folr(new LagrangianLinearTIR(C));
  folr->setFPtr(F);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSetFPtr: ", folr->F() == F, true);
  std::cout << "--> setFPtr test ended with success." <<std::endl;
}

// set E

// setEPtr
void LagrangianLinearTIRTest::testSetEPtr()
{
  std::cout << "--> Test: setEPtr." <<std::endl;
  SP::LagrangianLinearTIR folr(new LagrangianLinearTIR(C));
  folr->setEPtr(e);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSetEPtr: ", folr->e() == e, true);
  std::cout << "--> setEPtr test ended with success." <<std::endl;
}



void LagrangianLinearTIRTest::testGetJacPtr()
{
  std::cout << "--> Test: jac." <<std::endl;
  SP::LagrangianLinearTIR folr(new LagrangianLinearTIR(C));
  folr->setDPtr(D);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testGetJachq: ", folr->jachq() == C, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testGetJachlambda: ", folr->jachlambda() == D, true);

  std::cout << "--> setBPtr test ended with success." <<std::endl;
}

void LagrangianLinearTIRTest::End()
{
  std::cout << "===========================================" <<std::endl;
  std::cout << " ===== End of LagrangianLinearTIR Tests ===== " <<std::endl;
  std::cout << "=========================================== " <<std::endl;
}
