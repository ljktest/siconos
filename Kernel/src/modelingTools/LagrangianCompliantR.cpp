/* Siconos-Kernel, Copyright INRIA 2005-2010.
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

// \todo : create a work vector for all tmp vectors used in computeg, computeh ...

#include "LagrangianCompliantR.hpp"
#include "RelationXML.hpp"
#include "Interaction.hpp"
#include "LagrangianDS.hpp"
#include "Plugin.hpp"

using namespace std;
using namespace RELATION;

// xml constructor
LagrangianCompliantR::LagrangianCompliantR(SP::RelationXML LRxml): LagrangianR(LRxml, CompliantR)
{
  if (!LRxml->hasH())
    RuntimeException::selfThrow("LagrangianCompliantR:: xml constructor failed, can not find a definition for h.");

  hName = LRxml->gethPlugin();
  setComputehFunction(SSL::getPluginName(hName), SSL::getPluginFunctionName(hName));

  //   if(!LRxml->hasJacobianH())
  //     RuntimeException::selfThrow("LagrangianCompliantR:: xml constructor failed, can not find a definition for Jach0.");
  //   Jach.resize(2);
  //   LRxml->readJacobianXML<PluggedMatrix,SP_PluggedMatrix>(Jach[0], LRxml, 0);
  //   LRxml->readJacobianXML<PluggedMatrix,SP_PluggedMatrix>(Jach[1], LRxml, 1);
}

// constructor from a set of data
LagrangianCompliantR::LagrangianCompliantR(const string& computeh, const std::vector<string> & computeg): LagrangianR(CompliantR)
{
  Plugin::setFunction(&hPtr, SSL::getPluginName(computeh), SSL::getPluginFunctionName(computeh));
  pluginNameHPtr = computeh;
  Plugin::setFunction(&_jachqPtr, SSL::getPluginName(computeg[0]), SSL::getPluginFunctionName(computeg[0]));
  pluginNameJachqPtr = computeg[0];
  Plugin::setFunction(&_jachlambdaPtr, SSL::getPluginName(computeg[1]), SSL::getPluginFunctionName(computeg[1]));
  pluginNameJachlambdaPtr = computeg[1];

  // Warning: we cannot allocate memory for Jach[0] matrix since no interaction
  // is connected to the relation. This will be done during initialize.
  // We only set the name of the plugin-function and connect it to the user-defined function.
}

void LagrangianCompliantR::initComponents()
{
  LagrangianR::initComponents();
  unsigned int sizeY = interaction()->getSizeOfY();
  _workL.reset(new SimpleVector(sizeY));

  if (! _jachlambda)
    _jachlambda.reset(new SimpleMatrix(sizeY, sizeY));
  else
    _jachlambda->resize(sizeY, sizeY);
}

void LagrangianCompliantR::computeh(double time)
{
  if (hPtr)
  {
    // get vector y of the current interaction
    SP::SiconosVector y = interaction()->y(0);
    SP::SiconosVector lambda = interaction()->lambda(0);

    // Warning: temporary method to have contiguous values in memory, copy of block to simple.
    *_workX = *data[q0];
    *_workZ = *data[z];
    *_workY = *y;
    *_workL = *lambda;

    unsigned int sizeQ = _workX->size();
    unsigned int sizeY = y->size();
    unsigned int sizeZ = _workZ->size();

    hPtr(sizeQ, &(*_workX)(0), sizeY, &(*_workL)(0), &(*_workY)(0), sizeZ, &(*_workZ)(0));

    // Copy data that might have been changed in the plug-in call.
    *data[z] = *_workZ;
    *y = *_workY;
  }
}

void LagrangianCompliantR::computeJachq(double time)
{

  if (_jachqPtr)
  {
    // Warning: temporary method to have contiguous values in memory, copy of block to simple.
    *_workX = *data[q0];
    *_workZ = *data[z];

    unsigned int sizeY = _jachq->size(0);
    unsigned int sizeQ = _workX->size();
    unsigned int sizeZ = _workZ->size();

    // get vector lambda of the current interaction
    *_workL = *interaction()->lambda(0);
    (_jachqPtr)(sizeQ, &(*_workX)(0), sizeY, &(*_workL)(0), &(*_jachq)(0, 0), sizeZ, &(*_workZ)(0));
    // Copy data that might have been changed in the plug-in call.
    *data[z] = *_workZ;
  }
}
void LagrangianCompliantR::computeJachlambda(double time)
{

  if (_jachlambdaPtr)
  {
    // Warning: temporary method to have contiguous values in memory, copy of block to simple.
    *_workX = *data[q0];
    *_workZ = *data[z];

    unsigned int sizeY = _jachq->size(0);
    unsigned int sizeQ = _workX->size();
    unsigned int sizeZ = _workZ->size();

    // get vector lambda of the current interaction
    *_workL = *interaction()->lambda(0);
    (_jachlambdaPtr)(sizeQ, &(*_workX)(0), sizeY, &(*_workL)(0), &(*_jachlambda)(0, 0), sizeZ, &(*_workZ)(0));
    // Copy data that might have been changed in the plug-in call.
    *data[z] = *_workZ;
  }
}

void LagrangianCompliantR::computeOutput(double time, unsigned int derivativeNumber)
{
  if (derivativeNumber == 0)
    computeh(time);
  else
  {
    SP::SiconosVector y = interaction()->y(derivativeNumber);
    computeJachq(time);
    computeJachlambda(time);
    if (derivativeNumber == 1)
    {
      SP::SiconosVector lambda = interaction()->lambda(derivativeNumber);
      // y = Jach[0] q1 + Jach[1] lambda
      prod(*_jachq, *data[q1], *y);
      prod(*_jachlambda, *lambda, *y, false);
    }
    else if (derivativeNumber == 2)
      prod(*_jachq, *data[q2], *y); // Approx: y[2] = Jach[0]q[2], other terms are neglected ...
    else
      RuntimeException::selfThrow("LagrangianCompliantR::computeOutput(time,index), index out of range or not yet implemented.");
  }
}

void LagrangianCompliantR::computeInput(const double time, const unsigned int level)
{
  computeJachq(time);
  // get lambda of the concerned interaction
  SP::SiconosVector lambda = interaction()->lambda(level);

  // data[name] += trans(G) * lambda
  prod(*lambda, *_jachq, *data[p0 + level], false);

  //   SP::SiconosMatrix  GT = new SimpleMatrix(*G[0]);
  //   GT->trans();
  //   *data[name] += prod(*GT, *lambda);
  //  gemv(CblasTrans, 1.0,*(G[0]), *lambda, 1.0, *data[name]); => not yet implemented for BlockVectors.
}

LagrangianCompliantR* LagrangianCompliantR::convert(Relation *r)
{
  LagrangianCompliantR* lnlr = dynamic_cast<LagrangianCompliantR*>(r);
  return lnlr;
}

