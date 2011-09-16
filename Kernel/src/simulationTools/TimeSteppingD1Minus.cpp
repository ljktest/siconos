/* Siconos-Kernel, Copyright INRIA 2005-2011.
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

#include "TimeSteppingD1Minus.hpp"
#include "SimulationXML.hpp"
#include "OneStepNSProblemXML.hpp"
#include "Topology.hpp"
#include "LCP.hpp"
#include "Relay.hpp"
#include "Model.hpp"
#include "TimeDiscretisation.hpp"
#include "NonSmoothDynamicalSystem.hpp"
#include "UnitaryRelation.hpp"
#include "OneStepIntegrator.hpp"
#include "Interaction.hpp"
#include "EventsManager.hpp"
#include "FrictionContact.hpp"
#include "Moreau.hpp"
#include "D1MinusLinear.hpp"

#include <debug.h>

using namespace std;

/** Pointer to function, used to set the behavior of simulation when
  ns solver failed.  If equal to null, use DefaultCheckSolverOutput
  else (set with setCheckSolverFunction) call the pointer below).
  Note FP: (temporary) bad method to set checkSolverOutput but it
  works ... It may be better to use plug-in?
  */
static CheckSolverFPtr checkSolverOutput = NULL;

static bool cmp_osi_type_d1minuslinear(SP::OneStepIntegrator osi)
{
  return osi->getType() == OSI::D1MINUSLINEAR;
}

TimeSteppingD1Minus::TimeSteppingD1Minus(SP::TimeDiscretisation td,
    SP::OneStepIntegrator osi,
    SP::OneStepNSProblem osnspb)
  : Simulation(td), _newtonTolerance(1e-6), _newtonMaxIteration(50), _newtonOptions(SICONOS_TS_NONLINEAR)
{

  _computeResiduY = false;
  _computeResiduR = false;

  if (osi) insertIntegrator(osi);
  (*_allNSProblems).resize(SICONOS_NB_OSNSP_TS);
  if (osnspb) insertNonSmoothProblem(osnspb, SICONOS_OSNSP_TS_VELOCITY);

}

TimeSteppingD1Minus::TimeSteppingD1Minus(SP::TimeDiscretisation td, int nb)
  : Simulation(td), _newtonTolerance(1e-6), _newtonMaxIteration(50), _newtonOptions(SICONOS_TS_NONLINEAR)
{
  _computeResiduY = false;
  _computeResiduR = false;

  (*_allNSProblems).resize(nb);
}

// --- Destructor ---
TimeSteppingD1Minus::~TimeSteppingD1Minus()
{
}

bool TimeSteppingD1Minus::predictorDeactivate(SP::UnitaryRelation ur, unsigned int i)
{
  double h = timeStep();
  double y = ur->getYRef(i - 1); // for i=1 it is the position -> historic notation y
  double yDot = ur->getYRef(i); // for i=1 it is the velocity -> historic notation yDot
  DEBUG_PRINTF("TS::predictorDeactivate yref=%e, yDot=%e, y_estimated=%e.\n", y, yDot, y + 0.5 * h * yDot);
  y += 0.5 * h * yDot;
  assert(!isnan(y));
  if (y > 0)
    DEBUG_PRINTF("TS::predictorDeactivate DEACTIVATE.\n");
  return (y > 0);
}

bool TimeSteppingD1Minus::predictorActivate(SP::UnitaryRelation ur, unsigned int i)
{
  double h = timeStep();
  double y = ur->getYRef(i - 1); // for i=1 it is the position -> historic notation y
  double yDot = ur->getYRef(i); // for i=1 it is the velocity -> historic notation yDot
  DEBUG_PRINTF("TS::predictorActivate yref=%e, yDot=%e, y_estimated=%e.\n", y, yDot, y + 0.5 * h * yDot);
  y += 0.5 * h * yDot;
  assert(!isnan(y));
  if (y <= 0)
    DEBUG_PRINTF("TS::predictorActivate ACTIVATE.\n");
  return (y <= 0);
}

