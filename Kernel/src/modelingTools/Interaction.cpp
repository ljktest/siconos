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
#include <assert.h>

// #define DEBUG_STDOUT
// #define DEBUG_MESSAGES
#include "debug.h"

#include "Interaction.hpp"
#include "RelationTypes.hpp"
#include "ComplementarityConditionNSL.hpp"
#include "RelayNSL.hpp"
#include "NewtonImpactNSL.hpp"
#include "NewtonImpactFrictionNSL.hpp"
#include "DynamicalSystem.hpp"

#include "LagrangianDS.hpp"

#include "FirstOrderR.hpp"
#include "LagrangianR.hpp"
#include "NewtonEulerR.hpp" // ??
#include "NewtonEulerDS.hpp" // ??

#include "BlockVector.hpp"
#include "FirstOrderNonLinearDS.hpp"

using namespace std;
using namespace RELATION;

// --- CONSTRUCTORS ---

/* initialisation with empty set */
Interaction::Interaction(unsigned int interactionSize, SP::NonSmoothLaw NSL, SP::Relation rel, unsigned int number):
  _initialized(false), _number(number), _interactionSize(interactionSize), _sizeOfDS(0), _has2Bodies(false),
  _y(2),  _nslaw(NSL), _relation(rel)
{}

void Interaction::initialize(double t0, VectorOfBlockVectors& DSlink, VectorOfVectors& workVInter, VectorOfSMatrices& workMInter, SiconosMatrix& osnsMInter, DynamicalSystem& ds1, VectorOfVectors& workV1, DynamicalSystem& ds2, VectorOfVectors& workV2, bool computeResiduY, bool computeResiduR)
{
  DEBUG_PRINTF("Interaction::initialize(double t0) with t0 = %f \n", t0);

  if (!_initialized)
  {
    assert(relation() && "Interaction::initialize failed, relation() == NULL");

    assert(nslaw() && "Interaction::initialize failed, non smooth law == NULL");

    // compute number of relations.

    if (_interactionSize != nslaw()->size())
    {
      RuntimeException::selfThrow("Interaction::initialize() - _interactionSize != nslaw()->size() . Obsolete !");
    }

    initData(DSlink);
    // Initialize interaction work vectors, depending on Dynamical systems
    // linked to the interaction.
    initDSData(ds1, workV1, DSlink);

    if(&ds1 != &ds2)
    {
      initDSData(ds2, workV2, DSlink);
    }

    initializeMemory(computeResiduY);

    // Relation initializes the work vectors and matrices
    _relation->initialize(*this, DSlink, workVInter, workMInter);

    if (computeResiduR)
    {
      RELATION::TYPES relationType = _relation->getType();
      if (relationType == FirstOrder)
      {
        if (!workVInter[FirstOrderRVec::g_alpha])
          workVInter[FirstOrderRVec::g_alpha].reset(new SiconosVector(_sizeOfDS));
        if (!workVInter[FirstOrderRVec::residuR])
          workVInter[FirstOrderRVec::residuR].reset(new SiconosVector(_sizeOfDS));
      }
      else if (relationType == Lagrangian)
        RuntimeException::selfThrow("Interaction::initialize() - computeResiduR for LagrangianR is not implemented");
      else if (relationType == NewtonEuler)
        RuntimeException::selfThrow("Interaction::initialize() - computeResiduR for NewtonEulerR is not implemented");
    }

    if (_steps > 1) // Multi--step methods
    {
      // Comoyte the old Values of Output with stored values in Memory
      for (unsigned int k = 0; k < _steps - 1; k++)
      {
        /** ComputeOutput to fill the Memory
         * We assume the state x is stored in xMemory except for the  initial
         * condition which has not been swap yet.
         */
        //        relation()->LinkDataFromMemory(k);
        for (unsigned int i = 0; i < _upperLevelForOutput + 1; ++i)
        {
          computeOutput(t0, DSlink, workVInter, workMInter, osnsMInter, i);
          _yMemory[i]->swap(*_y[i]);
        }
      }
    }

    // Compute y values for t0
    for (unsigned int i = 0; i < _upperLevelForOutput + 1; ++i)
    {
      computeOutput(t0, DSlink, workVInter, workMInter, osnsMInter, i);
    }
    _initialized = true;
  }
}

