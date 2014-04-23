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

// \todo : create a work vector for all tmp vectors used in computeg, computeh ...

#include "LagrangianCompliantR.hpp"
#include "Interaction.hpp"
#include "LagrangianDS.hpp"

#include "BlockVector.hpp"

using namespace RELATION;

// constructor from a set of data
LagrangianCompliantR::LagrangianCompliantR(const std::string& pluginh, const std::string& pluginJacobianhq, const std::string& pluginJacobianhlambda) : LagrangianR(CompliantR)
{
  zeroPlugin();
  setComputehFunction(SSLH::getPluginName(pluginh), SSLH::getPluginFunctionName(pluginh));
  _pluginJachq->setComputeFunction(SSLH::getPluginName(pluginJacobianhq), SSLH::getPluginFunctionName(pluginJacobianhq));
  _pluginJachlambda->setComputeFunction(SSLH::getPluginName(pluginJacobianhlambda), SSLH::getPluginFunctionName(pluginJacobianhlambda));
}

void LagrangianCompliantR::zeroPlugin()
{
  _pluginJachq.reset(new PluggedObject());
  _pluginJachlambda.reset(new PluggedObject());
}

const std::string LagrangianCompliantR::getJachlambdaName() const
{
  if (_pluginJachlambda->fPtr)
    return _pluginJachlambda->getPluginName();
  return "unamed";

}
const std::string LagrangianCompliantR::getJachqName() const
{
  if (_pluginJachq->fPtr)
    return _pluginJachq->getPluginName();
  return "unamed";
}


void LagrangianCompliantR::initComponents(Interaction& inter, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM)
{
  LagrangianR::initComponents(inter, DSlink, workV, workM);
  unsigned int sizeY = inter.getSizeOfY();

  if (! _jachlambda)
    _jachlambda.reset(new SimpleMatrix(sizeY, sizeY));
  else
    _jachlambda->resize(sizeY, sizeY);
}

void LagrangianCompliantR::computeh(double time, SiconosVector& q0, SiconosVector& lambda, SiconosVector& z, SiconosVector& y)
{
  if (_pluginh->fPtr)
  {
    // get vector y of the current interaction

    // Warning: temporary method to have contiguous values in memory, copy of block to simple.
    ((FPtr2)(_pluginh->fPtr))(q0.size(), &(q0)(0), y.size(), &(lambda)(0), &(y)(0), z.size(), &(z)(0));

  }
}

void LagrangianCompliantR::computeJachq(double time, SiconosVector& q0, SiconosVector& lambda, SiconosVector& z)
{

  if (_pluginJachq->fPtr)
  {
    // Warning: temporary method to have contiguous values in memory, copy of block to simple.
    // get vector lambda of the current interaction
    ((FPtr2)(_pluginJachq->fPtr))(q0.size(), &(q0)(0), lambda.size(), &(lambda)(0), &(*_jachq)(0, 0), z.size(), &(z)(0));
    // Copy data that might have been changed in the plug-in call.
  }
}
void LagrangianCompliantR::computeJachlambda(double time, SiconosVector& q0, SiconosVector& lambda, SiconosVector& z)
{

  if (_pluginJachlambda->fPtr)
  {
    // get vector lambda of the current interaction
    ((FPtr2)_pluginJachlambda->fPtr)(q0.size(), &(q0)(0), lambda.size(), &(lambda)(0), &(*_jachlambda)(0, 0), z.size(), &(z)(0));
    // Copy data that might have been changed in the plug-in call.
  }
}

void LagrangianCompliantR::computeOutput(double time, Interaction& inter, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM, SiconosMatrix& osnsM, unsigned int derivativeNumber)
{
  SiconosVector workZ = *DSlink[LagrangianR::z];
  if (derivativeNumber == 0)
  {
    SiconosVector& y = *inter.y(0);
    SiconosVector& lambda = *inter.lambda(0);
    SiconosVector workQ = *DSlink[LagrangianR::q0];

    computeh(time, workQ, lambda, workZ, y);
  }
  else
  {
    SiconosVector& y = *inter.y(derivativeNumber);
    SiconosVector& lambda = *inter.lambda(derivativeNumber);
    SiconosVector workQ = *DSlink[LagrangianR::q0];
    computeJachq(time, workQ, lambda, workZ);
    computeJachlambda(time, workQ, lambda, workZ);
    if (derivativeNumber == 1)
    {
      // y = Jach[0] q1 + Jach[1] lambda
      prod(*_jachq, *DSlink[LagrangianR::q1], y);
      prod(*_jachlambda, lambda, y, false);
    }
    else if (derivativeNumber == 2)
      prod(*_jachq, *DSlink[LagrangianR::q2], y); // Approx: y[2] = Jach[0]q[2], other terms are neglected ...
    else
      RuntimeException::selfThrow("LagrangianCompliantR::computeOutput, index out of range or not yet implemented.");
  }

  *DSlink[LagrangianR::z] = workZ;
}

void LagrangianCompliantR::computeInput(double time, Interaction& inter, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM, SiconosMatrix& osnsM, unsigned int level)
{
  // get lambda of the concerned interaction
  SiconosVector& lambda = *inter.lambda(level);

  SiconosVector workQ = *DSlink[LagrangianR::q0];
  SiconosVector workZ = *DSlink[LagrangianR::z];
  computeJachq(time, workQ, lambda, workZ);
  // data[name] += trans(G) * lambda
  prod(lambda, *_jachq, *DSlink[LagrangianR::p0 + level], false);
  *DSlink[LagrangianR::z] = workZ;
}
