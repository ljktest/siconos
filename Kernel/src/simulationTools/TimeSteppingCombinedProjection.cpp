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

#include "TimeStepping.hpp"

#include "TimeSteppingCombinedProjection.hpp"
#include "NewtonEulerDS.hpp"
#include "NewtonEulerFrom1DLocalFrameR.hpp"
#include "OneStepIntegrator.hpp"



using namespace std;
//#define TSPROJ_DEBUG
//#define CORRECTIONSVELOCITIES

#define DEBUG_MESSAGES
#define DEBUG_WHERE_MESSAGES
#include <debug.h>



TimeSteppingCombinedProjection::TimeSteppingCombinedProjection(SP::TimeDiscretisation td,
    SP::OneStepIntegrator osi,
    SP::OneStepNSProblem osnspb_velo,
    SP::OneStepNSProblem osnspb_pos,
    unsigned int level)
  : TimeStepping(td, osi, osnspb_velo)
{
  (*_allNSProblems).resize(SICONOS_NB_OSNSP_TSP);
  insertNonSmoothProblem(osnspb_pos, SICONOS_OSNSP_TS_POS);

  _indexSetLevelForProjection = level;
  _constraintTol = 1e-04;
  _constraintTolUnilateral = 1e-08;
  _projectionMaxIteration = 10;
  _doCombinedProj = 1;
  _isIndexSetsStable = false;
}

// --- Destructor ---
TimeSteppingCombinedProjection::~TimeSteppingCombinedProjection()
{
}


void TimeSteppingCombinedProjection::computeLevelsForInputAndOutput(SP::Interaction inter, bool init)
{
  Simulation::computeLevelsForInputAndOutput(inter, init);
  // Add the  specific indexSets
  if (!init) // We are not computing the levels at the initialization
  {
    SP::Topology topo = model()->nonSmoothDynamicalSystem()->topology();
    unsigned int indxSize = topo->indexSetsSize();
    topo->indexSetsResize(indxSize + 1);
    topo->resetIndexSetPtr(indxSize);
  }
}



void TimeSteppingCombinedProjection::initOSNS()
{
  TimeStepping::initOSNS();

  (*_allNSProblems)[SICONOS_OSNSP_TS_POS]->setLevelMin(_indexSetLevelForProjection);
  (*_allNSProblems)[SICONOS_OSNSP_TS_POS]->setLevelMax(_indexSetLevelForProjection);
}