// Initialize and InitializeMemory are separated in two functions
// since we need to know the relative degree to know
// "numberOfDerivatives", while numberOfRelations and the size of the
// non smooth law are required inputs to compute the relative degree.
void Interaction::initializeMemory(bool computeResiduY)
{

  DEBUG_PRINT("Interaction::initializeMemory() \n");
  // Warning: this function is called from Simulation initialize,
  // since we need to know :
  // the levels _lowerLevelForOutput and _upperLevelForOutput to size Y
  // and the levels _lowerLevelForInput and _upperLevelForInput to size  Lambda.
  // this depends on many criteria (simulation type, osi type, ds type, nonsmooth type)
  // and they are computed in Simulation::computeLevelsForInputAndOutput

  // Memory allocation for y and lambda

  //  assert(_upperLevelForOutput >=0);
  assert(_upperLevelForOutput >= _lowerLevelForOutput);
  //  assert(_upperLevelForInput >=0);
  assert(_upperLevelForInput >= _lowerLevelForInput);



  // in order to simplify we size from 0 to _upperLevelForXXX
  _y.resize(_upperLevelForOutput + 1) ;
  _yOld.resize(_upperLevelForOutput + 1);
  _y_k.resize(_upperLevelForOutput + 1);

  _lambda.resize(_upperLevelForInput + 1);
  _lambdaOld.resize(_upperLevelForInput + 1);

  _yMemory.resize(_upperLevelForOutput + 1);
  _lambdaMemory.resize(_upperLevelForInput + 1);



  // get the dimension of the non smooth law, ie the size of an Interaction blocks (one per relation)
  unsigned int nslawSize = nslaw()->size();
  if (computeResiduY)
  {
    _h_alpha.reset(new SiconosVector(nslawSize));
    _residuY.reset(new SiconosVector(nslawSize));
  }
  _yForNSsolver.reset(new SiconosVector(nslawSize));

  for (unsigned int i = _lowerLevelForOutput ;
       i < _upperLevelForOutput + 1 ;
       i++)
  {
    _y[i].reset(new SiconosVector(nslawSize));
    _yOld[i].reset(new SiconosVector(nslawSize));
    _y_k[i].reset(new SiconosVector(nslawSize));
    assert(_steps > 0);
    _yMemory[i].reset(new SiconosMemory(_steps, nslawSize));
    _y[i]->zero();
    _yOld[i]->zero();
    _y_k[i]->zero();
  }


  for (unsigned int i = _lowerLevelForInput ;
       i < _upperLevelForInput + 1 ;
       i++)
  {
    DEBUG_PRINTF("Interaction::initializeMemory(). _lambda[%i].reset()\n",i)
    _lambda[i].reset(new SiconosVector(nslawSize));
    _lambdaOld[i].reset(new SiconosVector(nslawSize));
    _lambdaOld[i]->zero();
    _lambdaMemory[i].reset(new SiconosMemory(_steps, nslawSize));
  }
}
void Interaction::resetAllLambda()
{
   for (unsigned int i = _lowerLevelForInput ;
       i < _upperLevelForInput + 1 ;
       i++)
  {
    if (_lambda[i])
      _lambda[i]->zero();
  }

}


void Interaction::resetLambda(unsigned int level)
{
  if (_lambda[level])
    _lambda[level]->zero();
}

void Interaction::initData(VectorOfBlockVectors& DSlink)
{
  RELATION::TYPES relationType = _relation->getType();
  if (relationType == FirstOrder)
    initDataFirstOrder(DSlink);
  else if (relationType == Lagrangian)
    initDataLagrangian(DSlink);
  else if (relationType == NewtonEuler)
    initDataNewtonEuler(DSlink);
  else
    RuntimeException::selfThrow("Interaction::initData unknown initialization procedure for \
        a relation of type: " + relationType);

}

void Interaction::initDSData(DynamicalSystem& ds, VectorOfVectors& workVDS, VectorOfBlockVectors& DSlink)
{
  RELATION::TYPES relationType = _relation->getType();
  if (relationType == FirstOrder)
    initDSDataFirstOrder(ds, workVDS, DSlink);
  else if (relationType == Lagrangian)
    initDSDataLagrangian(ds, workVDS, DSlink);
  else if (relationType == NewtonEuler)
    initDSDataNewtonEuler(ds, workVDS, DSlink);
  else
    RuntimeException::selfThrow("Interaction::initDSData unknown initialization procedure for \
        a relation of type: " + relationType);
}

