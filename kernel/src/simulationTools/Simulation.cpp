/* Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 *
 * Copyright 2016 INRIA.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
#include "Simulation.hpp"
#include "EventDriven.hpp"
#include "DynamicalSystem.hpp"
#include "NonSmoothDynamicalSystem.hpp"
#include "Topology.hpp"
#include "Interaction.hpp"
#include "Relation.hpp"
#include "EventsManager.hpp"
#include "LagrangianDS.hpp"

// One Step Integrators
#include "OneStepIntegrator.hpp"

// One Step Non Smooth Problems
#include "LCP.hpp"
#include "QP.hpp"
#include "Relay.hpp"
#include "NonSmoothLaw.hpp"
#include "TypeName.hpp"
// for Debug
// #define DEBUG_BEGIN_END_ONLY
#define DEBUG_NOCOLOR
#define DEBUG_STDOUT
#define DEBUG_MESSAGES
#include <debug.h>
#include <fstream>

#include "Model.hpp"



// --- Constructor with a TimeDiscretisation (and thus a Model) and an
// --- id ---
Simulation::Simulation(SP::TimeDiscretisation td):
  _name("unnamed"), _tinit(0.0), _tend(0.0), _tout(0.0),
  _numberOfIndexSets(0),
  _tolerance(DEFAULT_TOLERANCE), _printStat(false),
  _staticLevels(false)
{
  if (!td)
    RuntimeException::selfThrow("Simulation constructor - timeDiscretisation == NULL.");
  _useRelativeConvergenceCriterion = false;
  _relativeConvergenceCriterionHeld = false;
  _relativeConvergenceTol = 10e-3;

  // === indexSets will be updated during initialize() call ===

  _allOSI.reset(new OSISet());
  _allNSProblems.reset(new OneStepNSProblems());
  _eventsManager.reset(new EventsManager(td)); //
}

// --- Destructor ---
Simulation::~Simulation()
{
  clear();
  // -> see shared ressources for this
  if (statOut.is_open()) statOut.close();
}

void Simulation::setTimeDiscretisationPtr(SP::TimeDiscretisation td)
{
  _eventsManager->setTimeDiscretisationPtr(td);
}

double Simulation::getTk() const
{
  return _eventsManager->getTk();
}

double Simulation::getTkp1() const
{
  return _eventsManager->getTkp1();
}

double Simulation::getTkp2() const
{
  return _eventsManager->getTkp2();
}

double Simulation::currentTimeStep() const
{
  return _eventsManager->currentTimeStep();
}

double Simulation::startingTime() const
{
  return _eventsManager->startingTime();
}

double Simulation::nextTime() const
{
  return _eventsManager->nextTime();
}

bool Simulation::hasNextEvent() const
{
  return _eventsManager->hasNextEvent();
}


// clear all maps to break shared_ptr cycle
void Simulation::clear()
{
  if (_allOSI)
  {
    _allOSI->clear();
  }
  if (_allNSProblems)
  {
    _allNSProblems->clear();
  }
}

// Getters/setters

void Simulation::insertIntegrator(SP::OneStepIntegrator osi)
{
  _allOSI->insert(osi);
}

SP::InteractionsGraph Simulation::indexSet(unsigned int i)
{
  return _nsds->topology()->indexSet(i) ;
}

SP::OneStepNSProblem Simulation::oneStepNSProblem(int Id)
{
  if (!(*_allNSProblems)[Id])
    RuntimeException::selfThrow("Simulation - oneStepNSProblem(Id) - The One Step NS Problem is not in the simulation.");

  return (*_allNSProblems)[Id];
}

void Simulation::updateIndexSets()
{

  DEBUG_BEGIN("Simulation::updateIndexSets()\n");
  // update I0 indices
  unsigned int nindexsets = _nsds->topology()->indexSetsSize();

  DEBUG_PRINTF("  nindexsets = %d\n", nindexsets   );
  if (nindexsets > 1)
  {
    for (unsigned int i = 1; i < nindexsets ; ++i)
    {
      updateIndexSet(i);
      _nsds->topology()->indexSet(i)->update_vertices_indices();
      _nsds->topology()->indexSet(i)->update_edges_indices();
    }
  }
  DEBUG_END("Simulation::updateIndexSets()\n");

}

void Simulation::insertNonSmoothProblem(SP::OneStepNSProblem osns, int Id)
{
  if (_allNSProblems->size() > 0 && ((*_allNSProblems)[Id]))
    RuntimeException::selfThrow("Simulation - insertNonSmoothProblem(osns), trying to insert a OSNSP already existing. ");
  (*_allNSProblems)[Id] = osns;

}

void Simulation::initialize(SP::Model m, bool withOSI)
{
  // === Connection with the model ===
  assert(m && "Simulation::initialize(model) - model = NULL.");

  _T = m->finalT();

  _nsds =  m->nonSmoothDynamicalSystem();

  // === Events manager initialization ===
  _eventsManager->initialize(_T);
  _tinit = _eventsManager->startingTime();
  //===


  if (withOSI)
  {
    if (numberOfOSI() == 0)
      RuntimeException::selfThrow("Simulation::initialize No OSI !");


    DynamicalSystemsGraph::VIterator dsi, dsend;
    SP::DynamicalSystemsGraph DSG = _nsds->topology()->dSG(0);
    for (std11::tie(dsi, dsend) = DSG->vertices(); dsi != dsend; ++dsi)
    {
      SP::OneStepIntegrator osi = DSG->properties(*dsi).osi;
      SP::DynamicalSystem ds = DSG->bundle(*dsi);
      if (!osi)
      {
        // By default, if the user has not set the OSI, we assign the first OSI to all DS
        _nsds->topology()->setOSI(ds,*_allOSI->begin());
        //std::cout << "By default, if the user has not set the OSI, we assign the first OSI to all DS"<<std::endl;
      }
      osi = DSG->properties(*dsi).osi;
      ds->initialize(m->t0(), osi->getSizeMem());
    }



    // This is the default
    _levelMinForInput = LEVELMAX;
    _levelMaxForInput = 0;
    _levelMinForOutput = LEVELMAX;
    _levelMaxForOutput = 0;

    _numberOfIndexSets=0;

    // === OneStepIntegrators initialization ===
    for (OSIIterator itosi = _allOSI->begin();
         itosi != _allOSI->end(); ++itosi)
    {
      (*itosi)->setSimulationPtr(shared_from_this());
      (*itosi)->initialize(*m);
      _numberOfIndexSets = std::max<int>((*itosi)->numberOfIndexSets(), _numberOfIndexSets);

      /* V.A. This operation should be osi dependent only
       * in a near future, we should only use _level*For* osi only
       */
      _levelMinForInput = std::min<int>((*itosi)->levelMinForOutput(), _levelMinForInput);
      _levelMaxForInput = std::max<int>((*itosi)->levelMaxForOutput(), _levelMaxForInput);
      _levelMinForOutput = std::min<int>((*itosi)->levelMinForInput(), _levelMinForOutput);
      _levelMaxForOutput = std::max<int>((*itosi)->levelMaxForInput(), _levelMaxForOutput);

    }
  }
  SP::Topology topo = _nsds->topology();
  unsigned int indxSize = topo->indexSetsSize();
  assert (_numberOfIndexSets >0);
  if ((indxSize == LEVELMAX) || (indxSize < _numberOfIndexSets ))
  {
    topo->indexSetsResize(_numberOfIndexSets);
    // Init if the size has changed
    for (unsigned int i = indxSize; i < topo->indexSetsSize(); i++) // ++i ???
      topo->resetIndexSetPtr(i);
  }


  // Initialize OneStepNSProblem(s). Depends on the type of simulation.
  // Warning FP : must be done in any case, even if the interactions set
  // is empty.
  initOSNS();

  // Process events at time _tinit. Useful to save values in memories
  // for example.  Warning: can not be called during
  // eventsManager->initialize, because it needs the initialization of
  // OSI, OSNS ...
  _eventsManager->preUpdate(*this);

  _tend =  _eventsManager->nextTime();

  // End of initialize:

  //  - all OSI and OSNS (ie DS and Interactions) states are computed
  //  - for time _tinit and saved into memories.
  //  - Sensors or related objects are updated for t=_tinit.
  //  - current time of the model is equal to t1, time of the first
  //  - event after _tinit.
  //  - currentEvent of the simu. corresponds to _tinit and nextEvent
  //  - to _tend.

  // If _printStat is true, open output file.
  if (_printStat)
  {
    statOut.open("simulationStat.dat", std::ios::out | std::ios::trunc);
    if (!statOut.is_open())
      SiconosVectorException::selfThrow("writing error : Fail to open file simulationStat.dat ");
    statOut << "============================================" <<std::endl;
    statOut << " Siconos Simulation of type " << Type::name(*this) << "." <<std::endl;
    statOut <<std::endl;
    statOut << "The tolerance parameter is equal to: " << _tolerance <<std::endl;
    statOut <<std::endl <<std::endl;
  }
}