void TimeSteppingCombinedProjection::advanceToEvent()
{
  _isIndexSetsStable = false;
  SP::Topology topo = model()->nonSmoothDynamicalSystem()->topology();
  if (model()->nonSmoothDynamicalSystem()->topology()->numberOfIndexSet() > _indexSetLevelForProjection)
  {
    SP::UnitaryRelationsGraph indexSet1 = topo->indexSet(1);
    SP::UnitaryRelationsGraph indexSet2 = topo->indexSet(2);
    assert(indexSet1);
    assert(indexSet2);

    indexSet1->clear();
    indexSet2->clear();
  }


  while (!_isIndexSetsStable)
  {
    if (model()->nonSmoothDynamicalSystem()->topology()->numberOfIndexSet() > _indexSetLevelForProjection)
    {
      updateIndexSet(1);
    }
    else
      _isIndexSetsStable = true;
    /** First step, Solve the standard velocity formulation.*/
#ifdef TSPROJ_DEBUG
    cout << "TimeStepping::newtonSolve begin :\n";
#endif
    TimeStepping::newtonSolve(_newtonTolerance, _newtonMaxIteration);
#ifdef TSPROJ_DEBUG
    cout << "TimeStepping::newtonSolve end : Number of iterations=" << getNewtonNbSteps() << "\n";
    cout << "                              : newtonResiduDSMax=" << newtonResiduDSMax() << "\n";
    cout << "                              : newtonResiduYMax=" << newtonResiduYMax() << "\n";
    cout << "                              : newtonResiduRMax=" << newtonResiduRMax() << "\n";
#endif
    if (model()->nonSmoothDynamicalSystem()->topology()->numberOfIndexSet() > _indexSetLevelForProjection)
    {
      updateIndexSet(2);
    }
    if (!_doCombinedProj)
      return;
    int info = 0;

    /** Second step, Perform the projection on constraints.*/
#ifdef TSPROJ_DEBUG
    cout << "TimeSteppingCombinedProjection::newtonSolve begin projection:\n";
#endif
    SP::DynamicalSystemsGraph dsGraph = model()->nonSmoothDynamicalSystem()->dynamicalSystems();


    bool runningProjection = false;
    unsigned int cmp = 0;
    SP::InteractionsSet allInteractions = model()->nonSmoothDynamicalSystem()->interactions();


    if (model()->nonSmoothDynamicalSystem()->topology()->numberOfIndexSet() > _indexSetLevelForProjection)
      computeCriteria(&runningProjection);

    while (runningProjection && cmp < _projectionMaxIteration)
    {
      cmp++;
#ifdef TSPROJ_DEBUG
      printf("TimeSteppingCombinedProjection projection step = %d\n", cmp);
#endif
      info = 0;
#ifdef TSPROJ_DEBUG
      cout << "TimeSteppingProjectOnConstraint compute OSNSP." << endl ;
#endif
      info = computeOneStepNSProblem(SICONOS_OSNSP_TS_POS);

      if (info)
      {
        cout << "TimeSteppingCombinedProjection1 project on constraints failed." << endl ;
        return;
      }

      for (DynamicalSystemsGraph::VIterator aVi2 = dsGraph->begin(); aVi2 != dsGraph->end(); ++aVi2)
      {
        SP::DynamicalSystem ds = dsGraph->bundle(*aVi2);
        Type::Siconos dsType = Type::value(*ds);
        if (dsType == Type::NewtonEulerDS)
        {
          SP::NewtonEulerDS neds = boost::static_pointer_cast<NewtonEulerDS>(ds);
          neds->normalizeq();
          neds->updateT();
        }
        else if (dsType == Type::LagrangianDS || dsType == Type::LagrangianLinearTIDS)
        {

        }
        else
          RuntimeException::selfThrow("TimeSteppingCombinedProjection :: - Ds is not from NewtonEulerDS neither from LagrangianDS.");

      }

      updateWorldFromDS();

      computeCriteria(&runningProjection);

      //cout<<"||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  Z:"<<endl;
      //(*_allNSProblems)[SICONOS_OSNSP_TS_POS]->display();
      //(boost::static_pointer_cast<LinearOSNS>((*_allNSProblems)[SICONOS_OSNSP_TS_POS]))->z()->display();
      if (info)
        cout << "TimeSteppingCombinedProjection2 project on constraints failed." << endl ;
#ifdef TSPROJ_DEBUG
      cout << "TimeSteppingCombinedProjection::Projection end : Number of iterations=" << cmp << "\n";
#endif

      //cout<<"during projection before normalizing of q:\n";
      //for (InteractionsIterator it = allInteractions->begin(); it != allInteractions->end(); it++)
      //{
      //  (*it)->relation()->computeh(getTkp1());
      //}
    }
#ifdef TSPROJ_DEBUG
    cout << "TimeSteppingCombinedProjection::newtonSolve end projection:\n";
#endif
  }
  return;
}

void TimeSteppingCombinedProjection::computeCriteria(bool * runningProjection)
{

  SP::UnitaryRelationsGraph indexSet = model()->nonSmoothDynamicalSystem()->topology()->indexSet(_indexSetLevelForProjection);
  UnitaryRelationsGraph::VIterator aVi, viend;

  double maxViolationEquality = -1e24;
  double minViolationEquality = +1e24;
  double maxViolationUnilateral = -1e24;
  double minViolationUnilateral = +1e24;

  *runningProjection = false;

  for (boost::tie(aVi, viend) = indexSet->vertices();
       aVi != viend; ++aVi)
  {
    SP::UnitaryRelation UR = indexSet->bundle(*aVi);
    SP::Interaction interac = UR->interaction();
    interac->relation()->computeOutput(getTkp1(), 0);
    interac->relation()->computeJach(getTkp1());
    if (Type::value(*(interac->nonSmoothLaw())) ==  Type::NewtonImpactFrictionNSL ||
        Type::value(*(interac->nonSmoothLaw())) == Type::NewtonImpactNSL)
    {
      double criteria = interac->y(0)->getValue(0);
#ifdef TSPROJ_DEBUG
      printf("unilatreal interac->y(0)->getValue(0) %e.\n", interac->y(0)->getValue(0));
#endif

      if (criteria > maxViolationUnilateral) maxViolationUnilateral = criteria;
      if (criteria < minViolationUnilateral) minViolationUnilateral = criteria;
      if (criteria < - _constraintTolUnilateral)
      {
        *runningProjection = true;
#ifdef TSPROJ_DEBUG
        printf("TSProj newton criteria unilateral true %e.\n", criteria);
#endif
      }
    }
    else
    {
      if (interac->y(0)->normInf()  > maxViolationEquality) maxViolationEquality = interac->y(0)->normInf() ;
      if (interac->y(0)->normInf()  < minViolationEquality) minViolationEquality = interac->y(0)->normInf() ;
      if (interac->y(0)->normInf() > _constraintTol)
      {
        *runningProjection = true;
#ifdef TSPROJ_DEBUG
        printf("TSProj  newton criteria equality true %e.\n", interac->y(0)->normInf());
#endif
      }
    }

  }
#ifdef TSPROJ_DEBUG
  printf("TSProj newton min/max criteria projection\n");
  std::cout << "                 runningProjection"  << *runningProjection << std::endl;
  printf("              min criteria equality =  %e.\n", minViolationEquality);
  printf("              max criteria equality =  %e.\n", maxViolationEquality);
  printf("              max criteria unilateral =  %e.\n", maxViolationUnilateral);
  printf("              min criteria unilateral =  %e.\n", minViolationUnilateral);
#endif
}


