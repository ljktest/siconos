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
/*! \file TimeStepping.hpp
 *  \brief Time-Stepping simulation
 */
#ifndef TimeStepping_H
#define TimeStepping_H

#include "Simulation.hpp"

/** type of function used to post-treat output info from solver. */
typedef void (*CheckSolverFPtr)(int, Simulation*);

/** \class TimeStepping 
    \brief Event-capturing Time-Stepping simulation
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) Apr 26, 2004
 *
 * This class implements the basic algorithm for Event-capturing Time-Stepping 
 * simulations.
 *
 * References :
 *
 * V. Acary and B. Brogliato. Numerical Methods for Nonsmooth Dynamical Systems: 
 * Applications in Mechanics and Electronics, volume 35 of Lecture Notes in
 * Applied and Computational Mechanics. Springer Verlag, 2008.
 *
 */

#define SICONOS_TS_LINEAR 1
#define SICONOS_TS_LINEAR_IMPLICIT 2
#define SICONOS_TS_NONLINEAR 3


class TimeStepping : public Simulation
{
protected:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(TimeStepping);

  /** Default Newton tolerance used in call of run() of ComputeOneStep() */
  double _newtonTolerance;

  /** Default maximum number of Newton iteration*/
  unsigned int _newtonMaxIteration;

  /** Number of steps perfomed is the Newton Loop */
  unsigned int _newtonNbSteps;

  /** unsigned int  _newtonOptions
   *  option in the Newon iteration
   *  SICONOS_TS_LINEAR or SICONOS_TS_LINEAR_IMPLICIT SICONOS_TS_NONLINEAR will force a single iteration of the Newton Solver
   * SICONOS_TS_NONLINEAR (default) will perform the newton iteration up to convergence
   */

  unsigned int _newtonOptions;
  /** Maximum Residual for the Dynamical system */
  double _newtonResiduDSMax;

  /** Maximum Residual for the output of the relation */
  double _newtonResiduYMax;

  /** Maximum Residual for the input of the relation */
  double _newtonResiduRMax;

  /** boolean variable to known whether the ResiduY has to be computed or not
   *  if true, the ResiduY is computed and the convergence is checked
   */
  bool _computeResiduY;

  /** boolean variable to known whether the ResiduR has to be computed or not
   *  if true, the ResiduR is computed and the convergence is checked
   */
  bool _computeResiduR;

  /** boolean variable to known whether Newton iterations converges or not
   */
  bool _isNewtonConverge;

  /** Default Constructor
   */
  TimeStepping() : 
    _computeResiduY(false), 
    _computeResiduR(false), 
    _isNewtonConverge(false) {};
  
public:

  /** initialisation specific to TimeStepping for OneStepNSProblem.
  */
  virtual void initOSNS();

  /** Constructor with the time-discretisation.
   *  \param td pointer to a timeDiscretisation used in the integration 
   *  (linked to the model that owns this simulation)
   *  \param osi one step integrator (default none)
   *  \param osnspb one step non smooth problem (default none)
   */
  TimeStepping(SP::TimeDiscretisation td,
               SP::OneStepIntegrator osi = SP::OneStepIntegrator(),
               SP::OneStepNSProblem osnspb = SP::OneStepNSProblem());

  /** Constructor with the time-discretisation.
   *  \param td pointer to a timeDiscretisation used in the integration 
   *  (linked to the model that owns this simulation)
   *  \param nb number of non smooth problem
   */
  TimeStepping(SP::TimeDiscretisation td, int nb);

  /** Destructor.
  */
  virtual ~TimeStepping();

  /** update indexSets[i] of the topology, using current y and lambda values of Interactions.
   *  \param i the number of the set to be updated
   */
  virtual void updateIndexSet(unsigned int i);

  // /** Used by the updateIndexSet function in order to deactivate SP::Interaction.
  //  */
  // virtual bool predictorDeactivate(SP::Interaction inter, unsigned int i);

  // /** Used by the updateIndexSet function in order to activate SP::Interaction.
  //  */
  // virtual bool predictorActivate(SP::Interaction inter, unsigned int i);

  /** increment model current time according to User TimeDiscretisation and call SaveInMemory. */
  virtual void nextStep();

  using Simulation::update;

  /** update input, state of each dynamical system and output
   *  \param levelInput lambda order used to compute input
   */
  void update(unsigned int levelInput);

  /** integrates all the DynamicalSystems taking not into account nslaw, reactions (ie non-smooth part) ...
  */
  void computeFreeState();

