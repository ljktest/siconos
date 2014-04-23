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

#include "LagrangianScleronomousR.hpp"
#include "Interaction.hpp"
#include "LagrangianDS.hpp"

#include "BlockVector.hpp"
//#define DEBUG_MESSAGES
//#define DEBUG_STDOUT
#include "debug.h"




using namespace RELATION;

// constructor from a set of data
LagrangianScleronomousR::LagrangianScleronomousR(const std::string& pluginh, const std::string& pluginJacobianhq):
  LagrangianR(ScleronomousR)
{
  zeroPlugin();
  setComputehFunction(SSLH::getPluginName(pluginh), SSLH::getPluginFunctionName(pluginh));

  _pluginJachq->setComputeFunction(pluginJacobianhq);

 // Warning: we cannot allocate memory for Jach[0] matrix since no interaction
  // is connected to the relation. This will be done during initialize.
  // We only set the name of the plugin-function and connect it to the user-defined function.
}
// constructor from a data used for EventDriven scheme
LagrangianScleronomousR::LagrangianScleronomousR(const std::string& pluginh, const std::string& pluginJacobianhq, const std::string& pluginDotJacobianhq):
  LagrangianR(ScleronomousR)
{
  zeroPlugin();
  setComputehFunction(SSLH::getPluginName(pluginh), SSLH::getPluginFunctionName(pluginh));

  _pluginJachq->setComputeFunction(pluginJacobianhq);

  _plugindotjacqh->setComputeFunction(pluginDotJacobianhq);
}



void LagrangianScleronomousR::zeroPlugin()
{
  LagrangianR::zeroPlugin();
  _pluginJachq.reset(new PluggedObject());
  _plugindotjacqh.reset(new PluggedObject());
}

void LagrangianScleronomousR::initComponents(Interaction& inter, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM)
{
  if (_plugindotjacqh && _plugindotjacqh->fPtr)
  {
    if (!_dotjachq)
    {
      unsigned int sizeY = inter.getSizeOfY();
      unsigned int sizeDS = inter.getSizeOfDS();
      _dotjachq.reset(new SimpleMatrix(sizeY, sizeDS));
    }
  }
}

void LagrangianScleronomousR::computeh(SiconosVector& q, SiconosVector& z, SiconosVector& y)
{
  DEBUG_PRINT(" LagrangianScleronomousR::computeh(Interaction& inter, SP::BlockVector q, SP::BlockVector z)\n");

  if (_pluginh && _pluginh->fPtr)
  {
    ((FPtr3)(_pluginh->fPtr))(q.size(), &(q(0)) , y.size(), &(y(0)), z.size(), &(z(0)));

    DEBUG_EXPR(q.display());
    DEBUG_EXPR(z.display());
    DEBUG_EXPR(y.display());

  }
  // else nothing
}
void LagrangianScleronomousR::computeJachq(SiconosVector& q, SiconosVector& z)
{
  if (_pluginJachq)
  {
    if (_pluginJachq->fPtr)
    {
      // get vector lambda of the current interaction
      ((FPtr3)(_pluginJachq->fPtr))(q.size(), &(q)(0), _jachq->size(0), &(*_jachq)(0, 0), z.size(), &(z)(0));
   }
  }
}

void LagrangianScleronomousR::computeDotJachq(SiconosVector& q, SiconosVector& z, SiconosVector& qDot)
{
  if (_plugindotjacqh)
  {
    if (_plugindotjacqh->fPtr)
    {
      // get vector _jachqDo of the current interaction
      // else
      // {
      //   if (_dotjachq->size(0) == 0) // if the matrix dimension are null
      //     _dotjachq->resize(sizeY, sizeDS);
      //   else
      //   {
      //     if ((_dotjachq->size(1) != sizeDS && _dotjachq->size(0) != sizeY))
      //       RuntimeException::selfThrow("LagrangianR::initComponents inconsistent sizes between Jach[1] matrix and the interaction.");
      //   }
      // }

      ((FPtr2)(_plugindotjacqh->fPtr))(q.size(), &(q)(0), qDot.size(), &(qDot)(0), &(*_dotjachq)(0, 0), z.size(), &(z)(0));
    }
  }
}