void TimeSteppingCombinedProjection::updateIndexSet(unsigned int i)
{
  // To update IndexSet i: add or remove UnitaryRelations from
  // this set, depending on y values.
  // boost::default_color_type is used to organize update in UnitaryRelationsGraph:
  // - white_color : undiscovered vertex (UnitaryRelation)
  // - gray_color : discovered vertex (UnitaryRelation) but searching descendants
  // - black_color : discovered vertex (UnitaryRelation) together with the descendants

  assert(!_model.expired());
  assert(model()->nonSmoothDynamicalSystem());
  assert(model()->nonSmoothDynamicalSystem()->topology());

  SP::Topology topo = model()->nonSmoothDynamicalSystem()->topology();

  assert(i < topo->indexSetsSize() &&
         "TimeStepping::updateIndexSet(i), indexSets[i] does not exist.");
  // IndexSets[0] must not be updated in simulation, since it belongs to Topology.
  assert(i > 0 &&
         "TimeStepping::updateIndexSet(i=0), indexSets[0] cannot be updated.");

  // For all Unitary Relations in indexSet[i-1], compute y[i-1] and
  // update the indexSet[i].
  SP::UnitaryRelationsGraph indexSet0 = topo->indexSet(0);
  SP::UnitaryRelationsGraph indexSet1 = topo->indexSet(1);
  SP::UnitaryRelationsGraph indexSet2 = topo->indexSet(2);
  assert(indexSet0);
  assert(indexSet1);
  assert(indexSet2);

  topo->setHasChanged(false);

  DEBUG_PRINTF("update indexSets start : indexSet0 size : %i\n", (int)(indexSet0->size()));
  DEBUG_PRINTF("update IndexSets start : indexSet1 size : %i\n", (int)(indexSet1->size()));
  DEBUG_PRINTF("update IndexSets start : indexSet2 size : %i\n", (int)(indexSet2->size()));

  // Check indexSet1
  UnitaryRelationsGraph::VIterator ui1, ui1end, v1next;
  boost::tie(ui1, ui1end) = indexSet1->vertices();

  //Remove interactions from the indexSet1
  for (v1next = ui1; ui1 != ui1end; ui1 = v1next)
  {
    ++v1next;
    SP::UnitaryRelation ur1 = indexSet1->bundle(*ui1);
    if (!(indexSet0->is_vertex(ur1)))
    {
      // UnitaryRelation is not in indexSet0 anymore.
      // ui1 becomes invalid
      indexSet1->remove_vertex(ur1);
      topo->setHasChanged(true);
    }
  }

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
        bool activate = true;
        if (Type::value(*(ur0->interaction()->nonSmoothLaw())) != Type::EqualityConditionNSL)
        {
          SP::Interaction inter =  ur0->interaction();
          DSIterator itDS = inter->dynamicalSystemsBegin();
          SP::OneStepIntegrator Osi = integratorOfInteraction(inter);
          activate = Osi->addInteractionInIndexSet(inter, i);
        }
        if (activate)
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

  indexSet2->clear();
  // Scan indexSet1
  for (v1next = ui1; ui1 != ui1end; ui1 = v1next)
  {
    ++v1next;
    SP::UnitaryRelation ur1 = indexSet1->bundle(*ui1);
    bool activate = true;
    if (Type::value(*(ur1->interaction()->nonSmoothLaw())) != Type::EqualityConditionNSL)
    {
      SP::Interaction inter =  ur1->interaction();
      DSIterator itDS = inter->dynamicalSystemsBegin();
      SP::OneStepIntegrator Osi = integratorOfInteraction(inter);
      activate = Osi->addInteractionInIndexSet(inter, i);
    }
    if (activate)
    {
      assert(!indexSet2->is_vertex(ur1));

      // vertex and edges insertion in indexSet2
      indexSet2->copy_vertex(ur1, *indexSet1);
      topo->setHasChanged(true);
      assert(indexSet2->is_vertex(ur1));
    }
  }






  DEBUG_PRINTF("update indexSets end : indexSet0 size : %i\n", (int)(indexSet0->size()));
  DEBUG_PRINTF("update IndexSets end : indexSet1 size : %i\n", (int)(indexSet1->size()));
  DEBUG_PRINTF("update IndexSets end : indexSet2 size : %i\n", (int)(indexSet2->size()));


}
