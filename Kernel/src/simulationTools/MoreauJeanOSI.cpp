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
#include "MoreauJeanOSI.hpp"
#include "Simulation.hpp"
#include "Model.hpp"
#include "NonSmoothDynamicalSystem.hpp"
#include "NewtonEulerDS.hpp"
#include "LagrangianLinearTIDS.hpp"
#include "NewtonEulerR.hpp"
#include "LagrangianRheonomousR.hpp"
#include "NewtonImpactNSL.hpp"
#include "MultipleImpactNSL.hpp"
#include "NewtonImpactFrictionNSL.hpp"
#include "CxxStd.hpp"

#include "BlockVector.hpp"

//#define DEBUG_STDOUT
//#define DEBUG_MESSAGES
//#define DEBUG_WHERE_MESSAGES
#include <debug.h>


using namespace RELATION;
// --- constructor from a minimum set of data ---
MoreauJeanOSI::MoreauJeanOSI(SP::DynamicalSystem ds, double theta, double gamma) :
  OneStepIntegrator(OSI::MOREAUJEANOSI), _useGammaForRelation(false)
{
  OSIDynamicalSystems->insert(ds);
  _theta = theta;
  if (!isnan(gamma))
  {
    _gamma = gamma;
    _useGamma = true;
  }
  else
  {
    _gamma = 1.0;
    _useGamma = false;
  }
}

// --- constructor from a set of data ---
MoreauJeanOSI::MoreauJeanOSI(double theta, double gamma):
  OneStepIntegrator(OSI::MOREAUJEANOSI), _useGammaForRelation(false)
{
  _theta = theta;
  if (!isnan(gamma))
  {
    _gamma = gamma;
    _useGamma = true;
  }
  else
  {
    _gamma = 1.0;
    _useGamma = false;
  }
}

const SimpleMatrix MoreauJeanOSI::getW(SP::DynamicalSystem ds)
{
  int dsN = ds->number();
  assert(ds &&
         "MoreauJeanOSI::getW(ds): ds == NULL.");
  //    return *(WMap[0]);
  assert(WMap[dsN] &&
         "MoreauJeanOSI::getW(ds): W[ds] == NULL.");
  return *(WMap[dsN]); // Copy !!
}

SP::SimpleMatrix MoreauJeanOSI::W(SP::DynamicalSystem ds)
{
  assert(ds && "MoreauJeanOSI::W(ds): ds == NULL.");
  //  return WMap[0];
  //  if(WMap[ds]==NULL)
  //    RuntimeException::selfThrow("MoreauJeanOSI::W(ds): W[ds] == NULL.");
  return WMap[ds->number()];
}

void MoreauJeanOSI::setW(const SiconosMatrix& newValue, SP::DynamicalSystem ds)
{
  // Check if ds is in the OSI
  if (!OSIDynamicalSystems->isIn(ds))
    RuntimeException::selfThrow("MoreauJeanOSI::setW(newVal,ds) - ds does not belong to this Integrator ...");

  // Check dimensions consistency
  unsigned int line = newValue.size(0);
  unsigned int col  = newValue.size(1);

  if (line != col) // Check that newValue is square
    RuntimeException::selfThrow("MoreauJeanOSI::setW(newVal,ds) - newVal is not square! ");

  if (!ds)
    RuntimeException::selfThrow("MoreauJeanOSI::setW(newVal,ds) - ds == NULL.");

  unsigned int sizeW = ds->getDim(); // n for first order systems, ndof for lagrangian.
  unsigned int dsN = ds->number();
  if (line != sizeW) // check consistency between newValue and dynamical system size
    RuntimeException::selfThrow("MoreauJeanOSI::setW(newVal,ds) - unconsistent dimension between newVal and dynamical system to be integrated ");

  // Memory allocation for W, if required
  if (!WMap[dsN]) // allocate a new W if required
  {
    WMap[dsN].reset(new SimpleMatrix(newValue));
  }
  else  // or fill-in an existing one if dimensions are consistent.
  {
    if (line == WMap[dsN]->size(0) && col == WMap[dsN]->size(1))
      *(WMap[dsN]) = newValue;
    else
      RuntimeException::selfThrow("MoreauJeanOSI - setW: inconsistent dimensions with problem size for given input matrix W");
  }
}

void MoreauJeanOSI::setWPtr(SP::SimpleMatrix newPtr, SP::DynamicalSystem ds)
{
  unsigned int line = newPtr->size(0);
  unsigned int col  = newPtr->size(1);
  if (line != col) // Check that newPtr is square
    RuntimeException::selfThrow("MoreauJeanOSI::setWPtr(newVal) - newVal is not square! ");

  if (!ds)
    RuntimeException::selfThrow("MoreauJeanOSI::setWPtr(newVal,ds) - ds == NULL.");

  unsigned int sizeW = ds->getDim(); // n for first order systems, ndof for lagrangian.
  if (line != sizeW) // check consistency between newValue and dynamical system size
    RuntimeException::selfThrow("MoreauJeanOSI::setW(newVal) - unconsistent dimension between newVal and dynamical system to be integrated ");

  WMap[ds->number()] = newPtr;                  // link with new pointer
}



const SimpleMatrix MoreauJeanOSI::getWBoundaryConditions(SP::DynamicalSystem ds)
{
  assert(ds &&
         "MoreauJeanOSI::getWBoundaryConditions(ds): ds == NULL.");
  //    return *(WBoundaryConditionsMap[0]);
  unsigned int dsN = ds->number();
  assert(_WBoundaryConditionsMap[dsN] &&
         "MoreauJeanOSI::getWBoundaryConditions(ds): WBoundaryConditions[ds] == NULL.");
  return *(_WBoundaryConditionsMap[dsN]); // Copy !!
}

SP::SiconosMatrix MoreauJeanOSI::WBoundaryConditions(SP::DynamicalSystem ds)
{
  assert(ds && "MoreauJeanOSI::WBoundaryConditions(ds): ds == NULL.");
  //  return WBoundaryConditionsMap[0];
  //  if(WBoundaryConditionsMap[ds]==NULL)
  //    RuntimeException::selfThrow("MoreauJeanOSI::WBoundaryConditions(ds): W[ds] == NULL.");
  return _WBoundaryConditionsMap[ds->number()];
}