// It could be interesting to make Interaction a pure virtual class and to derive 3
// classes, one for each type of relation
void Interaction::initDataFirstOrder(VectorOfBlockVectors& DSlink)
{
  // Get the DS concerned by the interaction of this relation
  DSlink.resize(FirstOrderRDS::DSlinkSize);
  DSlink[FirstOrderRDS::x].reset(new BlockVector());
  DSlink[FirstOrderRDS::xfree].reset(new BlockVector());
  DSlink[FirstOrderRDS::xPartialNS].reset(new BlockVector());
  DSlink[FirstOrderRDS::deltax].reset(new BlockVector()); // displacements
  DSlink[FirstOrderRDS::r].reset(new BlockVector());
  DSlink[FirstOrderRDS::z].reset(new BlockVector());
}

void Interaction::initDSDataFirstOrder(DynamicalSystem& ds, VectorOfVectors& workVDS, VectorOfBlockVectors& DSlink)
{
  // Put x/r ... of each DS into a block. (Pointers links, no copy!!)
  FirstOrderNonLinearDS& lds = static_cast<FirstOrderNonLinearDS&>(ds);
  DSlink[FirstOrderRDS::x]->insertPtr(lds.x());
  DSlink[FirstOrderRDS::xfree]->insertPtr(workVDS[FirstOrderDS::xfree]);
  DSlink[FirstOrderRDS::xPartialNS]->insertPtr(workVDS[FirstOrderDS::xPartialNS]);
  DSlink[FirstOrderRDS::deltax]->insertPtr(workVDS[FirstOrderDS::deltaxForRelation]);
  DSlink[FirstOrderRDS::r]->insertPtr(lds.r());
  DSlink[FirstOrderRDS::z]->insertPtr(lds.z());
}

void Interaction::initDataLagrangian(VectorOfBlockVectors& DSlink)
{

  DEBUG_PRINT("Interaction::initDataLagrangian()\n");
  DSlink.resize(LagrangianRDS::DSlinkSize);
  DSlink[LagrangianRDS::xfree].reset(new BlockVector());
  DSlink[LagrangianRDS::q0].reset(new BlockVector()); // displacement
  DSlink[LagrangianRDS::q1].reset(new BlockVector()); // velocity
  DSlink[LagrangianRDS::q2].reset(new BlockVector()); // acceleration
  DSlink[LagrangianRDS::p0].reset(new BlockVector());
  DSlink[LagrangianRDS::p1].reset(new BlockVector());
  DSlink[LagrangianRDS::p2].reset(new BlockVector());
  DSlink[LagrangianRDS::z].reset(new BlockVector());
}

void Interaction::initDSDataLagrangian(DynamicalSystem& ds, VectorOfVectors& workVDS, VectorOfBlockVectors& DSlink)
{
  // check dynamical system type
  assert((Type::value(ds) == Type::LagrangianLinearTIDS ||
          Type::value(ds) == Type::LagrangianDS));

  // convert vDS systems into LagrangianDS and put them in vLDS
  LagrangianDS& lds = static_cast<LagrangianDS&> (ds);

  // Put q/velocity/acceleration of each DS into a block. (Pointers links, no copy!!)
//  DSlink[LagrangianRDS::xfree]->insertPtr(workVDS[LagrangianDS::xfree]);
  DSlink[LagrangianRDS::xfree]->insertPtr(ds.workspace(DynamicalSystem::free));
  DSlink[LagrangianRDS::q0]->insertPtr(lds.q());

  DEBUG_PRINTF("DSlink[LagrangianRDS::q0]->insertPtr(lds.q()) with LagrangianR::q0 = %i\n",LagrangianR::q0);
  DEBUG_EXPR(DSlink[LagrangianRDS::q0]->display());
  DEBUG_EXPR(lds.q()->display());
  DEBUG_EXPR(std::cout << DSlink[LagrangianRDS::q0] << std::endl;);

  DSlink[LagrangianRDS::q1]->insertPtr(lds.velocity());
  DSlink[LagrangianRDS::q2]->insertPtr(lds.acceleration());
  DSlink[LagrangianRDS::z]->insertPtr(lds.z());

  // Put NonsmoothInput _p of each DS into a block. (Pointers links, no copy!!)
  for (unsigned int k = _lowerLevelForInput;
       k < _upperLevelForInput + 1; k++)
  {
    assert(lds.p(k));
    assert(DSlink[LagrangianRDS::p0 + k]);
    DSlink[LagrangianRDS::p0 + k]->insertPtr(lds.p(k));
  }
}

