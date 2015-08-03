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
 *
 */
/** \file Hem5OSI.hpp
 * Hem5OSI solver (from E. Hairer software lists)
 */
#ifndef Hem5OSI_H
#define Hem5OSI_H

#include"OneStepIntegrator.hpp"
#include<vector>

#include <hairer.h>

#define HEM5_ATOL_DEFAULT 100 * MACHINE_PREC;
#define HEM5_RTOL_DEFAULT 10 * MACHINE_PREC;

/** Hem5OSI solver (odepack)
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) Apr 26, 2004
 *
 * Many parameters are required as input/output for LSODAR. See the documentation of this function
 * in Numerics/src/odepack/opkdmain.f to have a full description of these parameters.  \n
 * Most of them are read-only parameters (ie can not be set by user). \n
 *  Except: \n
 *  - jt: Jacobian type indicator (1 means a user-supplied full Jacobian, 2 means an internally generated full Jacobian). \n
 *    Default = 2.
 *  - itol, rtol and atol \n
 *    ITOL   = an indicator for the type of error control. \n
 *    RTOL   = a relative error tolerance parameter, either a scalar or array of length NEQ. \n
 *    ATOL   = an absolute error tolerance parameter, either a scalar or an array of length NEQ.  Input only.
 */
class Hem5OSI : public OneStepIntegrator, public std11::enable_shared_from_this<Hem5OSI>
{
private:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(Hem5OSI);

  /** vector of integer data for the integrator
   * _intData[0] NQ size of the position vector q
   * _intData[1] NV size of the velocity  vector v NQ >= NQ
   * _intData[2] NU size of the external dynamic vector u
   * _intData[3] NL size of the Lagrange multiplier vector lambda
   * _intData[4] ITOL indicates whether RTOL and ATOL are scalar (ITOL=0), or array of
   *             dimension NQ + NV + NU (ITOL=1)
   * _intData[5] IOUT selects the dense output formula
   * _intData[6] LWK length of real array rwork
   * _intData[7] LIWK length of integer array iwork
   * See hem5.f
   */
  std::vector<integer> _intData;


  integer _idid;

  /** relative tolerance */
  SA::doublereal rtol;
  /** absolute tolerance */
  SA::doublereal atol;
  /** real work array */
  SA::doublereal rwork;
  /** integer work array */
  SA::integer iwork;


  doublereal _timeStep; // initial step size guess

  /** temporary vector to save q values */
  SP::BlockVector _qWork;
  /** temporary vector to save v values */
  SP::BlockVector _vWork;
  /** temporary vector to save v values */
  SP::BlockVector _uWork;
  /** temporary vector to save a values */
  SP::BlockVector _aWork;
  /** temporary vector to save lambda values */
  SP::BlockVector _lambdaWork;
  /** temporary vector to save forces values */
  SP::BlockVector _forcesWork;

  SP::SiconosVector _qtmp;
  SP::SiconosVector _vtmp;
  SP::SiconosVector _utmp;
  SP::SiconosVector _atmp;
  SP::SiconosVector _lambdatmp;
  SP::SiconosVector _forcestmp;

  /** nslaw effects
   */
  struct _NSLEffectOnFreeOutput;
  friend struct _NSLEffectOnFreeOutput;


public:

  /** constructor from a minimum set of data
   */
  Hem5OSI();

  DEPRECATED_OSI_API(Hem5OSI(SP::DynamicalSystem ds));

  /** destructor
   */
  ~Hem5OSI() {};

  /** get vector of integer parameters for lsodar
   *  \return a vector<integer>
   */
  inline const std::vector<integer> intData() const
  {
    return _intData;
  }

  /** get _intData[i]
   * \param i index
   * \return an integer
   */
  inline integer intData(unsigned int i) const
  {
    return _intData[i];
  }
  /** set _intData[i]
   * \param i index
   * \param newValue
   */
  inline void setIntData(unsigned int i, int newValue)
  {
    _intData[i] = newValue;
  }

