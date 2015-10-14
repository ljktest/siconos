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
#include "NewMarkAlphaOSI.hpp"
#include "Simulation.hpp"
#include "LagrangianLinearTIDS.hpp"
#include "LagrangianScleronomousR.hpp"
#include "LagrangianR.hpp"
#include "NonSmoothLaw.hpp"
#include "NewtonEulerR.hpp"
#include "Model.hpp"
#include "NonSmoothDynamicalSystem.hpp"
#include "OneStepNSProblem.hpp"

using namespace RELATION;
//#define DEBUG_NEWMARK

NewMarkAlphaOSI::NewMarkAlphaOSI(SP::DynamicalSystem newDS, double new_beta, double new_gamma, double new_alpha_m, double new_alpha_f, bool flag = false) :
  OneStepIntegrator(OSI::NEWMARKALPHAOSI)
{
  OSIDynamicalSystems->insert(newDS);
  _beta = new_beta;
  _gamma = new_gamma;
  _alpha_m = new_alpha_m;
  _alpha_f = new_alpha_f;
  _orderDenseOutput = 5;
  _IsVelocityLevel = flag;
}


NewMarkAlphaOSI::NewMarkAlphaOSI(SP::DynamicalSystem newDS, double _rho_infty, bool flag = false) :
  OneStepIntegrator(OSI::NEWMARKALPHAOSI)
{
  OSIDynamicalSystems->insert(newDS);
  _alpha_m = (2 * _rho_infty - 1) / (_rho_infty + 1);
  _alpha_f = _rho_infty / (_rho_infty + 1);
  _gamma = 0.5 + _alpha_f - _alpha_m;
  _beta = 0.25 * std::pow((_gamma + 0.5), 2);
  _orderDenseOutput = 5.0;
  _IsVelocityLevel = flag;
}

NewMarkAlphaOSI::NewMarkAlphaOSI(double new_beta, double new_gamma, double new_alpha_m, double new_alpha_f, bool flag = false):
  OneStepIntegrator(OSI::NEWMARKALPHAOSI)
{
  _beta = new_beta;
  _gamma = new_gamma;
  _alpha_m = new_alpha_m;
  _alpha_f = new_alpha_f;
  _orderDenseOutput = 5.0;
  _IsVelocityLevel = flag;
}

NewMarkAlphaOSI::NewMarkAlphaOSI(double _rho_infty, bool flag = false):
  OneStepIntegrator(OSI::NEWMARKALPHAOSI)
{
  _alpha_m = (2 * _rho_infty - 1) / (_rho_infty + 1);
  _alpha_f = _rho_infty / (_rho_infty + 1);
  _gamma = 0.5 + _alpha_f - _alpha_m;
  _beta = 0.25 * std::pow((_gamma + 0.5), 2);
  _orderDenseOutput = 5.0;
  _IsVelocityLevel = flag;
}

const SimpleMatrix NewMarkAlphaOSI::getW(SP::DynamicalSystem ds)
{
  int dsN = ds->number();
  assert(ds && "NewMarkAlphaOSI::getW(ds): ds == NULL.");
  assert(WMap[dsN] && "NewMarkAlphaOSI::getW(ds): W[ds] == NULL.");
  return *(WMap[dsN]); // Copy !!
}

SP::SimpleMatrix NewMarkAlphaOSI::W(SP::DynamicalSystem ds)
{
  assert(ds && "NewMarkAlphaOSI::W(ds): ds == NULL.");
  assert(WMap[ds->number()] && "NewMarkAlphaOSI::W(ds): W[ds] == NULL.");
  return WMap[ds->number()];
}

void NewMarkAlphaOSI::initW(SP::DynamicalSystem ds)
{
  // This function is to allocate memory for a matrix W and insert this matrix into WMap with ds as a key

  if (!ds)
    RuntimeException::selfThrow("NewMarkAlphaOSI::initW(t,ds) - ds == NULL");

  if (!OSIDynamicalSystems->isIn(ds))
    RuntimeException::selfThrow("NewMarkAlphaOSI::initW(t,ds) - ds does not belong to the OSI.");

  unsigned int dsN = ds->number();
  if (WMap.find(dsN) != WMap.end())
    RuntimeException::selfThrow("NewMarkAlphaOSI::initW(t,ds) - W(ds) is already in the map and has been initialized.");

  WMap[dsN].reset(new SimpleMatrix(ds->getDim(), ds->getDim())); // allocate memory
  computeW(ds);
}


