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
#include "NonSmoothSolverXML.h"
#include "NonSmoothSolver.h" // For DEFAULT_XXX constants

NonSmoothSolverXML::NonSmoothSolverXML(xmlNodePtr solverNode):
  rootNode(solverNode), iparamNode(NULL), dparamNode(NULL)
{
  // Connect dparam and iparam nodes.
  xmlNodePtr node = NULL;
  if ((node = SiconosDOMTreeTools::findNodeChild(rootNode, "iparam")) != NULL)
    iparamNode = node;
  if ((node = SiconosDOMTreeTools::findNodeChild(rootNode, "dparam")) != NULL)
    dparamNode = node;
}

NonSmoothSolverXML::~NonSmoothSolverXML()
{
  rootNode = NULL;
  iparamNode = NULL;
  dparamNode = NULL;
}


void NonSmoothSolverXML::getIParam(std::vector<int>& iparam)
{
  // No error output if the node is null. It is the role of the function that call getIparam to check that.
  if (iparamNode != NULL)
    SiconosDOMTreeTools::getVector(iparamNode, iparam);
}

void NonSmoothSolverXML::getDParam(std::vector<double>& dparam)
{
  // No error output if the node is null. It is the role of the function that call getDparam to check that.
  if (dparamNode != NULL)
    SiconosDOMTreeTools::getVector(dparamNode, dparam);
}