void MoreauJeanOSI::initialize()
{
  OneStepIntegrator::initialize();
  // Get initial time
  double t0 = simulationLink->model()->t0();
  // Compute W(t0) for all ds
  ConstDSIterator itDS;
  for (itDS = OSIDynamicalSystems->begin(); itDS != OSIDynamicalSystems->end(); ++itDS)
  {
    // Memory allocation for workX. workX[ds*] corresponds to xfree (or vfree in lagrangian case).
    // workX[*itDS].reset(new SiconosVector((*itDS)->getDim()));

    // W initialization
    initW(t0, *itDS);


    (*itDS)->allocateWorkVector(DynamicalSystem::local_buffer, WMap[(*itDS)->number()]->size(0));
  }

  SP::OneStepNSProblems  allOSNS  = simulationLink->oneStepNSProblems();
  ((*allOSNS)[SICONOS_OSNSP_TS_VELOCITY])->setIndexSetLevel(1);
  ((*allOSNS)[SICONOS_OSNSP_TS_VELOCITY])->setInputOutputLevel(1);
  //  ((*allOSNS)[SICONOS_OSNSP_TS_VELOCITY])->initialize(simulationLink);
}
void MoreauJeanOSI::initW(double t, SP::DynamicalSystem ds)
{
  // This function:
  // - allocate memory for a matrix W
  // - insert this matrix into WMap with ds as a key

  if (!ds)
    RuntimeException::selfThrow("MoreauJeanOSI::initW(t,ds) - ds == NULL");

  if (!OSIDynamicalSystems->isIn(ds))
    RuntimeException::selfThrow("MoreauJeanOSI::initW(t,ds) - ds does not belong to the OSI.");
  unsigned int dsN = ds->number();
  if (WMap.find(dsN) != WMap.end())
    RuntimeException::selfThrow("MoreauJeanOSI::initW(t,ds) - W(ds) is already in the map and has been initialized.");


  // Memory allocation for W
  //  WMap[ds].reset(new SimpleMatrix(sizeW,sizeW));
  //   SP::SiconosMatrix W = WMap[ds];

  double h = simulationLink->timeStep();
  Type::Siconos dsType = Type::value(*ds);


  if (dsType == Type::LagrangianDS)
  {
    SP::LagrangianDS d = std11::static_pointer_cast<LagrangianDS> (ds);
    SP::SiconosMatrix K = d->jacobianqForces(); // jacobian according to q
    SP::SiconosMatrix C = d->jacobianqDotForces(); // jacobian according to velocity
    WMap[dsN].reset(new SimpleMatrix(*d->mass())); //*W = *d->mass();

    SP::SiconosMatrix W = WMap[dsN];

    if (C)
      scal(-h * _theta, *C, *W, false); // W -= h*_theta*C

    if (K)
      scal(-h * h * _theta * _theta, *K, *W, false); //*W -= h*h*_theta*_theta**K;

    // WBoundaryConditions initialization
    if (d->boundaryConditions())
      initWBoundaryConditions(d);

  }
  // 2 - Lagrangian linear systems
  else if (dsType == Type::LagrangianLinearTIDS)
  {
    SP::LagrangianLinearTIDS d = std11::static_pointer_cast<LagrangianLinearTIDS> (ds);
    SP::SiconosMatrix K = d->K();
    SP::SiconosMatrix C = d->C();
    WMap[dsN].reset(new SimpleMatrix(*d->mass())); //*W = *d->mass();
    SP::SiconosMatrix W = WMap[dsN];

    if (C)
      scal(h * _theta, *C, *W, false); // W += h*_theta *C

    if (K)
      scal(h * h * _theta * _theta, *K, *W, false); // W = h*h*_theta*_theta*K

    // WBoundaryConditions initialization
    if (d->boundaryConditions())
      initWBoundaryConditions(d);


  }

  // === ===
  else if (dsType == Type::NewtonEulerDS)
  {
    WMap[dsN].reset(new SimpleMatrix(3, 3));
  }
  else RuntimeException::selfThrow("MoreauJeanOSI::initW - not yet implemented for Dynamical system type :" + dsType);

  // Remark: W is not LU-factorized nor inversed here.
  // Function PLUForwardBackward will do that if required.




}


void MoreauJeanOSI::initWBoundaryConditions(SP::DynamicalSystem ds)
{
  // This function:
  // - allocate memory for a matrix WBoundaryConditions
  // - insert this matrix into WBoundaryConditionsMap with ds as a key

  if (!ds)
    RuntimeException::selfThrow("MoreauJeanOSI::initWBoundaryConditions(t,ds) - ds == NULL");

  if (!OSIDynamicalSystems->isIn(ds))
    RuntimeException::selfThrow("MoreauJeanOSI::initWBoundaryConditions(t,ds) - ds does not belong to the OSI.");

  Type::Siconos dsType = Type::value(*ds);
  unsigned int dsN = ds->number();

  if (dsType == Type::LagrangianLinearTIDS || dsType == Type::LagrangianDS)
  {


    if (_WBoundaryConditionsMap.find(dsN) != _WBoundaryConditionsMap.end())
      RuntimeException::selfThrow("MoreauJeanOSI::initWBoundaryConditions(t,ds) - WBoundaryConditions(ds) is already in the map and has been initialized.");

    // Memory allocation for WBoundaryConditions
    unsigned int sizeWBoundaryConditions = ds->getDim(); // n for first order systems, ndof for lagrangian.

    SP::LagrangianDS d = std11::static_pointer_cast<LagrangianDS> (ds);

    unsigned int numberBoundaryConditions = d->boundaryConditions()->velocityIndices()->size();
    _WBoundaryConditionsMap[dsN].reset(new SimpleMatrix(sizeWBoundaryConditions, numberBoundaryConditions));
    computeWBoundaryConditions(ds);
  }
  else
    RuntimeException::selfThrow("MoreauJeanOSI::initWBoundaryConditions - not yet implemented for Dynamical system type :" + dsType);
}


void MoreauJeanOSI::computeWBoundaryConditions(SP::DynamicalSystem ds)
{
  // Compute WBoundaryConditions matrix of the Dynamical System ds, at
  // time t and for the current ds state.

  // When this function is called, WBoundaryConditionsMap[ds] is
  // supposed to exist and not to be null Memory allocation has been
  // done during initWBoundaryConditions.

  assert(ds &&
         "MoreauJeanOSI::computeWBoundaryConditions(t,ds) - ds == NULL");

  Type::Siconos dsType = Type::value(*ds);
  unsigned int dsN = ds->number();
  if (dsType == Type::LagrangianLinearTIDS || dsType == Type::LagrangianDS)
  {
    assert((_WBoundaryConditionsMap.find(dsN) != _WBoundaryConditionsMap.end()) &&
           "MoreauJeanOSI::computeW(t,ds) - W(ds) does not exists. Maybe you forget to initialize the osi?");

    SP::SimpleMatrix WBoundaryConditions = _WBoundaryConditionsMap[dsN];

    SP::SiconosVector columntmp(new SiconosVector(ds->getDim()));

    int columnindex = 0;

    std::vector<unsigned int>::iterator itindex;

    SP::LagrangianDS d = std11::static_pointer_cast<LagrangianDS> (ds);

    for (itindex = d->boundaryConditions()->velocityIndices()->begin() ;
         itindex != d->boundaryConditions()->velocityIndices()->end();
         ++itindex)
    {

      WMap[dsN]->getCol(*itindex, *columntmp);
      /*\warning we assume that W is symmetric in the Lagrangian case
        we store only the column and not the row */

      WBoundaryConditions->setCol(columnindex, *columntmp);
      double diag = (*columntmp)(*itindex);
      columntmp->zero();
      (*columntmp)(*itindex) = diag;

      WMap[dsN]->setCol(*itindex, *columntmp);
      WMap[dsN]->setRow(*itindex, *columntmp);

      columnindex ++;
    }
  }
  else
    RuntimeException::selfThrow("MoreauJeanOSI::computeWBoundaryConditions - not yet implemented for Dynamical system type :" + dsType);
}