void NewMarkAlphaOSI::computeW(SP::DynamicalSystem ds)
{
  double beta_prime = (1 - _alpha_m) / ((1 - _alpha_f) * _beta);
  double gamma_prime = _gamma / _beta;
  double h = simulationLink->nextTime() - simulationLink->startingTime(); // step size
  if (h < 100 * MACHINE_PREC)
    RuntimeException::selfThrow("In NewMarkAlphaOSI::initW(t,ds), time integration is too small");
  // make sure that W is initialized before computing
  unsigned int dsN = ds->number();
  SP::SiconosMatrix W = WMap[dsN];
  Type::Siconos dsType = Type::value(*ds);
  SP::SiconosMatrix M;
  SP::SiconosMatrix K;
  SP::SiconosMatrix C;
  if ((dsType == Type::LagrangianDS) || (dsType == Type::LagrangianLinearTIDS))
  {
    if (dsType == Type::LagrangianDS)
    {
      SP::LagrangianDS d = std11::static_pointer_cast<LagrangianDS>(ds);
      d->computeMass(); // update mass matrix
      M = d->mass();    // mass matrix
      K = d->jacobianqForces(); // jacobian according to q
      C = d->jacobianqDotForces(); // jacobian according to velocity
    }
    else // LagrangianLinearTIDS
    {
      SP::LagrangianLinearTIDS d = std11::static_pointer_cast<LagrangianLinearTIDS>(ds);
      M = d->mass();    // mass matrix
      K = d->K();       // matrix K
      if (K)
        *K *= -1.0;     // K = -K
      C = d->C();       // matrix C
      if (C)
        *C *= -1.0;     // C = -C
    }
    // Compute W = (beta_prime/h^2)*M - (gamma_prime/h)*C - K
    scal(beta_prime / (h * h), *M, *W, true);
    if (C)
      scal(-gamma_prime / h, *C, *W, false);
    if (K)
      scal(-1.0, *K, *W, false);
    //
#ifdef DEBUG_NEWMARK
    std::cout.precision(15);
    std::cout << "Iteration matrix W: ";
    W->display();
#endif
  }
  else
  {
    RuntimeException::selfThrow("In NewMarkAlphaOSI::initW(t,ds), this type of Dynamical System is not yet implemented");
  }
}