void Simulation::initializeInteraction(double time, SP::Interaction inter)
{
  // determine which (lower and upper) levels are required for this Interaction
  // in this Simulation.

  // Get the interaction properties from the topology for initialization.
  SP::InteractionsGraph indexSet0 = _nsds->topology()->indexSet0();
  InteractionsGraph::VDescriptor ui = indexSet0->descriptor(inter);

  // This calls computeOutput() and initializes qMemory and q_k.
  DynamicalSystemsGraph &DSG = *_nsds->topology()->dSG(0);

  //SP::OneStepIntegrator osi = indexSet0->properties(ui).osi;
  SP::DynamicalSystem ds1;
  SP::DynamicalSystem ds2;
  // --- Get the dynamical system(s) (edge(s)) connected to the current interaction (vertex) ---
  if (indexSet0->properties(ui).source != indexSet0->properties(ui).target)
  {
    DEBUG_PRINT("a two DS Interaction\n");
    ds1 = indexSet0->properties(ui).source;
    ds2 = indexSet0->properties(ui).target;
  }
  else
  {
    DEBUG_PRINT("a single DS Interaction\n");
    ds1 = indexSet0->properties(ui).source;
    ds2 = ds1;
    // \warning this looks like some debug code, but it gets executed even with NDEBUG.
    // may be compiler does something smarter, but still it should be rewritten. --xhub
    InteractionsGraph::OEIterator oei, oeiend;
    for (std11::tie(oei, oeiend) = indexSet0->out_edges(ui);
         oei != oeiend; ++oei)
    {
      // note : at most 4 edges
      ds2 = indexSet0->bundle(*oei);
      if (ds2 != ds1)
      {
        assert(false);
        break;
      }
    }
  }
  assert(ds1);
  assert(ds2);

  OneStepIntegrator& osi1 = *DSG.properties(DSG.descriptor(ds1)).osi;
  OneStepIntegrator& osi2 = *DSG.properties(DSG.descriptor(ds2)).osi;

  if (&osi1 == &osi2 )
    osi1.initializeInteraction(time, *inter, indexSet0->properties(ui),  DSG);
  else
    {
      osi1.initializeInteraction(time, *inter, indexSet0->properties(ui),  DSG);
      osi2.initializeInteraction(time, *inter, indexSet0->properties(ui),  DSG);
    }
}



