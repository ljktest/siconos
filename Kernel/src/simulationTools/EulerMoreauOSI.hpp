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

/** \file EulerMoreauOSI.hpp */

#ifndef EulerMoreauOSI_H
#define EulerMoreauOSI_H

#include "OneStepIntegrator.hpp"

class Simulation;
class SiconosMatrix;

const unsigned int EULERMOREAUSTEPSINMEMORY = 1;

/** \class EulerMoreauOSI Time-Integrator for Dynamical Systems
 *  \brief One Step time Integrator for First Order Dynamical Systems.
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.7.0.
 *  \date (Creation) Dec 26, 2013
 *
 * This integrator is the work horse of the event--capturing time stepping schemes
 * for first order systems.
 * It is mainly based on some extensions of the Backward Euler and \f$\theta-\gamma\f$
 * schemes proposed in the pionnering work of J.J. Moreau for the sweeping process
 * 
 * J.J. Moreau. Evolution problem associated with a moving convex set in a Hilbert space.
 * Journal of Differential Equations, 26, pp 347--374, 1977.
 *
 * Variants are now used to integrate LCS, Relay systems, Higher order sweeping process see 
 * for instance
 *
 *
 * Consistency of a time-stepping method for a class of piecewise linear networks\br
 * M.K. Camlibel, W.P.M.H. Heemels, and J.M. Schumacher
 * IEEE Transactions on Circuits and Systems I, 2002, 49(3):349--357
 *
 * Numerical methods for nonsmooth dynamical systems: applications in mechanics and electronics\br
 * V Acary, B Brogliato Springer Verlag 2008
 *
 * Convergence of time-stepping schemes for passive and extended linear complementarity systems
 * L. Han, A. Tiwari, M.K. Camlibel, and J.-S. Pang SIAM Journal on Numerical Analysis 2009, 47(5):3768-3796
 *
 * On preserving dissipativity properties of linear complementarity dynamical systems with the &theta-method\br
 * Greenhalgh Scott, Acary Vincent, Brogliato Bernard Numer. Math., , 2013.
 *
 * Main time--integration schemes are based on the following \f$\theta-\gamma\f$ scheme
 *
 * \f{equation}{
 *  \begin{cases}
 *   \label{eq:toto1}
 *     M x_{k+1} = M x_{k} +h\theta f(x_{k+1},t_{k+1})+h(1-\theta) f(x_k,t_k) + h \gamma r(t_{k+1})
 *   + h(1-\gamma)r(t_k)  \\[2mm]
 *   y_{k+1} =  h(t_{k+1},x_{k+1},\lambda _{k+1}) \\[2mm]
 *   r_{k+1} = g(x_{k+1},\lambda_{k+1},t_{k+1})\\[2mm]
 *    \mbox{nslaw} ( y_{k+1} , \lambda_{k+1})
 * \end{cases}
 * \f}
 * where \f$\theta = [0,1]\f$ and \f$\gamma \in [0,1]\f$. 
 * As in Acary & Brogliato 2008, we call the previous problem  the ``one--step nonsmooth problem''.
 *
 * Another variant can also be used (FullThetaGamma scheme)
 *  \f{equation}{
 *   \begin{cases}
 *     M x_{k+1} = M x_{k} +h f(x_{k+\theta},t_{k+1}) + h r(t_{k+\gamma}) \\[2mm]
 *     y_{k+\gamma} =  h(t_{k+\gamma},x_{k+\gamma},\lambda _{k+\gamma}) \\[2mm]
 *     r_{k+\gamma} = g(x_{k+\gamma},\lambda_{k+\gamma},t_{k+\gamma})\\[2mm]
 *     \mbox{nslaw} ( y_{k+\gamma} , \lambda_{k+\gamma})
 *   \end{cases}
 * \f}
 * 
 *
 * EulerMoreauOSI class is used to define some time-integrators methods for a
 * list of first order dynamical systems. A EulerMoreauOSI instance is defined by 
 * the value of theta  and possibly gamma and the list of
 * concerned dynamical systems.  
 *
 * Each DynamicalSystem is associated to a SiconosMatrix, named "W", which is the "iteration" matrix.
 * W matrices are initialized and computed in initW and computeW. Depending on the DS type, they may
 * depend on time t and DS state x.
 *
 * For first order systems, the implementation uses _r for storing the
 * the input due to the nonsmooth law. This EulerMoreauOSI scheme assumes that the
 * relative degree is zero or one and one level for _r is sufficient
 *
 * Main functions:
 *
 * - computeFreeState(): computes xfree (or vfree), dynamical systems
 *   state without taking non-smooth part into account \n
 *
 * - updateState(): computes x (q,v), the complete dynamical systems
 *    states.
 *
 * See User's guide, for details.
 *
 */

class EulerMoreauOSI : public OneStepIntegrator
{
protected:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(EulerMoreauOSI);


  /** Stl map that associates a W EulerMoreauOSI matrix to each DynamicalSystem of the OSI */
  MapOfDSMatrices WMap;