void MoreauJeanOSI::computeW(double t, SP::DynamicalSystem ds)
{
  // Compute W matrix of the Dynamical System ds, at time t and for the current ds state.

  // When this function is called, WMap[ds] is supposed to exist and not to be null
  // Memory allocation has been done during initW.

  assert(ds &&
         "MoreauJeanOSI::computeW(t,ds) - ds == NULL");
  unsigned int dsN = ds->number();
  assert((WMap.find(dsN) != WMap.end()) &&
         "MoreauJeanOSI::computeW(t,ds) - W(ds) does not exists. Maybe you forget to initialize the osi?");

  double h = simulationLink->timeStep();
  Type::Siconos dsType = Type::value(*ds);

  SP::SiconosMatrix W = WMap[dsN];

  if (dsType == Type::LagrangianLinearTIDS)
  {
    // Nothing: W does not depend on time.
  }
  else if (dsType == Type::LagrangianDS)
  {
    SP::LagrangianDS d = std11::static_pointer_cast<LagrangianDS> (ds);
    SP::SiconosMatrix K = d->jacobianqForces(); // jacobian according to q
    SP::SiconosMatrix C = d->jacobianqDotForces(); // jacobian according to velocity

    d->computeMass();
    *W = *d->mass();

    if (C)
    {
      d->computeJacobianqDotForces(t);
      scal(-h * _theta, *C, *W, false); // W -= h*_theta*C
    }

    if (K)
    {
      d->computeJacobianqForces(t);
      scal(-h * h * _theta * _theta, *K, *W, false); //*W -= h*h*_theta*_theta**K;
    }
  }
  // === ===
  else if (dsType == Type::NewtonEulerDS)
  {
    SP::NewtonEulerDS d = std11::static_pointer_cast<NewtonEulerDS> (ds);
    d->computeJacobianvFL(t);
    double thetaFL = _theta;
    *(d->luW()) = *(d->jacobianvFL());
    scal(h * thetaFL, *(d->jacobianvFL()), *(d->luW()), true);
    *(d->luW()) += *(d->mass());

    //cout<<"MoreauJeanOSI::computeW luW before LUFact\n";
    //d->luW()->display();

    d->luW()->PLUFactorizationInPlace();
  }
  else RuntimeException::selfThrow("MoreauJeanOSI::computeW - not yet implemented for Dynamical system type :" + dsType);

  // Remark: W is not LU-factorized here.
  // Function PLUForwardBackward will do that if required.
}



