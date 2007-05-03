/* Siconos-Kernel version 2.0.1, Copyright INRIA 2005-2006.
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
#include "NewtonImpactNSL.h"
#include "NewtonImpactNSLXML.h"
using namespace std;

NewtonImpactNSL::NewtonImpactNSL(): NonSmoothLaw(NEWTONIMPACTNSLAW, 1), e(0.0)
{}

NewtonImpactNSL::NewtonImpactNSL(NonSmoothLawXML* nslawxml):
  NonSmoothLaw(NEWTONIMPACTNSLAW, nslawxml), e(0.0)
{
  if (nslawxml != NULL)
    e = (static_cast<NewtonImpactNSLXML*>(nslawxml))->getE();
  else RuntimeException::selfThrow("NewtonImpactNSL:: xml constructor, xml file=NULL");
}

NewtonImpactNSL::NewtonImpactNSL(double newE):
  NonSmoothLaw(NEWTONIMPACTNSLAW, 1), e(newE)
{}

NewtonImpactNSL::~NewtonImpactNSL()
{}

bool NewtonImpactNSL::isVerified() const
{
  bool res = false;
  // to do
  RuntimeException::selfThrow("NewtonImpactFrictionNSL:: isVerified, not yet implemented!");
  return res;
}

void NewtonImpactNSL::display() const
{
  cout << "===============================================================================" << endl;
  cout << "=== Newton impact (frictionless) non-smooth law coefficient of restitution: " << e << endl;
  cout << "===============================================================================" << endl;
}

void NewtonImpactNSL::saveNonSmoothLawToXML()
{
  static_cast<NewtonImpactNSLXML*>(nslawxml)->setE(e);
}

NewtonImpactNSL* NewtonImpactNSL::convert(NonSmoothLaw* nsl)
{
  NewtonImpactNSL* nilnsl = dynamic_cast<NewtonImpactNSL*>(nsl);
  return nilnsl;
}


