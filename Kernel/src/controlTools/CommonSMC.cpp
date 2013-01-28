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

#include "CommonSMC.hpp"
#include "ModelingTools.hpp"
#include "SimulationTools.hpp"


void CommonSMC::initialize(SP::Model m)
{
  if (_Csurface == NULL)
  {
    RuntimeException::selfThrow("CommonSMC::initialize - you have to set _Csurface before initializing the Actuator");
  }
  else
  {
    Actuator::initialize(m);
  }
}

void CommonSMC::setCsurface(const SiconosMatrix& newValue)
{
  // check dimensions ...
  if (newValue.size(1) != _nDim)
  {
    RuntimeException::selfThrow("CommonSMC::setCsurface - inconstency between the dimension of the state space and Csurface");
  }
  else
  {
    if (_Csurface)
    {
      *_Csurface = newValue;
    }
    else
    {
      _Csurface.reset(new SimpleMatrix(newValue));
    }
    _sDim = newValue.size(0);
  }
}

void CommonSMC::setCsurfacePtr(SP::SiconosMatrix newPtr)
{
  // check dimensions ...
  if (newPtr->size(1) != _nDim)
  {
    RuntimeException::selfThrow("CommonSMC::setCsurfacePtr - inconstency between the dimension of the state space and Csurface");
  }
  else
  {
    _Csurface = newPtr;
    _sDim = newPtr->size(0);
  }
}

void CommonSMC::setSaturationMatrix(const SiconosMatrix& newValue)
{
  // check dimensions ...
  if (newValue.size(1) != _nDim)
  {
    RuntimeException::selfThrow("CommonSMC::setSaturationMatrix - inconstency between the dimension of the state space and D");
  }
  else
  {
    if (_D)
    {
      *_D = newValue;
    }
    else
    {
      _D.reset(new SimpleMatrix(newValue));
    }
  }
}

void CommonSMC::setSaturationMatrixPtr(SP::SiconosMatrix newPtr)
{
  // check dimensions ...
  if (newPtr->size(1) != _nDim)
  {
    RuntimeException::selfThrow("CommonSMC::setSaturationMatrixPtr - inconstency between the dimension of the state space and D");
  }
  else
  {
    _D = newPtr;
  }
}

void CommonSMC::setB(const SiconosMatrix& B)
{
    _B.reset(new SimpleMatrix(B));
}