void TimeSteppingD1Minus::updateIndexSet(unsigned int i)
{
  // To update IndexSet i: add or remove UnitaryRelations from
  // this set, depending on y values.
  // boost::default_color_type is used to organize update in UnitaryRelationsGraph:
  // - white_color : undiscovered vertex (UnitaryRelation)
  // - gray_color : discovered vertex (UnitaryRelation) but searching descendants
  // - black_color : discovered vertex (UnitaryRelation) together with the descendants
  // Originally this function was only evaluated for i=1 (OSI::MOREAU), it is re-arranged
  // for being available on arbitrary kinematic levels.

  assert(!_model.expired());
  assert(model()->nonSmoothDynamicalSystem());
  assert(model()->nonSmoothDynamicalSystem()->topology());

  SP::Topology topo = model()->nonSmoothDynamicalSystem()->topology();

  assert(i < topo->indexSetsSize() &&
         "TimeSteppingD1Minus::updateIndexSet(i), indexSets[i] does not exist.");
  // IndexSets[0] must not be updated in simulation, since it belongs to Topology.
  assert(i > 0 &&
         "TimeSteppingD1Minus::updateIndexSet(i=0), indexSets[0] cannot be updated.");

  // For all Unitary Relations in indexSet[i-1], compute y[i-1] and
  // update the indexSet[i].
  SP::UnitaryRelationsGraph indexSet0 = topo->indexSet(0); // (ALL UnitaryRelations : formula (8.30) of Acary2008)
  SP::UnitaryRelationsGraph indexSet1 = topo->indexSet(1); // (CLOSED UnitaryRelations : formula (8.31) of Acary2008)
  assert(indexSet0);
  assert(indexSet1);

  topo->setHasChanged(false); // WHY

  DEBUG_PRINTF("update indexSets start : indexSet0 size : %d\n", indexSet0->size());
  DEBUG_PRINTF("update IndexSets start : indexSet1 size : %d\n", indexSet1->size());

  // Check indexSet1
  UnitaryRelationsGraph::VIterator ui1, ui1end, v1next;
  boost::tie(ui1, ui1end) = indexSet1->vertices();

  //Remove interactions from the indexSet1
  for (v1next = ui1; ui1 != ui1end; ui1 = v1next)
  {

    cout << "v1=" << *v1next << endl;
    cout << "ui1=" << *ui1 << endl;
    cout << "ui1end=" << *ui1end << endl;
    ++v1next;

    SP::UnitaryRelation ur1 = indexSet1->bundle(*ui1);
    if (indexSet0->is_vertex(ur1))
    {
      UnitaryRelationsGraph::VDescriptor ur1_descr0 = indexSet0->descriptor(ur1);

      assert((indexSet0->color(ur1_descr0) == boost::white_color));

      indexSet0->color(ur1_descr0) = boost::gray_color;
      if (Type::value(*(ur1->interaction()->nonSmoothLaw())) != Type::EqualityConditionNSL)
      {
        if (predictorDeactivate(ur1, i))
        {
          // Unitary relation is not active
          // ui1 becomes invalid
          indexSet0->color(ur1_descr0) = boost::black_color;
          indexSet1->remove_vertex(ur1);
          ur1->lambda(1)->zero();
          topo->setHasChanged(true);
        }
      }
    }
    else
    {
      // UnitaryRelation is not in indexSet0 anymore.
      // ui1 becomes invalid
      indexSet1->remove_vertex(ur1);
      topo->setHasChanged(true);
    }
  }
  cout << "SEPP" << endl;

  // indexSet0\indexSet1 scan
  UnitaryRelationsGraph::VIterator ui0, ui0end;
  //Add interaction in indexSet1
  for (boost::tie(ui0, ui0end) = indexSet0->vertices(); ui0 != ui0end; ++ui0)
  {
    if (indexSet0->color(*ui0) == boost::black_color)
    {
      // reset
      indexSet0->color(*ui0) = boost::white_color ;
    }
    else
    {
      if (indexSet0->color(*ui0) == boost::gray_color)
      {
        // reset
        indexSet0->color(*ui0) = boost::white_color;

        assert(indexSet1->is_vertex(indexSet0->bundle(*ui0)));
        /*assert( { !predictorDeactivate(indexSet0->bundle(*ui0),i) ||
          Type::value(*(indexSet0->bundle(*ui0)->interaction()->nonSmoothLaw())) == Type::EqualityConditionNSL ;
          });*/
      }
      else
      {
        assert(indexSet0->color(*ui0) == boost::white_color);

        SP::UnitaryRelation ur0 = indexSet0->bundle(*ui0);
        assert(!indexSet1->is_vertex(ur0));
        if (Type::value(*(ur0->interaction()->nonSmoothLaw())) != Type::EqualityConditionNSL)
          if (predictorActivate(ur0, i))
          {
            assert(!indexSet1->is_vertex(ur0));

            // vertex and edges insertion in indexSet1
            indexSet1->copy_vertex(ur0, *indexSet0);
            topo->setHasChanged(true);
            assert(indexSet1->is_vertex(ur0));
          }
      }
    }
  }

  assert(indexSet1->size() <= indexSet0->size());

  DEBUG_PRINTF("update indexSets end : indexSet0 size : %d\n", indexSet0->size());
  DEBUG_PRINTF("update IndexSets end : indexSet1 size : %d\n", indexSet1->size());
}

