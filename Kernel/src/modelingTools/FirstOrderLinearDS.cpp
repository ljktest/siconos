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
#include "FirstOrderLinearDS.hpp"
//#include "Plugin.hpp"

typedef void (*computeAfct)(double, unsigned int, unsigned int, double*, unsigned int, double*);

// --- Constructors ---

// From a minimum set of data, A and b connected to a plug-in
FirstOrderLinearDS::FirstOrderLinearDS(SP::SiconosVector newX0, const std::string& APlugin, const std::string& bPlugin):
  FirstOrderNonLinearDS(newX0)
{

  _pluginb.reset(new PluggedObject());
  _pluginA.reset(new PluggedObject());
  _pluginA->setComputeFunction(APlugin);
  _pluginb->setComputeFunction(bPlugin);

  _f.reset(new SiconosVector(getDim()));
  _A.reset(new SimpleMatrix(getDim(), getDim()));
  _b.reset(new SiconosVector(getDim()));

  checkDynamicalSystem();
}

// From a minimum set of data, A from a given matrix
FirstOrderLinearDS::FirstOrderLinearDS(SP::SiconosVector newX0, SP::SiconosMatrix newA):
  FirstOrderNonLinearDS(newX0)
{
  _f.reset(new SiconosVector(getDim()));
  _pluginb.reset(new PluggedObject());
  _pluginA.reset(new PluggedObject());
  if ((newA->size(0) != _n) || (newA->size(1) != _n))
    RuntimeException::selfThrow("FirstOrderLinearDS - constructor(number,x0,A): inconsistent dimensions with problem size for input matrix A");

  _A = newA;
  checkDynamicalSystem();
}

FirstOrderLinearDS::FirstOrderLinearDS(SP::SiconosVector newX0):
  FirstOrderNonLinearDS(newX0)
{
  _f.reset(new SiconosVector(getDim()));
  _pluginb.reset(new PluggedObject());
  _pluginA.reset(new PluggedObject());
  checkDynamicalSystem();
}
// From a minimum set of data, A from a given matrix
FirstOrderLinearDS::FirstOrderLinearDS(SP::SiconosVector newX0, SP::SiconosMatrix newA, SP::SiconosVector newB):
  FirstOrderNonLinearDS(newX0)
{
  _pluginb.reset(new PluggedObject());
  _pluginA.reset(new PluggedObject());
  if ((newA->size(0) != _n) || (newA->size(1) != _n))
    RuntimeException::selfThrow("FirstOrderLinearDS - constructor(x0,A,b): inconsistent dimensions with problem size for input matrix A");

  if (newB->size() != _n)
    RuntimeException::selfThrow("FirstOrderLinearDS - constructor(x0,A,b): inconsistent dimensions with problem size for input vector b ");

  _A = newA;
  _b = newB;
  _f.reset(new SiconosVector(getDim()));

  checkDynamicalSystem();
}

// Copy constructor
FirstOrderLinearDS::FirstOrderLinearDS(const FirstOrderLinearDS & FOLDS): FirstOrderNonLinearDS(FOLDS)
{
  _A.reset(new SimpleMatrix(*(FOLDS.A())));

  if (_b)
    _b.reset(new SiconosVector(*(FOLDS.b())));

  if (Type::value(FOLDS) == Type::FirstOrderLinearDS)
  {
    _pluginA.reset(new PluggedObject(*(FOLDS.getPluginA())));
    _pluginb.reset(new PluggedObject(*(FOLDS.getPluginB())));
  }
}

bool FirstOrderLinearDS::checkDynamicalSystem() // useless ...?
{
  bool output = DynamicalSystem::checkDynamicalSystem();
  if (!output)  std::cout << "FirstOrderLinearDS Warning: your dynamical system seems to be uncomplete (check = false)" << std::endl;
  return output;
}

void FirstOrderLinearDS::initRhs(double time)
{
  computeRhs(time); // If necessary, this will also compute A and b.
  if (! _jacxRhs)  // if not allocated with a set or anything else
  {
    if (_A && ! _M)  // if M is not defined, then A = jacobianRhsx, no memory allocation for that one.
      _jacxRhs = _A;
    else if (_A && _M)
      _jacxRhs.reset(new SimpleMatrix(_n, _n));
    // else no allocation, jacobian is equal to 0.
  }
  computeJacobianRhsx(time);
}