double MoreauJeanOSI::computeResidu()
{
  DEBUG_PRINT("MoreauJeanOSI::computeResidu(), start\n");
  // This function is used to compute the residu for each "MoreauJeanOSI-discretized" dynamical system.
  // It then computes the norm of each of them and finally return the maximum
  // value for those norms.
  //
  // The state values used are those saved in the DS, ie the last computed ones.
  //  $\mathcal R(x,r) = x - x_{k} -h\theta f( x , t_{k+1}) - h(1-\theta)f(x_k,t_k) - h r$
  //  $\mathcal R_{free}(x,r) = x - x_{k} -h\theta f( x , t_{k+1}) - h(1-\theta)f(x_k,t_k) $

  double t = simulationLink->nextTime(); // End of the time step
  double told = simulationLink->startingTime(); // Beginning of the time step
  double h = t - told; // time step length

  DEBUG_PRINTF("nextTime %f\n", t);
  DEBUG_PRINTF("startingTime %f\n", told);
  DEBUG_PRINTF("time step size %f\n", h);


  // Operators computed at told have index i, and (i+1) at t.

  // Iteration through the set of Dynamical Systems.
  //
  DSIterator it;
  SP::DynamicalSystem ds; // Current Dynamical System.
  Type::Siconos dsType ; // Type of the current DS.

  double maxResidu = 0;
  double normResidu = maxResidu;

  for (it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    ds = *it; // the considered dynamical system
    dsType = Type::value(*ds); // Its type
    SP::SiconosVector residuFree = ds->workspace(DynamicalSystem::freeresidu);

    // 3 - Lagrangian Non Linear Systems
    if (dsType == Type::LagrangianDS)
    {
      DEBUG_PRINT("MoreauJeanOSI::computeResidu(), dsType == Type::LagrangianDS");
      // residu = M(q*)(v_k,i+1 - v_i) - h*theta*forces(t,v_k,i+1, q_k,i+1) - h*(1-theta)*forces(ti,vi,qi) - pi+1

      // -- Convert the DS into a Lagrangian one.
      SP::LagrangianDS d = std11::static_pointer_cast<LagrangianDS> (ds);

      // Get state i (previous time step) from Memories -> var. indexed with "Old"
      SP::SiconosVector qold = d->qMemory()->getSiconosVector(0);
      SP::SiconosVector vold = d->velocityMemory()->getSiconosVector(0);
      SP::SiconosVector q = d->q();


      d->computeMass();
      SP::SiconosMatrix M = d->mass();
      SP::SiconosVector v = d->velocity(); // v = v_k,i+1
      //residuFree->zero();
      DEBUG_EXPR(residuFree->display());
      DEBUG_EXPR(qold->display());
      DEBUG_EXPR(vold->display());
      DEBUG_EXPR(M->display());


      //    std::cout << "(*v-*vold)->norm2()" << (*v-*vold).norm2() << std::endl;

      prod(*M, (*v - *vold), *residuFree); // residuFree = M(v - vold)


      if (d->forces())  // if fL exists
      {
        // computes forces(ti,vi,qi)
        d->computeForces(told, qold, vold);
        double coef = -h * (1 - _theta);
        // residuFree += coef * fL_i
        scal(coef, *d->forces(), *residuFree, false);

        // computes forces(ti+1, v_k,i+1, q_k,i+1) = forces(t,v,q)
        //d->computeForces(t);
        // or  forces(ti+1, v_k,i+1, q(v_k,i+1))
        //or
        SP::SiconosVector qbasedonv(new SiconosVector(*qold));
        *qbasedonv +=  h * ((1 - _theta)* *vold + _theta * *v);
        d->computeForces(t, qbasedonv, v);
        coef = -h * _theta;
        // residuFree += coef * fL_k,i+1
        scal(coef, *d->forces(), *residuFree, false);
      }

      if (d->boundaryConditions())
      {

        d->boundaryConditions()->computePrescribedVelocity(t);

        unsigned int columnindex = 0;
        SP::SimpleMatrix WBoundaryConditions = _WBoundaryConditionsMap[ds->number()];
        SP::SiconosVector columntmp(new SiconosVector(ds->getDim()));

        for (std::vector<unsigned int>::iterator  itindex = d->boundaryConditions()->velocityIndices()->begin() ;
             itindex != d->boundaryConditions()->velocityIndices()->end();
             ++itindex)
        {

          double DeltaPrescribedVelocity =
            d->boundaryConditions()->prescribedVelocity()->getValue(columnindex)
            - vold->getValue(columnindex);

          WBoundaryConditions->getCol(columnindex, *columntmp);
          *residuFree -= *columntmp * (DeltaPrescribedVelocity);

          residuFree->setValue(*itindex, columntmp->getValue(*itindex)   * (DeltaPrescribedVelocity));

          columnindex ++;

        }
      }

      *(d->workspace(DynamicalSystem::free)) = *residuFree; // copy residuFree in Workfree
      //       std::cout << "MoreauJeanOSI::ComputeResidu LagrangianDS residufree :"  << std::endl;
      //      residuFree->display();

      DEBUG_EXPR(d->workspace(DynamicalSystem::free)->display());

      if (d->p(1))
        *(d->workspace(DynamicalSystem::free)) -= *d->p(1); // Compute Residu in Workfree Notation !!
      //       std::cout << "MoreauJeanOSI::ComputeResidu LagrangianDS residu :"  << std::endl;
      //      d->workspace(DynamicalSystem::free)->display();
      normResidu = d->workspace(DynamicalSystem::free)->norm2();
    }
    // 4 - Lagrangian Linear Systems
    else if (dsType == Type::LagrangianLinearTIDS)
    {
      DEBUG_PRINT("MoreauJeanOSI::computeResidu(), dsType == Type::LagrangianLinearTIDS");
      // ResiduFree = h*C*v_i + h*Kq_i +h*h*theta*Kv_i+hFext_theta     (1)
      // This formulae is only valid for the first computation of the residual for v = v_i
      // otherwise the complete formulae must be applied, that is
      // ResiduFree = M(v - vold) + h*((1-theta)*(C v_i + K q_i) +theta * ( C*v + K(q_i+h(1-theta)v_i+h theta v)))
      //                     +hFext_theta     (2)
      // for v != vi, the formulae (1) is wrong.
      // in the sequel, only the equation (1) is implemented

      // -- Convert the DS into a Lagrangian one.
      SP::LagrangianLinearTIDS d = std11::static_pointer_cast<LagrangianLinearTIDS> (ds);

      // Get state i (previous time step) from Memories -> var. indexed with "Old"
      SP::SiconosVector qold = d->qMemory()->getSiconosVector(0); // qi
      SP::SiconosVector vold = d->velocityMemory()->getSiconosVector(0); //vi
      DEBUG_PRINT("MoreauJeanOSI::computeResidu(), qold :");
#ifdef DEBUG_MESSAGES
      qold->display();
#endif
      DEBUG_PRINT("MoreauJeanOSI::computeResidu(), vold :");
#ifdef DEBUG_MESSAGES
      vold->display();
#endif
      DEBUG_PRINT("MoreauJeanOSI::computeResidu(), q :");
#ifdef DEBUG_MESSAGES
      d->q()->display();
#endif

      // --- ResiduFree computation Equation (1) ---
      residuFree->zero();
      double coeff;
      // -- No need to update W --

      SP::SiconosVector v = d->velocity(); // v = v_k,i+1

      SP::SiconosMatrix C = d->C();
      if (C)
        prod(h, *C, *vold, *residuFree, false); // vfree += h*C*vi

      SP::SiconosMatrix K = d->K();
      if (K)
      {
        coeff = h * h * _theta;
        prod(coeff, *K, *vold, *residuFree, false); // vfree += h^2*_theta*K*vi
        prod(h, *K, *qold, *residuFree, false); // vfree += h*K*qi
      }

      SP::SiconosVector Fext = d->fExt();
      if (Fext)
      {
        // computes Fext(ti)
        d->computeFExt(told);
        coeff = -h * (1 - _theta);
        scal(coeff, *(d->fExt()), *residuFree, false); // vfree -= h*(1-_theta) * fext(ti)
        // computes Fext(ti+1)
        d->computeFExt(t);
        coeff = -h * _theta;
        scal(coeff, *(d->fExt()), *residuFree, false); // vfree -= h*_theta * fext(ti+1)
      }


      // Computation of the complete residual Equation (2)
      //   ResiduFree = M(v - vold) + h*((1-theta)*(C v_i + K q_i) +theta * ( C*v + K(q_i+h(1-theta)v_i+h theta v)))
      //                     +hFext_theta     (2)
      //       SP::SiconosMatrix M = d->mass();
      //       SP::SiconosVector realresiduFree (new SiconosVector(*residuFree));
      //       realresiduFree->zero();
      //       prod(*M, (*v-*vold), *realresiduFree); // residuFree = M(v - vold)
      //       SP::SiconosVector qkplustheta (new SiconosVector(*qold));
      //       qkplustheta->zero();
      //       *qkplustheta = *qold + h *((1-_theta)* *vold + _theta* *v);
      //       if (C){
      //         double coef = h*(1-_theta);
      //         prod(coef, *C, *vold , *realresiduFree, false);
      //         coef = h*(_theta);
      //         prod(coef,*C, *v , *realresiduFree, false);
      //       }
      //       if (K){
      //         double coef = h*(1-_theta);
      //         prod(coef,*K , *qold , *realresiduFree, false);
      //         coef = h*(_theta);
      //         prod(coef,*K , *qkplustheta , *realresiduFree, false);
      //       }

      //       if (Fext)
      //       {
      //         // computes Fext(ti)
      //         d->computeFExt(told);
      //         coeff = -h*(1-_theta);
      //         scal(coeff, *Fext, *realresiduFree, false); // vfree -= h*(1-_theta) * fext(ti)
      //         // computes Fext(ti+1)
      //         d->computeFExt(t);
      //         coeff = -h*_theta;
      //         scal(coeff, *Fext, *realresiduFree, false); // vfree -= h*_theta * fext(ti+1)
      //       }


      if (d->boundaryConditions())
      {
        d->boundaryConditions()->computePrescribedVelocity(t);

        unsigned int columnindex = 0;
        SP::SimpleMatrix WBoundaryConditions = _WBoundaryConditionsMap[ds->number()];
        SP::SiconosVector columntmp(new SiconosVector(ds->getDim()));

        for (std::vector<unsigned int>::iterator  itindex = d->boundaryConditions()->velocityIndices()->begin() ;
             itindex != d->boundaryConditions()->velocityIndices()->end();
             ++itindex)
        {

          double DeltaPrescribedVelocity =
            d->boundaryConditions()->prescribedVelocity()->getValue(columnindex)
            - vold->getValue(columnindex);

          WBoundaryConditions->getCol(columnindex, *columntmp);
          *residuFree += *columntmp * (DeltaPrescribedVelocity);

          residuFree->setValue(*itindex, - columntmp->getValue(*itindex)   * (DeltaPrescribedVelocity));

          columnindex ++;

        }
      }


      //        std::cout << "MoreauJeanOSI::ComputeResidu LagrangianLinearTIDS residufree :"  << std::endl;
      //       residuFree->display();

      //        std::cout << "MoreauJeanOSI::ComputeResidu LagrangianLinearTIDS realresiduFree :"  << std::endl;
      //       realresiduFree->display();

      (* d->workspace(DynamicalSystem::free)) = *residuFree; // copy residuFree in Workfree
      if (d->p(1))
        *(d->workspace(DynamicalSystem::free)) -= *d->p(1); // Compute Residu in Workfree Notation !!

      //      std::cout << "MoreauJeanOSI::ComputeResidu LagrangianLinearTIDS residu :"  << std::endl;
      //      d->workspace(DynamicalSystem::free)->display();

      //      *realresiduFree-= *d->p(1);
      //       std::cout << "MoreauJeanOSI::ComputeResidu LagrangianLinearTIDS realresidu :"  << std::endl;
      //      realresiduFree->display();


      //     normResidu = d->workspace(DynamicalSystem::free)->norm2();
      normResidu = 0.0; // we assume that v = vfree + W^(-1) p
      //     normResidu = realresiduFree->norm2();

    }
    else if (dsType == Type::NewtonEulerDS)
    {
      // residu = M(q*)(v_k,i+1 - v_i) - h*_theta*forces(t,v_k,i+1, q_k,i+1) - h*(1-_theta)*forces(ti,vi,qi) - pi+1

      // -- Convert the DS into a Lagrangian one.
      SP::NewtonEulerDS d = std11::static_pointer_cast<NewtonEulerDS> (ds);

      // Get state i (previous time step) from Memories -> var. indexed with "Old"
      SP::SiconosVector qold = d->qMemory()->getSiconosVector(0);
      SP::SiconosVector vold = d->velocityMemory()->getSiconosVector(0);

      SP::SiconosVector q = d->q();


      SP::SiconosMatrix massMatrix = d->mass();
      SP::SiconosVector v = d->velocity(); // v = v_k,i+1
      prod(*massMatrix, (*v - *vold), *residuFree); // residuFree = M(v - vold)
      if (d->forces())  // if fL exists
      {
        // computes forces(ti,vi,qi)
        SP::SiconosVector fLold = d->fLMemory()->getSiconosVector(0);
        double _thetaFL = 0.5;
        double coef = -h * (1 - _thetaFL);
        // residuFree += coef * fL_i
        scal(coef, *fLold, *residuFree, false);
        d->computeForces(t);
        //        printf("cpmputeFreeState d->FL():\n");
        //  d->forces()->display();
        coef = -h * _thetaFL;
        scal(coef, *d->forces(), *residuFree, false);
      }
      *(d->workspace(DynamicalSystem::free)) = *residuFree;
      //cout<<"MoreauJeanOSI::computeResidu :\n";
      // residuFree->display();
      if (d->p(1))
        *(d->workspace(DynamicalSystem::free)) -= *d->p(1);
      normResidu = d->workspace(DynamicalSystem::free)->norm2();
    }
    else
      RuntimeException::selfThrow("MoreauJeanOSI::computeResidu - not yet implemented for Dynamical system type: " + dsType);

    if (normResidu > maxResidu) maxResidu = normResidu;

  }
  return maxResidu;
}

