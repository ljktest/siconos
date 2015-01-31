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
/*! \file
  SchatzmanPaoliOSI Time-Integrator for Dynamical Systems
*/

#ifndef SCHATZMANPAOLIOSI_H
#define SCHATZMANPAOLIOSI_H

#include "OneStepIntegrator.hpp"
#include "SimpleMatrix.hpp"


const unsigned int SCHATZMANPAOLISTEPSINMEMORY = 2;

/**  SchatzmanPaoliOSI Time-Integrator for Dynamical Systems
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) Apr 26, 2004
 *
 *
 * SchatzmanPaoliOSI class is used to define some time-integrators methods for a
 * list of dynamical systems.

 * A SchatzmanPaoliOSI instance is defined by the value of theta and the list of
 * concerned dynamical systems.  Each DynamicalSystem is associated to
 * a SiconosMatrix, named "W"
 *
 * W matrices are initialized and computed in initW and
 * computeW. Depending on the DS type, they may depend on time and DS
 * state (x).
 *
 *
 * For Lagrangian systems, the implementation uses _p[0] for storing the
 * discrete multiplier.
 *
 * Main functions:
 *
 * - computeFreeState(): computes xfree (or vfree), dynamical systems
 *   state without taking non-smooth part into account \n
 *
 * - updateState(): computes x (q,v), the complete dynamical systems
 *    states.
 *
 */
class SchatzmanPaoliOSI : public OneStepIntegrator
{
protected:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(SchatzmanPaoliOSI);


  /** Stl map that associates a W SchatzmanPaoliOSI matrix to each DynamicalSystem of the OSI */
  MapOfDSMatrices WMap;

  /** Stl map that associates the columns of  W SchatzmanPaoliOSI matrix to each DynamicalSystem of the OSI if it has some boundary conditions */
  MapOfDSMatrices _WBoundaryConditionsMap;

  /** Stl map that associates a theta parameter for the integration
  *  scheme to each DynamicalSystem of the OSI */
  double _theta;

  /** A gamma parameter for the integration scheme to each DynamicalSystem of the OSI
   * This parameter is used to apply a theta-method to the input $r$
   */
  double _gamma;

  /** a boolean to known is the parameter must be used or not
   */
  bool _useGamma;

  /** a boolean to known is the parameter must be used or not
   */
  bool _useGammaForRelation;

  /** nslaw effects
   */
  struct _NSLEffectOnFreeOutput;
  friend struct _NSLEffectOnFreeOutput;



  /** Default constructor
   */
  SchatzmanPaoliOSI() {};

public:

  /** constructor from a minimum set of data: one DS and its theta
   *  \param SP::DynamicalSystem : the DynamicalSystem linked to the OneStepIntegrator
   *  \param Theta value
   */
  DEPRECATED_OSI_API(SchatzmanPaoliOSI(SP::DynamicalSystem, double));

  /** constructor from theta value only
   *  \param theta value for all these DS.
   */
  SchatzmanPaoliOSI(double theta);

  /** constructor from a minimum set of data: one DS and its theta
   *  \param SP::DynamicalSystem : the DynamicalSystem linked to the OneStepIntegrator
   *  \param theta value
   *  \param gamma value
   */
  DEPRECATED_OSI_API(SchatzmanPaoliOSI(SP::DynamicalSystem, double, double));

  /** constructor from theta value only
   *  \param theta value for all these DS.
   *  \param gamma value for all these DS.
   */
  SchatzmanPaoliOSI(double theta, double gamma);

  /** destructor
   */
  virtual ~SchatzmanPaoliOSI() {};

  // --- GETTERS/SETTERS ---

  /** get the value of W corresponding to DynamicalSystem ds
   * \param a pointer to DynamicalSystem, optional, default =
   * NULL. get W[0] in that case
   *  \return SimpleMatrix
   */
  const SimpleMatrix getW(SP::DynamicalSystem = SP::DynamicalSystem());

  /** get W corresponding to DynamicalSystem ds
   * \param a pointer to DynamicalSystem, optional, default =
   * NULL. get W[0] in that case
   * \return pointer to a SiconosMatrix
   */
  SP::SimpleMatrix W(SP::DynamicalSystem ds);

  /** set the value of W[ds] to newValue
   * \param SiconosMatrix newValue
   * \param a pointer to DynamicalSystem,
   */
  void setW(const SiconosMatrix&, SP::DynamicalSystem);

  /** set W[ds] to pointer newPtr
   * \param SP::SiconosMatrix  newPtr
   * \param a pointer to DynamicalSystem
   */
  void setWPtr(SP::SimpleMatrix newPtr, SP::DynamicalSystem);

  // -- WBoundaryConditions --

