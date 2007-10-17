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

#include "EventDriven.h"
#include "SimulationXML.h"
#include "OneStepNSProblemXML.h"
#include "SimulationXML.h"
#include "UnitaryRelation.h"
#include "Lsodar.h"
#include "LCP.h"
#include "Model.h"
#include "Interaction.h"
#include "EventsManager.h"
#include "NonSmoothDynamicalSystem.h"
#include "Topology.h"
#include "DynamicalSystem.h"

using namespace std;

EventDriven::EventDriven(TimeDiscretisation * td): Simulation(td, "EventDriven"), istate(1), tinit(0), tend(0)
{}

// --- XML constructor ---
EventDriven::EventDriven(SimulationXML* strxml, Model *newModel): Simulation(strxml, newModel, "EventDriven"), istate(1), tinit(0), tend(0)
{
  // === One Step NS Problem ===
  // We read data in the xml output (mainly Interactions concerned and solver) and assign them to
  // both one step ns problems ("acceleration" and "impact").
  // At the time, only LCP is available for Event Driven.

  if (simulationxml->hasOneStepNSProblemXML())
  {
    // OneStepNSProblem - Memory allocation/construction
    string type = simulationxml->getOneStepNSProblemXMLPtr()->getNSProblemType();
    if (type == LCP_TAG)  // LCP
    {
      (*allNSProblems)["acceleration"] = new LCP(simulationxml->getOneStepNSProblemXMLPtr(), this);
      isNSProblemAllocatedIn[(*allNSProblems)["acceleration"] ] = true;
      (*allNSProblems)["impact"] = new LCP(simulationxml->getOneStepNSProblemXMLPtr(), this);
      isNSProblemAllocatedIn[(*allNSProblems)["impact"] ] = true;
    }
    else
      RuntimeException::selfThrow("EventDriven::xml constructor - wrong type of NSProblem: inexistant or not yet implemented");

    (*allNSProblems)["acceleration"]->setId("acceleration");
    (*allNSProblems)["impact"]->setId("impact");
  }
}

// --- Destructor ---
EventDriven::~EventDriven()
{}

void EventDriven::updateIndexSet(unsigned int i)
{
  if (i > indexSets.size())
    RuntimeException::selfThrow("EventDriven::updateIndexSet(i), indexSets[i] does not exist.");

  if (i == 0) // IndexSets[0] must not be updated by this function.
    RuntimeException::selfThrow("EventDriven::updateIndexSet(i=0), indexSets[0] can not be updated.");

  // for all Unitary Relations in indexSet[i-1], compute y[i-1] and update the indexSet[i]
  UnitaryRelationsIterator it, itForFind;

  double y;

  if (i == 1) // Action is different when first set is treated
  {
    // We get jroot array, output of root information
    // Warning! work only if there is only one osi in the simulation.
    if (allOSI->size() > 1)
      RuntimeException::selfThrow("EventDriven::updateIndexSet(i), not yet implemented for several OneStepIntegrators in the same Simulation process.");
    Lsodar * lsodar = static_cast<Lsodar*>(*(allOSI->begin()));
    integer * jroot = lsodar->getJroot();
    unsigned int nsLawSize; // size of each UR, which corresponds to the related nsLaw size
    unsigned int absolutePosition = 0; // global position of the UR in the vector of constraints, ie in jroot.
    bool out = true;
    for (it = indexSets[i - 1]->begin(); it != indexSets[i - 1]->end(); ++it)
    {
      itForFind = indexSets[i]->find(*it);
      nsLawSize = (*it)->getNonSmoothLawSize();

      // 1 - If the UR is not yet in the set
      if (itForFind == indexSets[i]->end())
      {
        // Each UR may handle several constraints and thus corresponds to a set of position in jroot
        for (unsigned int pos = absolutePosition; pos < (absolutePosition + nsLawSize); ++pos)
        {
          if (jroot[pos] == 1)
          {
            indexSets[i]->insert(*it);
            break; // if one, at least, of the nsLawSize constraints is active, the UR is inserted into the set.
          }
        }
      }
      else // if the UR was already in the set
      {
        out = true;
        for (unsigned int pos = absolutePosition; pos < (absolutePosition + nsLawSize); ++pos)
        {
          if (jroot[pos] == 1)
          {
            out = false;
            break; // if one, at least, of the nsLawSize constraints is active, the UR remains in the set.
          }
        }
        if (out)
        {
          indexSets[i]->erase(*it);
          (*it)->getLambdaPtr(i)->zero();
        }

      }
      absolutePosition += nsLawSize; // step to next UR ...
    }
  }
  else
  {
    for (it = indexSets[i - 1]->begin(); it != indexSets[i - 1]->end(); ++it)
    {
      // check if current Unitary Relation (ie *it) is in indexSets[i]
      // (if not itForFind will be equal to indexSets.end())
      itForFind = indexSets[i]->find(*it);

      // Get y[i-1] double value
      y = (*it)->getYRef(i - 1);


      // if y[i-1] <=0, then the unitary relation is added in indexSets[i] (if it was not already there)
      // else if y[i-1] > 0 and if the unitary relation was in the set, it is removed.
      if (itForFind == indexSets[i]->end()) // If the UR is not yet in the set ...
      {
        if (fabs(y) < tolerance)
          indexSets[i]->insert(*it);
      }
      else  // if the UR is already in the set
      {
        if (fabs(y) > tolerance)
        {
          indexSets[i]->erase(*it);
          (*it)->getLambdaPtr(i)->zero();
        }
      }
    }
  }
}