void Interaction::initDataNewtonEuler(VectorOfBlockVectors& DSlink)
{
  DSlink.resize(NewtonEulerRDS::DSlinkSize);
  DSlink[NewtonEulerRDS::xfree].reset(new BlockVector());
  DSlink[NewtonEulerRDS::q0].reset(new BlockVector()); // displacement
  DSlink[NewtonEulerRDS::velocity].reset(new BlockVector()); // velocity
//  DSlink[NewtonEulerRDS::deltaq].reset(new BlockVector());
  DSlink[NewtonEulerRDS::dotq].reset(new BlockVector()); // qdot
  //  data[NewtonEulerRDS::q2].reset(new BlockVector()); // acceleration
  DSlink[NewtonEulerRDS::z].reset(new BlockVector()); // z vector
  DSlink[NewtonEulerRDS::p0].reset(new BlockVector());
  DSlink[NewtonEulerRDS::p1].reset(new BlockVector());
  DSlink[NewtonEulerRDS::p2].reset(new BlockVector());
}

void Interaction::initDSDataNewtonEuler(DynamicalSystem& ds, VectorOfVectors& workVDS, VectorOfBlockVectors& DSlink)
{
  // check dynamical system type
  assert((Type::value(ds) == Type::NewtonEulerDS) && "Interaction initDSData failed, not implemented for dynamical system of that type.\n");

  // convert vDS systems into NewtonEulerDS and put them in vLDS
  NewtonEulerDS& lds = static_cast<NewtonEulerDS&>(ds);
  // Put q/velocity/acceleration of each DS into a block. (Pointers links, no copy!!)
//  DSlink[NewtonEulerRDS::xfree]->insertPtr(workVDS[NewtonEulerDS::xfree]);
  DSlink[NewtonEulerRDS::xfree]->insertPtr(ds.workspace(DynamicalSystem::free));
  DSlink[NewtonEulerRDS::q0]->insertPtr(lds.q());
  DSlink[NewtonEulerRDS::velocity]->insertPtr(lds.velocity());
  //  DSlink[NewtonEulerRDS::deltaq]->insertPtr(lds.deltaq());
  DSlink[NewtonEulerRDS::dotq]->insertPtr(lds.dotq());
  //    data[NewtonEulerRDS::q2]->insertPtr( lds.acceleration());
  if (lds.p(0))
      DSlink[NewtonEulerRDS::p0]->insertPtr(lds.p(0));
  if (lds.p(1))
    DSlink[NewtonEulerRDS::p1]->insertPtr(lds.p(1));
  if (lds.p(2))
    DSlink[NewtonEulerRDS::p2]->insertPtr(lds.p(2));

  DSlink[NewtonEulerRDS::z]->insertPtr(lds.z());
}
// --- GETTERS/SETTERS ---

void Interaction::setY(const VectorOfVectors& newVector)
{

  unsigned int size = newVector.size();

  _y.clear();
  _y.resize(size);

  for (unsigned int i = 0; i < size; i++)
    _y[i].reset(new SiconosVector(*(newVector[i]))); // -> copy !
}

void Interaction::setYPtr(const VectorOfVectors& newVector)
{
  _y.clear();

  // copy
  _y = newVector; // smart ptr
}

void Interaction::setY(const unsigned int  index, const SiconosVector& newY)
{
  assert(_y.size() > index &&
         "Interaction::setY, index out of range ");

  // set y[index]
  if (! _y[index])
  {
    _y[index].reset(new SiconosVector(newY));
  }
  else
  {
    assert(_y[index]->size() == newY.size() &&
           "Interaction::setY(index,newY), inconsistent sizes between y(index) and newY ");
    *(_y[index]) = newY;
  }
}