  /** get WBoundaryConditions map
   *  \return a MapOfDSMatrices
   */
  inline MapOfDSMatrices getWBoundaryConditionsMap() const
  {
    return _WBoundaryConditionsMap;
  };

  /** get the value of WBoundaryConditions corresponding to DynamicalSystem ds
   * \param a pointer to DynamicalSystem, optional, default =
   * NULL. get WBoundaryConditions[0] in that case
   *  \return SimpleMatrix
   */
  const SimpleMatrix getWBoundaryConditions(SP::DynamicalSystem = SP::DynamicalSystem());

  /** get WBoundaryConditions corresponding to DynamicalSystem ds
   * \param a pointer to DynamicalSystem, optional, default =
   * NULL. get WBoundaryConditions[0] in that case
   * \return pointer to a SiconosMatrix
   */
  SP::SiconosMatrix WBoundaryConditions(SP::DynamicalSystem ds);

  // -- theta --

  /** get theta
   *  \return a double
   */
  inline double theta()
  {
    return _theta;
  };

  /** set the value of theta
   *  \param a double
   */
  inline void setTheta(double newTheta)
  {
    _theta = newTheta;
  };

  // -- gamma --

  /** get gamma
   *  \return a double
   */
  inline double gamma()
  {
    return _gamma;
  };

  /** set the value of gamma
   *  \param a double
   */
  inline void setGamma(double newGamma)
  {
    _gamma = newGamma;
    _useGamma = true;
  };

  // -- useGamma --

  /** get bool useGamma
   *  \return a bool
   */
  inline bool useGamma()
  {
    return _useGamma;
  };

  /** set the boolean to indicate that we use gamma
   *  \param a bool
   */
  inline void setUseGamma(bool newUseGamma)
  {
    _useGamma = newUseGamma;
  };

  /** get bool gammaForRelation for the relation
   *  \return a
   */
  inline bool useGammaForRelation()
  {
    return _useGammaForRelation;
  };


  /** set the boolean to indicate that we use gamma for the relation
   *  \param a bool
   */
  inline void setUseGammaForRelation(bool newUseGammaForRelation)
  {
    _useGammaForRelation = newUseGammaForRelation;
    if (_useGammaForRelation) _useGamma = false;
  };


  // --- OTHER FUNCTIONS ---

  /** initialization of the SchatzmanPaoliOSI integrator; for linear time
      invariant systems, we compute time invariant operator (example :
      W)
   */
  void initialize();

  /** init WMap[ds] SchatzmanPaoliOSI matrix at time t
   *  \param the time (double)
   *  \param a pointer to DynamicalSystem
   */
  void initW(double, SP::DynamicalSystem);

  /** compute WMap[ds] SchatzmanPaoliOSI matrix at time t
   *  \param the time (double)
   *  \param a pointer to DynamicalSystem
   */
  void computeW(double, SP::DynamicalSystem);

  /** compute WBoundaryConditionsMap[ds] SchatzmanPaoliOSI matrix at time t
   *  \param the time (double)
   *  \param a pointer to DynamicalSystem
   */
  void computeWBoundaryConditions(SP::DynamicalSystem);

  /** init WBoundaryConditionsMap[ds] SchatzmanPaoliOSI
   *  \param a pointer to DynamicalSystem
   */
  void initWBoundaryConditions(SP::DynamicalSystem);

  /** return the maximum of all norms for the "SchatzmanPaoliOSI-discretized" residus of DS
      \return a double
   */
  double computeResidu();

  /** integrates the Dynamical System linked to this integrator
   *  without boring the constraints
   */
  virtual void computeFreeState();

  /** integrates the Interaction linked to this integrator, without taking non-smooth effects into account
   * \param vertex of the interaction graph
   * \param osnsp pointer to OneStepNSProblem
   */
  virtual void computeFreeOutput(InteractionsGraph::VDescriptor& vertex_inter, OneStepNSProblem* osnsp);

  void prepareNewtonIteration(double time);

  /** integrate the system, between tinit and tend (->iout=true), with possible stop at tout (->iout=false)
   *  \param tinit initial time
   *  \param tend end time
   *  \param tout real end time
   *  \param idid useless flag (for SchatzmanPaoliOSI, used in LsodarOSI)
   */
  void integrate(double& tinit, double& tend, double& tout, int& idid);

  /** updates the state of the Dynamical Systems
   *  \param level level of interest for the dynamics: not used at the time
   */
  virtual void updateState(const unsigned int level);

  /** Displays the data of the SchatzmanPaoliOSI's integrator
   */
  void display();

  /** visitors hook
  */
  ACCEPT_STD_VISITORS();

};

#endif // SCHATZMANPAOLIOSI_H