void FirstOrderLinearDS::updatePlugins(double time)
{
  computeA(time);
  if (_b)
    computeb(time);
}

void FirstOrderLinearDS::setComputeAFunction(const std::string& pluginPath, const std::string& functionName)
{
  _pluginA->setComputeFunction(pluginPath, functionName);
  //   Plugin::setFunction(&_APtr, pluginPath, functionName);
  //   SSLH::buildPluginName(pluginNameAPtr,pluginPath,functionName);
}

void FirstOrderLinearDS::setComputeAFunction(LDSPtrFunction fct)
{
  _pluginA->setComputeFunction((void*)fct);
  //  _APtr=fct;
}
void FirstOrderLinearDS::setComputebFunction(const std::string& pluginPath, const std::string& functionName)
{
  //  Plugin::setFunction(&_bPtr, pluginPath, functionName);
  _pluginb->setComputeFunction(pluginPath, functionName);
  if (!_b)
    _b.reset(new SiconosVector(getDim()));
  //  SSLH::buildPluginName(pluginNamebPtr,pluginPath,functionName);
}

void FirstOrderLinearDS::setComputebFunction(LDSPtrFunction fct)
{
  _pluginb->setComputeFunction((void*)fct);
  //  _bPtr = fct;
}


void FirstOrderLinearDS::computeA(double time)
{
  if (_A && _pluginA->fPtr)
  {
    ((computeAfct)_pluginA->fPtr)(time, _n, _n, &(*_A)(0, 0), _z->size(), &(*_z)(0));
  }
}

void FirstOrderLinearDS::computeb(double time)
{
  if (_b && _pluginb->fPtr)
    ((LDSPtrFunction)_pluginb->fPtr)(time, _n, &(*_b)(0), _z->size(), &(*_z)(0));
}
/*This function is called only by LsodarOSI and eventDriven*/
void FirstOrderLinearDS::computeRhs(double time, bool isDSup)
{
  // second argument is useless at the time - Used in derived classes
  // compute A=jacobianfx

  *_x[1] = * _r;

  if (_A)
  {
    computeA(time);
    prod(*_A, *_x[0], *_x[1], false);
  }

  // compute and add b if required
  if (_b)
  {
    computeb(time);
    *_x[1] += *_b;
  }

  if (_M)
  {
    // allocate invM at the first call of the present function
    if (! _invM)
      _invM.reset(new SimpleMatrix(*_M));

    _invM->PLUForwardBackwardInPlace(*_x[1]);
  }
}

void FirstOrderLinearDS::computeJacobianRhsx(double time, bool isDSup)
{
  computeA(time);

  if (_M && _A)
  {
    *_jacxRhs = *_A;
    // copy M into invM for LU-factorisation, at the first call of this function.
    if (! _invM)
      _invM.reset(new SimpleMatrix(*_M));
    // solve MjacobianRhsx = A
    _invM->PLUForwardBackwardInPlace(*_jacxRhs);
  }
  // else jacobianRhsx = A, pointers equality.

}

void FirstOrderLinearDS::display() const
{
  std::cout << "=== Linear system display, " << _number << std::endl;
  std::cout << "=============================" << std::endl;
}

void FirstOrderLinearDS::computef(double time)
{
  updatePlugins(time);
  prod(*_A, *_x[0], *_f);
  if (_b)
  {
    computeb(time);
    *_f += *_b;
  }
}

void FirstOrderLinearDS::computef(double time, SP::SiconosVector x2)
{
  //  RuntimeException::selfThrow("FirstOrderLinearDS::computeF - Must not be used");
  updatePlugins(time);
  prod(*_A, *x2, *_f);
  if (_b)
  {
    *_f += *_b;
  }
}

void FirstOrderLinearDS::setA(const SiconosMatrix& newA)
{
  if (_A)
    *_A = newA;
  else
    _A.reset(new SimpleMatrix(newA));
}

void FirstOrderLinearDS::zeroPlugin()
{
  if (_pluginM)
    _pluginM.reset(new PluggedObject());
  if (_pluginA)
    _pluginA.reset(new PluggedObject());
  if (_pluginb)
    _pluginb.reset(new PluggedObject());
}
