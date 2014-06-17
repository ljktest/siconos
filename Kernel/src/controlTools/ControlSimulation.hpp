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

/*! \file ControlSimulation.hpp
  \brief Abstract class - General interface for all Control Dynamical Systems.
*/

#ifndef CONTROLDYNAMICALSYSTEM_H
#define CONTROLDYNAMICALSYSTEM_H

#include "SiconosPointers.hpp"
#include "SiconosAlgebraTypeDef.hpp"
#include "ControlTypeDef.hpp"
#include "SiconosControlFwd.hpp"

class ControlSimulation
{
private:
  /** serialization hooks */
  ACCEPT_SERIALIZATION(ControlSimulation);

protected:
  /** default constructor */
  ControlSimulation() {};

  /** Constructor with the minimal set of data
   * \param t0 the starting time \f$t_0\f$
   * \param T the end time T
   * \param h the simulation time step
   * \param x0 the initial state
   * */
  ControlSimulation(double t0, double T, double h, SP::SiconosVector x0);

  /** Starting time */
  double _t0;
  /** End time */
  double _T;
  /** Simulation step */
  double _h;
  /** Theta for MoreauJeanOSI */
  double _theta;
  /** Time spent computing */
  double _elapsedTime;
  /** rough estimation of the number of points to save */
  unsigned int _N;
  /** Dimension of the state space */
  unsigned int _nDim;
  /** Starting point */
  SP::SiconosVector _x0;
  /** Matrix for saving result */
  SP::SimpleMatrix _dataM;
  /** DynamicalSystem */
  SP::DynamicalSystem _processDS;
  /** Model */
  SP::Model _model;
  /** TimeDiscretisation */
  SP::TimeDiscretisation _processTD;
  /** TimeStepping */
  SP::TimeStepping _processSimulation;
  /** MoreauJeanOSI */
  SP::OneStepIntegrator _processIntegrator;
  /** the ControlManager */
  SP::ControlManager _CM;

public:

  /** destructor */
  virtual ~ControlSimulation() {};

  /** Modify the value of theta (for MoreauJeanOSI)
   * \param newTheta the new value of theta */
  void setTheta(unsigned int newTheta);

  /** Initialize the ControlSimulation, instantiate all objects
   * \param x0 starting point
   */
  void initialize();

  /** Add a Sensor
   * \param newSensor the SP::Sensor to be added
   * \param td the TimeDiscretisation associated with the Sensor
   */
  void addSensorPtr(SP::Sensor newSensor, SP::TimeDiscretisation td);

  /** Add an Actuator
   * \param newActuator the SP::Actuator to be added
   * \param td the TimeDiscretisation associated with the Actuator
   */
  void addActuatorPtr(SP::Actuator newActuator, SP::TimeDiscretisation td);

  /** Return the DynamicalSystem
   * \return the SP::DynamicalSystem
   */
  inline SP::DynamicalSystem processDS() const
  {
    return _processDS;
  };

  /** Return the Simulation
   * \return the process Simulation (a SP::TimeStepping)
  */
  inline SP::TimeStepping simulation() const
  {
    return _processSimulation;
  };

  /** Return the Model
   * \return the SP::Model
  */
  inline SP::Model model() const
  {
    return _model;
  }

  /** Return the data matrix
   * \return the data matrix
   */
  inline SP::SimpleMatrix data() const
  {
    return _dataM;
  }

  /** Return the elapsed time computing
   * \return the elapsed time computing
   */
  inline double elapsedTime() const
  {
    return _elapsedTime;
  }

  /** Return the ControlManager
   * \return a SP::ControlManager
   */
  inline SP::ControlManager CM() const
  {
    return _CM;
  };

  /** Change the FirstOrderLinearDS
   * \param ds the new SP::DynamicalSystem
   */
  inline void setProcessDS(SP::DynamicalSystem ds)
  {
    _processDS = ds;
  };

  /** Run the simulation */
  virtual void run();
};

#endif // CONTROLDYNAMICALSYSTEM_H
