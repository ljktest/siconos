/* Siconos-Kernel, Copyright INRIA 2005-2010.
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
/*! \file NewtonEulerR.h

*/
#ifndef NEWTONEULERRELATION_H
#define NEWTONEULERRELATION_H

#include "Relation.hpp"

class DynamicalSystem;
class RelationXML;
class SimpleMatrix;
class SimpleVector;

/** NewtonEuler (Non Linear) Relation (generic interface)
 *
 * \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date Apr 27, 2004
 *
 * Relations for NewtonEuler Dynamical Systems. This class is only an interface for specific (Linear, Scleronomous ...)
 * NewtonEuler Relations (see derived classes).
 *
 *  Class name = type+subType.
 *
 * If y = h(...), all the gradients of are handled by G object.
 * For example, G[0] = \f$ \nabla_q h(q,...) \f$.
 *
 * In corresponding derived classes, h and Gi are connected to plug-in functions (user-defined).
 *
 */

/**Pointer to function - Plug-in utilities*/
typedef void (*FPtr2)(unsigned int, const double*, unsigned int, const double*, double*, unsigned int, double*);

/**Pointer to function - Plug-in utilities*/
typedef void (*FPtr3)(unsigned int, const double*, unsigned int, double*, unsigned int, double*);

/**Pointer to function - Plug-in utilities*/
typedef void (*FPtr4)(unsigned int, const double*, double, unsigned int, double*, unsigned int, double*);


class NewtonEulerR : public Relation
{
public:

  enum DataNames {z, q0, q1, q2, p0, p1, p2, sizeDataNames};

protected:
  unsigned int _ysize;
  unsigned int _xsize;
  unsigned int _qsize;

  SP::SimpleVector _workQ;

  /** Jacobian matrices of H */
  SP::SiconosMatrix _jachq;
  SP::SiconosMatrix _jachqDot;
  SP::SiconosMatrix _jachlambda;

  /**vector e*/
  SP::SiconosVector _e;

  /**vector of contact forces*/
  SP::SimpleVector _contactForce;

  /*updated in UR*/
  SP::SiconosMatrix _jachqT;

  /** basic constructor
      \param the sub-type of the relation
   */
  NewtonEulerR(RELATION::SUBTYPES lagType): Relation(RELATION::NewtonEuler, lagType) {}

  /** constructor from xml file
   *  \param relationXML
   *  \param string: relation subType
   */
  NewtonEulerR(SP::RelationXML relxml, RELATION::SUBTYPES newSubType): Relation(relxml, RELATION::NewtonEuler, newSubType) {}

  /** initialize components specific to derived classes.
   */
  virtual void initComponents();

public:
  NewtonEulerR(): Relation(RELATION::NewtonEuler, RELATION::NonLinearR) {}

  /** destructor
   */
  virtual ~NewtonEulerR() {};

  // -- Jach --

  /** get matrix Jach[index]
   *  \return a SimpleMatrix
  inline const SimpleMatrix getJach(unsigned int  index = 0) const { return *(Jach.at(index)); }
   */

  /** get a pointer on matrix Jach[index]
   *  \return a pointer on a SiconosMatrix
   */
  inline SP::SiconosMatrix jachq() const
  {
    return _jachq;
  }
  inline void setJachq(SP::SiconosMatrix newJachq)
  {
    _jachq = newJachq;
  }
  inline SP::SiconosMatrix jacQDotH() const
  {
    return _jachqDot;
  }
  inline SP::SiconosMatrix jachlambda() const
  {
    return _jachlambda;
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

  /** set the value of Jach[index] to newValue (copy)
   *  \param SiconosMatrix newValue
   *  \param unsigned int: index position in Jach vector

  template <class U> void setJach(const U& newValue, unsigned int index = 0)
    {
      assert(index>=Jach.size()&&"NewtonEulerR:: setJach(mat,index), index out of range. Maybe you do not set the sub-type of the relation?");

      if(Jach[index]) Jach[index]->resize(newValue.size(0), newValue.size(1));
      setObject<PluggedMatrix,SP_PluggedMatrix,U>(Jach[index],newValue);
    }
  */
  /** set Jach[index] to pointer newPtr (pointer link)
   *  \param SP::SiconosMatrix  newPtr
   *  \param unsigned int: index position in Jach vector
   */
  inline void setJachqPtr(SP::SiconosMatrix newPtr)
  {
    _jachq = newPtr ;
  }

  /** To get the name of Jach[i] plugin
   *  \return a string
  const std::string getJachName(unsigned int i) const {return Jach[i]->getPluginName();}
   */

  /** true if Jach[i] is plugged
   *  \return a bool
  const bool isJachPlugged(unsigned int i) const {return Jach[i]->isPlugged();}
   */

  /** Gets the number of computed jacobians for h
      \return an unsigned int.
  inline unsigned int numberOfJacobiansForH() const { return Jach.size();}
  */


  /** initialize the relation (check sizes, memory allocation ...)
      \param SP to Interaction: the interaction that owns this relation
  */
  void initialize(SP::Interaction);

  /** to compute y = h(q,v,t) using plug-in mechanism
   * \param: double, current time
   */
  virtual void computeh(double);

  /** default function to compute jacobianH
   *  \param double : current time
   *  \param index for jacobian (0: jacobian according to x, 1 according to lambda)

  void computeJachx(double);*/
  virtual void computeJachlambda(double)
  {
    ;
  }
  virtual void computeJachq(double)
  {
    ;
  }
  virtual void computeJachqDot(double)
  {
    ;
  }
  virtual void computeJacglambda(double)
  {
    ;
  }
  virtual void computeJacgq(double)
  {
    ;
  }
  virtual void computeJacgqDot(double)
  {
    ;
  }
  /* compute all the H Jacobian */
  virtual void computeJach(double t)
  {
    computeJachq(t);
    computeJachqDot(t);
    computeJachlambda(t);
  }
  /* compute all the G Jacobian */
  virtual void computeJacg(double t)
  {
    computeJacgq(t);
    computeJacgqDot(t);
    computeJacglambda(t);
  }


  /** to compute output
    *  \param double : current time
    *  \param unsigned int: number of the derivative to compute, optional, default = 0.
    */
  virtual void computeOutput(double, unsigned int = 0) ;

  /** to compute p
   *  \param double : current time
   *  \param unsigned int: "derivative" order of lambda used to compute input
   */
  virtual void computeInput(double, unsigned int = 0) ;

  /** copy the data of the Relation to the XML tree
   */
  void saveRelationToXML() const;




  /**
  * return a SP on the C matrix.
  * The matrix C in the linear case, else it returns Jacobian of the output with respect to x.
  *
  */
  virtual SP::SiconosMatrix C() const
  {
    return jachq();
  }
  /**
   * return a SP on the D matrix.
   * The matrix D in the linear case, else it returns Jacobian of the output with respect to lambda.
   */
  virtual SP::SiconosMatrix D() const
  {
    return jachlambda();
  }
  /**
   * return a SP on the B matrix.
   * The matrix B in the linear case, else it returns Jacobian of the input with respect to lambda.
   */
  virtual SP::SiconosMatrix B() const
  {
    return jacglambda();
  }

  SP::SimpleVector contactForce()
  {
    return _contactForce;
  }

  /** main relation members display
   */
  void display() const;


  ACCEPT_STD_VISITORS();

};
TYPEDEF_SPTR(NewtonEulerR);
#endif // NEWTONEULERRELATION_H
