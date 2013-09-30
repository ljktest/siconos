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

#include "OneStepIntegrator.hpp"
#include "OneStepIntegratorXML.hpp"
#include "Simulation.hpp"
#include "Model.hpp"
#include "DynamicalSystem.hpp"
#include "NonSmoothDynamicalSystem.hpp"



OneStepIntegrator::OneStepIntegrator(const OSI::TYPES& id):
  integratorType(id), _sizeMem(1)
{
  OSIDynamicalSystems.reset(new DynamicalSystemsSet());
  OSIInteractions.reset(new InteractionsSet());
}

// --- Xml constructor ---
OneStepIntegrator::OneStepIntegrator(const OSI::TYPES& id, SP::OneStepIntegratorXML osixml,
                                     SP::DynamicalSystemsSet dsList, SP::InteractionsSet interactionsList):
  integratorType(id), _sizeMem(1), integratorXml(osixml)
{
  if (!integratorXml)
    RuntimeException::selfThrow("OneStepIntegrator::xml constructor - OneStepIntegratorXML object == NULL");

  OSIDynamicalSystems.reset(new DynamicalSystemsSet());
  OSIInteractions.reset(new InteractionsSet());

  // load DS list if present
  if (osixml->hasDSList())
  {
    assert(dsList && "OneStepIntegrator xml constructor: empty ds list from NSDS.");
    if (osixml->hasAllDS()) // if flag all=true is present -> get all ds from the nsds
      OSIDynamicalSystems->insert(dsList->begin(), dsList->end());

    else
    {
      // get list of ds numbers implicate in the OSI
      std::vector<int> dsNumbers;
      osixml->getDSNumbers(dsNumbers);
      // get corresponding ds and insert them into the set.
      for (std::vector<int>::iterator it = dsNumbers.begin(); it != dsNumbers.end(); ++it)
        OSIDynamicalSystems->insert(dsList->getPtr(*it));
    }
  }

  // load interactions list if present
  if (osixml->hasInteractionsList())
  {
    assert(interactionsList && "OneStepIntegrator xml constructor: empty interaction list from NSDS.");
    if (osixml->hasAllInteractions()) // if flag all=true is present -> get all interactions
      OSIInteractions->insert(interactionsList->begin(), interactionsList->end());

    else
    {
      // get list of interactions numbers implicate in the OSI
      std::vector<int> interactionsNumbers;
      osixml->getInteractionsNumbers(interactionsNumbers);
      // get corresponding interactions and insert them into the set.
      for (std::vector<int>::iterator it = interactionsNumbers.begin(); it != interactionsNumbers.end(); ++it)
        OSIInteractions->insert(interactionsList->getPtr(*it));
    }
  }
}

void OneStepIntegrator::insertDynamicalSystem(SP::DynamicalSystem ds)
{
  OSIDynamicalSystems->insert(ds);
}

void OneStepIntegrator::setInteractions(const InteractionsSet& newSet)
{
  OSIInteractions->insert(newSet.begin(), newSet.end());
}

// SP::SiconosVector OneStepIntegrator::getWorkX(SP::DynamicalSystem ds)
// {
//   assert(workX.find(ds)!=workX.end()&&"OneStepIntegrator::getWorkX(ds): this vector does not exists for ds.");
//   return workX[ds];
// }

void OneStepIntegrator::initialize()
{
}

void OneStepIntegrator::saveInMemory()
{
  for_each(OSIDynamicalSystems->begin(), OSIDynamicalSystems->end(), std11::bind(&DynamicalSystem::swapInMemory, _1));
}

double OneStepIntegrator::computeResidu()
{
  RuntimeException::selfThrow("OneStepIntegrator::computeResidu not implemented for integrator of type " + integratorType);
  return 0.0;
}

void OneStepIntegrator::computeFreeState()
{
  RuntimeException::selfThrow("OneStepIntegrator::computeFreeState not implemented for integrator of type " + integratorType);
}
void OneStepIntegrator::computeFreeOutput(SP::Interaction inter, OneStepNSProblem * osnsp)
{
  RuntimeException::selfThrow("OneStepIntegrator::computeFreeOutput not implemented for integrator of type " + integratorType);
}
void OneStepIntegrator::resetNonSmoothPart()
{
  for_each(OSIDynamicalSystems->begin(), OSIDynamicalSystems->end(), std11::bind(&DynamicalSystem::resetAllNonSmoothPart, _1));
  for_each(OSIInteractions->begin(), OSIInteractions->end(), std11::bind(&Interaction::resetAllLambda, _1));
}
void OneStepIntegrator::resetNonSmoothPart(unsigned int level)
{
  for_each(OSIDynamicalSystems->begin(), OSIDynamicalSystems->end(), std11::bind(&DynamicalSystem::resetNonSmoothPart, _1, level));
  for_each(OSIInteractions->begin(), OSIInteractions->end(), std11::bind(&Interaction::resetLambda, _1, level));

}

void OneStepIntegrator::display()
{
  std::cout << "==== OneStepIntegrator display =====" <<std::endl;
  std::cout << "| integratorType : " << integratorType <<std::endl;
  std::cout << "| _sizeMem: " << _sizeMem <<std::endl;
  std::cout << "====================================" <<std::endl;
}

void OneStepIntegrator::saveIntegratorToXML()
{
  //   if(integratorXml != 0)
  //     {
  //       vector<int> dsConcerned;
  //       dsConcerned.push_back(ds->getNumber());
  //       integratorXml->setDSConcerned( &dsConcerned );
  //     }
  //   else
  //RuntimeException::selfThrow("OneStepIntegrator::saveIntegratorToXML - OneStepIntegratorXML object = NULL");
  RuntimeException::selfThrow("OneStepIntegrator::saveIntegratorToXML - Not yet implemented.");
}