void  LagrangianScleronomousR::computedotjacqhXqdot(double time, Interaction& inter, VectorOfBlockVectors& DSlink)
{
  DEBUG_PRINT("LagrangianScleronomousR::computeNonLinearH2dot starts");
  // Compute the H Jacobian dot
  SiconosVector q = *DSlink[LagrangianR::q0];
  SiconosVector z = *DSlink[LagrangianR::z];
  SiconosVector qDot = *DSlink[LagrangianR::q1];
  LagrangianScleronomousR::computeDotJachq(q, z, qDot);
  _dotjacqhXqdot.reset(new SiconosVector(_dotjachq->size(0)));
  DEBUG_EXPR(qDot.display(););
  DEBUG_EXPR(_dotjachq->display(););
  prod(*_dotjachq, qDot, *_dotjacqhXqdot);
  DEBUG_PRINT("LagrangianScleronomousR::computeNonLinearH2dot ends");
  *DSlink[LagrangianR::z] = z;
}

void LagrangianScleronomousR::computeOutput(double time, Interaction& inter, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM, SiconosMatrix& osnsM, unsigned int derivativeNumber)
{

  DEBUG_PRINTF("LagrangianScleronomousR::computeOutput(double time, Interaction& inter, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM, SiconosMatrix& osnsM, unsigned int derivativeNumber) with time = %f and derivativeNumber = %i\n", time, derivativeNumber);
  SiconosVector& y = *inter.y(derivativeNumber);
  SiconosVector q = *DSlink[LagrangianR::q0];
  SiconosVector z = *DSlink[LagrangianR::z];
  if (derivativeNumber == 0)
  { 
    computeh(q, z, y);
  }
  else
  {
   computeJachq(q, z);

    if (derivativeNumber == 1)
      prod(*_jachq, *DSlink[LagrangianR::q1], y);
    else if (derivativeNumber == 2)
    {
      SiconosVector qDot = *DSlink[LagrangianR::q1];
      computeDotJachq(q, z, qDot);
      prod(*_jachq, *DSlink[LagrangianR::q2], y);
      prod(*_dotjachq, *DSlink[LagrangianR::q1], y, false);
    }
    else
      RuntimeException::selfThrow("LagrangianScleronomousR::computeOutput(double time, Interaction& inter, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM, SiconosMatrix& osnsM, unsigned int derivativeNumber), index out of range");
  }
  *DSlink[LagrangianR::z] = z;
}

void LagrangianScleronomousR::computeInput(double time, Interaction& inter, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM, SiconosMatrix& osnsM, unsigned int level)
{
  DEBUG_PRINT("void LagrangianScleronomousR::computeInput(double time, Interaction& inter, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM, SiconosMatrix& osnsM, unsigned int level) \n");

  DEBUG_PRINTF("level = %i\n", level);

  SiconosVector q = *DSlink[LagrangianR::q0];
  SiconosVector z = *DSlink[LagrangianR::z];
  computeJachq(q, z);
  // get lambda of the concerned interaction
  SiconosVector& lambda = *inter.lambda(level);
  // data[name] += trans(G) * lambda
  prod(lambda, *_jachq, *DSlink[LagrangianR::p0 + level], false);
  DEBUG_EXPR(DSlink[LagrangianR::p0 + level]->display(););
  *DSlink[LagrangianR::z] = z;
}

void LagrangianScleronomousR::computeJach(double time, Interaction& inter, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM)
{
    SiconosVector q = *DSlink[LagrangianR::q0];
    SiconosVector z = *DSlink[LagrangianR::z];
    SiconosVector qDot = *DSlink[LagrangianR::q1];
    computeJachq(q, z);
    // computeJachqDot(time, inter);
    computeDotJachq(q, z, qDot);
    // computeJachlambda(time, inter);
    // computehDot(time,inter);
    *DSlink[LagrangianR::z] = z;
}

const std::string LagrangianScleronomousR::getJachqName() const
{
  if (_pluginJachq->fPtr)
    return _pluginJachq->getPluginName();
  return "unamed";

}