void TimeSteppingD1Minus::initOSNS()
{
  // === creates links between work vector in OSI and work vector in
  // Unitary Relations
  SP::OneStepIntegrator  osi;

  ConstDSIterator itDS;

  SP::Topology topo =  model()->nonSmoothDynamicalSystem()->topology();
  SP::UnitaryRelationsGraph indexSet0 = topo->indexSet(0);

  UnitaryRelationsGraph::VIterator ui, uiend;

  // For each Unitary relation in I0 ...
  for (boost::tie(ui, uiend) = indexSet0->vertices();
       ui != uiend; ++ui)
  {
    SP::UnitaryRelation ur = indexSet0->bundle(*ui);
    indexSet0->bundle(*ui)->initialize("TimeSteppingD1Minus");
    // creates a POINTER link between workX[ds] (xfree) and the
    // corresponding unitaryBlock in each UR for each ds of the
    // current UR.
    for (itDS = ur->interaction()->dynamicalSystemsBegin();
         itDS != ur->interaction()->dynamicalSystemsEnd(); ++itDS)
    {
      //osi = osiMap[*itDS];
      ur->insertInWorkFree((*itDS)->workFree()); // osi->getWorkX(*itDS));
    }
  }


  if (!_allNSProblems->empty()) // ie if some Interactions have been
    // declared and a Non smooth problem
    // built.
  {
    //if (_allNSProblems->size()>1)
    //  RuntimeException::selfThrow("TimeSteppingD1Minus::initialize, at the time, a time stepping simulation can not have more than one non smooth problem.");

    // At the time, we consider that for all systems, levelMin is
    // equal to the minimum value of the relative degree - 1 except
    // for degree 0 case where we keep 0.

    assert(model()->nonSmoothDynamicalSystem()->topology()->isUpToDate());

    initLevelMin();

    // === update all index sets ===
    updateIndexSets();

    // initialization of  OneStepNonSmoothProblem
    for (OSNSIterator itOsns = _allNSProblems->begin(); itOsns != _allNSProblems->end(); ++itOsns)
    {
      (*itOsns)->setLevels(_levelMin, _levelMax);
      (*itOsns)->initialize(shared_from_this());
    }
  }
}

void TimeSteppingD1Minus::initLevelMin()
{
  assert(model()->nonSmoothDynamicalSystem()->topology()->minRelativeDegree() >= 0);

  _levelMin = model()->nonSmoothDynamicalSystem()->topology()->minRelativeDegree();

  if (_levelMin != 0)
    _levelMin--;
}

void TimeSteppingD1Minus::initLevelMax()
{
  _levelMax = model()->nonSmoothDynamicalSystem()->topology()->maxRelativeDegree();
  // Interactions initialization (here, since level depends on the
  // type of simulation) level corresponds to the number of Y and
  // Lambda derivatives computed.

  if (find_if(_allOSI->begin(), _allOSI->end(), cmp_osi_type_d1minuslinear) != _allOSI->end())
  {
    // at least one d1minuslinear osi found
    if (_levelMax == 0)
      _levelMax++;
    // like event driven scheme
  }
  else
  {
    // pure moreau case
    if (_levelMax != 0)
      _levelMax--;
    // level max is equal to relative degree-1. But for relative degree 0 case, we keep 0 value for _levelMax
  }
}

void TimeSteppingD1Minus::nextStep()
{
  processEvents();
}


void TimeSteppingD1Minus::update(unsigned int levelInput)
{
  // 1 - compute input (lambda -> r)
  if (!_allNSProblems->empty())
    updateInput(levelInput);

  // 2 - compute state for each dynamical system

  OSIIterator itOSI;
  for (itOSI = _allOSI->begin(); itOSI != _allOSI->end() ; ++itOSI)
    (*itOSI)->updateState(levelInput);
  /*Because the dof of DS have been updated,
    the world (CAO for example) must be updated.*/
  updateWorldFromDS();

  // 3 - compute output ( x ... -> y)
  if (!_allNSProblems->empty())
  {
    updateOutput(0, _levelMax);
  }
}

