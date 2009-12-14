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

// \todo : create a work vector for all tmp vectors used in computeG, computeh ...

#include "LagrangianRheonomousR.hpp"
#include "RelationXML.hpp"
#include "Interaction.hpp"
#include "LagrangianDS.hpp"

using namespace std;
using namespace RELATION;

// xml constructor
LagrangianRheonomousR::LagrangianRheonomousR(SP::RelationXML LRxml): LagrangianR(LRxml, RheonomousR)
{
  // h plug-in
  if (!LRxml->hasH())
    RuntimeException::selfThrow("LagrangianRheonomousR:: xml constructor failed, can not find a definition for h.");
  hName = LRxml->getHPlugin();
  setComputeHFunction(SSL::getPluginName(hName), SSL::getPluginFunctionName(hName));

  // Read hDot
  if (!LRxml->hasHDot())
    RuntimeException::selfThrow("LagrangianRheonomousR:: xml constructor failed, can not find a definition for hDot.");
  if (LRxml->isHDotPlugin())
  {
    //    hDot.reset(new PVT2(LRxml->getHDotPlugin()));
    _pluginhDot->setComputeFunction(SSL::getPluginName(LRxml->getHDotPlugin()), SSL::getPluginFunctionName(LRxml->getHDotPlugin()));
  }
  else
    _hDot.reset(new SimpleVector(LRxml->getHDotVector()));

  if (!LRxml->hasJacobianH())
    RuntimeException::selfThrow("LagrangianRheonomousR:: xml constructor failed, can not find a definition for G0.");
  if (LRxml->isJacobianHPlugin(0))
    _pluginJacQH->setComputeFunction(LRxml->getJacobianHPlugin(0));
  else
    JacQH.reset(new SimpleMatrix(LRxml->getJacobianHMatrix(0)));
}

// constructor from a set of data
LagrangianRheonomousR::LagrangianRheonomousR(const string& computeh, const string& computehDot, const string& strcomputeJacqh):
  LagrangianR(RheonomousR)
{
  // h
  setComputeHFunction(SSL::getPluginName(computeh), SSL::getPluginFunctionName(computeh));

  // hDot
  setComputeHDotFunction(SSL::getPluginName(computehDot), SSL::getPluginFunctionName(computehDot));
  _pluginJacQH.reset(new PluggedObject());
  _pluginJacQH->setComputeFunction(strcomputeJacqh);

  unsigned int sizeY = interaction()->getSizeOfY();
  unsigned int sizeQ = _workX->size();
  JacQH.reset(new SimpleMatrix(sizeY, sizeQ));
}

void LagrangianRheonomousR::initComponents()
{
  LagrangianR::initComponents();

  unsigned int sizeY = interaction()->getSizeOfY();
  // hDot
  if (!_hDot)
    _hDot.reset(new SimpleVector(sizeY));
  else
    _hDot->resize(sizeY);
}
// void LagrangianRheonomousR::setComputeHFunction(const string& pluginPath, const string& functionName){
//   Plugin::setFunction(&hPtr, pluginPath, functionName);
// }

void LagrangianRheonomousR::setComputeHDotFunction(const string& pluginPath, const string& functionName)
{
  _pluginhDot->setComputeFunction(pluginPath, functionName);
}

void LagrangianRheonomousR::computeh(double time)
{
  if (_pluginh->fPtr)
  {
    // get vector y of the current interaction
    SP::SiconosVector y = interaction()->y(0);

    // Warning: temporary method to have contiguous values in
    // memory, copy of block to simple.
    *_workX = *data[q0];
    *_workZ = *data[z];
    *_workY = *y;

    unsigned int sizeQ = _workX->size();
    unsigned int sizeY = y->size();
    unsigned int sizeZ = _workZ->size();

    ((FPtr4)(_pluginh->fPtr))(sizeQ, &(*_workX)(0), time, sizeY,  &(*_workY)(0), sizeZ, &(*_workZ)(0));

    // Copy data that might have been changed in the plug-in call.
    *data[z] = *_workZ;
    *y = *_workY;
  }
  // else nothing
}
void LagrangianRheonomousR::computeG(double time)
{
  assert(false && "LagrangianScleronomousR::computeG : G is computed in computeInput!\n");
}

void LagrangianRheonomousR::computehDot(double time)
{
  if (_pluginhDot->fPtr)
  {
    // Warning: temporary method to have contiguous values in
    // memory, copy of block to simple.
    *_workX = *data[q0];
    *_workZ = *data[z];

    unsigned int sizeQ = _workX->size();
    unsigned int sizeY = _hDot->size();
    unsigned int sizeZ = _workZ->size();

    ((FPtr4)(_pluginhDot->fPtr))(sizeQ, &(*_workX)(0), time, sizeY,  &(*_hDot)(0), sizeZ, &(*_workZ)(0));

    // Copy data that might have been changed in the plug-in call.
    *data[z] = *_workZ;
  }
  // else nothing
}

void LagrangianRheonomousR::computeJacqh(double time)
{
  // Note that second input arg is useless.
  if (_pluginJacQH->fPtr)
  {
    // Warning: temporary method to have contiguous values in
    // memory, copy of block to simple.
    *_workX = *data[q0];
    *_workZ = *data[z];

    unsigned int sizeY = JacQH->size(0);
    unsigned int sizeQ = _workX->size();
    unsigned int sizeZ = _workZ->size();
    ((FPtr4)(_pluginJacQH->fPtr))(sizeQ, &(*_workX)(0), time, sizeY, &(*JacQH)(0, 0), sizeZ, &(*_workZ)(0));
    // Copy data that might have been changed in the plug-in call.
    *data[z] = *_workZ;
  }
  // else nothing.
}

void LagrangianRheonomousR::computeOutput(double time, unsigned int derivativeNumber)
{
  if (derivativeNumber == 0)
    computeh(time);
  else
  {
    SP::SiconosVector y = interaction()->y(derivativeNumber);
    computeJacqh(time);
    if (derivativeNumber == 1)
    {
      computehDot(time); // \todo: save hDot directly into y[1] ?
      prod(*JacQH, *data[q1], *y);
      *y += *_hDot;
    }
    else if (derivativeNumber == 2)
      prod(*JacQH, *data[q2], *y); // Approx:,  ...
    // \warning :  the computation of y[2] (in event-driven simulation for instance) is approximated by  y[2] = JacH[0]q[2]. For the moment, other terms are neglected (especially, partial derivatives with respect to time).
    else
      RuntimeException::selfThrow("LagrangianRheonomousR::computeOutput(time,index), index out of range or not yet implemented.");
  }
}

void LagrangianRheonomousR::computeInput(double time, unsigned int level)
{
  computeJacqh(time);
  // get lambda of the concerned interaction
  SP::SiconosVector lambda = interaction()->lambda(level);
  // data[name] += trans(G) * lambda
  prod(*lambda, *JacQH, *data[p0 + level], false);

  //   SP::SiconosMatrix  GT = new SimpleMatrix(*G[0]);
  //   GT->trans();
  //   *data[name] += prod(*GT, *lambda);
  //  gemv(CblasTrans, 1.0,*(G[0]), *lambda, 1.0, *data[name]);
}

LagrangianRheonomousR* LagrangianRheonomousR::convert(Relation *r)
{
  return dynamic_cast<LagrangianRheonomousR*>(r);
}