void MoreauJeanOSI::computeFreeState()
{
  // This function computes "free" states of the DS belonging to this Integrator.
  // "Free" means without taking non-smooth effects into account.

  double t = simulationLink->nextTime(); // End of the time step

  // Operators computed at told have index i, and (i+1) at t.

  //  Note: integration of r with a theta method has been removed
  //  SiconosVector *rold = static_cast<SiconosVector*>(d->rMemory()->getSiconosVector(0));

  // Iteration through the set of Dynamical Systems.
  //
  DSIterator it; // Iterator through the set of DS.

  SP::DynamicalSystem ds; // Current Dynamical System.
  SP::SiconosMatrix W; // W MoreauJeanOSI matrix of the current DS.
  Type::Siconos dsType ; // Type of the current DS.
  for (it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    ds = *it; // the considered dynamical system
    dsType = Type::value(*ds); // Its type
    W = WMap[ds->number()]; // Its W MoreauJeanOSI matrix of iteration.
    // 3 - Lagrangian Non Linear Systems
    if (dsType == Type::LagrangianDS)
    {
      // IN to be updated at current time: W, M, q, v, fL
      // IN at told: qi,vi, fLi

      // Note: indices i/i+1 corresponds to value at the beginning/end of the time step.
      // Index k stands for Newton iteration and thus corresponds to the last computed
      // value, ie the one saved in the DynamicalSystem.
      // "i" values are saved in memory vectors.

      // vFree = v_k,i+1 - W^{-1} ResiduFree
      // with
      // ResiduFree = M(q_k,i+1)(v_k,i+1 - v_i) - h*theta*forces(t,v_k,i+1, q_k,i+1) - h*(1-theta)*forces(ti,vi,qi)

      // -- Convert the DS into a Lagrangian one.
      SP::LagrangianDS d = std11::static_pointer_cast<LagrangianDS> (ds);

      // Get state i (previous time step) from Memories -> var. indexed with "Old"
      SP::SiconosVector qold = d->qMemory()->getSiconosVector(0);
      SP::SiconosVector vold = d->velocityMemory()->getSiconosVector(0);

      // --- ResiduFree computation ---
      // ResFree = M(v-vold) - h*[theta*forces(t) + (1-theta)*forces(told)]
      //
      // vFree pointer is used to compute and save ResiduFree in this first step.
      SP::SiconosVector vfree = d->workspace(DynamicalSystem::free);//workX[d];
      (*vfree) = *(d->workspace(DynamicalSystem::freeresidu));

      // -- Update W --
      // Note: during computeW, mass and jacobians of fL will be computed/
      computeW(t, d);
      SP::SiconosVector v = d->velocity(); // v = v_k,i+1

      // -- vfree =  v - W^{-1} ResiduFree --
      // At this point vfree = residuFree
      // -> Solve WX = vfree and set vfree = X
      W->PLUForwardBackwardInPlace(*vfree);
      // -> compute real vfree
      *vfree *= -1.0;
      *vfree += *v;

    }
    // 4 - Lagrangian Linear Systems
    else if (dsType == Type::LagrangianLinearTIDS)
    {
      // IN to be updated at current time: Fext
      // IN at told: qi,vi, fext
      // IN constants: K,C

      // Note: indices i/i+1 corresponds to value at the beginning/end of the time step.
      // "i" values are saved in memory vectors.

      // vFree = v_i + W^{-1} ResiduFree    // with
      // ResiduFree = (-h*C -h^2*theta*K)*vi - h*K*qi + h*theta * Fext_i+1 + h*(1-theta)*Fext_i

      // -- Convert the DS into a Lagrangian one.
      SP::LagrangianLinearTIDS d = std11::static_pointer_cast<LagrangianLinearTIDS> (ds);

      // Get state i (previous time step) from Memories -> var. indexed with "Old"
      SP::SiconosVector qold = d->qMemory()->getSiconosVector(0); // qi
      SP::SiconosVector vold = d->velocityMemory()->getSiconosVector(0); //vi

      // --- ResiduFree computation ---

      // vFree pointer is used to compute and save ResiduFree in this first step.


      // Velocity free and residu. vFree = RESfree (pointer equality !!).
      SP::SiconosVector vfree = d->workspace(DynamicalSystem::free);//workX[d];
      (*vfree) = *(d->workspace(DynamicalSystem::freeresidu));

      W->PLUForwardBackwardInPlace(*vfree);
      *vfree *= -1.0;
      *vfree += *vold;

    }
    else if (dsType == Type::NewtonEulerDS)
    {
      // IN to be updated at current time: W, M, q, v, fL
      // IN at told: qi,vi, fLi

      // Note: indices i/i+1 corresponds to value at the beginning/end of the time step.
      // Index k stands for Newton iteration and thus corresponds to the last computed
      // value, ie the one saved in the DynamicalSystem.
      // "i" values are saved in memory vectors.

      // vFree = v_k,i+1 - W^{-1} ResiduFree
      // with
      // ResiduFree = M(q_k,i+1)(v_k,i+1 - v_i) - h*theta*forces(t,v_k,i+1, q_k,i+1) - h*(1-theta)*forces(ti,vi,qi)

      // -- Convert the DS into a Lagrangian one.
      SP::NewtonEulerDS d = std11::static_pointer_cast<NewtonEulerDS> (ds);
      computeW(t, d);
      // Get state i (previous time step) from Memories -> var. indexed with "Old"
      SP::SiconosVector qold = d->qMemory()->getSiconosVector(0);
      SP::SiconosVector vold = d->velocityMemory()->getSiconosVector(0);

      // --- ResiduFree computation ---
      // ResFree = M(v-vold) - h*[theta*forces(t) + (1-theta)*forces(told)]
      //
      // vFree pointer is used to compute and save ResiduFree in this first step.
      SP::SiconosVector vfree = d->workspace(DynamicalSystem::free);//workX[d];
      (*vfree) = *(d->workspace(DynamicalSystem::freeresidu));
      //*(d->vPredictor())=*(d->workspace(DynamicalSystem::freeresidu));

      // -- Update W --
      // Note: during computeW, mass and jacobians of fL will be computed/
      SP::SiconosVector v = d->velocity(); // v = v_k,i+1

      // -- vfree =  v - W^{-1} ResiduFree --
      // At this point vfree = residuFree
      // -> Solve WX = vfree and set vfree = X
      //    std::cout<<"MoreauJeanOSI::computeFreeState residu free"<<endl;
      //    vfree->display();
      d->luW()->PLUForwardBackwardInPlace(*vfree);
      //    std::cout<<"MoreauJeanOSI::computeFreeState -WRfree"<<endl;
      //    vfree->display();
      //    scal(h,*vfree,*vfree);
      // -> compute real vfree
      *vfree *= -1.0;
      *vfree += *v;
    }
    else
      RuntimeException::selfThrow("MoreauJeanOSI::computeFreeState - not yet implemented for Dynamical system type: " + dsType);
  }

}

