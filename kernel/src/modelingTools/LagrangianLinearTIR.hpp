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
/*! \file LagrangianLinearTIR.hpp

 */
#ifndef LAGRANGIANLINEARRELATION_H
#define LAGRANGIANLINEARRELATION_H

#include "LagrangianR.hpp"


/**  Lagrangian Linear Relation.

\author SICONOS Development Team - copyright INRIA
\version 3.0.0.
\date (Creation) Apr 27, 2004

Lagrangian Relation with:

\f[
y= Cq + e + D\lambda + Fz
\f]

\f[
p = C^t \lambda
\f]

C is the only required input to built a LagrangianLinearTIR.
D is optional and may represent a stiffness in the relation




 */
class LagrangianLinearTIR : public LagrangianR
{

protected:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(LagrangianLinearTIR);



  /** C*/
  //SP::SiconosMatrix C;

  /** D matrix, coefficient of lambda in y */
  //SP::SiconosMatrix D;

  /** F matrix, coefficient of z */
  SP::SiconosMatrix _F;

  /** e*/
  SP::SiconosVector _e;

public:

  /** Default constructor
  */
  LagrangianLinearTIR() : LagrangianR(RELATION::LinearTIR) {};

  /** create the Relation from a set of data
  *  \param C the matrix C
  */
  LagrangianLinearTIR(SP::SiconosMatrix C);

  /** create the Relation from a set of data
  *  \param C the matrix C
  *  \param D the matrix D
  *  \param F the matrix F
  *  \param e the vector e
  */
  LagrangianLinearTIR(SP::SiconosMatrix C, SP::SiconosMatrix D, SP::SiconosMatrix F, SP::SiconosVector e);

  /** create the Relation from a set of data
  *  \param C the matrix C
  *  \param e the vector e
  */
  LagrangianLinearTIR(SP::SiconosMatrix C, SP::SiconosVector e);

  /** create the Relation from a set of data
  *  \param C SiconosMatrix : the matrix C
  */
  LagrangianLinearTIR(const SiconosMatrix& C);

  /** create the Relation from a set of data
  *  \param C SiconosMatrix
  *  \param D SiconosMatrix
  *  \param F SiconosMatrix
  *  \param e SiconosVector
  */
  LagrangianLinearTIR(const SiconosMatrix& C, const SiconosMatrix& D, const SiconosMatrix& F, const SiconosVector& e);

  /** create the Relation from a set of data
  *  \param C SiconosMatrix
  *  \param e SiconosVector
  */
  LagrangianLinearTIR(const SiconosMatrix& C, const SiconosVector& e);

  /** destructor
  */
  virtual ~LagrangianLinearTIR() {};

  /** initialize LagrangianLinearTIR specific operators.
   * \param inter an Interaction using this relation
   * \param DSlink
   * \param workV
   * \param workM
   */
  void initComponents(Interaction& inter, VectorOfBlockVectors& DSlink,
                      VectorOfVectors& workV, VectorOfSMatrices& workM);

  /** default function to compute y
  *  \param time not used
  *  \param inter the Interaction we want to update
  *  \param interProp interaction properties
  *  \param derivativeNumber the derivative of y we want to compute
  */
  void computeOutput(double time, Interaction& inter, InteractionProperties& interProp, unsigned int derivativeNumber = 0);

  /** default function to compute r
  *  \param time not used
  *  \param inter the Interaction we want to update
  *  \param interProp interaction properties
  *  \param level the derivative of lambda we want to compute
  */
  void computeInput(double time, Interaction& inter, InteractionProperties& interProp, unsigned int level = 0);

  /* compute all the H Jacobian
   *  \param time not used
   *  \param inter the Interaction we want to update
   *  \param interProp interaction properties
   */
  void computeJach(double time, Interaction& inter, InteractionProperties& interProp)
  {
    ;
  }


  /* compute all the G Jacobian
   *  \param time not used
   *  \param inter the Interaction we want to update
   *  \param interProp interaction properties
   */
  void computeJacg(double time, Interaction& inter, InteractionProperties& interProp)
  {
    ;
  }





  // GETTERS/SETTERS

  // -- C --
  /** get C
   *  \return pointer on a plugged matrix
   */
  inline SP::SiconosMatrix C() const
  {
    return _jachq;
  }


  /** set C to pointer newPtr
   *  \param newPtr a SP to plugged matrix
   */
  inline void setCPtr(SP::SiconosMatrix newPtr)
  {
    _jachq = newPtr;
  }

  // -- D --

  /** get D
   *  \return pointer on a plugged matrix
   */
  inline SP::SiconosMatrix D() const
  {
    return _jachlambda;
  }


  /** set D to pointer newPtr
   * \param newPtr a SP to plugged matrix
   */
  inline void setDPtr(SP::SiconosMatrix newPtr)
  {
    _jachlambda = newPtr;
  }

  // -- F --

  /** get F
  *  \return pointer on a plugged matrix
  */
  inline SP::SiconosMatrix F() const
  {
    return _F;
  }

  /** set F to pointer newPtr
   * \param newPtr a SP to plugged matrix
   */
  inline void setFPtr(SP::SiconosMatrix newPtr)
  {
    _F = newPtr;
  }

  // -- e --

  /** get e
   *  \return pointer on a plugged vector
   */
  inline SP::SiconosVector e() const
  {
    return _e;
  }

  /** set e to pointer newPtr
   *  \param newPtr a SP to plugged vector
   */
  inline void setEPtr(SP::SiconosVector newPtr)
  {
    _e = newPtr;
  }

  /** get a pointer on matrix Jach[index]
   *  \return a pointer on a SiconosMatrix
   */

  /** print the data to the screen
   */
  void display() const;

  /**
   * \return true if the relation is linear.
   */

  virtual bool isLinear()
  {
    return true;
  }
  ACCEPT_STD_VISITORS();

};

TYPEDEF_SPTR(LagrangianLinearTIR)

#endif