int Simulation::computeOneStepNSProblem(int Id)
{
  DEBUG_BEGIN("Simulation::computeOneStepNSProblem(int Id)\n");
  DEBUG_PRINTF("with Id = %i\n", Id);

  if (!(*_allNSProblems)[Id])
    RuntimeException::selfThrow("Simulation - computeOneStepNSProblem, OneStepNSProblem == NULL, Id: " + Id);

  DEBUG_END("Simulation::computeOneStepNSProblem(int Id)\n");
  return (*_allNSProblems)[Id]->compute(nextTime());


}


SP::SiconosVector Simulation::y(unsigned int level, unsigned int coor)
{
  // return output(level) (ie with y[level]) for all Interactions.
  // assert(level>=0);

  DEBUG_BEGIN("Simulation::output(unsigned int level, unsigned int coor)\n");
  DEBUG_PRINTF("with level = %i and coor = %i \n", level,coor);

  InteractionsGraph::VIterator ui, uiend;
  SP::Interaction inter;
  SP::InteractionsGraph indexSet0 = _nsds->topology()->indexSet0();

  SP::SiconosVector y (new SiconosVector (_nsds->topology()->indexSet0()->size() ));
  int i=0;
  for (std11::tie(ui, uiend) = indexSet0->vertices(); ui != uiend; ++ui)
  {
    inter = indexSet0->bundle(*ui);
    assert(inter->lowerLevelForOutput() <= level);
    assert(inter->upperLevelForOutput() >= level);
    y->setValue(i,inter->y(level)->getValue(coor));
    i++;
  }
  DEBUG_END("Simulation::output(unsigned int level, unsigned int coor)\n");
  return y;
}