void EventDriven::updateIndexSetsWithDoubleCondition()
{

  // for all Unitary Relations in indexSet[i-1], compute y[i-1] and update the indexSet[i]
  UnitaryRelationsIterator it, itForFind;

  for (it = indexSets[2]->begin(); it != indexSets[2]->end(); ++it)
  {
    double gamma = (*it)->getYRef(2);
    double F     = (*it)->getLambdaRef(2);

    if (gamma > 0 && fabs(F) < tolerance)
      indexSets[2]->erase(*it);
    else if (fabs(gamma) < tolerance && fabs(F) < tolerance) // undetermined case
      RuntimeException::selfThrow("EventDriven::updateIndexSetsWithDoubleCondition(), undetermined case.");
  }
}

void EventDriven::initOSNS()
{
  tinit = eventsManager->getStartingTime();
  tend =  eventsManager->getNextTime();

  if (!allNSProblems->empty()) // ie if some Interactions have been declared and a Non smooth problem built.
  {
    // === OneStepNSProblem initialization. ===
    // First check that there are 2 osns: one "impact" and one "acceleration"
    if (allNSProblems->size() != 2)
      RuntimeException::selfThrow(" EventDriven::initialize, \n an EventDriven simulation must have two non smooth problem.\n Here, there are " + allNSProblems->size());

    if (allNSProblems->find("impact") == allNSProblems->end()) // ie if the impact problem does not exist
      RuntimeException::selfThrow("EventDriven::initialize, an EventDriven simulation must have an 'impact' non smooth problem.");
    if (allNSProblems->find("acceleration") == allNSProblems->end()) // ie if the acceleration-level problem does not exist
      RuntimeException::selfThrow("EventDriven::initialize, an EventDriven simulation must have an 'acceleration' non smooth problem.");

    // At the time, we consider that for all systems, levelMin is equal to the minimum value of the relative degree
    levelMin = model->getNonSmoothDynamicalSystemPtr()->getTopologyPtr()->getMinRelativeDegree();
    if (levelMin == 0)
      levelMin++;

    // === update all index sets ===
    updateIndexSets();

    // WARNING: only for Lagrangian systems - To be reviewed for other ones.
    (*allNSProblems)["impact"]->setLevels(levelMin - 1, levelMax - 1);
    (*allNSProblems)["impact"]->initialize();
    (*allNSProblems)["acceleration"]->setLevels(levelMin, levelMax);
    (*allNSProblems)["acceleration"]->initialize();
  }
}

void EventDriven::initLevelMax()
{
  levelMax = model->getNonSmoothDynamicalSystemPtr()->getTopologyPtr()->getMaxRelativeDegree();
  // Interactions initialization (here, since level depends on the type of simulation)
  // level corresponds to the number of Y and Lambda derivatives computed.
  if (levelMax == 0)
    levelMax++;
}

