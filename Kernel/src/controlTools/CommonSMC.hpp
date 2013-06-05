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

/*! \file CommonSMC.hpp
  \brief General interface to define a sliding mode actuator
  */

#ifndef CommonSMC_H
#define CommonSMC_H

#include "Actuator.hpp"
#include "SiconosAlgebraTypeDef.hpp"

#ifndef FirstOrderLinearDS_H
DEFINE_SPTR(FirstOrderLinearDS)
#endif
#ifndef TimeStepping_H
DEFINE_SPTR(TimeStepping)
#endif
#ifndef Relay_H
DEFINE_SPTR(Relay)
#endif
#ifndef EventsManager_H
DEFINE_SPTR(EventsManager)
#endif

class CommonSMC : public Actuator
{
private:
  /** serialization hooks */
  ACCEPT_SERIALIZATION(CommonSMC);

protected:
  /** default constructor */
  CommonSMC() {};

  /** codimension size */
  unsigned int _sDim;

  /** index for saving data */
  unsigned int _indx;

  /** control variable */
  SP::SiconosVector _u;

  /** the vector defining the surface (\f$ s = Cx \f$) */
  SP::SiconosMatrix _Csurface;

  /** the sensor that feed the controller */
  SP::ControlSensor _sensor;

  /** boolean to determined if the controller has been correctly initialized */
  bool _initDone;

  /** Do not use the state-continuous equivaluent control \f$u^{eq}\f$ */
  bool _noUeq;

  /** current \f$\Delta t\f$ (or timeStep) */
  double _curDeltaT;

  /** matrix describing the relation between the control value and sgn(s) */
  SP::SiconosMatrix _B;

  /** matrix describing the influence of \f$lambda\f$ on s */
  SP::SiconosMatrix _D;

  /** the Relation for the Controller */
  SP::Relation _relationSMC;

  /** the NonSmoothLaw for the controller */
  SP::NonSmoothLaw _sign;

  /** */
  SP::Interaction _interactionSMC;

  /** easy access to lambda */
  SP::SiconosVector _lambda;

  /** easy access to the state */
  SP::SiconosVector _xController;

  /** Relay solver type */
  int _numericsSolverId;

  /** Numerical precision expected for the Relay solver */
  double _precision;

  /** the Model for the controller */
  SP::Model _SMC;
  /** the DynamicalSystem for the controller */
  SP::FirstOrderLinearDS _DS_SMC;
  /** the TimeDiscretisation for the controller */
  SP::TimeDiscretisation _tD_SMC;
  /** Simulation for the controller */
  SP::TimeStepping _simulationSMC;
  /** Integrator for the controller */
  SP::OneStepIntegrator _integratorSMC;
  /** Theta for the controller */
  double _thetaSMC;
  /** OneStepNsProblem for the controller */
  SP::Relay _OSNSPB_SMC;
  /** SP::SiconosVector containing the control */
  SP::SiconosVector _sampledControl;
  /** SP::EventsManager of the SMC Simulation */
  SP::EventsManager _eventsManager;
  /** SP::NonSmoothLaw for computing the control law */
  SP::NonSmoothLaw _nsLawSMC;

  /** inverse of CB */
  SP::SimpleMatrix _invCB;
  /** Store \f$u_{eq}\f$ */
  SP::SiconosVector _ueq;
  /** Store \f$u^s\f$ */
  SP::SiconosVector _us;

public:

  /** Constructor with a TimeDiscretisation and a Model.
   * \param name the type of the SMC Actuator
   * \param t the SP::TimeDiscretisation (/!\ it should not be used elsewhere !)
   * \param ds the SP::DynamicalSystem we are controlling
   */
  CommonSMC(int name, SP::TimeDiscretisation t, SP::DynamicalSystem ds): Actuator(name, t, ds),
    _numericsSolverId(SICONOS_RELAY_LEMKE), _precision(1e-8), _thetaSMC(0.5) {}

  /** Constructor with a TimeDiscretisation, a Model and two matrices
   * \param name the type of the SMC Actuator
   * \param t the SP::TimeDiscretisation (/!\ it should not be used elsewhere !)
   * \param ds the SP::DynamicalSystem we are controlling
   */
  CommonSMC(int name, SP::TimeDiscretisation t, SP::DynamicalSystem ds, SP::SiconosMatrix B, SP::SiconosMatrix D):
    Actuator(name, t, ds), _B(B), _D(D), _numericsSolverId(SICONOS_RELAY_LEMKE), _precision(1e-8), _thetaSMC(0.5) {}


  /** Constructor with a TimeDiscretisation, a Model and a set of Sensor.
   * \param name the type of the SMC Actuator
   * \param t the SP::TimeDiscretisation (/!\ it should not be used elsewhere !)
   * \param ds the SP::DynamicalSystem we are controlling
   * \param sensorList the set of Sensor linked to this Actuator.
   */
  CommonSMC(int name, SP::TimeDiscretisation t, SP::DynamicalSystem ds, const Sensors& sensorList):
    Actuator(name, t, ds, sensorList), _numericsSolverId(SICONOS_RELAY_LEMKE), _precision(1e-8), _thetaSMC(0.5) {}

  /** Compute the new control law at each event
  */
  virtual void actuate() = 0;

  /** Initialization
   * \param m a SP::Model
   */
  virtual void initialize(SP::Model m);

  /** Set the value of _Csurface to newValue
   * \param newValue the new value for _Csurface
   */
  void setCsurface(const SiconosMatrix& newValue);

  /** Set _Csurface to pointer newPtr
   * \param newPtr a SP::SiconosMatrix containing the new value for _Csurface
   */
  void setCsurfacePtr(SP::SiconosMatrix newPtr);

  /** Set the value of _D to newValue
   * \param newValue the new value for _D
   */
  void setSaturationMatrix(const SiconosMatrix& newValue);

  /** Set _D to pointer newPtr
   * \param newPtr a SP::SiconosMatrix containing the new value for _D
   */
  void setSaturationMatrixPtr(SP::SiconosMatrix newPtr);

  /** get _lambda
   * \return a pointer to _lambda
   */
  inline SP::SiconosVector lambda() const
  {
    return _lambda;
  };

  /** Set the B matrix
   * \param B the new B matrix
  */
  void setB(const SiconosMatrix& B);

  /** Set the B matrix
   * \param B the new B matrix
   */
  inline void setBPtr(SP::SiconosMatrix B)
  {
    _B = B;
  };

  /** Set the solver
   * \param newNumericsSolverId the solver for the relay
   */
  inline void setSolver(const int newNumericsSolverId)
  {
    _numericsSolverId = newNumericsSolverId;
  };

  /** Set the precision
   * \param newPrecision a double
   */
  inline void setPrecision(const double newPrecision)
  {
    _precision = newPrecision;
  };

  /** Get the Relay problem associated with the controller. This is useful to
   * gain access to the data given to the Relay solver in Numerics
   * \return a reference to the Relay problem
   */
  inline const Relay& relay()
  {
    return * _OSNSPB_SMC;
  };

  inline SiconosVector& ueq()
  {
    return *_ueq;
  };
  inline SiconosVector& us()
  {
    return *_us;
  };

  inline void setTheta(const double& newTheta)
  {
    _thetaSMC = newTheta;
  };

  void computeUeq();

  /** Disable (or enable) the use of the state-continuous control \f$u^{eq}\f$
   * \param b disable the use of Ueq if true
   */
  inline void noUeq(const bool b)
  {
    _noUeq = b;
  };
};
DEFINE_SPTR(CommonSMC)
#endif