void Interaction::setYPtr(const unsigned int  index, SP::SiconosVector newY)
{
  assert(_y.size() > index &&
         "Interaction::setYPtr, index out of range");

  assert(newY->size() == _interactionSize &&
         "Interaction::setYPtr, interactionSize differs from newY vector size");

  _y[index] = newY;
}

void Interaction::setYOld(const VectorOfVectors& newVector)
{
  unsigned int size = newVector.size();
  _yOld.clear();
  _yOld.resize(size);

  for (unsigned int i = 0; i < size; i++)
    _yOld[i].reset(new SiconosVector(*(newVector[i]))); // -> copy !
}

void Interaction::setYOldPtr(const VectorOfVectors& newVector)
{
  // clear _yOld

  _yOld.clear();

  // copy
  _yOld = newVector; // smart ptr
}

void Interaction::setYOld(const unsigned int  index, const SiconosVector& newYOld)
{
  if (_yOld.size() <= index)
    RuntimeException::selfThrow("Interaction::setYOld, index out of range ");

  // set _yOld[index]
  if (! _yOld[index])
  {
    _yOld[index].reset(new SiconosVector(newYOld));
  }
  else
  {
    assert(_yOld[index]->size() == newYOld.size() &&
           "Interaction::setYOld(index,newYOld), inconsistent sizes between yOld(index) and newYOld");
    *(_yOld[index]) = newYOld;
  }
}

void Interaction::setYOldPtr(const unsigned int  index, SP::SiconosVector newYOld)
{
  assert(_yOld.size() > index &&
         "Interaction::setYOldPtr, index out of range");

  assert(newYOld->size() == _interactionSize &&
         "Interaction::setYOldPtr, interactionSize differs from newYOld vector size");

  _yOld[index] = newYOld;
}

void Interaction::setLambda(const VectorOfVectors& newVector)
{
  unsigned int size = newVector.size();
  _lambda.clear();
  _lambda.resize(size);

  for (unsigned int i = 0; i < size; i++)
    _lambda[i].reset(new SiconosVector(*(newVector[i]))); // -> copy !
}

void Interaction::setLambdaPtr(const VectorOfVectors& newVector)
{
  _lambda.clear();

  _lambda = newVector; // smart ptr
}

void Interaction::setLambda(const unsigned int  index, const SiconosVector& newLambda)
{
  assert(_lambda.size() <= index &&
         "Interaction::setLambda, index out of range");

  // set lambda[index]
  if (! _lambda[index])
  {
    _lambda[index].reset(new SiconosVector(newLambda));
  }
  else
  {
    assert(_lambda[index]->size() == newLambda.size() &&
           "Interaction::setLambda(index,newLambda), inconsistent sizes between lambda(index) and newLambda");
    *(_lambda[index]) = newLambda;
  }
}

void Interaction::setLambdaPtr(const unsigned int  index, SP::SiconosVector newLambda)
{
  assert(_lambda.size() > index &&
         "Interaction::setLambdaPtr, index out of range ");

  assert(newLambda->size() == _interactionSize &&
         "Interaction::setLambdaPtr, interactionSize differs from newLambda vector size ");

  _lambda[index] = newLambda;
}

void Interaction::setLambdaOld(const VectorOfVectors& newVector)
{
  unsigned int size = newVector.size();

  // clear lambdaOld
  _lambdaOld.clear();
  _lambdaOld.resize(size);

  for (unsigned int i = 0; i < size; i++)
    _lambdaOld[i].reset(new SiconosVector(*(newVector[i]))); // -> copy !
}

void Interaction::setLambdaOldPtr(const VectorOfVectors& newVector)
{
  // clear lambdaOld
  _lambdaOld.clear();

  // copy
  _lambdaOld = newVector; // smart ptrs
}

void Interaction::setLambdaOld(const unsigned int  index, const SiconosVector& newLambdaOld)
{
  assert(_lambdaOld.size() > index &&
         "Interaction::setLambdaOld, index out of range ");

  // set lambdaOld[index]
  if (! _lambdaOld[index])
  {
    _lambdaOld[index].reset(new SiconosVector(newLambdaOld));
  }
  else
  {
    if (_lambdaOld[index]->size() != newLambdaOld.size())
      RuntimeException::selfThrow("Interaction::setLambdaOld(index,newLambdaOld), inconsistent sizes between lambdaOld(index) and newLambdaOld ");
    *(_lambdaOld[index]) = newLambdaOld;
  }
}

