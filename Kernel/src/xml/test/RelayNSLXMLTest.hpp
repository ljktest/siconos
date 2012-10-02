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
#ifndef __RELAYNSLAWXMLTEST__
#define __RELAYNSLAWXMLTEST__

#include <cppunit/extensions/HelperMacros.h>

#include <string>
#include <cstdlib>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "RelayNSLXML.hpp"
#include "XMLException.hpp"
#include<iostream>



class RelayNSLXMLTest : public CppUnit::TestFixture
{


private:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(RelayNSLXMLTest);


  xmlDoc *doc;
  xmlNode *root;
  RelayNSLXML RNonSmoothLaw;

  // on nomme la suite de tests
  CPPUNIT_TEST_SUITE(RelayNSLXMLTest);

  // on ajoute les tests a effectuer :

  // les tests qui doivent passer

  CPPUNIT_TEST(testGetC);
  CPPUNIT_TEST(testGetD);
  CPPUNIT_TEST_EXCEPTION(testIfTagIsNotPresent, XMLException);


  // on termine
  CPPUNIT_TEST_SUITE_END();



  // declaration de fonctions de test
  void testGetC();
  void testGetD();
  void testIfTagIsNotPresent();


public:
  void setUp();
  void tearDown();



};

#endif