void EventDriven::computeF(OneStepIntegrator* osi, integer * sizeOfX, doublereal * time, doublereal * x, doublereal * xdot)
{

  // computeF is supposed to fill xdot in, using the definition of the dynamical systems belonging to the osi

  // Check osi type: only lsodar is allowed.
  if (osi->getType() != "Lsodar")
    RuntimeException::selfThrow("EventDriven::computeF(osi, ...), not yet implemented for a one step integrator of type " + osi->getType());

  Lsodar * lsodar = static_cast<Lsodar*>(osi);

  // fill in xWork vector (ie all the x of the ds of this osi) with x
  lsodar->fillXWork(sizeOfX, x);

  double t = *time;
  model->setCurrentTime(t);

  // solve a LCP at "acceleration" level if required
  if (!allNSProblems->empty())
  {
    if (!((*allNSProblems)["acceleration"]->getInteractions())->isEmpty())
    {
      (*allNSProblems)["acceleration"]->compute(t);
      updateInput(2); // Necessary to compute DS state below
    }
    // Compute the right-hand side ( xdot = f + r in DS) for all the ds, with the new value of input.
    //lsodar->computeRhs(t);
  }
  // update the DS of the OSI.
  lsodar->computeRhs(t);
  //  lsodar->updateState(2); // update based on the last saved values for the DS state, ie the ones computed by lsodar (x above)

  // Update Index sets?

  // Get the required value, ie xdot for output.
  SiconosVector * xtmp2; // The Right-Hand side
  DSIterator it;
  unsigned int i = 0;
  for (it = lsodar->dynamicalSystemsBegin(); it != lsodar->dynamicalSystemsEnd(); ++it)
  {
    xtmp2 = (*it)->getRhsPtr(); // Pointer link !
    for (unsigned int j = 0 ; j < (*it)->getN() ; ++j) // Warning: getN, not getDim !!!!
      xdot[i++] = (*xtmp2)(j);
  }
  xtmp2 = NULL;
}

void EventDriven::computeJacobianF(OneStepIntegrator* osi, integer *sizeOfX, doublereal *time, doublereal *x,  doublereal *jacob)
{
  if (osi->getType() != "Lsodar")
    RuntimeException::selfThrow("EventDriven::computeF(osi, ...), not yet implemented for a one step integrator of type " + osi->getType());

  Lsodar * lsodar = static_cast<Lsodar*>(osi);

  //   // Remark A: according to DLSODAR doc, each call to jacobian is preceded by a call to f with the same
  //   // arguments NEQ, T, and Y.  Thus to gain some efficiency, intermediate quantities shared by both calculations may be
  //   // saved in class members?
  // fill in xWork vector (ie all the x of the ds of this osi) with x
  // fillXWork(x); // -> copy // Maybe this step is not necessary? because of remark A above

  // Compute the jacobian of the vector field according to x for the current ds
  double t = *time;
  model->setCurrentTime(t);
  lsodar->computeJacobianRhs(t);

  //   // Save jacobianX values from dynamical system into current jacob (in-out parameter)

  DSIterator it;
  unsigned int i = 0;
  for (it = lsodar->dynamicalSystemsBegin(); it != lsodar->dynamicalSystemsEnd(); ++it)
  {
    SiconosMatrix * jacotmp = (*it)->getJacobianXRhsPtr(); // Pointer link !
    for (unsigned int j = 0 ; j < (*it)->getN() ; ++j)
    {
      for (unsigned k = 0 ; k < (*it)->getDim() ; ++k)
        jacob[i++] = (*jacotmp)(k, j);
    }
  }
}