SP::SiconosVector Simulation::lambda(unsigned int level, unsigned int coor)
{
  // return input(level) (ie with lambda[level]) for all Interactions.
  // assert(level>=0);

  DEBUG_BEGIN("Simulation::input(unsigned int level, unsigned int coor)\n");
  DEBUG_PRINTF("with level = %i and coor = %i \n", level,coor);

  InteractionsGraph::VIterator ui, uiend;
  SP::Interaction inter;
  SP::InteractionsGraph indexSet0 = _nsds->topology()->indexSet0();

  SP::SiconosVector lambda (new SiconosVector (_nsds->topology()->indexSet0()->size() ));
  int i=0;
  for (std11::tie(ui, uiend) = indexSet0->vertices(); ui != uiend; ++ui)
  {
    inter = indexSet0->bundle(*ui);
    assert(inter->lowerLevelForOutput() <= level);
    assert(inter->upperLevelForOutput() >= level);
    lambda->setValue(i,inter->lambda(level)->getValue(coor));
    i++;
  }
  DEBUG_END("Simulation::input(unsigned int level, unsigned int coor)\n");
  return lambda;
}


void Simulation::run()
{
  unsigned int count = 0; // events counter.

  std::cout << " ==== Start of " << Type::name(*this) << " simulation - This may take a while ... ====" <<std::endl;
  while (hasNextEvent())
  {
    advanceToEvent();
    processEvents();
    count++;
  }
  std::cout << "===== End of " << Type::name(*this) << " simulation. " << count << " events have been processed. ==== " <<std::endl;
}

void Simulation::processEvents()
{
  _eventsManager->processEvents(*this);

  if (_eventsManager->hasNextEvent())
  {
    // For TimeStepping Scheme, need to update IndexSets, but not for EventDriven scheme
    if (Type::value(*this) != Type::EventDriven)
    {
      updateIndexSets();
    }
  }

  /* should be evaluated only if needed */
  SP::DynamicalSystemsGraph dsGraph = _nsds->dynamicalSystems();
  for (DynamicalSystemsGraph::VIterator vi = dsGraph->begin(); vi != dsGraph->end(); ++vi)
  {
    dsGraph->bundle(*vi)->endStep();
  }

}


void Simulation::updateT(double T)
{
  _T = T;
  _eventsManager->updateT(T);
}


void Simulation::link(SP::Interaction inter,
                      SP::DynamicalSystem ds1,
                      SP::DynamicalSystem ds2)
{
  DEBUG_PRINTF("link interaction : %d\n", inter->number());

  nonSmoothDynamicalSystem()->link(inter, ds1, ds2);

  initializeInteraction(nextTime(), inter);

  _linkOrUnlink = true;
}

void Simulation::unlink(SP::Interaction inter)
{
  nonSmoothDynamicalSystem()->removeInteraction(inter);
  _linkOrUnlink = true;
}

void Simulation::updateInteractions()
{
  // Update interactions if a manager was provided
  if (_interman) {
    _linkOrUnlink = false;
    _interman->updateInteractions(shared_from_this());

    if (_linkOrUnlink)
      initOSNS();
  }
}
