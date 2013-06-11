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
#include "NonSmoothDynamicalSystem.hpp"
#include "NonSmoothDynamicalSystemXML.hpp"
#include "DynamicalSystemXML.hpp"
#include "Topology.hpp"
#include "Interaction.hpp"
#include "LagrangianLinearTIDS.hpp"
#include "FirstOrderLinearTIDS.hpp"


using namespace RELATION;

// --- CONSTRUCTORS/DESTRUCTOR ---

// xml constuctor
NonSmoothDynamicalSystem::NonSmoothDynamicalSystem(SP::NonSmoothDynamicalSystemXML newNsdsxml):
  _BVP(false), _nsdsxml(newNsdsxml), _mIsLinear(true)
{
  assert(_nsdsxml && "NonSmoothDynamicalSystem:: xml constructor, xml file=NULL");

  // === DS Vector fill-in: we sweep the list of DSXML and for each of
  // them, add a new DynamicalSystem in the set allDS. ===
  SetOfDSXML dsList = _nsdsxml->getDynamicalSystemsXML();
  SetOfDSXML::iterator it;
  Type::Siconos type;

  /** contains all the Dynamic Systems of the simulation */
  SP::DynamicalSystemsSet allDSLocal;

  allDSLocal.reset(new DynamicalSystemsSet());
  for (it = dsList.begin(); it != dsList.end(); ++it)
  {
    type = (*it)->getType();
    if (type  == Type::LagrangianDS)  // LagrangianDS
      allDSLocal->insert
      (SP::LagrangianDS(new LagrangianDS(*it)));
    else if (type == Type::LagrangianLinearTIDS)  // Lagrangian Linear Time Invariant
      allDSLocal->insert
      (SP::LagrangianLinearTIDS(new LagrangianLinearTIDS(*it)));
    else if (type == Type::FirstOrderLinearDS)  // Linear DS
      allDSLocal->insert
      (SP::FirstOrderLinearDS(new FirstOrderLinearDS(*it)));
    else if (type == Type::FirstOrderLinearTIDS)  // Linear Time Invariant DS
      allDSLocal->insert
      (SP::FirstOrderLinearTIDS(new FirstOrderLinearTIDS(*it)));
    else if (type == Type::FirstOrderNonLinearDS)  // Non linear DS
      allDSLocal->insert
      (SP::FirstOrderNonLinearDS(new FirstOrderNonLinearDS(*it)));
    else RuntimeException::
      selfThrow("NonSmoothDynamicalSystem::xml constructor, wrong Dynamical System type" + type);
    // checkDS.first is an iterator that points to the DS inserted into the set.
    _mIsLinear = (_mIsLinear && (*(--allDSLocal->end()))->isLinear());
  }



  // ===  The same process is applied for Interactions ===
  SetOfInteractionsXML  interactionsList = _nsdsxml->getInteractionsXML();
  SetOfInteractionsXMLIt it2;
  CheckInsertInteraction checkInter;

  SP::InteractionsSet allInteractionsLocal;
  allInteractionsLocal.reset(new InteractionsSet());
  for (it2 = interactionsList.begin(); it2 != interactionsList.end(); ++it2)
  {
    checkInter = allInteractionsLocal
                 ->insert(SP::Interaction(new Interaction(*it2, allDSLocal)));
    // checkInter.first is an iterator that points to the
    // Interaction inserted into the set.
    _mIsLinear = (_mIsLinear && (*checkInter.first)->relation()->isLinear());
  }

  // === Builds topology ===
  _topology.reset(new Topology(allDSLocal, allInteractionsLocal));
}

// Constructor with one DS and one Interaction (optional)
NonSmoothDynamicalSystem::NonSmoothDynamicalSystem(SP::DynamicalSystem newDS,
    SP::Interaction newInteraction,
    const bool& isBVP):
  _BVP(isBVP), _mIsLinear(true)
{
  // === Checks that sets are not empty ===
  if (!newDS)
    RuntimeException::
    selfThrow("NonSmoothDynamicalSystem:: constructor(SP::DynamicalSystem ds...): ds == NULL.");

  // Note that Interaction == NULL is possible and has sense.

  SP::DynamicalSystemsSet allDSLocal;
  SP::InteractionsSet allInteractionsLocal;
  allDSLocal.reset(new DynamicalSystemsSet());
  allInteractionsLocal.reset(new InteractionsSet());
  allDSLocal->insert(newDS);
  _mIsLinear = newDS->isLinear();
  if (newInteraction)
  {
    allInteractionsLocal->insert(newInteraction);
    _mIsLinear = (_mIsLinear && newInteraction->relation()->isLinear());
  }

  // === build topology ===
  _topology.reset(new Topology(allDSLocal, allInteractionsLocal));
}

NonSmoothDynamicalSystem::NonSmoothDynamicalSystem(DynamicalSystemsSet& listOfDS,
    InteractionsSet& listOfInteractions,
    const bool& isBVP):
  _BVP(isBVP), _mIsLinear(true)
{
  // === "copy" listOfDS/listOfInteractions in allDSLocal/allInteractions ===
  // Warning: DS/Interactions are not copied but pointers are inserted into the corresponding set.

  InteractionsIterator itInter;
  DSIterator itDS;
  for (itDS = listOfDS.begin(); itDS != listOfDS.end(); ++itDS)
  {
    _mIsLinear = ((*itDS)->isLinear() && _mIsLinear);
    if (!_mIsLinear) break;
  }

  for (itInter = listOfInteractions.begin(); itInter != listOfInteractions.end(); ++itInter)
  {
    _mIsLinear = ((*itInter)->relation()->isLinear() && _mIsLinear);
    if (!_mIsLinear) break;
  }

  SP::DynamicalSystemsSet allDSLocal;
  SP::InteractionsSet allInteractionsLocal;

  allDSLocal = createSPtrDynamicalSystemsSet(listOfDS);
  allInteractionsLocal = createSPtrInteractionsSet(listOfInteractions);

  // === build topology ===
  _topology.reset(new Topology(allDSLocal, allInteractionsLocal));
}

