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
/*! \file NewtonEulerR.hpp

 */
#ifndef NEWTONEULERRELATION_H
#define NEWTONEULERRELATION_H

#include "Relation.hpp"
#include "PluginTypes.hpp"

/** NewtonEuler (Non Linear) Relation (generic interface)
 *
 * \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date Apr 27, 2004
 *
 * \class NewtonEulerR 
 * Relations for NewtonEuler Dynamical Systems. This class is only an
 * interface for specific (Linear, Scleronomous ...)  NewtonEuler
 * Relations (see derived classes).
 *
 *  Class name = type+subType.
 *
 * If y = h(...), all the gradients of are handled by G object.
 * For example, G[0] = \f$ \nabla_q h(q,...) \f$.
 *
 * In corresponding derived classes, h and Gi are connected to plug-in functions (user-defined).
 * For more details, see the DevNotes.pdf, chapter NewtonEuler.
 */

class NewtonEulerR : public Relation
{
public:
// add deltaq ??? -- xhub 30/03/2014
  enum NewtonEulerRDS  {xfree, z, q0, velocity, dotq, p0, p1, p2, DSlinkSize};
  // enum NewtonEulerRVec {xfree, z, q0, dotq, p0, p1, p2, workVecSize};
  // enum NewtonEulerRMat {C, D, F, workMatSize};

protected:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(NewtonEulerR);

  /** Jacobian matrices of H */
  /** Jacobian matrices of \f$y = h(t,q,\dot q,\ldots)\f$ */

  /**The Jacobian of the constraints with respect to the generalized coodinates  \f$q\f$
   *  i.e. \f[\nabla^T_q h(t,q,\dot q,\ldots)\f]
   */
  SP::SiconosMatrix _jachq;

  /**The Jacobian of the constraints with respect to the generalized velocities  \f$\dot q\f$
   *  i.e. \f[\nabla^T_{\dot q} h(t,q,\dot q,\ldots)\f]
   */
  SP::SiconosMatrix _jachqDot;

  /**The time-derivative of Jacobian of the constraints with respect
     to the generalized coordinates  \f$ q\f$
   * i.e. \f[\frac{d}{dt} \nabla^T_{q} h(t,q,\dot q,\ldots).\f]
   * This value is useful to compute the second-order
   * time--derivative of the constraints with respect to time.
   */
  SP::SiconosMatrix _dotjachq;

  SP::SiconosMatrix _jachlambda;
  SP::SiconosMatrix _jacglambda;

  /**vector e*/
  SP::SiconosVector _e;
  /*Used for the projection formulation*/

  /**vector of contact forces, ie: _contactForce = B lambda. Useful for the end user.*/
  SP::SiconosVector _contactForce;

  /**updated in computeJachqT:
  In the case of the bilateral constrains, it is _jachq._T.
  In the case of a local frame, _jachqT is built from the geometrical datas(local frame, point of contact).*/
  SP::SiconosMatrix _jachqT;

  /** basic constructor
  \param lagType the sub-type of the relation
  */
  NewtonEulerR(RELATION::SUBTYPES lagType): Relation(RELATION::NewtonEuler, lagType) {}

  /** initialize components specific to derived classes.
   * \param inter  Interaction associated with the Relation
   * \param DSlink 
   * \param workV
   * \param workM 
   */
  virtual void initComponents(Interaction& inter, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM);

public:
  NewtonEulerR(): Relation(RELATION::NewtonEuler, RELATION::NonLinearR) {}

  /** destructor
  */
  virtual ~NewtonEulerR() {};

  // -- Jach --

  /** get a pointer on matrix Jach[index]
  *  \return a pointer on a SiconosMatrix
  */
  inline SP::SiconosMatrix jachq() const
  {
    return _jachq;
  }

  //proj_with_q  inline SP::SiconosMatrix jachqProj() const { return _jachqProj; }
  void setJachq(SP::SiconosMatrix newJachq);

  inline SP::SiconosMatrix jachqDot() const
  {
    return _jachqDot;
  }
  inline SP::SiconosMatrix dotJachq() const
  {
    assert(_dotjachq);
    return _dotjachq;
  }

  inline SP::SiconosVector secondOrderTimeDerivativeTerms()
  {
    assert(_secondOrderTimeDerivativeTerms);
    return _secondOrderTimeDerivativeTerms;
  };

  inline SP::SiconosMatrix jachlambda() const
  {
    return _jachlambda;
  }
  inline SP::SiconosMatrix jacglambda() const
  {
    return _jacglambda;
  }
  inline void setE(SP::SiconosVector newE)
  {
    _e = newE;
  }

  inline SP::SiconosMatrix jachqT() const
  {
    return _jachqT;
  }
  inline void setJachqT(SP::SiconosMatrix newJachqT)
  {
    _jachqT = newJachqT;
  }

  /** set Jach[index] to pointer newPtr (pointer link)
  *  \param newPtr the new matrix
  */
  void setJachqPtr(SP::SiconosMatrix newPtr);

  /** Plugin object for the time--derivative of Jacobian i.e.
  * \f[\frac{d}{dt} \nabla^T_{q} h(t,q,\dot q,\ldots).\f]
  * stored in _dotjachq
  */
  SP::PluggedObject _plugindotjacqh;

  /**  the additional  terms of the second order time derivative of y
   *
   *    \f$ \nabla_q h(q) \dot T v + \frac{d}{dt}(\nabla_q h(q) ) T v \f$
   *
   */
  SP::SiconosVector _secondOrderTimeDerivativeTerms;

