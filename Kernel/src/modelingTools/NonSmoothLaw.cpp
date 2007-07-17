/* Siconos-Kernel version 2.1.1, Copyright INRIA 2005-2007.
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

#include "NonSmoothLaw.h"
#include "NonSmoothLawXML.h"
using namespace std;

// Constructors
// warning -> this is an abstract class, so constructors are usefull only for
// calls in derived classes constructors

NonSmoothLaw::NonSmoothLaw(const string& newType, unsigned int newSize): nsLawType(newType), size(newSize), nslawxml(NULL)
{}

NonSmoothLaw::NonSmoothLaw(const string& newType, NonSmoothLawXML* newNsLawXml):
  nsLawType(newType), size(1), nslawxml(newNsLawXml)
{
  if (nslawxml == NULL)
    RuntimeException::selfThrow("NonSmoothLaw:: xml constructor, xml file==NULL");

  // Read size of the non smooth law
  if (! nslawxml->hasSize())
    RuntimeException::selfThrow("NonSmoothLaw:: xml constructor, an input is required for the nslaw size.");

  size = nslawxml->getSize();
}

NonSmoothLaw::~NonSmoothLaw()
{}
