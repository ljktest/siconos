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
#include "MoreauJeanCombinedProjectionOSI.hpp"
#include "Simulation.hpp"
#include "LagrangianDS.hpp"
#include "NewtonEulerDS.hpp"

//#define DEBUG_STDOUT
//#define DEBUG_MESSAGES
//#define DEBUG_WHERE_MESSAGES
#include <debug.h>

MoreauJeanCombinedProjectionOSI::MoreauJeanCombinedProjectionOSI(double theta) : MoreauJeanOSI(theta)
{
  _levelMinForOutput= 0;
  _levelMaxForOutput =1;
  _levelMinForInput =0;
  _levelMaxForInput =1;
  //_integratorType = OSI::MOREAUDIRECTPROJECTIONOSI;
}


void MoreauJeanCombinedProjectionOSI::initializeInteraction(double t0, Interaction &inter,
                                          InteractionProperties& interProp,
                                          DynamicalSystemsGraph & DSG)
{
  DEBUG_BEGIN("MoreauJeanOSI::initializeInteraction(...)\n");

  bool isInitializationNeeded = false;

  if (!(inter.lowerLevelForOutput() <= _levelMinForOutput && inter.upperLevelForOutput()  >= _levelMaxForOutput ))
  {
    //RuntimeException::selfThrow("MoreauJeanCombinedProjectionOSI::initializeInteraction, we must resize _y");
    inter.setLowerLevelForOutput(_levelMinForOutput);
    inter.setUpperLevelForOutput(_levelMaxForOutput);
    isInitializationNeeded = true;
  }

  if (!(inter.lowerLevelForInput() <= _levelMinForInput && inter.upperLevelForInput() >= _levelMaxForInput ))
  {
    // RuntimeException::selfThrow("MoreauJeanCombinedProjectionOSI::initializeInteraction, we must resize _lambda");
    inter.setLowerLevelForInput(_levelMinForInput);
    inter.setUpperLevelForInput(_levelMaxForInput);
    isInitializationNeeded = true;
  }

  if (isInitializationNeeded)
    inter.init();

  MoreauJeanOSI::initializeInteraction(t0, inter, interProp, DSG);

  DEBUG_END("MoreauJeanOSI::initializeInteraction(...)\n");
}
void MoreauJeanCombinedProjectionOSI::initializeDynamicalSystem(Model& m, double t, SP::DynamicalSystem ds)
{
  DEBUG_BEGIN("MoreauJeanCombinedProjectionOSI::initializeDynamicalSystem(Model& m, double t, SP::DynamicalSystem ds) \n");
 
  MoreauJeanOSI::initializeDynamicalSystem(m, t, ds);
  
  const DynamicalSystemsGraph::VDescriptor& dsv = _dynamicalSystemsGraph->descriptor(ds);
  VectorOfVectors& workVectors = *_dynamicalSystemsGraph->properties(dsv).workVectors;
  Type::Siconos dsType = Type::value(*ds);
  if(dsType == Type::LagrangianDS || dsType == Type::LagrangianLinearTIDS)
  {
    SP::LagrangianDS d = std11::static_pointer_cast<LagrangianDS> (ds);
    workVectors[OneStepIntegrator::qtmp].reset(new SiconosVector(d->ndof()));
  }
  else if(dsType == Type::NewtonEulerDS)
  {
    SP::NewtonEulerDS d = std11::static_pointer_cast<NewtonEulerDS>(ds);
    workVectors[OneStepIntegrator::qtmp].reset(new SiconosVector(d->getqDim()));
  }
  else
  {
    RuntimeException::selfThrow("MoreauJeanCombinedProjectionOSI::initialize() - DS not of the right type");
  }
  for (unsigned int k = _levelMinForInput ; k < _levelMaxForInput + 1; k++)
  {
    ds->initializeNonSmoothInput(k);
  }
  
  DEBUG_END("MoreauJeanCombinedProjectionOSI::initializeDynamicalSystem(Model& m, double t, SP::DynamicalSystem ds) \n");
}

void MoreauJeanCombinedProjectionOSI::initialize(Model& m)
{
  MoreauJeanOSI::initialize(m);
}





bool MoreauJeanCombinedProjectionOSI::addInteractionInIndexSet(SP::Interaction inter, unsigned int i)
{
  assert(i == 1 || i == 2);
  //double h = _simulation->timeStep();
  if(i == 1)  // index set for resolution at the velocity
  {
    double y = (inter->y(0))->getValue(0); // y(0) is the position
    DEBUG_PRINTF("MoreauJeanCombinedProjectionOSI::addInteractionInIndexSet yref=%e \n", y);
    DEBUG_EXPR(
      if(y <= 0)
      printf("MoreauJeanCombinedProjectionOSI::addInteractionInIndexSet ACTIVATE in indexSet level = %i.\n", i);
    )
      return (y <= 0);
  }
  else if(i == 2)   //  special index for the projection
  {
    DEBUG_EXPR(
      double lambda = 0;
      lambda = (inter->lambda(1))->getValue(0); // lambda(1) is the contact impulse for MoreauJeanOSI scheme
      printf("MoreauJeanCombinedProjectionOSI::addInteractionInIndexSet lambdaref=%e \n", lambda);
      if(lambda > 0)
      printf("MoreauJeanCombinedProjectionOSI::addInteractionInIndexSet ACTIVATE in indexSet level = %i.\n", i);
    )
      //    return (lambda > 0);
      return true;
  }
  else
  {
    return false;
  }
}


bool MoreauJeanCombinedProjectionOSI::removeInteractionInIndexSet(SP::Interaction inter, unsigned int i)
{
  assert(0);
  return(0);
}