void MoreauJeanOSI::prepareNewtonIteration(double time)
{
  ConstDSIterator itDS;
  for (itDS = OSIDynamicalSystems->begin(); itDS != OSIDynamicalSystems->end(); ++itDS)
  {
    computeW(time, *itDS);
  }
}


struct MoreauJeanOSI::_NSLEffectOnFreeOutput : public SiconosVisitor
{
  using SiconosVisitor::visit;

  OneStepNSProblem * _osnsp;
  SP::Interaction _inter;

  _NSLEffectOnFreeOutput(OneStepNSProblem *p, SP::Interaction inter) :
    _osnsp(p), _inter(inter) {};

  void visit(const NewtonImpactNSL& nslaw)
  {
    double e;
    e = nslaw.e();
    Index subCoord(4);
    subCoord[0] = 0;
    subCoord[1] = _inter->nonSmoothLaw()->size();
    subCoord[2] = 0;
    subCoord[3] = subCoord[1];
    subscal(e, *_inter->y_k(_osnsp->inputOutputLevel()), *(_inter->yForNSsolver()), subCoord, false);
  }

  void visit(const NewtonImpactFrictionNSL& nslaw)
  {
    double e;
    e = nslaw.en();
    // Only the normal part is multiplied by e
    (*_inter->yForNSsolver())(0) +=  e * (*_inter->y_k(_osnsp->inputOutputLevel()))(0);

  }
  void visit(const EqualityConditionNSL& nslaw)
  {
    ;
  }
  void visit(const MixedComplementarityConditionNSL& nslaw)
  {
    ;
  }
};