void TimeSteppingD1Minus::computeFreeState()
{
  std::for_each(_allOSI->begin(), _allOSI->end(), boost::bind(&OneStepIntegrator::computeFreeState, _1));
}

// compute simulation between current and next event.  Initial
// DS/interaction state is given by memory vectors and final state is
// the one saved in DS/Interaction at the end of this function
void TimeSteppingD1Minus::computeOneStep()
{
  advanceToEvent();
}


void TimeSteppingD1Minus::computeInitialResidu()
{
  //  cout<<"BEGIN computeInitialResidu"<<endl;
  double tkp1 = getTkp1();



  // SP::InteractionsSet allInteractions = model()->nonSmoothDynamicalSystem()->interactions();
  // for (InteractionsIterator it = allInteractions->begin(); it != allInteractions->end(); it++)
  // {
  //   (*it)->relation()->computeh(tkp1);
  //   (*it)->relation()->computeg(tkp1);
  // }


  double time = model()->currentTime();
  assert(abs(time - tkp1) < 1e-14);




  updateOutput(0, _levelMax);
  updateInput(_levelMin);



  SP::DynamicalSystemsGraph dsGraph = model()->nonSmoothDynamicalSystem()->dynamicalSystems();
  for (DynamicalSystemsGraph::VIterator vi = dsGraph->begin(); vi != dsGraph->end(); ++vi)
  {
    dsGraph->bundle(*vi)->updatePlugins(tkp1);
  }

  SP::InteractionsSet allInteractions = model()->nonSmoothDynamicalSystem()->interactions();

  for (OSIIterator it = _allOSI->begin(); it != _allOSI->end() ; ++it)
    (*it)->computeResidu();
  if (_computeResiduY)
    for (InteractionsIterator it = allInteractions->begin(); it != allInteractions->end(); it++)
    {
      (*it)->relation()->computeResiduY(tkp1);
    }

  //  cout<<"END computeInitialResidu"<<endl;
}

void TimeSteppingD1Minus::run()
{
  unsigned int count = 0; // events counter.
  // do simulation while events remains in the "future events" list of
  // events manager.
  cout << " ==== Start of " << Type::name(*this) << " simulation - This may take a while ... ====" << endl;
  while (_eventsManager->hasNextEvent())
  {

    //  if (_newtonOptions=="linear" || )
    //       advanceToEvent();

    //     else if (_newtonOptions=="Newton")
    //       newtonSolve(criterion,maxIter);

    //     else
    //       RuntimeException::selfThrow("TimeSteppingD1Minus::run(opt) failed. Unknow simulation option: "+opt);

    advanceToEvent();

    _eventsManager->processEvents();
    count++;
  }
  cout << "===== End of " << Type::name(*this) << "simulation. " << count << " events have been processed. ==== " << endl;
}

void TimeSteppingD1Minus::advanceToEvent()
{
  //   computeInitialResidu();
  //   /*advance To Event consists of one Newton iteration, here the jacobians are updated.*/
  //   prepareNewtonIteration();
  //   // solve ...
  //   computeFreeState();
  //   int info = 0;
  //   if (!_allNSProblems->empty())
  //     info = computeOneStepNSProblem(SICONOS_OSNSP_TS_VELOCITY);
  //   // Check output from solver (convergence or not ...)
  //   if (!checkSolverOutput)
  //     DefaultCheckSolverOutput(info);
  //   else
  //     checkSolverOutput(info, this);
  //   // Update
  //   update(_levelMin);

  newtonSolve(_newtonTolerance, _newtonMaxIteration);



}

