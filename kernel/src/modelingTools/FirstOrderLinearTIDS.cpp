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
#include "FirstOrderLinearTIDS.hpp"

#include <iostream>

// --- Constructors ---

// From a minimum set of data: A
FirstOrderLinearTIDS::FirstOrderLinearTIDS(SP::SiconosVector x0, SP::SiconosMatrix A):
  FirstOrderLinearDS(x0, A)
{
  checkDynamicalSystem();
}

// From a set of data: A and B
FirstOrderLinearTIDS::FirstOrderLinearTIDS(SP::SiconosVector x0, SP::SiconosMatrix A, SP::SiconosVector b):
  FirstOrderLinearDS(x0, A, b)
{
  checkDynamicalSystem();
}

void FirstOrderLinearTIDS::initRhs(double time)
{
  if (_M && !_invM)
    _invM.reset(new SimpleMatrix(*_M));

  computeRhs(time);

  if (! _jacxRhs)  // if not allocated with a set or anything else
  {
    if (_A && ! _M)  // if M is not defined, then A = _jacxRhs, no memory allocation for that one.
      _jacxRhs = _A;
    else if (_A && _M)
    {
      _jacxRhs.reset(new SimpleMatrix(*_A)); // Copy A into _jacxRhs
      // Solve M_jacxRhs = A
      _invM->PLUForwardBackwardInPlace(*_jacxRhs);
    }
    // else no allocation, jacobian is equal to 0.
  }
}

void FirstOrderLinearTIDS::computeRhs(double time, const bool isDSup)
{

  *_x[1] = * _r; // Warning: r update is done in Interactions/Relations

  if (_A)
    prod(*_A, *_x[0], *_x[1], false);

  // compute and add b if required
  if (_b)
    *_x[1] += *_b;

  if (_M)
    _invM->PLUForwardBackwardInPlace(*_x[1]);
}

void FirstOrderLinearTIDS::computeJacobianRhsx(double time, const bool isDSup)
{
  // Nothing to be done: _jacxRhs is constant and computed during initialize. But this function is required to avoid call to base class function.
}

void FirstOrderLinearTIDS::display() const
{
  std::cout << "===> Linear Time-invariant First Order System display, " << _number << ")." <<std::endl;
  std::cout << "- A " <<std::endl;
  if (_A) _A->display();
  else std::cout << "-> NULL" <<std::endl;
  std::cout << "- b " <<std::endl;
  if (_b) _b->display();
  else std::cout << "-> NULL" <<std::endl;

  std::cout << "- M: " <<std::endl;
  if (_M) _M->display();
  else std::cout << "-> NULL" <<std::endl;

  std::cout << "============================================" <<std::endl;
}