void MoreauJeanOSI::computeFreeOutput(InteractionsGraph::VDescriptor& vertex_inter, OneStepNSProblem* osnsp)
{
  /** \warning: ensures that it can also work with two different osi for two different ds ?
   */

  SP::OneStepNSProblems  allOSNS  = simulationLink->oneStepNSProblems();
  SP::InteractionsGraph indexSet = osnsp->simulation()->indexSet(osnsp->indexSetLevel());
  SP::Interaction inter = indexSet->bundle(vertex_inter);

  VectorOfBlockVectors& DSlink = *indexSet->properties(vertex_inter).DSlink;
  // Get relation and non smooth law types
  RELATION::TYPES relationType = inter->relation()->getType();
  RELATION::SUBTYPES relationSubType = inter->relation()->getSubType();

  unsigned int sizeY = inter->nonSmoothLaw()->size();

  unsigned int relativePosition = 0;



  Index coord(8);
  coord[0] = relativePosition;
  coord[1] = relativePosition + sizeY;
  coord[2] = 0;
  coord[4] = 0;
  coord[6] = 0;
  coord[7] = sizeY;
  SP::SiconosMatrix  C;
  SP::SiconosMatrix  D;
  SP::SiconosMatrix  F;
//  SP::BlockVector deltax;
  SiconosVector& yForNSsolver = *inter->yForNSsolver();
  SP::BlockVector Xfree;

  /** \todo VA. All of these values should be stored in a node in the interactionGraph
   * corrseponding to the Interaction
   * when a MoreauJeanOSI scheme is used.
   */

//  deltax = DSlink[FirstOrderR::deltax];;

  if (relationType == NewtonEuler)
  {
    Xfree = DSlink[NewtonEulerR::xfree];
  }
  else if (relationType == Lagrangian)
  {
    Xfree = DSlink[LagrangianR::xfree];
  }

  assert(Xfree);


  SP::Interaction mainInteraction = inter;
  assert(mainInteraction);
  assert(mainInteraction->relation());

  if (relationType == NewtonEuler)
  {
    SP::SiconosMatrix CT =  std11::static_pointer_cast<NewtonEulerR>(mainInteraction->relation())->jachqT();

    if (CT)
    {
      coord[3] = CT->size(1);
      coord[5] = CT->size(1);
      assert(Xfree);
      // creates a POINTER link between workX[ds] (xfree) and the
      // corresponding interactionBlock in each Interaction for each ds of the
      // current Interaction.
      // XXX Big quirks !!! -- xhub
      subprod(*CT, *Xfree, yForNSsolver, coord, true);
    }

  }
  else
  {
    C = mainInteraction->relation()->C();

    if (C)
    {

      assert(Xfree);

      coord[3] = C->size(1);
      coord[5] = C->size(1);
      // creates a POINTER link between workX[ds] (xfree) and the
      // corresponding interactionBlock in each Interactionfor each ds of the
      // current Interaction.
      if (_useGammaForRelation)
      {
        RuntimeException::selfThrow("MoreauJeanOSI::computeFreeOutput Configuration not possible");
//        subprod(*C, *deltax, yForNSsolver, coord, true);
      }
      else
      {
        subprod(*C, *Xfree, yForNSsolver, coord, true);
      }
    }

    if (relationType == Lagrangian)
    {
      SP::SiconosMatrix ID(new SimpleMatrix(sizeY, sizeY));
      ID->eye();

      Index xcoord(8);
      xcoord[0] = 0;
      xcoord[1] = sizeY;
      xcoord[2] = 0;
      xcoord[3] = sizeY;
      xcoord[4] = 0;
      xcoord[5] = sizeY;
      xcoord[6] = 0;
      xcoord[7] = sizeY;

      // For the relation of type LagrangianRheonomousR
      if (relationSubType == RheonomousR)
      {
        if (((*allOSNS)[SICONOS_OSNSP_TS_VELOCITY]).get() == osnsp)
        {
          SiconosVector q = *DSlink[LagrangianR::q0];
          SiconosVector z = *DSlink[LagrangianR::z];

          std11::static_pointer_cast<LagrangianRheonomousR>(inter->relation())->computehDot(simulation()->getTkp1(), q, z);
          *DSlink[LagrangianR::z] = z;
          subprod(*ID, *(std11::static_pointer_cast<LagrangianRheonomousR>(inter->relation())->hDot()), yForNSsolver, xcoord, false); // y += hDot
        }
        else
          RuntimeException::selfThrow("MoreauJeanOSI::computeFreeOutput not yet implemented for SICONOS_OSNSP ");
      }
      // For the relation of type LagrangianScleronomousR
      if (relationSubType == ScleronomousR)
      {

      }
    }

  }

  if (inter->relation()->getType() == Lagrangian || inter->relation()->getType() == NewtonEuler)
  {
    _NSLEffectOnFreeOutput nslEffectOnFreeOutput = _NSLEffectOnFreeOutput(osnsp, inter);
    inter->nonSmoothLaw()->accept(nslEffectOnFreeOutput);
  }


}
void MoreauJeanOSI::integrate(double& tinit, double& tend, double& tout, int& notUsed)
{
  // Last parameter is not used (required for LsodarOSI but not for MoreauJeanOSI).

  double h = tend - tinit;
  tout = tend;

  DSIterator it;
  SP::SiconosMatrix W;
  for (it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    SP::DynamicalSystem ds = *it;
    W = WMap[ds->number()];
    Type::Siconos dsType = Type::value(*ds);

    if (dsType == Type::LagrangianLinearTIDS)
    {
      // get the ds
      SP::LagrangianLinearTIDS d = std11::static_pointer_cast<LagrangianLinearTIDS> (ds);
      // get velocity pointers for current time step
      SP::SiconosVector v = d->velocity();
      // get q and velocity pointers for previous time step
      SP::SiconosVector vold = d->velocityMemory()->getSiconosVector(0);
      SP::SiconosVector qold = d->qMemory()->getSiconosVector(0);
      // get p pointer

      SP::SiconosVector p = d->p(1);

      // velocity computation :
      //
      // v = vi + W^{-1}[ -h*C*vi - h*h*theta*K*vi - h*K*qi + h*theta*Fext(t) + h*(1-theta) * Fext(ti) ] + W^{-1}*pi+1
      //

      *v = *p; // v = p

      double coeff;
      // -- No need to update W --
      SP::SiconosMatrix C = d->C();
      if (C)
        prod(-h, *C, *vold, *v, false); // v += -h*C*vi

      SP::SiconosMatrix K = d->K();
      if (K)
      {
        coeff = -h * h * _theta;
        prod(coeff, *K, *vold, *v, false); // v += -h^2*theta*K*vi
        prod(-h, *K, *qold, *v, false); // v += -h*K*qi
      }

      SP::SiconosVector Fext = d->fExt();
      if (Fext)
      {
        // computes Fext(ti)
        d->computeFExt(tinit);
        coeff = h * (1 - _theta);
        scal(coeff, *Fext, *v, false); // v += h*(1-theta) * fext(ti)
        // computes Fext(ti+1)
        d->computeFExt(tout);
        coeff = h * _theta;
        scal(coeff, *Fext, *v, false); // v += h*theta * fext(ti+1)
      }
      // -> Solve WX = v and set v = X
      W->PLUForwardBackwardInPlace(*v);
      *v += *vold;
    }
    else RuntimeException::selfThrow("MoreauJeanOSI::integrate - not yet implemented for Dynamical system type :" + dsType);
  }
}