  /** step from current event to next event of EventsManager
  */
  void advanceToEvent();

  /** run one time--step of the simulation
  */
  void computeOneStep();

  /** newton algorithm
   * \param criterion convergence criterion
   * \param maxStep maximum number of Newton steps
   */
  virtual void newtonSolve(double criterion, unsigned int maxStep);

  /** To known the number of steps performed by the Newton algorithm.
   * \return  the number of steps performed by the Newton algorithm
   */
  unsigned int getNewtonNbSteps()
  {
    return _newtonNbSteps;
  }

  /** initialize the Newton
   * It computes the initial residu and set the, if needed to Newton variable
   * to start the newton algorithm.
   */
  void initializeNewtonLoop();


  void prepareNewtonIteration();

  /** check the convergence of Newton algorithm according to criterion
   * \param criterion convergence criterion
   * \return bool = true if Newton method has converged
   */
  bool newtonCheckConvergence(double criterion);

  /*save y_k^p, the current Newton iteration*/
  void saveYandLambdaInOldVariables();

  /** run the simulation, from t0 to T
   * with default parameters if any setting has been done
   */
  void run();

  /** check returning value from computeOneStepNSProblem and process
   *  \param info solver-specific error code return by the nonsmooth solver
   */
  void DefaultCheckSolverOutput(int info);

  /** Set CheckSolverOutput function
   *  \param newF pointer to function steering the behavior of simulation when
   *  nonsmooth solver failed
   */
  void setCheckSolverFunction(CheckSolverFPtr newF);

  /**  */
  bool isNewtonConverge()
  {
    return _isNewtonConverge;
  };
  /** To specify if the output interaction residu must be computed.
   *  \param v set to true when the output interaction residu must be computed
   */
  void setComputeResiduY(bool v)
  {
    _computeResiduY = v;
  };

  /** To know if the output interaction residu must be computed.
   * \return bool _computeResiduY
   */
  virtual bool computeResiduY()
  {
    return _computeResiduY;
  };


  /** To specify if the input interaction residu must be computed.
   *  \param v set to true when the input interaction residu must be computed
   */
  void setComputeResiduR(bool v)
  {
    _computeResiduR = v;
  };

  /** To known if the input interaction residu must be computed.
   * \return bool _computeResiduR
   */
  virtual bool computeResiduR()
  {
    return _computeResiduR;
  };


  /** set the Default Newton tolerance
   *  \param tol Newton solver tolerance
   */
  void setNewtonTolerance(double tol)
  {
    _newtonTolerance = tol;
  };

  /** get the Newton tolerance
   *  \return default Newton solver tolerance
   */
  double newtonTolerance()
  {
    return   _newtonTolerance;
  };

  /** set the maximum number of Newton iteration
   *  \param maxStep maximum number of Newton solver iterations
   */
  void setNewtonMaxIteration(unsigned int maxStep)
  {
    _newtonMaxIteration = maxStep;
  };

  /** get the maximum number of Newton iteration
   *  \return maximum number of Newton solver iterations
   */
  double newtonMaxIteration()
  {
    return _newtonMaxIteration;
  };

  /** set the NewtonOptions
   *  \param v Newton solver options
   */
  void setNewtonOptions(unsigned int v)
  {
    _newtonOptions = v;
  };

  /** get the NewtonOptions
   *  \return Newton solver options - SICONOS_TS_LINEAR 1,
   *  SICONOS_TS_LINEAR_IMPLICIT 2, SICONOS_TS_NONLINEAR 3
   */
  unsigned int newtonOptions()
  {
    return _newtonOptions;
  };


  /** accessor to _newtonResiduDSMax
   * \return _newtonResiduDSMax
   */
  double newtonResiduDSMax()
  {
    return _newtonResiduDSMax;
  };

  /** accessor to _newtonResiduYMax
   * \return _newtonResiduYMax
   */
  double newtonResiduYMax()
  {
    return _newtonResiduYMax;
  };

  /** accessor to _newtonResiduRMax
   * \return _newtonResiduRMax
  */
  double newtonResiduRMax()
  {
    return _newtonResiduRMax;
  };


  /*TS set the ds->q memory, the world (CAD model for example) must be updated.
    Overload this method to update user model.*/
  virtual void updateWorldFromDS()
  {
    ;
  };



  /** visitors hook
  */
  ACCEPT_STD_VISITORS();

};

#endif // TimeStepping_H