NonSmoothDynamicalSystem::NonSmoothDynamicalSystem(DynamicalSystemsSet& listOfDS, const bool& isBVP):
  _BVP(isBVP), _mIsLinear(true)
{

  // === "copy" listOfDS/listOfInteractions in allDSLocal/allInteractions ===
  // Warning: DS/Interactions are not copied but pointers are inserted into the corresponding set.
  SP::DynamicalSystemsSet allDSLocal;
  SP::InteractionsSet allInteractionsLocal;


  allDSLocal.reset(new DynamicalSystemsSet());
  allInteractionsLocal.reset(new InteractionsSet());
  DSIterator itDS;
  for (itDS = listOfDS.begin(); itDS != listOfDS.end(); ++itDS)
  {
    allDSLocal->insert(*itDS);
    _mIsLinear = ((*itDS)->isLinear() && _mIsLinear);
  }

  // === build topology ===
  _topology.reset(new Topology(allDSLocal, allInteractionsLocal));
}

NonSmoothDynamicalSystem::~NonSmoothDynamicalSystem()
{
  clear();
}

// === DynamicalSystems management ===

void NonSmoothDynamicalSystem::saveNSDSToXML()
{
  if (_nsdsxml)
  {
    _nsdsxml->setBVP(_BVP);// no need to change the value of BVP, it mustn't change anyway

    DynamicalSystemsGraph::VIterator vi;
    for (vi = dynamicalSystems()->begin(); vi != dynamicalSystems()->end(); ++vi)
    {
      SP::DynamicalSystem ds = dynamicalSystems()->bundle(*vi);
      if (Type::value(*ds) == Type::LagrangianDS)
        (std11::static_pointer_cast<LagrangianDS>(ds))->saveDSToXML();
      else if (Type::value(*ds) == Type::LagrangianLinearTIDS)
        (std11::static_pointer_cast<LagrangianLinearTIDS>(ds))->saveDSToXML();
      else if (Type::value(*ds) == Type::FirstOrderLinearDS)
        (std11::static_pointer_cast<FirstOrderLinearDS>(ds))->saveDSToXML();
      else if (Type::value(*ds) == Type::FirstOrderLinearTIDS)
        (std11::static_pointer_cast<FirstOrderLinearDS>(ds))->saveDSToXML();
      else if (Type::value(*ds) == Type::FirstOrderNonLinearDS)
        ds->saveDSToXML();
      else RuntimeException::selfThrow("NonSmoothDynamicalSystem::saveToXML - bad kind of DynamicalSystem");
    }

    InteractionsIterator it2;
    for (it2 = _topology->interactions()->begin();
         it2 != interactions()->end(); ++it2)
      (*it2)->saveInteractionToXML();
  }
  else RuntimeException::
    selfThrow("NonSmoothDynamicalSystem::saveNSDSToXML - The NonSmoothDynamicalSystemXML object doesn't exists");
}

SP::DynamicalSystem NonSmoothDynamicalSystem::dynamicalSystemNumber(int nb) const
{
  SP::DynamicalSystemsSet allDSLocal = setOfGraph<DynamicalSystemsSet>(dynamicalSystems());

  // if ds number nb is not in the set ...
  assert(allDSLocal->isIn(nb) &&
         "NonSmoothDynamicalSystem::getDynamicalSystemOnNumber(nb), DS number nb is not in the set.");
  return allDSLocal->getPtr(nb);
}


void NonSmoothDynamicalSystem::display() const
{
  std::cout << " ===== Non Smooth Dynamical System display ===== " <<std::endl;
  std::cout << "---> isBVP = " << _BVP <<std::endl;
  dynamicalSystems()->begin();
  _topology->interactions()->display();
  std::cout << "===================================================" <<std::endl;
}

#include <limits>
double NonSmoothDynamicalSystem::nsdsConvergenceIndicator()
{
  // calculate the max value of all DS convergence indicators
  double convergenceIndicator = -std::numeric_limits<double>::infinity();
  double dsIndic ;
  DynamicalSystemsGraph::VIterator vi;
  for (vi = dynamicalSystems()->begin(); vi != dynamicalSystems()->end(); ++vi)
  {
    dsIndic = dynamicalSystems()->bundle(*vi)->dsConvergenceIndicator();
    if (dsIndic > convergenceIndicator) convergenceIndicator = dsIndic;
  }
  return(convergenceIndicator);
}

void NonSmoothDynamicalSystem::clear()
{
  _topology->clear();
}

void NonSmoothDynamicalSystem::setControlProperty(
  const InteractionsGraph::VDescriptor& vd,
  const DynamicalSystemsGraph::EDescriptor& ed,
  const bool isControlInteraction)
{
  InteractionsGraph& IG0 = *_topology->indexSet0();
  IG0.properties(vd).forControl = isControlInteraction;
  DynamicalSystemsGraph& DSG0 = *_topology->dSG(0);
  DSG0.properties(ed).forControl = isControlInteraction;
}
void NonSmoothDynamicalSystem::insertInteraction(SP::Interaction inter, const int isControlInteraction)
{
  std::pair<DynamicalSystemsGraph::EDescriptor,
      InteractionsGraph::VDescriptor> descrs =
        _topology->insertInteraction(inter);
  if (isControlInteraction == 1)
    setControlProperty(descrs.second, descrs.first, true);
}