void MoreauJeanOSI::updateState(const unsigned int level)
{

  DEBUG_PRINT("MoreauJeanOSI::updateState(const unsigned int level)\n");

  double h = simulationLink->timeStep();

  double RelativeTol = simulationLink->relativeConvergenceTol();
  bool useRCC = simulationLink->useRelativeConvergenceCriteron();
  if (useRCC)
    simulationLink->setRelativeConvergenceCriterionHeld(true);

  DSIterator it;
  SP::SiconosMatrix W;
  for (it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    SP::DynamicalSystem ds = *it;
    W = WMap[ds->number()];
    // Get the DS type

    Type::Siconos dsType = Type::value(*ds);

    // 3 - Lagrangian Systems
    if (dsType == Type::LagrangianDS || dsType == Type::LagrangianLinearTIDS)
    {
      // get dynamical system
      SP::LagrangianDS d = std11::static_pointer_cast<LagrangianDS> (ds);

      //    SiconosVector *vfree = d->velocityFree();
      SP::SiconosVector v = d->velocity();
      bool baux = dsType == Type::LagrangianDS && useRCC && simulationLink->relativeConvergenceCriterionHeld();

      // level == LEVELMAX => p(level) does not even exists (segfault)
      if (level != LEVELMAX && d->p(level) && d->p(level)->size() > 0)
      {

        assert(((d->p(level)).get()) &&
               " MoreauJeanOSI::updateState() *d->p(level) == NULL.");
        *v = *d->p(level); // v = p
        if (d->boundaryConditions())
          for (std::vector<unsigned int>::iterator
               itindex = d->boundaryConditions()->velocityIndices()->begin() ;
               itindex != d->boundaryConditions()->velocityIndices()->end();
               ++itindex)
            v->setValue(*itindex, 0.0);
        W->PLUForwardBackwardInPlace(*v);

        *v +=  * ds->workspace(DynamicalSystem::free);
      }
      else
      {
        *v =  * ds->workspace(DynamicalSystem::free);
      }


      int bc = 0;
      SP::SiconosVector columntmp(new SiconosVector(ds->getDim()));

      if (d->boundaryConditions())
      {
        for (std::vector<unsigned int>::iterator  itindex = d->boundaryConditions()->velocityIndices()->begin() ;
             itindex != d->boundaryConditions()->velocityIndices()->end();
             ++itindex)
        {
          _WBoundaryConditionsMap[ds->number()]->getCol(bc, *columntmp);
          /*\warning we assume that W is symmetric in the Lagrangian case*/
          double value = - inner_prod(*columntmp, *v);
          if (level != LEVELMAX && d->p(level))
          {
            value += (d->p(level))->getValue(*itindex);
          }
          /* \warning the computation of reactionToBoundaryConditions take into
             account the contact impulse but not the external and internal forces.
             A complete computation of the residu should be better */
          d->reactionToBoundaryConditions()->setValue(bc, value) ;
          bc++;
        }


      }






      // Compute q
      SP::SiconosVector q = d->q();
      // Save value of q in stateTmp for future convergence computation
      if (baux)
        ds->addWorkVector(q, DynamicalSystem::local_buffer);

      //  -> get previous time step state
      SP::SiconosVector vold = d->velocityMemory()->getSiconosVector(0);
      SP::SiconosVector qold = d->qMemory()->getSiconosVector(0);
      // *q = *qold + h*(theta * *v +(1.0 - theta)* *vold)
      double coeff = h * _theta;
      scal(coeff, *v, *q) ; // q = h*theta*v
      coeff = h * (1 - _theta);
      scal(coeff, *vold, *q, false); // q += h(1-theta)*vold
      *q += *qold;

      if (baux)
      {
        ds->subWorkVector(q, DynamicalSystem::local_buffer);
        double aux = ((ds->workspace(DynamicalSystem::local_buffer))->norm2()) / (ds->normRef());
        if (aux > RelativeTol)
          simulationLink->setRelativeConvergenceCriterionHeld(false);
      }

    }
    else if (dsType == Type::NewtonEulerDS)
    {
      // get dynamical system
      SP::NewtonEulerDS d = std11::static_pointer_cast<NewtonEulerDS> (ds);
      SP::SiconosVector v = d->velocity();
      DEBUG_PRINT("MoreauJeanOSI::updateState()\n ")
      DEBUG_EXPR(d->display());
      DEBUG_PRINT("MoreauJeanOSI::updateState() prev v\n")
      DEBUG_EXPR(v->display());

      // failure on bullet sims
      // d->p(level) is checked in next condition
      // assert(((d->p(level)).get()) &&
      //       " MoreauJeanOSI::updateState() *d->p(level) == NULL.");

      if (level != LEVELMAX && d->p(level) && d->p(level)->size() > 0)
      {
        /*d->p has been fill by the Relation->computeInput, it contains
          B \lambda _{k+1}*/
        *v = *d->p(level); // v = p
        d->luW()->PLUForwardBackwardInPlace(*v);

        DEBUG_EXPR(d->p(level)->display());
        DEBUG_PRINT("MoreauJeanOSI::updatestate hWB lambda\n");
        DEBUG_EXPR(v->display());
        *v +=  * ds->workspace(DynamicalSystem::free);
      }
      else
        *v =  * ds->workspace(DynamicalSystem::free);

      DEBUG_PRINT("MoreauJeanOSI::updatestate work free\n");
      DEBUG_EXPR(ds->workspace(DynamicalSystem::free)->display());
      DEBUG_PRINT("MoreauJeanOSI::updatestate new v\n");
      DEBUG_EXPR(v->display());

      //compute q
      //first step consists in computing  \dot q.
      //second step consists in updating q.
      //
      SP::SiconosMatrix T = d->T();
      SP::SiconosVector dotq = d->dotq();
      prod(*T, *v, *dotq, true);

      DEBUG_PRINT("MoreauJeanOSI::updateState v\n");
      DEBUG_EXPR(v->display());
      DEBUG_EXPR(dotq->display());

      SP::SiconosVector q = d->q();

      //  -> get previous time step state
      SP::SiconosVector dotqold = d->dotqMemory()->getSiconosVector(0);
      SP::SiconosVector qold = d->qMemory()->getSiconosVector(0);
      // *q = *qold + h*(theta * *v +(1.0 - theta)* *vold)
      double coeff = h * _theta;
      scal(coeff, *dotq, *q) ; // q = h*theta*v
      coeff = h * (1 - _theta);
      scal(coeff, *dotqold, *q, false); // q += h(1-theta)*vold
      *q += *qold;
      DEBUG_PRINT("new q before normalizing\n");
      DEBUG_EXPR(q->display());

      //q[3:6] must be normalized
      d->normalizeq();
      /* \warning VA 02/06/2013.
       * What is the reason of doing the following computation ?
       */
      dotq->setValue(3, (q->getValue(3) - qold->getValue(3)) / h);
      dotq->setValue(4, (q->getValue(4) - qold->getValue(4)) / h);
      dotq->setValue(5, (q->getValue(5) - qold->getValue(5)) / h);
      dotq->setValue(6, (q->getValue(6) - qold->getValue(6)) / h);
      d->updateT();
    }
    else RuntimeException::selfThrow("MoreauJeanOSI::updateState - not yet implemented for Dynamical system type: " + dsType);
  }
}


bool MoreauJeanOSI::addInteractionInIndexSet(SP::Interaction inter, unsigned int i)
{
  DEBUG_PRINT("addInteractionInIndexSet(SP::Interaction inter, unsigned int i)\n");

  assert(i == 1);
  double h = simulationLink->timeStep();
  double y = (inter->y(i - 1))->getValue(0); // for i=1 y(i-1) is the position
  double yDot = (inter->y(i))->getValue(0); // for i=1 y(i) is the velocity

  double gamma = 1.0 / 2.0;
  if (_useGamma)
  {
    gamma = _gamma;
  }
  DEBUG_PRINTF("MoreauJeanOSI::addInteractionInIndexSet of level = %i yref=%e, yDot=%e, y_estimated=%e.\n", i,  y, yDot, y + gamma * h * yDot);
  y += gamma * h * yDot;
  assert(!isnan(y));
  DEBUG_EXPR(
    if (y <= 0)
    DEBUG_PRINT("MoreauJeanOSI::addInteractionInIndexSet ACTIVATE.\n");
  );
  return (y <= 0.0);
}


bool MoreauJeanOSI::removeInteractionInIndexSet(SP::Interaction inter, unsigned int i)
{
  assert(i == 1);
  double h = simulationLink->timeStep();
  double y = (inter->y(i - 1))->getValue(0); // for i=1 y(i-1) is the position
  double yDot = (inter->y(i))->getValue(0); // for i=1 y(i) is the velocity
  double gamma = 1.0 / 2.0;
  if (_useGamma)
  {
    gamma = _gamma;
  }
  DEBUG_PRINTF("MoreauJeanOSI::addInteractionInIndexSet yref=%e, yDot=%e, y_estimated=%e.\n", y, yDot, y + gamma * h * yDot);
  y += gamma * h * yDot;
  assert(!isnan(y));

  DEBUG_EXPR(
    if (y > 0)
    DEBUG_PRINT("MoreauJeanOSI::removeInteractionInIndexSet DEACTIVATE.\n");
  );
  return (y > 0.0);
}



void MoreauJeanOSI::display()
{
  OneStepIntegrator::display();

  std::cout << "====== MoreauJeanOSI OSI display ======" <<std::endl;
  DSIterator it;
  for (it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    std::cout << "--------------------------------" <<std::endl;
    std::cout << "--> W of dynamical system number " << (*it)->number() << ": " <<std::endl;
    if (WMap[(*it)->number()]) WMap[(*it)->number()]->display();
    else std::cout << "-> NULL" <<std::endl;
    std::cout << "--> and corresponding theta is: " << _theta <<std::endl;
  }
  std::cout << "================================" <<std::endl;
}