  /** Stl map that associates the columns of  W EulerMoreauOSI matrix to each DynamicalSystem of the OSI if it has some boundary conditions */
  MapOfDSMatrices _WBoundaryConditionsMap;

  /** Stl map that associates a theta parameter for the integration
  *  scheme to each DynamicalSystem of the OSI */
  double _theta;

  /** A gamma parameter for the integration scheme to each DynamicalSystem of the OSI
   * This parameter is used to apply a theta-method to the input $r$
   */
  double _gamma;

  /** a boolean to know if the parameter must be used or not
   */
  bool _useGamma;

  /** a boolean to know if the parameter must be used or not
   */
  bool _useGammaForRelation;

  /** nslaw effects
   */
  struct _NSLEffectOnFreeOutput;
  friend struct _NSLEffectOnFreeOutput;


  /** Default constructor
   */
  EulerMoreauOSI() {};

public:
  /** constructor from a minimum set of data: one DS and its theta
   *  \param ds SP::DynamicalSystem : the DynamicalSystem linked to the OneStepIntegrator
   *  \param theta value of the parameter
   */
  EulerMoreauOSI(SP::DynamicalSystem, double);

  /** constructor from theta value only
   *  \param theta value for all DS.
   */
  EulerMoreauOSI(double);

  /** constructor from a minimum set of data: one DS and its theta
   *  \param SP::DynamicalSystem : the DynamicalSystem linked to the OneStepIntegrator
   *  \param Theta value
   *  \param gamma value
   */
  EulerMoreauOSI(SP::DynamicalSystem, double, double);

  /** constructor from theta value only
   *  \param theta value for all these DS.
   *  \param gamma value for all these DS.
   */
  EulerMoreauOSI(double, double);

  /** destructor
   */
  virtual ~EulerMoreauOSI() {};

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

  /** initialization of the EulerMoreauOSI integrator; for linear time
      invariant systems, we compute time invariant operator (example :
      W)
   */
  virtual void initialize();

  /** init WMap[ds] EulerMoreauOSI matrix at time t
   *  \param the time (double)
   *  \param a pointer to DynamicalSystem
   */
  void initW(double, SP::DynamicalSystem);

  /** compute WMap[ds] EulerMoreauOSI matrix at time t
   *  \param the time (double)
   *  \param a pointer to DynamicalSystem
   */
  void computeW(double, SP::DynamicalSystem);

  /** compute WBoundaryConditionsMap[ds] EulerMoreauOSI matrix at time t
   *  \param the time (double)
   *  \param a pointer to DynamicalSystem
   */
  void computeWBoundaryConditions(SP::DynamicalSystem);

  /** init WBoundaryConditionsMap[ds] EulerMoreauOSI
   *  \param a pointer to DynamicalSystem
   */
  void initWBoundaryConditions(SP::DynamicalSystem);

  /** return the maximum of all norms for the "EulerMoreauOSI-discretized" residus of DS
      \return a double
   */
  double computeResidu();

  /** Perform the integration of the dynamical systems linked to this integrator
   *  without taking into account the nonsmooth input (_r or _p)
   */
  virtual void computeFreeState();

  /** integrates the Interaction linked to this integrator, without taking non-smooth effects into account
   * \param vertex of the interaction graph
   * \param osnsp pointer to OneStepNSProblem
   */
  virtual void computeFreeOutput(InteractionsGraph::VDescriptor& vertex_inter, OneStepNSProblem* osnsp);

  /** Apply the rule to one Interaction to known if is it should be included
   * in the IndexSet of level i
   */
  //virtual bool addInteractionInIndexSet(SP::Interaction inter, unsigned int i);

  /** Apply the rule to one Interaction to known if is it should be removed
   * in the IndexSet of level i
   */
  //virtual bool removeInteractionInIndexSet(SP::Interaction inter, unsigned int i);


  /**
   */
  void prepareNewtonIteration(double time);


  /** integrate the system, between tinit and tend (->iout=true), with possible stop at tout (->iout=false)
   *  \param double: tinit, initial time
   *  \param double: tend, end time
   *  \param double: tout, real end time
   *  \param int: useless flag (for EulerMoreauOSI, used in LsodarOSI)
   */
  void integrate(double&, double&, double&, int&);

  /** updates the state of the Dynamical Systems
   *  \param level the level of interest for the dynamics: not used at the time
   */
  virtual void updateState(const unsigned int level);

  /** copy the matrix W of the OneStepNSProblem to the XML tree
   */
  void saveWToXML();

  /** Displays the data of the EulerMoreauOSI's integrator
   */
  void display();

  /** insert a dynamical system in this Integrator
   *  \param a SP::DynamicalSystem
   */
  void insertDynamicalSystem(SP::DynamicalSystem ds);

  /** visitors hook
  */
  ACCEPT_STD_VISITORS();

};

TYPEDEF_SPTR(EulerMoreauOSI)

#endif // EulerMoreauOSI_H
