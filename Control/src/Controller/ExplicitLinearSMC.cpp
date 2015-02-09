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


#include "ActuatorFactory.hpp"
#include "SiconosVector.hpp"
#include "FirstOrderLinearTIDS.hpp"
#include "ControlSensor.hpp"

#include "ExplicitLinearSMC.hpp"


ExplicitLinearSMC::ExplicitLinearSMC(SP::ControlSensor sensor): CommonSMC(EXPLICIT_LINEAR_SMC, sensor)
{
}

ExplicitLinearSMC::ExplicitLinearSMC(SP::ControlSensor sensor, SP::SimpleMatrix B): CommonSMC(EXPLICIT_LINEAR_SMC, sensor, B)
{
}


ExplicitLinearSMC::~ExplicitLinearSMC()
{
  _sigma.reset();
}

void ExplicitLinearSMC::initialize(const Model& m)
{
  CommonSMC::initialize(m);

  _sigma.reset(new SiconosVector(_u->size()));
}

void ExplicitLinearSMC::actuate()
{
  if (!_noUeq)
  {
    computeUeq();
  }

  prod(*_Csurface, _sensor->y(), *_sigma);

  unsigned int sDim = _u->size();

  if (_D) // we are using a saturation
  {
    for (unsigned int i = 0; i < sDim; i++)
    {
      if ((*_sigma)(i) > (*_D)(i, i))
        (*_us)(i) = -_alpha;
      else if ((*_sigma)(i) < -(*_D)(i, i))
        (*_us)(i) = _alpha;
      else
      {
        if ((*_D)(i, i) != 0)
          (*_us)(i) = -(*_sigma)(i) / (*_D)(i, i);
        else
          (*_us)(i) = 0;
      }
    }
  }
  else
  {
    for (unsigned int i = 0; i < sDim; i++)
    {
      if ((*_sigma)(i) > 0)
        (*_us)(i) = -_alpha;
      else if ((*_sigma)(i) < 0)
        (*_us)(i) = _alpha;
      else
        (*_us)(i) = 0;
    }
  }

  *_u = *_us;
  *_u += *_ueq;
}

AUTO_REGISTER_ACTUATOR(EXPLICIT_LINEAR_SMC, ExplicitLinearSMC)