double NewMarkAlphaOSI::computeResidu()
{
  // Compute the residual for each Dynamical system at step n and at iteration k
  // R_{n,k} = M_{n,k} ddotq_{n,k} - F_{n,k} - p_{n,k}
  // R_free = M_{n,k} ddotq_{n,k} - F_{n,k};
  // Compute norm of R_{n,k} for each DS
  // Take maximum norm of R_{n,k} over all DS
  double t = simulationLink->nextTime(); // End of the time step
  // Iteration through the set of Dynamical Systems.
  //
  DSIterator it;
  SP::DynamicalSystem ds; // Current Dynamical System.
  Type::Siconos dsType ; // Type of the current DS.
  double maxResidu = 0.0;
  double normResidu = 0.0;
  SP::SiconosVector _residu;

  for (it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    ds = *it; // the considered dynamical system
    dsType = Type::value(*ds); // Its type
    SP::SiconosVector freeR = ds->workspace(DynamicalSystem::freeresidu);
    freeR->zero();
    if ((dsType == Type::LagrangianDS) || (dsType == Type::LagrangianLinearTIDS))
    {
      // -- Convert the DS into a Lagrangian one.
      SP::LagrangianDS d = std11::static_pointer_cast<LagrangianDS>(ds);
      // get position, velocity and acceleration
      SP::SiconosVector q = d->q();
      SP::SiconosVector v = d->velocity();
      SP::SiconosVector a = d->acceleration();
      SP::SiconosVector F;
      SP::SiconosMatrix M = d->mass();
      // get the reaction force p
      SP::SiconosVector p = d->p(2);
      // Compute free residual
      prod(*M, *a, *freeR, true); // freeR = M*a
      // For LagrangianDS (non linear Lagrangian DS)
      if (dsType == Type::LagrangianDS)
      {
        // Update mass matrix
        d->computeMass();
        F = d->forces();
        if (F)
          // Compute F = F_ext - F_int - F_Gyr
          d->computeForces(t);
      }
      // For LagrangianLinearTIDS
      if (dsType == Type::LagrangianLinearTIDS)
      {
        // We need to add F_int = Cv + Kq to freeR
        SP::LagrangianLinearTIDS dtids = std11::static_pointer_cast<LagrangianLinearTIDS>(ds);
        SP::SiconosMatrix K = dtids->K();
        SP::SiconosMatrix C = dtids->C();
        F = dtids->fExt(); // Note that for LagrangianLinearTIDS, F = F_ext
        if (F)
        {
          dtids->computeFExt(t);
        }
        if (K)
          prod(*K, *q, *freeR, false); // f = M*a + C*v + K*q
        if (C)
          prod(*C, *v, *freeR, false); // freeR = M*a + C*v
      }

      *freeR -= *F;            // freeR = _workspace[freeresidu] - F
      // Compute residual
      _residu.reset(new SiconosVector(*freeR)); // _residu = freeR
      *_residu -= *p;                                 // _residu = _workspace[freeresidu] - p
      // Compute Euclidean norm of the residual
      normResidu = _residu->norm2();
      // Take maximum value of norm over all DS
      if (normResidu > maxResidu)
      {
        maxResidu = normResidu;
      }
      //
#ifdef DEBUG_NEWMARK
      std::cout.precision(15);
      std::cout << "Residu Free: ";
      freeR->display();
      std::cout << "Residu: ";
      _residu->display();
      std::cout << "ResiduMax: " << maxResidu <<std::endl;
#endif
    }
    else
    {
      RuntimeException::selfThrow("In NewMarkAlphaOSI::computeResidu(t,ds), this type of Dynamical System is not yet implemented");
    }
  }
  return maxResidu;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NewMarkAlphaOSI::computeFreeState()
{
  // Compute delta q_free = - ((W_{n,k})^-1)*R_free
  //Loop through the set of DS
  DSIterator it;            // Iterator
  SP::DynamicalSystem ds;   // Current Dynamical System.
  SP::SiconosMatrix W;      // W matrix of the current DS.
  Type::Siconos dsType ;    // Type of the current DS.
  for (it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
  {
    ds = *it; // the considered dynamical system
    dsType = Type::value(*ds); // Its type
    // Get iteration matrix W, make sure that W was updated before
    W = WMap[ds->number()]; // Its W matrix of iteration.
    SP::SiconosVector _qfree = ds->workspace(DynamicalSystem::free); // q_free
    SP::SiconosVector freeR = ds->workspace(DynamicalSystem::freeresidu);
    // -- Convert the DS into a Lagrangian one.
    if ((dsType == Type::LagrangianDS) || (dsType == Type::LagrangianLinearTIDS))
    {
      SP::LagrangianDS d = std11::static_pointer_cast<LagrangianDS>(ds);
      *_qfree = *(freeR);
      W->PLUForwardBackwardInPlace(*_qfree); //_qfree = (W^-1)*R_free
      *_qfree *= -1.0; //_qfree = -(W^-1)*R_free
      //
#ifdef DEBUG_NEWMARK
      std::cout << "delta q_free: " <<std::endl;
      _qfree->display();
#endif
    }
    else
    {
      RuntimeException::selfThrow("In NewMarkAlphaOSI::computeResidu(t,ds), this type of Dynamical System is not yet implemented");
    }
  }
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NewMarkAlphaOSI::computeFreeOutput(InteractionsGraph::VDescriptor& vertex_inter, OneStepNSProblem* osnsp)
{
  double t = simulationLink->nextTime();
  SP::InteractionsGraph indexSet = osnsp->simulation()->indexSet(osnsp->indexSetLevel());
  SP::Interaction inter = indexSet->bundle(vertex_inter);

  VectorOfBlockVectors& DSlink = *indexSet->properties(vertex_inter).DSlink;

  // Get the type of relation
  RELATION::TYPES relationType = inter->relation()->getType();
  RELATION::SUBTYPES relationSubType = inter->relation()->getSubType();
  // Get the set of OSNSPs
  SP::OneStepNSProblems  allOSNS  = simulationLink->oneStepNSProblems();
  // get the size of the interaction
  unsigned int sizeY = inter->nonSmoothLaw()->size();
  // get pointer to delta q_free of Dynamical Systems concerned with the interaction

  SP::BlockVector q_free;
  if (relationType == Lagrangian)
  {
    q_free = DSlink[LagrangianR::xfree];
  }
  assert(q_free);


  // get pointer to yForNSsolver vector
  SiconosVector& yForNSsolver = *inter->yForNSsolver();
  assert(q_free && "In NewMarkAlphaOSI::computeFreeOutput: pointer q_free has not initialized yet");
  assert(inter->relation() && "In NewMarkAlphaOSI::computeFreeOutput, relation associated with the interaction does not exist.");
  SP::SiconosMatrix C = inter->relation()->C();
  assert(C && "In NewMarkAlphaOSI::computeFreeOutput: Jacobian matrix does not exist");
  if (relationType == Lagrangian)
  {
    if (relationSubType == RheonomousR)
    {
      RuntimeException::selfThrow("NewMarkAlphaOSI::computeFreeOutput  not yet implemented with LagrangianRheonomousR");
    }

    if (relationSubType == ScleronomousR)
    {
      Index coord(8);
      coord[0] = 0;
      coord[1] = sizeY;
      coord[2] = 0;
      coord[3] = C->size(1);
      coord[4] = 0;
      coord[5] = C->size(1);
      coord[6] = 0;
      coord[7] = sizeY;
      if (((*allOSNS)[SICONOS_OSNSP_ED_SMOOTH_ACC]).get() == osnsp) // LCP at acceleration level
      {
        subprod(*C, *q_free, yForNSsolver, coord, true);
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
        SP::LagrangianScleronomousR _SclerR = std11::static_pointer_cast<LagrangianScleronomousR>(inter->relation());
        _SclerR->computedotjacqhXqdot(t, *inter, DSlink);
        subprod(*ID, *(_SclerR->dotjacqhXqdot()), yForNSsolver, xcoord, false); // y += NonLinearPart
      }
      else if (((*allOSNS)[SICONOS_OSNSP_ED_SMOOTH_POS]).get() == osnsp) // LCP at position level
      {
        // Update Jacobian matrix
        inter->relation()->computeJach(t, *inter, indexSet->properties(vertex_inter));
        if (inter->relation()->getType() == NewtonEuler)
        {
          SP::DynamicalSystem ds1 = indexSet->properties(vertex_inter).source;
          SP::DynamicalSystem ds2 = indexSet->properties(vertex_inter).target;
          SP::NewtonEulerR ner = (std11::static_pointer_cast<NewtonEulerR>(inter->relation()));
          ner->computeJachqT(*inter, ds1, ds2);
        }
        // cumpute yForNSsolver = y_{n,k} + G*q_free
        if (!_IsVelocityLevel) // output at the position level y_{n,k} = g_{n,k}
        {
          inter->computeOutput(t, indexSet->properties(vertex_inter), 0); // Update output of level 0
          yForNSsolver = *(inter->y(0)); //g_{n,k}
        }
        else                  // output at the velocity level y_{n,k} = (h/gamma_prime)*dotg_{n,k}
        {
          double h = simulationLink->nextTime() - simulationLink->startingTime();
          double gamma_prime = _gamma / _beta;
          inter->computeOutput(t, indexSet->properties(vertex_inter), 1); // Update output of level 1
          yForNSsolver = (h / gamma_prime) * (*(inter->y(1))); //(h/gamma_prime)*dotg_{n,k}
        }
        subprod(*C, *q_free, yForNSsolver, coord, false);
      }
      else
      {
        RuntimeException::selfThrow("NewMarkAlphaOSI::computeFreeOutput, this OSNSP does not exist");
      }
    }
#ifdef DEBUG_NEWMARK
    std::cout << "Free output yForNSsolver: ";
    yForNSsolver.display();
#endif
  }
  else
  {
    RuntimeException::selfThrow("In NewMarkAlphaOSI::computeFreeOutput, this type of relation is not yet implemented");
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NewMarkAlphaOSI::initialize()
{
  // Initialize OneStepIntegrator
  OneStepIntegrator::initialize();
  // Initialize W, acceleration-like for all ds
  ConstDSIterator itDS;
  Type::Siconos dsType ;    // Type of the current DS.
  for (itDS = OSIDynamicalSystems->begin(); itDS != OSIDynamicalSystems->end(); ++itDS)
  {
    // W initialization
    initW(*itDS);
    // allocate memory for work space for Newton iteration procedure
    (*itDS)->allocateWorkVector(DynamicalSystem::local_buffer, WMap[(*itDS)->number()]->size(0));
    //Allocate the memory to stock the acceleration-like variable
    dsType = Type::value(**itDS); // Its type
    if ((dsType == Type::LagrangianDS) || (dsType == Type::LagrangianLinearTIDS))
    {
      (*itDS)->allocateWorkVector(DynamicalSystem::acce_like, (*itDS)->getDim()); // allocate memory for the acceleration-like of DS
      (*itDS)->allocateWorkVector(DynamicalSystem::acce_memory, (*itDS)->getDim()); // allocate memory to stock acceleration
    }
    else
    {
      RuntimeException::selfThrow("In NewMarkAlphaOSI::initialize: this type of DS is not yet implemented");
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NewMarkAlphaOSI::prepareNewtonIteration(double time)
{
  // Compute matrix W for all Dynamical Systems
  ConstDSIterator itDS;
  for (itDS = OSIDynamicalSystems->begin(); itDS != OSIDynamicalSystems->end(); ++itDS)
  {
    computeW(*itDS);
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NewMarkAlphaOSI::prediction()
{
  // Step size
  double h = simulationLink->nextTime() - simulationLink->startingTime();
  if (h < 100 * MACHINE_PREC)
    RuntimeException::selfThrow("In NewMarkAlphaOSI::prediction, time integration is too small");
  // Loop over all DS
  ConstDSIterator itDS;
  Type::Siconos dsType ;    // Type of the current DS.
  SP::SiconosVector _q, _dotq, _ddotq, _a;
  for (itDS = OSIDynamicalSystems->begin(); itDS != OSIDynamicalSystems->end(); ++itDS)
  {
    dsType = Type::value(**itDS); // Its type
    if ((dsType == Type::LagrangianDS) || (dsType == Type::LagrangianLinearTIDS))
    {
      SP::LagrangianDS d = std11::static_pointer_cast<LagrangianDS>(*itDS);
      _q = d->q();                // generalized coordinate
      _dotq = d->velocity();      // generalized velocity
      _ddotq = d->acceleration(); // generalized acceleration
      _a = d->workspace(DynamicalSystem::acce_like); // acceleration-like
      // Save the acceleration before the prediction
      *(d->workspace(DynamicalSystem::acce_memory)) = *(_ddotq);
      //
#ifdef DEBUG_NEWMARK
      std::cout.precision(15);
      std::cout << "Before prediction" <<std::endl;
      std::cout << "Position q: ";
      _q->display();
      std::cout << "Velocity dotq: ";
      _dotq->display();
      std::cout << "Acceleration ddotq: ";
      _ddotq->display();
      std::cout << "Acceleration-like a: ";
      _a->display();
#endif
      //
      *_q = *_q + (*_dotq) * h + (*_a) * (h*h * (0.5 - _beta)); // q_{n+1} = q_n + (h^2)*(0.5 - beta)*a_n
      *_dotq = *_dotq + (*_a) * (h * (1 - _gamma));              // dotq_{n+1} = dotq_n + h*(1 - gamma)*a_n
      *_a = (_alpha_f / (1 - _alpha_m)) * (*_ddotq) - (_alpha_m / (1 - _alpha_m)) * (*_a); // a_{n+1} = (alpha_f*ddotq_n - alpha_m*a_n)/(1 - alpha_m)
      *_q = *_q + (h*h * _beta) * (*_a);
      *_dotq = *_dotq + (h * _gamma) * (*_a);
      _ddotq->zero();
      // Display message for debug
#ifdef DEBUG_NEWMARK
      std::cout.precision(15);
      std::cout << "After prediction" <<std::endl;
      std::cout << "Position q: ";
      _q->display();
      std::cout << "Velocity dotq: ";
      _dotq->display();
      std::cout << "Acceleration ddotq: ";
      _ddotq->display();
      std::cout << "Acceleration-like a: ";
      _a->display();
#endif
    }
    else
    {
      RuntimeException::selfThrow("In NewMarkAlphaOSI::prediction: this type of DS is not yet implemented");
    }
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NewMarkAlphaOSI::correction()
{
  double h = simulationLink->nextTime() - simulationLink->startingTime();
  double beta_prime = (1 - _alpha_m) / ((1 - _alpha_f) * _beta);
  double gamma_prime = _gamma / _beta;
  //Make sure that the input of the concerned Dynamical Systems is updated after solving LCP
  ConstDSIterator itDS;
  Type::Siconos dsType ;    // Type of the current DS
  SP::SiconosVector delta_q;
  for (itDS = OSIDynamicalSystems->begin(); itDS != OSIDynamicalSystems->end(); ++itDS)
  {
    SP::DynamicalSystem ds = *itDS;
    SP::SimpleMatrix W = WMap[ds->number()]; // Iteration matrix W_{n+1,k} computed at kth iteration
    SP::SiconosVector _r = ds->workspace(DynamicalSystem::freeresidu); // Free residu r_{n+1,k}
    dsType = Type::value(*ds); // Its type
    if ((dsType == Type::LagrangianDS) || (dsType == Type::LagrangianLinearTIDS))
    {
      SP::LagrangianDS d = std11::static_pointer_cast<LagrangianDS>(ds);
      SP::SiconosVector _p = d->p(2); // resultant force p_{n+1,k+1} of DS at (k+1)th iteration
      // Compute delta_q = W_{n+1,k}^{-1}(p_{n+1,k+1} - r_{n+1,k})
      delta_q.reset(new SiconosVector(*_p - *_r)); // copy (p_{n+1,k+1} - r_{n+1,k}) to delta_q
      W->PLUForwardBackwardInPlace(*delta_q);
      // Correction q_{n+1,k+1}, dotq_{n+1,k+1}, ddotq_{n+1,k+1}
      *(d->q()) += *delta_q; // q_{n+1,k+1} = q_{n+1,k} + delta_q
      *(d->velocity()) += (gamma_prime / h) * (*delta_q); // dotq_{n+1,k+1} = dotq_{n+1,k} + (gamma_prime/h)*delta_q
      *(d->acceleration()) += beta_prime / (h*h) * (*delta_q); // ddotq_{n+1,k+1} = ddotq_{n+1,k} + (beta_prime/h^2)*delta_q
      //a_{n+1,k+1} = a_{n+1,k} + ((1-alpha_f)/(1-alpha_m))*(beta_prime/h^2)*delta_q
      *(d->workspace(DynamicalSystem::acce_like)) += ((1 - _alpha_f) / (1 - _alpha_m)) * ((beta_prime / (h*h)) * (*delta_q));
      //
#ifdef DEBUG_NEWMARK
      std::cout.precision(15);
      std::cout << "After correction" <<std::endl;
      std::cout << "Position q : ";
      d->q()->display();
      std::cout << "Velocity dotq : ";
      d->velocity()->display();
      std::cout << "Acceleration ddotq : ";
      d->acceleration()->display();
      std::cout << "Acceleration-like a : ";
      d->workspace(DynamicalSystem::acce_like)->display();
#endif
    }
    else
    {
      RuntimeException::selfThrow("In NewMarkAlphaOSI::updateState: this type of DS is not yet implemented");
    }
  }

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NewMarkAlphaOSI::integrate(double& t_ini, double& t_end, double& t_out, int& flag)
{
  RuntimeException::selfThrow("In NewMarkAlphaOSI::integrate, this method does nothing in the NewMarkAlpha OneStepIntegrator!!!");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NewMarkAlphaOSI::updateState(const unsigned int level)
{
  // Compute all required (ie time-dependent) data for the DS of the OSI.
  DSIterator it;

  if (level == 1) // ie impact case: compute velocity
  {
    for (it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
    {
      SP::LagrangianDS lds = std11::static_pointer_cast<LagrangianDS>(*it);
      lds->computePostImpactVelocity();
    }
  }
  else if (level == 2)
  {
    double time = simulationLink->model()->currentTime();
    for (it = OSIDynamicalSystems->begin(); it != OSIDynamicalSystems->end(); ++it)
      (*it)->update(time);
  }
  else RuntimeException::selfThrow("In NewMarkAlphaOSI::updateState, index is out of range. Index = " + level);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NewMarkAlphaOSI::computeCoefsDenseOutput(SP::DynamicalSystem ds)
{
  double h = simulationLink->nextTime() - simulationLink->startingTime();
  Type::Siconos dsType = Type::value(*ds);    // Type of the current DS
  SP::SiconosVector q_n, dotq_n, ddotq_n, q_np1, dotq_np1, ddotq_np1;
  SP::SiconosVector _vec(new SiconosVector(ds->getDim()));
  if ((dsType == Type::LagrangianDS) || (dsType == Type::LagrangianLinearTIDS))
  {
    SP::LagrangianDS d = std11::static_pointer_cast<LagrangianDS>(ds);
    q_n = d->qMemory()->getSiconosVector(0); // q_n
    dotq_n = d->velocityMemory()->getSiconosVector(0); // dotq_n
    ddotq_n = d->workspace(DynamicalSystem::acce_memory); // ddotq_n
    q_np1 = d->q(); // q_{n+1}
    dotq_np1 = d->velocity(); // dotq_{n+1}
    ddotq_np1 = d->acceleration(); // ddotq_{n+1}
    SP::SimpleMatrix _CoeffsDense = d->getWorkMatrix(LagrangianDS::coeffs_denseoutput); // matrix of coefficients [a0 a1 a2 a3 a4 a5]
    if (_CoeffsDense->size(1) != 6)
    {
      RuntimeException::selfThrow("In NewMarkAlphaOSI::computeCoefsDenseOutput: the number of polynomial coeffcients considered here must equal to 6 (dense output polynomial of order 5)");
    }
    //a0 = q_n
    (*_vec) = (*q_n);
    _CoeffsDense->setCol(0, (*_vec));
    std::cout << "a0: ";
    _vec->display();
    //a1 = h*dotq_n
    (*_vec) = h * (*dotq_n);
    _CoeffsDense->setCol(1, (*_vec));
    std::cout << "a1: ";
    _vec->display();
    //a2 = 0.5*h^2*ddotq_n
    (*_vec) = (0.5 * h * h) * (*ddotq_n);
    _CoeffsDense->setCol(2, (*_vec));
    std::cout << "a2: ";
    _vec->display();
    //a3 = -10*q_n - 6*h*dotq_n - 1.5*h^2*ddotq_n + 10*q_{n+1} - 4*h*dotq_{n+1} + 0.5*h^2*ddotq_{n+1}
    (*_vec) = (-10.0) * (*q_n) - (6.0 * h) * (*dotq_n) - (1.5 * h * h) * (*ddotq_n) + 10.0 * (*q_np1) - (4.0 * h) * (*dotq_np1) + (0.5 * h *h ) * (*ddotq_np1);
    _CoeffsDense->setCol(3, (*_vec));
    std::cout << "a3: ";
    _vec->display();
    //a4 = 15*q_n + 8*h*dotq_n + 1.5*h^2*ddotq_n - 15*q_{n+1} + 7*h*dotq_{n+1} - h^2*ddotq_{n+1}
    (*_vec) = 15.0 * (*q_n) + (8.0 * h) * (*dotq_n) + (1.5 * h *h) * (*ddotq_n) - 15.0 * (*q_np1) + (7.0 * h) * (*dotq_np1) - h*h * (*ddotq_np1);
    _CoeffsDense->setCol(4, (*_vec));
    std::cout << "a4: ";
    _vec->display();
    //a5 = -6*q_n - 3*h*dotq_n - 0.5*h^2*ddotq_n + 6*q_{n+1} - 3*h*dotq_{n+1} + 0.5*h^2*ddotq_{n+1}
    (*_vec) = (-6.0) * (*q_n) - (3.0 * h) * (*dotq_n) - (0.5 * h*h) * (*ddotq_n) + 6.0 * (*q_np1) - (3.0 * h) * (*dotq_np1) + (0.5 * h*h) * (*ddotq_np1);
    _CoeffsDense->setCol(5, (*_vec));
    std::cout << "a5: ";
    _vec->display();
    //
#ifdef DEBUG_NEWMARK
    std::cout << "==================== In NewMarkAlphaOSI::computeCoefsDenseOutput ================" <<std::endl;
    std::cout << "DS number: " << ds->number() <<std::endl;
    std::cout << "q_n: ";
    q_n->display();
    std::cout << "dotq_n: ";
    dotq_n->display();
    std::cout << "ddotq_n: ";
    ddotq_n->display();
    std::cout << "q_n+1: ";
    q_np1->display();
    std::cout << "dotq_n+1: ";
    dotq_np1->display();
    std::cout << "ddotq_n+1: ";
    ddotq_np1->display();
    std::cout << "Dense output coefficient matrix: " <<std::endl;
    _CoeffsDense->display();
#endif
  }
  else
  {
    RuntimeException::selfThrow("In NewMarkAlphaOSI::computeCoefsDenseOutput: this type of DS has not been implemented yet");
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NewMarkAlphaOSI::prepareEventLocalization()
{
  ConstDSIterator itDS;
  for (itDS = OSIDynamicalSystems->begin(); itDS != OSIDynamicalSystems->end(); ++itDS)
  {
    // Compute coefficients of the dense output polynomial for all Dynamical Systems
    computeCoefsDenseOutput(*itDS);
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NewMarkAlphaOSI::DenseOutputallDSs(double t)
{
  // Make sure that all coefficients of the dense output polynomial for all DSs has been computed before
  double t_n = simulationLink->startingTime();
  double t_np1 = simulationLink->nextTime();
  double h = t_np1 - t_n;
  double theta = (t - t_n) / h;
  SP::SiconosVector _vec1(new SiconosVector(_orderDenseOutput + 1));
  assert((_vec1->size() == 6) && "There are six coefficients of the dense output polynomial");
  (*_vec1)(0) = 1.0;
  (*_vec1)(1) = theta;
  (*_vec1)(2) = std::pow(theta, 2);
  (*_vec1)(3) = std::pow(theta, 3);
  (*_vec1)(4) = std::pow(theta, 4);
  (*_vec1)(5) = std::pow(theta, 5);
  //
  SP::SiconosVector _vec2(new SiconosVector(_orderDenseOutput + 1));
  assert((_vec2->size() == 6) && "There are six coefficients of the dense output polynomial");
  (*_vec2)(0) = 0.0;
  (*_vec2)(1) = 1.0 / h;
  (*_vec2)(2) = (2.0 * theta) / h;
  (*_vec2)(3) = (3.0 * std::pow(theta, 2)) / h;
  (*_vec2)(4) = (4.0 * std::pow(theta, 3)) / h;
  (*_vec2)(5) = (5.0 * std::pow(theta, 4)) / h;
  //
  SP::SiconosVector _vec3(new SiconosVector(_orderDenseOutput + 1));
  assert((_vec3->size() == 6) && "There are six coefficients of the dense output polynomial");
  (*_vec3)(0) = 0.0;
  (*_vec3)(1) = 0.0;
  (*_vec3)(2) = 2.0 / std::pow(h, 2);
  (*_vec3)(3) = (6.0 * theta) / std::pow(h, 2);
  (*_vec3)(4) = (12.0 * std::pow(theta, 2)) / std::pow(h, 2);
  (*_vec3)(5) = (20.0 * std::pow(theta, 3)) / std::pow(h, 2);
  //
  SP::SimpleMatrix Matrix_coeffs;
  ConstDSIterator itDS;
  Type::Siconos dsType;    // Type of the current DS
  for (itDS = OSIDynamicalSystems->begin(); itDS != OSIDynamicalSystems->end(); ++itDS)
  {
    dsType = Type::value(**itDS);
    if ((dsType == Type::LagrangianDS) || (dsType == Type::LagrangianLinearTIDS))
    {
      SP::LagrangianDS d = std11::static_pointer_cast<LagrangianDS>(*itDS);
      Matrix_coeffs = d->getWorkMatrix(LagrangianDS::coeffs_denseoutput);
      prod(*Matrix_coeffs, *_vec1, *(d->q()), true); // q = Matrix_coeffs*_vec1
      prod(*Matrix_coeffs, *_vec2, *(d->velocity()), true); // dotq = Matrix_coeffs*_vec2
      prod(*Matrix_coeffs, *_vec3, *(d->acceleration()), true); // ddotq = Matrix_coeffs*_vec3
    }
    else
    {
      RuntimeException::selfThrow("In NewMarkAlphaOSI::DenseOutputallDSs: this type of DS has not been implemented yet");
    }

  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NewMarkAlphaOSI::display()
{

}