void Interaction::setLambdaOldPtr(const unsigned int  index, SP::SiconosVector newLambdaOld)
{
  if (_lambdaOld.size() > index)
    RuntimeException::selfThrow("Interaction::setLambdaOldPtr, index out of range ");
  if (newLambdaOld->size() != _interactionSize)
    RuntimeException::selfThrow("Interaction::setLambdaOldPtr, interactionSize differs from newLambdaOld vector size ");

  _lambdaOld[index] = newLambdaOld;
}


// --- OTHER FUNCTIONS ---

void Interaction::swapInOldVariables()
{
  // i corresponds to the derivative number and j the relation number.
  for (unsigned int i = _lowerLevelForOutput; i < _upperLevelForOutput + 1 ; i++)
  {
    assert(_y[i]);
    assert(_yOld[i]);

    *(_yOld[i]) = *(_y[i]) ;
  }

  for (unsigned int i = _lowerLevelForInput; i < _upperLevelForInput + 1  ; i++)
  {
    assert(_lambdaOld[i]);
    assert(_lambda[i]);
    *(_lambdaOld[i]) = *(_lambda[i]);
  }
}

void Interaction::swapInMemory()
{
  // i corresponds to the derivative number and j the relation number.
  for (unsigned int  i = _lowerLevelForOutput; i < _upperLevelForOutput + 1 ; i++)
  {
    *(_y_k[i]) = *(_y[i]) ;
    _yMemory[i]->swap(*_y[i]);
  }

  for (unsigned int i = _lowerLevelForInput; i < _upperLevelForInput + 1  ; i++)
  {
    _lambdaMemory[i]->swap(*_lambda[i]);
  }

}

void Interaction::display() const
{
  std::cout << "======= Interaction display =======" <<std::endl;

  if (_initialized)
    std::cout << "The interaction is initialized" <<std::endl;
  else
    cout << "The interaction is not initialized" << endl;
  cout << "| number : " << _number << endl;
  cout << "| relativeDegree : " << _relativeDegree << endl;
  cout << "| lowerLevelForOutput : " << _lowerLevelForOutput << endl;
  cout << "| upperLevelForOutput : " << _upperLevelForOutput << endl;
  cout << "| lowerLevelForInput : " << _lowerLevelForInput << endl;
  cout << "| upperLevelForInput : " << _upperLevelForInput << endl;
  cout << "| interactionSize : " << _interactionSize << endl;
  cout << "|  _sizeOfDS : " << _sizeOfDS << endl;

  _relation->display();
  if (_initialized)
  {
    for (unsigned int i = 0; i < _upperLevelForOutput + 1; i++)
    {
      std::cout << "| y[" << i  << "] : " <<std::endl;
      if (_y[i]) _y[i]->display();
      else std::cout << "->NULL" <<std::endl;
    }
    for (unsigned int i = 0; i < _upperLevelForOutput + 1; i++)
    {
      std::cout << "| yOld[" << i  << "] : " <<std::endl;
      if (_yOld[i]) _yOld[i]->display();
      else std::cout << "->NULL" <<std::endl;
    }
    for (unsigned int i = 0; i < _upperLevelForInput + 1; i++)
    {
      std::cout << "| lambda[" << i  << "] : " <<std::endl;
      if (_lambda[i]) _lambda[i]->display();
      else std::cout << "->NULL" <<std::endl;
    }

  }
  std::cout << "===================================" <<std::endl;
}

void Interaction::computeOutput(double time, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM, SiconosMatrix& osnsM, unsigned int derivativeNumber)
{
  relation()->computeOutput(time, *this, DSlink, workV, workM, osnsM, derivativeNumber);
}

void Interaction::computeInput(double time, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM, SiconosMatrix& osnsM, unsigned int level)
{
  relation()->computeInput(time, *this, DSlink, workV, workM, osnsM, level);
}