  /** get relative tolerance parameter for Hem5
   *  \return a doublereal*
   */
  inline const SA::doublereal getRtol() const
  {
    return rtol;
  }

  /** get absolute tolerance parameter for Hem5
   *  \return a doublereal*
   */
  inline const SA::doublereal getAtol() const
  {
    return atol;
  }

  /** get the maximum number of steps for one call
  *\return an interger
  */
  inline  int getMaxNstep()const
  {
    return iwork[11];
  }

  /** get real work vector parameter for lsodar
   *  \return a doublereal*
   */
  inline const SA::doublereal getRwork() const
  {
    return rwork;
  }

  /** get iwork
   *  \return a pointer to integer
   */
  inline SA::integer getIwork() const
  {
    return iwork;
  }

  /** set itol, rtol and atol (tolerance parameters for Hem5)
   *  \param itol integer (itol value)
   *  \param rtol doublereal * (rtol)
   *  \param atol doublereal * (atol)
   */
  void setTol(integer itol, SA::doublereal rtol, SA::doublereal atol);

  /** set itol, rtol and atol (scalar tolerance parameters for Hem5)
   *  \param itol integer (itol value)
   *  \param rtol double (rtol)
   *  \param atol double (atol)
   */
  void setTol(integer itol, doublereal rtol, doublereal atol);

  /** set the maximul number of steps for one call of Hem5OSI
   *\param nstepmax an integer
   */
  void setMaxNstep(integer nstepmax);

  /** set the minimum and maximum step sizes
   * \param maxstepsize double (maximul step size)
   */
  void setMaxStepSize(doublereal maxstepsize);

  /** update _intData
   */
  void updateIntData();

  /** update doubleData and iwork memory size, when changes occur in _intData.
   */
  void updateData();

  /** fill qWork with a doublereal
   *  \param sizex integer*, size of x array
   *  \param x doublereal* x:array of double
   */
  void fillqWork(integer* sizex, doublereal* x) ;

  /** fill vWork with a doublereal
   *  \param sizex integer*, size of x array
   *  \param x doublereal* x:array of double
   */
  void fillvWork(integer* sizex, doublereal* x) ;

  /** compute rhs(t) for all dynamical systems in the set
   */
  void computeRhs(double) ;

  /** compute jacobian of the rhs at time t for all dynamical systems in the set
   */
  void computeJacobianRhs(double) ;

  unsigned int numberOfConstraints();

  fprobfunction fprob;

  soloutfunction solout;

  void f(integer* sizeOfX, doublereal* time, doublereal* x, doublereal* xdot);

  void g(integer* nEq, doublereal* time, doublereal* x, integer* ng, doublereal* gOut);

  void jacobianfx(integer*, doublereal*, doublereal*, integer*, integer*,  doublereal*, integer*);

  /** initialization of the integrator
   */
  void initialize();

  /** integrate the system, between tinit and tend (->iout=true), with possible stop at tout (->iout=false)
   *  \param tinit initial time
   *  \param tend end time
   *  \param tout real end time
   *  \param idid in-out parameter, input: 1 for first call, else 2. Output: 2 if no root was found, else 3.
   */
  void integrate(double& tinit, double& tend, double& tout, int& idid);

  /** update the state of the DynamicalSystems attached to this Integrator
   *  \param level level of interest for the dynamics
   */
  void updateState(const unsigned int level);

  void prepareNewtonIteration(double time)
  {
    assert(0);
  };

  /** integrates the Interaction linked to this integrator, without taking non-smooth effects into account
   * \param vertex_inter of the interaction graph
   * \param osnsp pointer to OneStepNSProblem
   */
  virtual void computeFreeOutput(InteractionsGraph::VDescriptor& vertex_inter, OneStepNSProblem* osnsp);

  /** print the data to the screen
   */
  void display();

  /** visitors hook
  */
  ACCEPT_STD_VISITORS();
};

#endif // Hem5OSI_H