/*update of the nabla */
/*discretisation of the Interactions */
void   TimeSteppingD1Minus::prepareNewtonIteration()
{
  //  cout << "update the operators" <<endl ;

  DSOSIConstIterator it = _osiMap.begin();
  while (it != _osiMap.end())
  {
    if ((it->second)->getType() == OSI::MOREAU)
    {
      Moreau::convert(&(*(it->second)))->computeW(getTkp1(), it->first);
    }
    if ((it->second)->getType() == OSI::D1MINUSLINEAR)
    {
      D1MinusLinear::convert(&(*(it->second)))->computeW(getTkp1(), it->first);
    }
    ++it;
  }

  SP::InteractionsSet allInteractions = model()->nonSmoothDynamicalSystem()->interactions();
  for (InteractionsIterator it = allInteractions->begin(); it != allInteractions->end(); it++)
  {
    (*it)->relation()->computeJach(getTkp1());
    (*it)->relation()->computeJacg(getTkp1());
  }


  /*reset to zero the ds buffers*/
  SP::DynamicalSystemsGraph dsGraph = model()->nonSmoothDynamicalSystem()->dynamicalSystems();


  /* should be evaluated only if needed */
  for (DynamicalSystemsGraph::VIterator vi = dsGraph->begin(); vi != dsGraph->end(); ++vi)
  {
    dsGraph->bundle(*vi)->preparStep();
    //     (*itds)->xp()->zero();
    //     (*itds)->R()->zero();
  }
  /**/
  for (InteractionsIterator it = allInteractions->begin(); it != allInteractions->end(); it++)
  {
    (*it)->relation()->preparNewtonIteration();
  }
  bool topoHasChanged = model()->nonSmoothDynamicalSystem()->topology()->hasChanged();
  if (topoHasChanged)
    for (OSNSIterator itOsns = _allNSProblems->begin(); itOsns != _allNSProblems->end(); ++itOsns)
    {
      (*itOsns)->setHasBeUpdated(false);
    }

}
void TimeSteppingD1Minus::saveYandLambdaInMemory()
{
  // Save OSNS state (Interactions) in Memory.
  OSNSIterator itOsns;
  for (itOsns = _allNSProblems->begin(); itOsns != _allNSProblems->end(); ++itOsns)
    (*itOsns)->saveInMemory();

}
void TimeSteppingD1Minus::newtonSolve(double criterion, unsigned int maxStep)
{
  bool isNewtonConverge = false;
  _newtonNbSteps = 0; // number of Newton iterations
  int info = 0;
  //cout<<"||||||||||||||||||||||||||||||| ||||||||||||||||||||||||||||||| BEGIN NEWTON IT "<<endl;
  bool isLinear  = (_model.lock())->nonSmoothDynamicalSystem()->isLinear();
  computeInitialResidu();

  if ((_newtonOptions == SICONOS_TS_LINEAR || _newtonOptions == SICONOS_TS_LINEAR_IMPLICIT)
      || isLinear)
  {
    _newtonNbSteps++;
    prepareNewtonIteration();
    computeFreeState();
    if (!_allNSProblems->empty())
      info = computeOneStepNSProblem(SICONOS_OSNSP_TS_VELOCITY);
    // Check output from solver (convergence or not ...)
    if (!checkSolverOutput)
      DefaultCheckSolverOutput(info);
    else
      checkSolverOutput(info, this);

    update(_levelMin);

    //isNewtonConverge = newtonCheckConvergence(criterion);

    saveYandLambdaInMemory();
  }

  else if (_newtonOptions == SICONOS_TS_NONLINEAR)
  {
    while ((!isNewtonConverge) && (_newtonNbSteps < maxStep) && (!info))
    {
      _newtonNbSteps++;
      prepareNewtonIteration();
      computeFreeState();
      if (info)
        cout << "new loop because of info\n" << endl;
      if (!_allNSProblems->empty())
        info = computeOneStepNSProblem(SICONOS_OSNSP_TS_VELOCITY);
      if (info)
        cout << "info!" << endl;
      // Check output from solver (convergence or not ...)
      if (!checkSolverOutput)
        DefaultCheckSolverOutput(info);
      else
        checkSolverOutput(info, this);

      update(_levelMin);
      isNewtonConverge = newtonCheckConvergence(criterion);
      if (!isNewtonConverge && !info)
      {
        saveYandLambdaInMemory();
      }
    }
    if (!isNewtonConverge)
      cout << "TimeSteppingD1Minus::newtonSolve -- Newton process stopped: max. number of steps  reached." << endl ;
    else if (info)
      cout << "TimeSteppingD1Minus::newtonSolve -- Newton process stopped: solver failed." << endl ;
    //    else
    //      cout << "TimeSteppingD1Minus::newtonSolve succed nbit="<<_newtonNbSteps<<"maxStep="<<maxStep<<endl;
  }
  else
    RuntimeException::selfThrow("TimeSteppingD1Minus::NewtonSolve failed. Unknow newtonOptions: " + _newtonOptions);
}