void Interaction::getLeftInteractionBlockForDS(unsigned int pos, SP::SiconosMatrix InteractionBlock, VectorOfSMatrices& workM) const
{
  SP::SiconosMatrix originalMatrix;
  RELATION::TYPES relationType = relation()->getType();
  RELATION::SUBTYPES relationSubType = relation()->getSubType();

  if (relationType == FirstOrder)
  {
    SP::SiconosMatrix CMat = std11::static_pointer_cast<FirstOrderR> (relation())->C();
    if (CMat)
      originalMatrix = CMat;
    else if (relationSubType != LinearTIR)
      originalMatrix = workM[FirstOrderRMat::C];
  }
  else if (relationType == Lagrangian)
  {
    SP::LagrangianR r = std11::static_pointer_cast<LagrangianR> (relation());
    originalMatrix = r->jachq();
  }
  else if (relationType == NewtonEuler)
  {
    SP::NewtonEulerR r = std11::static_pointer_cast<NewtonEulerR> (relation());
    originalMatrix = r->jachqT();
  }
  else
    RuntimeException::selfThrow("Interaction::getLeftInteractionBlockForDS, not yet implemented for relations of type " + relationType);

  // copy sub-interactionBlock of originalMatrix into InteractionBlock
  // dim of the sub-interactionBlock
  Index subDim(2);
  subDim[0] = InteractionBlock->size(0);
  subDim[1] = InteractionBlock->size(1);
  // Position (row,col) of first element to be read in originalMatrix
  // and of first element to be set in InteractionBlock
  Index subPos(4);
  subPos[0] = 0; //_relativePosition;
  subPos[1] = pos;
  subPos[2] = 0;
  subPos[3] = 0;
  setBlock(originalMatrix, InteractionBlock, subDim, subPos);
}

void Interaction::getLeftInteractionBlockForDSProjectOnConstraints(unsigned int pos, SP::SiconosMatrix InteractionBlock) const
{
  //Type::Siconos dsType = Type::value(*ds);
  //if (dsType != Type::NewtonEulerDS)
  //  RuntimeException::selfThrow("Interaction::getLeftInteractionBlockForDSForProject- ds is not from NewtonEulerDS.");

  RELATION::TYPES relationType = relation()->getType();
  if (relationType != NewtonEuler)
    RuntimeException::selfThrow("Interaction::getLeftInteractionBlockForDSForProject- relation is not from NewtonEulerR.");

  SP::SiconosMatrix originalMatrix;
  SP::NewtonEulerR r = std11::static_pointer_cast<NewtonEulerR> (relation());
  //proj_with_q originalMatrix = r->jachqProj();
  originalMatrix = r->jachq();

  // copy sub-interactionBlock of originalMatrix into InteractionBlock
  // dim of the sub-interactionBlock
  Index subDim(2);
  subDim[0] = InteractionBlock->size(0);
  subDim[1] = InteractionBlock->size(1);
  // Position (row,col) of first element to be read in originalMatrix
  // and of first element to be set in InteractionBlock
  Index subPos(4);
  subPos[0] = 0;//_relativePosition;
  subPos[1] = pos;
  subPos[2] = 0;
  subPos[3] = 0;
  setBlock(originalMatrix, InteractionBlock, subDim, subPos);
}

void Interaction::getRightInteractionBlockForDS(unsigned int pos, SP::SiconosMatrix InteractionBlock, VectorOfSMatrices& workM) const
{
  SP::SiconosMatrix originalMatrix; // Complete matrix, Relation member.
  RELATION::TYPES relationType = relation()->getType();
  RELATION::SUBTYPES relationSubType = relation()->getSubType();

  if (relationType == FirstOrder)
  {
    SP::SiconosMatrix BMat = std11::static_pointer_cast<FirstOrderR> (relation())->B();
    if (BMat)
      originalMatrix = BMat;
    else if (relationSubType != LinearTIR)
      originalMatrix = workM[FirstOrderRMat::B];
  }
  else if (relationType == Lagrangian || relationType == NewtonEuler)
  {
    RuntimeException::selfThrow("Interaction::getRightInteractionBlockForDS, call not permit " + relationType);
  }
  else
    RuntimeException::selfThrow("Interaction::getRightInteractionBlockForDS, not yet implemented for relations of type " + relationType);

  if (! originalMatrix)
    RuntimeException::selfThrow("Interaction::getRightInteractionBlockForDS(DS, InteractionBlock, ...): the right interactionBlock is a NULL pointer (miss matrix B or H or gradients ...in relation ?)");

  // copy sub-interactionBlock of originalMatrix into InteractionBlock
  // dim of the sub-interactionBlock
  Index subDim(2);
  subDim[0] = InteractionBlock->size(0);
  subDim[1] = InteractionBlock->size(1);
  // Position (row,col) of first element to be read in originalMatrix
  // and of first element to be set in InteractionBlock
  Index subPos(4);
  subPos[0] = pos;
  subPos[1] = 0;//_relativePosition;
  subPos[2] = 0;
  subPos[3] = 0;
  setBlock(originalMatrix, InteractionBlock, subDim, subPos);
}