  /** initialize the relation (check sizes, memory allocation ...)
   * \param inter the interaction using this relation
   * \param DSlink the container of the link to DynamicalSystem attributes
   * \param workV the work vectors
   * \param workM work matrices
  */
  void initialize(Interaction& inter, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM);

  /** to compute y = h(q,v,t) using plug-in mechanism
  * \param time current time
  * \param q0 the position
  * \param y the output
  */
  virtual void computeh(double time, BlockVector& q0, SiconosVector& y);

  /** default function to compute jacobianH
   * \param time current time
   * \param inter the interaction using this relation
   */

  virtual void computeJachlambda(double time, Interaction& inter)
  {
    ;
  }
  /** compute the jacobian of h w.r.t. q 
   * \param time current time
   * \param inter the interaction using this relation
   * \param DSlink the container of the link to DynamicalSystem attributes
   */
  virtual void computeJachq(double time, Interaction& inter, VectorOfBlockVectors& DSlink)
  {
    ;
  }
  /** compute the jacobian of h w.r.t. \f$\dot{q}\f$ 
   * \param time current time
   * \param inter the interaction using this relation
   */
  virtual void computeJachqDot(double time, Interaction& inter)
  {
    /* \warning. This method should never be called, since we are only considering
     * holonomic NewtonEulerR up to now
     */
    assert(0) ;
  }
  virtual void computeDotJachq(double time, SiconosVector& workQ, SiconosVector& workZ, SiconosVector& workQdot);


  /** compute the jacobian of h w.r.t. \f$\dot{q}\f$ 
   * \param time current time
   * \param inter the interaction using this relation
   */
  virtual void computeJacglambda(double time, Interaction& inter)
  {
    ;
  }
  /** compute the jacobian of h w.r.t. \f$\dot{q}\f$ 
   * \param time current time
   * \param inter the interaction using this relation
   */
  virtual void computeJacgq(double time, Interaction& inter)
  {
    ;
  }
  /** compute the jacobian of h w.r.t. \f$\dot{q}\f$ 
   * \param time current time
   * \param inter the interaction using this relation
   */
  virtual void computeJacgqDot(double time, Interaction& inter)
  {
    ;
  }

  /* default implementation consists in multiplying jachq and T
     \param inter interaction that owns the relation
     \param ds1 dynamical system linked to this interaction (source)
     \param ds2 second ds linked to this interaction (target). If there is 
     only one ds in the inter, call this function with ..., ds, ds)
  */
  virtual void computeJachqT(Interaction& inter, SP::DynamicalSystem ds1, SP::DynamicalSystem ds2);

  /** compute all the jacobian of h 
   * \param time current time
   * \param inter the interaction using this relation
   * \param interProp Interaction properties
   */
  virtual void computeJach(double time, Interaction& inter, InteractionProperties& interProp);

  /** compute all the jacobian of g 
   * \param time current time
   * \param inter the interaction using this relation
   * \param interProp Interaction properties
   */
  virtual void computeJacg(double time, Interaction& inter, InteractionProperties& interProp)
  {
    computeJacgq(time, inter);
    computeJacgqDot(time, inter);
    computeJacglambda(time, inter);
  }

  /** To compute the terms of the second order time derivative of y
      \f$ \nabla_q h(q) \dot T v + \frac{d}{dt}(\nabla_q h(q) ) T v \f$
      \param time  current time
      \param inter interaction that owns the relation
      \param DSlink the container of the link to DynamicalSystem attributes
      \param ds1 dynamical system linked to this interaction (source)
      \param ds2 second ds linked to this interaction (target). If there is 
      only one ds in the inter, call this function with ..., ds, ds)
   */
  void computeSecondOrderTimeDerivativeTerms(double time, Interaction& inter, VectorOfBlockVectors& DSlink, SP::DynamicalSystem ds1, SP::DynamicalSystem ds2);

  /** to compute output
   * \param time current time
   * \param inter the interaction using this relation
   * \param interProp Interaction properties
   * \param derivativeNumber number of the derivative to compute, optional, default = 0.
   */
  virtual void computeOutput(double time, Interaction& inter, InteractionProperties& interProp, unsigned int derivativeNumber = 0);

  /** to compute the input
   * \param time current time
   * \param inter the interaction using this relation
   * \param interProp Interaction properties
   * \param level number of the derivative to compute, optional, default = 0.
   */
  virtual void computeInput(double time, Interaction& inter, InteractionProperties& interProp, unsigned int level = 0);

  /**
  * return a SP on the C matrix.
  * The matrix C in the linear case, else it returns Jacobian of the output with respect to x.
  * \return SP::SiconosMatrix
  */
  virtual inline SP::SiconosMatrix C() const
  {
    return _jachq;
  }
  /**
  * return a SP on the D matrix.
  * The matrix D in the linear case, else it returns Jacobian of the output with respect to lambda.
  * \return SP::SiconosMatrix
  */
  virtual inline SP::SiconosMatrix D() const
  {
    return _jachlambda;
  }
  /**
  * return a SP on the B matrix.
  * The matrix B in the linear case, else it returns Jacobian of the input with respect to lambda.
  * \return SP::SiconosMatrix
  */
  virtual inline SP::SiconosMatrix B() const
  {
    return _jacglambda;
  }
  /** A buffer containing the forces due to this.
  It is an output unused for the computation.
  Fix : is it usefull ?
  \return SP::SiconosVector
  */
  inline SP::SiconosVector contactForce() const
  {
    return _contactForce;
  };

  ACCEPT_STD_VISITORS();

};
#endif // NEWTONEULERRELATION_H
