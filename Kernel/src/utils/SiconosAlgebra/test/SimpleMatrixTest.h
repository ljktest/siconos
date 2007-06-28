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
#ifndef __SimpleMatrixTest__
#define __SimpleMatrixTest__

#include <cppunit/extensions/HelperMacros.h>
#include "SimpleMatrix.h"

using namespace std;

class SimpleMatrixTest : public CppUnit::TestFixture
{


private:

  // test suite
  CPPUNIT_TEST_SUITE(SimpleMatrixTest);

  CPPUNIT_TEST(testConstructor0);
  CPPUNIT_TEST(testConstructor1);
  CPPUNIT_TEST(testConstructor2);
  //CPPUNIT_TEST(testConstructor3);
  CPPUNIT_TEST(testConstructor4);
  CPPUNIT_TEST(testConstructor5);
  CPPUNIT_TEST(testConstructor6);
  CPPUNIT_TEST(testConstructor7);
  CPPUNIT_TEST(testConstructor8);
  CPPUNIT_TEST(testConstructor9);
  CPPUNIT_TEST(testConstructor10);
  CPPUNIT_TEST(testConstructor11);
  CPPUNIT_TEST(testConstructor12);
  CPPUNIT_TEST(testGetSetRowCol);
  CPPUNIT_TEST(testZero);
  CPPUNIT_TEST(testEye);
  CPPUNIT_TEST(testResize);
  CPPUNIT_TEST(testNormInf);
  CPPUNIT_TEST(testGetBlock);
  CPPUNIT_TEST(testMatrixCopy);
  CPPUNIT_TEST(testTrans);
  CPPUNIT_TEST(testAssignment);
  CPPUNIT_TEST(testOperators1);
  CPPUNIT_TEST(testOperators2);
  CPPUNIT_TEST(testOperators3);
  CPPUNIT_TEST(testOperators4);
  CPPUNIT_TEST(testOperators5);
  CPPUNIT_TEST(testOperators6);
  CPPUNIT_TEST(testOperators7);
  CPPUNIT_TEST(testOperators8);
  CPPUNIT_TEST(testOperators9);
  CPPUNIT_TEST(testOperators10);
  CPPUNIT_TEST(testOperators11);
  CPPUNIT_TEST(testOperators12);
  CPPUNIT_TEST(testOperators13);
  CPPUNIT_TEST(testPow);
  CPPUNIT_TEST(testProd);
  CPPUNIT_TEST(End);
  CPPUNIT_TEST_SUITE_END();

  void testConstructor0();
  void testConstructor1();
  void testConstructor2();
  //  void testConstructor3();
  void testConstructor4();
  void testConstructor5();
  void testConstructor6();
  void testConstructor7();
  void testConstructor8();
  void testConstructor9();
  void testConstructor10();
  void testConstructor11();
  void testConstructor12();
  void testGetSetRowCol();
  void testZero();
  void testEye();
  void testResize();
  void testNormInf();
  void testGetBlock();
  void testMatrixCopy();
  void testTrans();
  void testAssignment();
  void testOperators1();
  void testOperators2();
  void testOperators3();
  void testOperators4();
  void testOperators5();
  void testOperators6();
  void testOperators7();
  void testOperators8();
  void testOperators9();
  void testOperators10();
  void testOperators11();
  void testOperators12();
  void testOperators13();
  void testPow();
  void testProd();
  void End();

  SiconosMatrix *SicM;
  SimpleMatrix *SimM;
  string fic1, fic2;
  SimpleVector* vect1, *vect2, *vect3;
  DenseMat * D;
  TriangMat *T, *T2;
  SymMat *S, *S2;
  BandedMat *B, *B2;
  SparseMat *SP;
  ZeroMat * Z, *Z2;
  IdentityMat* I, *I2;
  double tol;

public:
  void setUp();
  void tearDown();

};

#endif