void Interaction::getExtraInteractionBlock(SP::SiconosMatrix InteractionBlock, VectorOfSMatrices& workM) const
{
  // !!! Warning: we suppose that D is interactionBlock diagonal, ie that
  // there is no coupling between Interaction through D !!!  Any
  // coupling between relations through D must be taken into account
  // thanks to the nslaw (by "increasing" its dimension).

  RELATION::TYPES relationType = relation()->getType();
  RELATION::SUBTYPES relationSubType = relation()->getSubType();
  SP::SiconosMatrix D;

  if (relationType == FirstOrder)
  {
    SP::SiconosMatrix DMat = std11::static_pointer_cast<FirstOrderR> (relation())->D();
    if (DMat)
      D = DMat;
    else if (relationSubType != LinearTIR)
      D = workM[FirstOrderRMat::D];
  }
  else if (relationType == Lagrangian)
  {
    D = std11::static_pointer_cast<LagrangianR> (relation())->jachlambda();
  }
  else if (relationType == NewtonEuler)
  {
    D = std11::static_pointer_cast<NewtonEulerR> (relation())->jachlambda();
  }
  else
    RuntimeException::selfThrow("Interaction::getLeftInteractionBlockForDS, not yet implemented for relations of type " + relationType);

  if (!D)
  {
    InteractionBlock->zero();
    return; //ie no extra interactionBlock
  }

  // copy sub-interactionBlock of originalMatrix into InteractionBlock
  // dim of the sub-interactionBlock
  Index subDim(2);
  subDim[0] = InteractionBlock->size(0);
  subDim[1] = InteractionBlock->size(1);
  // Position (row,col) of first element to be read in originalMatrix
  // and of first element to be set in InteractionBlock
  Index subPos(4);
  subPos[0] = 0;//_relativePosition;
  subPos[1] = 0;//_relativePosition;
  subPos[2] = 0;
  subPos[3] = 0;
  setBlock(D, InteractionBlock, subDim, subPos);
}

void Interaction::doExtraForCentralInteractionBlockForDS(SiconosMatrix& m, VectorOfSMatrices& workM, double h) const
{
  RELATION::TYPES relationType = relation()->getType();
  if ((relationType == FirstOrder) && (workM[FirstOrderRMat::Ktilde]))
  {
    workM[FirstOrderRMat::Ktilde]->PLUForwardBackwardInPlace(m);
    SP::SimpleMatrix K = std11::static_pointer_cast<FirstOrderR>(_relation)->K();
    if (!K) K = workM[FirstOrderRMat::K];
    prod(*K, m, *workM[FirstOrderRMat::Khat], true);
    *workM[FirstOrderRMat::Khat] *= h;
  }
}

void Interaction::computeResiduY(double time)
{
  //Residu_y = y_alpha_k+1 - H_alpha;
  *_residuY = *_h_alpha;
  scal(-1, *_residuY, *_residuY);

  (*_residuY) += *(y(0));

}

void Interaction::computeResiduR(double time, VectorOfBlockVectors& DSlink, VectorOfVectors& workV)
{
  //Residu_r = r_alpha_k+1 - g_alpha;
  *workV[FirstOrderRVec::residuR] = *DSlink[FirstOrderRDS::r];
  *workV[FirstOrderRVec::residuR] -= *workV[FirstOrderRVec::g_alpha];

//RuntimeException::selfThrow("Interaction::computeResiduR do not use this function");
}