void EventDriven::computeG(OneStepIntegrator* osi, integer * sizeOfX, doublereal* time, doublereal* x, integer * ng, doublereal * gOut)
{
  UnitaryRelationsIterator itUR;
  unsigned int nsLawSize, k = 0 ;
  SiconosVector * y = NULL, * lambda = NULL;

  Lsodar * lsodar = static_cast<Lsodar*>(osi);

  // fill in xWork vector (ie all the x of the ds of this osi) with x
  lsodar->fillXWork(sizeOfX, x); // That may be not necessary? Check if computeF is called for each computeG.

  double t = *time;
  model->setCurrentTime(t);

  // IN: - lambda[2] obtained during LCP call in computeF()
  //     - y[0]: need to be updated.

  if (!allNSProblems->empty())
    updateOutput(0, 0);

  // If UR is in IndexSet2, g = lambda[2], else g = y[0]
  for (itUR = indexSets[0]->begin(); itUR != indexSets[0]->end(); ++itUR)
  {
    nsLawSize = (*itUR)->getNonSmoothLawSize();
    if (indexSets[2]->find(*itUR) != indexSets[2]->end()) // ie if the current Unitary Relation is in indexSets[2]
    {
      // Get lambda at acc. level, solution of LCP acc, called during computeF().
      lambda = (*itUR)->getLambdaPtr(2);
      for (unsigned int i = 0; i < nsLawSize; i++)
        gOut[k++] = (*lambda)(i);
    }
    else
    {
      y = (*itUR)->getYPtr(0);
      for (unsigned int i = 0; i < nsLawSize; i++)
        gOut[k++] = (*y)(i);
    }
  }
}

void EventDriven::updateImpactState()
{
  OSIIterator itOSI;
  // Compute input = R(lambda[1])
  updateInput(1);

  // Compute post-impact velocity
  for (itOSI = allOSI->begin(); itOSI != allOSI->end() ; ++itOSI)
    (*itOSI)->updateState(1);
}

void EventDriven::update(unsigned int levelInput)
{
  if (!allNSProblems->empty())
  {
    // compute input (lambda -> r)
    updateInput(levelInput);

    // Update dynamical systems states
    OSIIterator itOSI;
    for (itOSI = allOSI->begin(); itOSI != allOSI->end() ; ++itOSI)
      (*itOSI)->updateState(levelInput);

    // Update output (y)
    updateOutput(levelInput, levelInput);
  }
  // Warning: index sets are not updated in this function !!
}

void EventDriven::advanceToEvent()
{
  // WARNING: this is supposed to work for only one OSI, including all the DS.
  // To be reviewed for multiple OSI case (if it has sense?).

  // ---> Step 1: integrate the smooth dynamics from current event to next event;
  // Starting event = last accessed event.
  // Next event = next time step or first root of the 'g' function found by integrator (Lsodar)

  // if istate == 1 => first call. It this case we suppose that tinit and tend have been initialized before.

  if (istate == 2 || istate == 3)
  {
    tinit = eventsManager->getStartingTime();
    tend =  eventsManager->getNextTime();
  }

  double tout = tend;
  bool isNewEventOccur = false;  // set to true if a new event occur during integration

  // call integrate method for each OSI, between tinit and tend.
  OSIIterator it;
  for (it = allOSI->begin(); it != allOSI->end(); ++it)
  {
    (*it)->integrate(tinit, tend, tout, istate); // integrate must return a flag (istate) telling if tend has been reached or not.

    if (printStat)
    {
      statOut << " =================> Results after advanceToEvent <================= " << endl;
      statOut << " Starting time: " << tinit << endl;

    }
    if (istate == 3) // ie if tout is not equal to tend: one or more roots have been found.
    {
      isNewEventOccur = true;
      // Add an event into the events manager list
      eventsManager->scheduleEvent("NonSmoothEvent", tout);
      if (printStat)
        statOut << " -----------> New non-smooth event at time " << tout << endl;
    }
    if (printStat)
    {
      Lsodar * lsodar = static_cast<Lsodar*>(*it);
      statOut << "Results at time " << tout << ":" << endl;
      integer * iwork = lsodar->getIwork();
      statOut << "Number of steps: " << iwork[10] << ", number of f evaluations: " << iwork[11] << ", number of jacobianF eval.: " << iwork[12] << "." << endl;

    }
  }


  // Set model time to tout
  model->setCurrentTime(tout);
  // Update all the index sets ...
  updateOutput(1, 1);
  updateIndexSets();
}

EventDriven* EventDriven::convert(Simulation *str)
{
  EventDriven* ed = dynamic_cast<EventDriven*>(str);
  return ed;
}
