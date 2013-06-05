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
#include "NewtonImpactFrictionNSL.hpp"
#include "NewtonImpactFrictionNSLXML.hpp"


// Default (private)
NewtonImpactFrictionNSL::NewtonImpactFrictionNSL():
  NonSmoothLaw(), _en(0.0), _et(0.0), _mu(0.0)
{}

NewtonImpactFrictionNSL::NewtonImpactFrictionNSL(unsigned int newSize):
  NonSmoothLaw(newSize), _en(0.0), _et(0.0), _mu(0.0)
{}

NewtonImpactFrictionNSL::NewtonImpactFrictionNSL(SP::NonSmoothLawXML nslawxml):
  NonSmoothLaw(nslawxml), _en(0.0), _et(0.0), _mu(0.0)
{
  assert((nslawxml->hasSize()) && // size is a required input for Friction
         "NewtonImpactFrictionNSL:: xml constructor, size is a required xml input.");

  assert((size() == 2 || size() == 3) &&
         "NewtonImpactFrictionNSL:: xml constructor, wrong size value = " + size());

  _en = (std11::static_pointer_cast<NewtonImpactFrictionNSLXML>(nslawxml))->getEn();
  if ((std11::static_pointer_cast<NewtonImpactFrictionNSLXML>(nslawxml))->hasEt())
    _et = (std11::static_pointer_cast<NewtonImpactFrictionNSLXML>(nslawxml))->getEt();
  _mu = (std11::static_pointer_cast<NewtonImpactFrictionNSLXML>(nslawxml))->getMu();
}

NewtonImpactFrictionNSL::NewtonImpactFrictionNSL(double newEn, double newEt, double newMu, unsigned int newSize):
  NonSmoothLaw(newSize), _en(newEn), _et(newEt), _mu(newMu)
{}

NewtonImpactFrictionNSL::~NewtonImpactFrictionNSL()
{}

bool NewtonImpactFrictionNSL::isVerified() const
{
  bool res = false;
  // to do
  RuntimeException::selfThrow("NewtonImpactFrictionNSL:: isVerified, not yet implemented!");
  return res;
}

void NewtonImpactFrictionNSL::display() const
{
  std::cout << "=== Newton impact-friction non-smooth law data display ===" <<std::endl;
  std::cout << " Normal Newton coefficient of restitution: " << _en <<std::endl;
  std::cout << " Tangential Newton coefficient of restitution: " << _et <<std::endl;
  std::cout << "Friction coefficient: " << _mu <<std::endl;
  std::cout << "==========================================================" <<std::endl;
}

void NewtonImpactFrictionNSL::saveNonSmoothLawToXML()
{
  std11::static_pointer_cast<NewtonImpactFrictionNSLXML>(_nslawxml)->setEn(_en);
  std11::static_pointer_cast<NewtonImpactFrictionNSLXML>(_nslawxml)->setEt(_et);
  std11::static_pointer_cast<NewtonImpactFrictionNSLXML>(_nslawxml)->setMu(_mu);
}

NewtonImpactFrictionNSL* NewtonImpactFrictionNSL::convert(NonSmoothLaw* nsl)
{
  NewtonImpactFrictionNSL* nilnsl = dynamic_cast<NewtonImpactFrictionNSL*>(nsl);
  return nilnsl;
}


