/* Siconos-Kernel version 3.0.0, Copyright INRIA 2005-2008.
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
#include "FirstOrderType1R.h"
#include "RelationXML.h"
#include "Interaction.h"
#include "FirstOrderNonLinearDS.h"
#include "FirstOrderR.cpp"

using namespace std;

// xml constructor
FirstOrderType1R::FirstOrderType1R(SP::RelationXML FORxml):
  BaseClass(FORxml, RELATION::Type1R)
{
  JacH.resize(1);
  JacG.resize(1);
  // input g
  if (FORxml->hasG())
  {
    gName = FORxml->getGPlugin();
    setComputeGFunction(SSL::getPluginName(gName), SSL::getPluginFunctionName(gName));
    // Gradients
    if (!FORxml->hasJacobianG())
      RuntimeException::selfThrow("FirstOrderType1R xml constructor failed. No input for gradient(s) of g function.");

    if (FORxml->isJacobianGPlugin(0))
      JacG[0].reset(new PluggedMatrix(FORxml->getJacobianGPlugin(0)));
    else
      JacG[0].reset(new PluggedMatrix(FORxml->getJacobianGMatrix(0)));
  }

  // output h
  if (FORxml->hasH())
  {
    hName = FORxml->getHPlugin();
    setComputeHFunction(SSL::getPluginName(hName), SSL::getPluginFunctionName(hName));
    // Gradients
    if (!FORxml->hasJacobianH())
      RuntimeException::selfThrow("FirstOrderType1R xml constructor failed. No input for gradients of h function.");
    if (FORxml->isJacobianHPlugin(0))
      JacH[0].reset(new PluggedMatrix(FORxml->getJacobianHPlugin(0)));
    else
      JacH[0].reset(new PluggedMatrix(FORxml->getJacobianHMatrix(0)));
  }
}

FirstOrderType1R::FirstOrderType1R(const string& computeOut, const string& computeIn):
  BaseClass(RELATION::Type1R)
{
  // Size vector of pointers to functions.
  // Connect input and output to plug-in
  setComputeHFunction(SSL::getPluginName(computeOut), SSL::getPluginFunctionName(computeOut));
  setComputeGFunction(SSL::getPluginName(computeIn), SSL::getPluginFunctionName(computeIn));
  // The jacobians are not set, and thus considered as null matrices at this point.
  JacG.resize(1);
  JacH.resize(1);
  JacH[0].reset(new PluggedMatrix());
  JacG[0].reset(new PluggedMatrix());
}

FirstOrderType1R::FirstOrderType1R(const string& computeOut, const string& computeIn, const string& computeJX, const string& computeJL):
  BaseClass(RELATION::Type1R)
{
  // Size vector of pointers to functions.
  // Connect input and output to plug-in
  setComputeHFunction(SSL::getPluginName(computeOut), SSL::getPluginFunctionName(computeOut));
  setComputeGFunction(SSL::getPluginName(computeIn), SSL::getPluginFunctionName(computeIn));
  JacG.resize(1);
  JacH.resize(1);
  JacH[0].reset(new PluggedMatrix(computeJX));
  JacG[0].reset(new PluggedMatrix(computeJL));
}

void FirstOrderType1R::initialize(SP::Interaction inter)
{
  BaseClass::initialize(inter);

  // Check if an Interaction is connected to the Relation.
  unsigned int sizeY = interaction->getSizeOfY();
  unsigned int sizeDS = interaction->getSizeOfDS();
  unsigned int sizeZ = interaction->getSizeZ();
  if (!interaction)
    RuntimeException::selfThrow("FirstOrderR::initialize failed. No Interaction linked to the present relation.");

  // Update data member (links to DS variables)
  initDSLinks();
  // Initialize work vectors

  workX.reset(new SimpleVector(sizeDS));
  workZ.reset(new SimpleVector(sizeZ));
  workY.reset(new SimpleVector(sizeY));

  // The initialization of each component depends on the way the Relation was built ie if the matrix/vector
  // was read from xml or not
  if (JacH[0]->size(0) == 0) // if the matrix dim are null
    JacH[0]->resize(sizeY, sizeDS);
  else
    assert((JacH[0]->size(1) == sizeDS && JacH[0]->size(0) == sizeY) &&
           "FirstOrderType1R::initialize inconsistent sizes between JacH[0] matrix and the interaction.");

  // Same work for jacobianLambdaG
  if (JacG[0]->size(0) == 0) // if the matrix dim are null
    JacG[0]->resize(sizeDS, sizeY);
  else
    assert((JacG[0]->size(0) == sizeDS && JacG[0]->size(1) == sizeY) &&
           "FirstOrderType1R::initialize inconsistent sizes between JacG[0] matrix and the interaction.");
}

void FirstOrderType1R::computeH(double t)
{
  computeOutput(t, 0);
}

void FirstOrderType1R::computeG(double t)
{
  computeInput(t, 0);
}

void FirstOrderType1R::computeOutput(double t, unsigned int)
{
  assert(output && "FirstOrderType1R::computeOutput() is not linked to a plugin function");

  SP::SiconosVector y = interaction->getYPtr(0);
  // Warning: temporary method to have contiguous values in memory, copy of block to simple.

  *workX = *data[x];
  *workZ = *data[z];
  *workY = *y;

  unsigned int sizeY = y->size();
  unsigned int sizeX = data[x]->size();
  unsigned int sizeZ = data[z]->size(); //

  output(sizeX, &(*workX)(0), sizeY, &(*workY)(0), sizeZ, &(*workZ)(0));

  // Rebuilt y/z from Tmp
  *y = *workY;
  *data[z] = *workZ;
}

void FirstOrderType1R::computeInput(double t, unsigned int level)
{
  assert(input && "FirstOrderType1R::computeInput() is not linked to a plugin function");

  SP::SiconosVector lambda = interaction->getLambdaPtr(level);
  // Warning: temporary method to have contiguous values in memory, copy of block to simple.

  *workX = *data[r];
  *workZ = *data[z];
  *workY = *lambda;

  unsigned int sizeY = lambda->size();
  unsigned int sizeZ = data[z]->size();
  unsigned int sizeR = workX->size();

  input(sizeY, &(*workY)(0), sizeR, &(*workX)(0), sizeZ, &(*workZ)(0));

  *data[r] = *workX;
  *data[z] = *workZ;
}

void FirstOrderType1R::computeJacH(double, unsigned int index)
{
  //
  assert(index == 0 && "FirstOrderType1R::computeJacobianH(index): index is out of range");
  assert(JacH[0]->fPtr && "FirstOrderType1R::computeJacobianH() failed; not linked to a plug-in function.");

  // Warning: temporary method to have contiguous values in memory, copy of block to simple.
  *workX = *data[x];
  *workZ = *data[z];

  unsigned int sizeY = interaction->getSizeOfY();
  unsigned int sizeX = data[x]->size();
  unsigned int sizeZ = data[z]->size();

  (JacH[0]->fPtr)(sizeX, &(*workX)(0), sizeY, &(*(JacH[0]))(0, 0), sizeZ, &(*workZ)(0));

  // Rebuilt z from Tmp
  *data[z] = *workZ;
}

void FirstOrderType1R::computeJacG(double, unsigned int index)
{
  assert(index == 0 && "FirstOrderType1R::computeJacobianG(index): index is out of range");
  assert(JacG[0]->fPtr && "FirstOrderType1R::computeJacobianG() failed; not linked to a plug-in function.");

  SP::SiconosVector lambda = interaction->getLambdaPtr(0);
  // Warning: temporary method to have contiguous values in memory, copy of block to simple.
  *workZ = *data[z];
  *workY = *lambda;

  unsigned int sizeY = lambda->size();
  unsigned int sizeX = data[x]->size();
  unsigned int sizeZ = data[z]->size();

  (JacG[0]->fPtr)(sizeY, &(*workY)(0), sizeX, &(*(JacG[0]))(0, 0), sizeZ, &(*workZ)(0));

  // Rebuilt z from Tmp
  *data[z] = *workZ;
}

FirstOrderType1R* FirstOrderType1R::convert(Relation *r)
{
  return dynamic_cast<FirstOrderType1R*>(r);
}