bool TimeSteppingD1Minus::newtonCheckConvergence(double criterion)
{
  bool checkConvergence = true;
  //_relativeConvergenceCriterionHeld is true means that the RCC is
  //activated, and the relative criteron helds.  In this case the
  //newtonCheckConvergence has to return true. End of the Newton
  //iterations
  if (_relativeConvergenceCriterionHeld)
  {
    return true;
  }
  // get the nsds indicator of convergence
  // We compute cvg = abs(xi+1 - xi)/xi and if cvg < criterion
  //  if (nsdsConverge < criterion )

  double residu = 0.0;
  _newtonResiduDSMax = 0.0;
  for (OSIIterator it = _allOSI->begin(); it != _allOSI->end() ; ++it)
  {
    residu = (*it)->computeResidu();

    if (residu > _newtonResiduDSMax) _newtonResiduDSMax = residu;
    if (residu > criterion)
    {
      checkConvergence = false;
      //break;
    }
  }




  if (_computeResiduY)
  {
    //check residuy.
    _newtonResiduYMax = 0.0;
    residu = 0.0;
    SP::InteractionsSet allInteractions = model()->nonSmoothDynamicalSystem()->interactions();
    for (InteractionsIterator it = allInteractions->begin(); it != allInteractions->end(); it++)
    {
      (*it)->relation()->computeResiduY(getTkp1());
      residu = (*it)->relation()->residuY()->norm2();
      if (residu > _newtonResiduYMax) _newtonResiduYMax = residu;
      if (residu > criterion)
      {
        //      cout<<"residuY > criteron"<<residu<<">"<<criterion<<endl;
        checkConvergence = false;
        //break;
      }
    }
  }
  if (_computeResiduR)
  {
    //check residur.
    _newtonResiduRMax = 0.0;
    residu = 0.0;
    SP::InteractionsSet allInteractions = model()->nonSmoothDynamicalSystem()->interactions();
    for (InteractionsIterator it = allInteractions->begin(); it != allInteractions->end(); it++)
    {
      (*it)->relation()->computeResiduR(getTkp1());
      residu = (*it)->relation()->residuR()->norm2();
      cout << "residuR =" << residu << ">" << criterion << endl;
      if (residu > _newtonResiduRMax) _newtonResiduRMax = residu;
      if (residu > criterion)
      {
        cout << "residuR > criteron" << residu << ">" << criterion << endl;
        checkConvergence = false;
        //break;
      }
    }

  }

  return(checkConvergence);
}

TimeSteppingD1Minus* TimeSteppingD1Minus::convert(Simulation *str)
{
  TimeSteppingD1Minus* ts = dynamic_cast<TimeSteppingD1Minus*>(str);
  return ts;
}

void TimeSteppingD1Minus::DefaultCheckSolverOutput(int info)
{
  // info = 0 => ok
  // else: depend on solver
  if (info != 0)
  {
    cout << "TimeSteppingD1Minus::check non smooth solver output warning: output message from solver is equal to " << info << endl;
    //       cout << "=> may have failed? (See Numerics solver documentation for details on the message meaning)." << endl;
    //      cout << "=> may have failed? (See Numerics solver documentation for details on the message meaning)." << endl;
    //     RuntimeException::selfThrow(" Non smooth problem, solver convergence failed ");
    /*      if(info == 1)
            cout <<" reach max iterations number with solver " << solverName << endl;
            else if (info == 2)
            {
            if (solverName == "LexicoLemke" || solverName == "CPG" || solverName == "NLGS")
            RuntimeException::selfThrow(" negative diagonal term with solver "+solverName);
            else if (solverName == "QP" || solverName == "NSQP" )
            RuntimeException::selfThrow(" can not satisfy convergence criteria for solver "+solverName);
            else if (solverName == "Latin")
            RuntimeException::selfThrow(" Choleski factorisation failed with solver Latin");
            }
            else if (info == 3 && solverName == "CPG")
            cout << "pWp null in solver CPG" << endl;
            else if (info == 3 && solverName == "Latin")
            RuntimeException::selfThrow("Null diagonal term with solver Latin");
            else if (info == 5 && (solverName == "QP" || solverName == "NSQP"))
            RuntimeException::selfThrow("Length of working array insufficient in solver "+solverName);
            else
            RuntimeException::selfThrow("Unknown error type in solver "+ solverName);
            */
  }
}

void TimeSteppingD1Minus::setCheckSolverFunction(CheckSolverFPtr newF)
{
  checkSolverOutput = newF;
}
