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
#include "SiconosMemoryTest.hpp"
#include "SiconosVector.hpp"
#include "BlockVector.hpp"

using  std::cout;
using std::endl;

// test suite registration
CPPUNIT_TEST_SUITE_REGISTRATION(SiconosMemoryTest);

void SiconosMemoryTest::setUp()
{
  _sizeMem = 3;
  unsigned int sizeVect = 3;

  std::vector<double> v(sizeVect);
  v[0] = 1;
  v[1] = 2;
  v[2] = 3;
  std::vector<double> w(sizeVect);
  w[0] = 4;
  w[1] = 5;
  w[2] = 6;
  std::vector<double> z(sizeVect);
  z[0] = 7;
  z[1] = 8;
  z[2] = 9;
  q1.reset(new SiconosVector(v));
  q2.reset(new SiconosVector(w));
  q3.reset(new SiconosVector(z));
  c1.reset(new BlockVector());
  c2.reset(new BlockVector());

  c1->insert(*q1);
  c1->insert(*q2);
  c2->insert(*q3);

  V1.reset(new MemoryContainer);
  V2.reset(new MemoryContainer);
  V3.reset(new MemoryContainer);

  V1->push_back(q1);
  V1->push_back(q2);
  //  V2->push_back(c1);
  //  V2->push_back(c2);
  V3->push_back(q2);
  V3->push_back(q1);

}

void SiconosMemoryTest::tearDown()
{}

// Constructor: data=memorySize
void SiconosMemoryTest::testBuildMemory1()
{
  std::cout << "=====================================" <<std::endl;
  std::cout << "===  SiconosMemory tests start ...=== " <<std::endl;
  std::cout << "=====================================" <<std::endl;

  std::cout << "--> Test: constructor 0." <<std::endl;
  SP::SiconosMemory tmp1(new SiconosMemory(4));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildMemory1 : memorysize OK", tmp1->getMemorySize() == 4, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildMemory1 : nbVectorsInMemory OK", tmp1->nbVectorsInMemory() == 0, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildMemory1 : size vector OK", tmp1->vectorMemory()->size() == 0, true);
  std::cout << "-->  testBuildMemory1 ended with success." <<std::endl;
}

// Constructor: copy of a std::vector of siconos vectors
void SiconosMemoryTest::testBuildMemory2()
{
  std::cout << "--> Test: constructor 1." <<std::endl;
  SP::SiconosMemory tmp1(new SiconosMemory(*V1));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildMemory2 : memorysize OK", tmp1->getMemorySize() == 2, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildMemory2 : nbVectorsInMemory OK", tmp1->nbVectorsInMemory() == 2, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildMemory2 : size vector OK", tmp1->vectorMemory()->size() == 2, true);

  //  SP::SiconosMemory tmp2(new SiconosMemory(*V2));
  //  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildMemory2 : memorysize OK", tmp2->getMemorySize() == 2, true);
  //  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildMemory2 : nbVectorsInMemory OK", tmp2->nbVectorsInMemory() == 2, true);
  //  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildMemory2 : size vector OK", tmp2->vectorMemory()->size() == 2, true);
  std::cout << "-->  testBuildMemory2 ended with success." <<std::endl;
}

// Constructor: copy of a std::vector of siconos vectors + memory size

void SiconosMemoryTest::testBuildMemory3()
{
  std::cout << "--> Test: constructor 1." <<std::endl;
  SP::SiconosMemory tmp1(new SiconosMemory(2, *V1));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildMemory3 : memorysize OK", tmp1->getMemorySize() == 2, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildMemory3 : nbVectorsInMemory OK", tmp1->nbVectorsInMemory() == 2, true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildMemory3 : size vector OK", tmp1->vectorMemory()->size() == 2, true);

  //  SP::SiconosMemory tmp2(new SiconosMemory(2,*V2));
  //  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildMemory3 : memorysize OK", tmp2->getMemorySize() == 2, true);
  //  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildMemory3 : nbVectorsInMemory OK", tmp2->nbVectorsInMemory() == 2, true);
  //  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildMemory3 : size vector OK", tmp2->vectorMemory()->size() == 2, true);
  std::cout << "-->  testBuildMemory3 ended with success." <<std::endl;
}

// setVectorMemory
void SiconosMemoryTest::testSetVectorMemory()
{
  std::cout << "--> Test: setVectorMemory." <<std::endl;
  //  SP::SiconosMemory tmp1(new SiconosMemory(*V1));
  //
  //  tmp1->setVectorMemory(*V2);
  //
  //  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildMemory1 : memorysize OK", tmp1->getMemorySize() == 2, true);
  //  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildMemory1 : _nbVectorsInMemory OK", tmp1->nbVectorsInMemory() == 2, true);
  //  CPPUNIT_ASSERT_EQUAL_MESSAGE("testBuildMemory1 : size vector OK", tmp1->vectorMemory()->size() == 2, true);
  std::cout << "-->  setVectorMemory test ended with success." <<std::endl;

}

// getSiconosVector
void SiconosMemoryTest::testGetSiconosVector()
{
  std::cout << "--> Test: getSiconosVector." <<std::endl;
  SP::SiconosMemory tmp1(new SiconosMemory(*V1));
  SP::SiconosVector tmp = tmp1->getSiconosVector(0);

  CPPUNIT_ASSERT_EQUAL_MESSAGE("testGetSiconosVector : *v1 size OK", tmp->size() == (*V1)[0]->size(), true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testGetSiconosVector : v1 values OK", *tmp == *((*V1)[0]), true);
  std::cout << "-->  getSiconosVector test ended with success." <<std::endl;
}

// swap

void SiconosMemoryTest::testSwap()
{
  std::cout << "--> Test: swap." <<std::endl;
  SP::SiconosMemory tmp1(new SiconosMemory(2));
  tmp1->swap(((*V1)[0]));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSwap : vector OK", *(tmp1->getSiconosVector(0)) == *((*V1)[0]), true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSwap : _nbVectorsInMemory OK", tmp1->nbVectorsInMemory() == 1, true);
  tmp1->swap(((*V1)[1]));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSwap : vector OK", *(tmp1->getSiconosVector(0)) == *((*V1)[1]), true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSwap : vector OK", *(tmp1->getSiconosVector(1)) == *((*V1)[0]), true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSwap : _nbVectorsInMemory OK", tmp1->nbVectorsInMemory() == 2, true);
  tmp1->swap(((*V1)[0]));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSwap : vector OK", *(tmp1->getSiconosVector(0)) == *((*V1)[0]), true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSwap : vector OK", *(tmp1->getSiconosVector(1)) == *((*V1)[1]), true);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("testSwap : _nbVectorsInMemory OK", tmp1->nbVectorsInMemory() == 2, true);
  std::cout << "-->  swap test ended with success." <<std::endl;
}

void SiconosMemoryTest::End()
{
  //   std::cout <<"======================================" <<std::endl;
  //   std::cout <<" ===== End of SiconosMemory Tests ===== " <<std::endl;
  //   std::cout <<"======================================" <<std::endl;
}


