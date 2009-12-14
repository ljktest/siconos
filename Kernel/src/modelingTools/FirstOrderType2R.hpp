/* Siconos-Kernel version 3.0.0, Copyright INRIA 2005-2008.
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
 * Contact: Vincent ACARY vincent.acary@inrialpes.fr
 */

/*! \file FirstOrderType2.h
\brief non linear relations, with y depending on dynamical systems state and r on lambda.
 */

#ifndef FirstOrderType2R_H
#define FirstOrderType2R_H

#include "FirstOrderR.hpp"


/** FirstOrder Non Linear Relation.
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) Apr 27, 2004
 *
 * Derived from FirstOrderR - See this class for more comments.
 *
 *  Relation for First Order Dynamical Systems, with:
 * \f{eqnarray}
 * y &=& h(X,\lambda,Z)\\
 * r &=& g(\lambda,Z)
 * \f}
 *
 * Operators (and their corresponding plug-in):
     - h: saved in Interaction as y (plug-in: output[0])
     - \f$ \nabla_x h \f$: jacobianH[0] ( output[1] )
     - g: saved in DS as r ( input[0])
     - \f$ \nabla_\lambda g \f$: jacobianG[0] ( input[1] )
 *
 */
class FirstOrderType2R : public FirstOrderR
{
protected:
  SP::SimpleMatrix JacXG;


public:
  /** xml constructor
   *  \param RelationXML smart pointer : the XML object.
   */
  FirstOrderType2R(SP::RelationXML);

  FirstOrderType2R();
  /** data constructor
   *  \param a string with computeOutput function name.
   *  \param a string with computeInput function name.
   */
  FirstOrderType2R(const std::string&, const std::string&);

  /** data constructor
   *  \param a string with computeOutput function name.
   *  \param a string with computeInput function name.
   *  \param a string: name of the function to compute the jacobian of h according to x
   *  \param a string: name of the function to compute the jacobian of g according to lambda
   */
  FirstOrderType2R(const std::string&, const std::string&, const std::string&, const std::string&);

  /** destructor
   */
  ~FirstOrderType2R() {};

  /** initialize the relation (check sizes, memory allocation ...)
      \param SP to Interaction: the interaction that owns this relation
   */
  virtual void initialize(SP::Interaction);

  /** default function to compute h
   *  \param double : current time
   */
  virtual void computeh(double);

  /** default function to compute g
   *  \param double : current time
   */
  virtual void computeg(double);

  /** default function to compute jacobianH
   *  \param double : not used
   *  \param not used
   */
  virtual void computeJacXH(double);
  virtual void computeJacLH(double);

  /** default function to compute jacobianG according to lambda
   *  \param double : current time
   *  \param index for jacobian: at the time only one possible jacobian => i = 0 is the default value .
   */
  virtual void computeJacLG(double);
  virtual void computeJacXG(double);

  virtual void computeJacG(double t);

  /** default function to compute y
   *  \param double: not used
   *  \param unsigned int: not used
   */
  virtual void computeOutput(double, unsigned int = 0);

  /** default function to compute r
   *  \param double : not used
   *  \param unsigned int: not used
   */
  virtual void computeInput(double, unsigned int = 0);
  /*
    inline SP_PluggedMatrix getB(){return JacG.at(1);};
  */

  virtual void preparNewtonIteration();

  /**
   * return true if the relation is linear.
   */

  virtual bool isLinear()
  {
    return false;
  }
  /** encapsulates an operation of dynamic casting. Needed by Python interface.
   *  \param Relation * : the relation which must be converted
   * \return a pointer on the relation if it is of the right type, NULL otherwise
   */
  static FirstOrderType2R* convert(Relation *r);
};

TYPEDEF_SPTR(FirstOrderType2R);

#endif
