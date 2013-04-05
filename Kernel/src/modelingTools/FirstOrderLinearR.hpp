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
/*! \file FirstOrderLinearR.hpp

 */
#ifndef FirstOrderLinearR_H
#define FirstOrderLinearR_H

#include "FirstOrderR.hpp"
/** Pointer to function used for plug-in for matrix-type operators (C,F etc) */
typedef void (*FOMatPtr1)(double, unsigned int, unsigned int, double*, unsigned int, double*);

/** Pointer to function used for plug-in for square matrix-type operators (D) */
typedef void (*FOMatPtr2)(double, unsigned int, double*, unsigned int, double*);

/** Pointer to function used for plug-in for vector-type operators (e) */
typedef void (*FOVecPtr)(double, unsigned int, double*, unsigned int, double*);

/** First Order Linear Relation

\author SICONOS Development Team - copyright INRIA
\version 3.0.0.
\date Apr 15, 2007

Linear Relation for First Order Dynamical Systems:

\f{eqnarray}
y &=& C(t,z)x(t) + F(t,z)z + D(t,z)\lambda + e(t,z) \\

R &=& B(t,z) \lambda
\f}

All coefficients can be plugged or not. Use isPlugged[name] to check if name ( = "C", "F" etc) is plugged.

Note: the connections (pointers equalities) between C, D, B and jacobianH and jacobianG of FirstOrderR class are done during initialize.

 */
class FirstOrderLinearR : public FirstOrderR
{

protected:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(FirstOrderLinearR);

  SP::SiconosMatrix _F;
  SP::SiconosVector _e;

public:

  /** default constructor, protected
  */
  FirstOrderLinearR();

  /** xml constructor
  \param SP::RelationXML : the XML object corresponding
  */
  FirstOrderLinearR(SP::RelationXML);

  /** Constructor with C and B plug-in names
  \param C plug-in name
  \param B plug-in name
  **/
  FirstOrderLinearR(const std::string&, const std::string&);

  /** Constructor all plug-in names
  \param C plug-in name
  \param D plug-in name
  \param F plug-in name
  \param e plug-in name
  \param B plug-in name
  **/
  FirstOrderLinearR(const std::string&, const std::string&, const std::string&, const std::string&, const std::string&);

  /** create the Relation from a set of data
  *  \param Plugged Matrix : the matrix C
  *  \param Plugged Matrix : the matrix B
  */
  FirstOrderLinearR(SP::SiconosMatrix, SP::SiconosMatrix);

  /** create the Relation from a set of data
  *  \param Plugged Matrix : C
  *  \param Plugged Matrix : D
  *  \param Plugged Matrix : F
  *  \param Plugged Vector : e
  *  \param Plugged Matrix : B
  */
  FirstOrderLinearR(SP::SiconosMatrix , SP::SiconosMatrix, SP::SiconosMatrix , SP::SiconosVector, SP::SiconosMatrix);

  /** create the Relation from a set of data
  *  \param  SiconosMatrix : the matrix C
  *  \param  SiconosMatrix : the matrix B

  FirstOrderLinearR(const SiconosMatrix& , const SiconosMatrix&);
  */
  /** create the Relation from a set of data
  *  \param  SiconosMatrix : C
  *  \param  SiconosMatrix : D
  *  \param  SiconosMatrix : F
  *  \param  SiconosVector  : e
  *  \param  SiconosMatrix : B

  FirstOrderLinearR(const SiconosMatrix& , const SiconosMatrix& , const SiconosMatrix&, const SiconosVector&, const SiconosMatrix&);
  */
  /** destructor
  */
  ~FirstOrderLinearR() {};

  // GETTERS/SETTERS

  // -- C --
  /** get the value of C
  *  \return plugged matrix

  inline const PluggedMatrix getC() const { return *(Jach.at(0)); }
  */
  /** get C
  *  \return pointer on a plugged matrix
  */
  //inline SP::SiconosMatrix C() const { return Jach.at(0); }

  /** set the value of C to newValue
  *  \param a plugged matrix

  template <class U> void setC(const U& newValue)
  {
  setJach(newValue,0);
  }
  */
  /** set C to pointer newPtr
  *  \param a SP to plugged matrix

  inline void setCPtr(SP_PluggedMatrix newPtr) {Jach[0] = newPtr;}
  */
  /** set a specified function to compute the matrix C
  *  \param string : the complete path to the plugin
  *  \param string : the function name to use in this plugin
  */


  void setComputeCFunction(const std::string& pluginPath, const std::string& functionName)
  {
    setComputeJachxFunction(pluginPath,  functionName);
  }


  // -- D --

  /** get the value of D
  *  \return plugged matrix
  */
  //inline const PluggedMatrix getD() const { return *(Jach.at(1)); }

  /** get D
  *  \return pointer on a plugged matrix
  */
  //inline SP::SiconosMatrix D() const { return Jach.at(1); }

  /** set the value of D to newValue
  *  \param a plugged matrix

  void setD(const U& newValue)
  {
  if(Jach.size()<2) Jach.resize(2);
  setJach(newValue,1);
  }
  */

  /** set a specified function to compute the matrix D
  *  \param string : the complete path to the plugin
  *  \param string : the function name to use in this plugin
  */
  void setComputeDFunction(const std::string& pluginPath, const std::string& functionName)
  {
    setComputeJachlambdaFunction(pluginPath,  functionName);
  }

  /** set D to pointer newPtr
  *  \param a SP to plugged matrix
  */
  inline void setDPtr(SP::SiconosMatrix newPtr)
  {
    _jachlambda = newPtr;
  }


  /** set F to pointer newPtr
  *  \param a SP to plugged matrix
  */
  inline void setFPtr(SP::SiconosMatrix newPtr)
  {
    _F = newPtr;
  }

  /** set a specified function to compute the matrix F
  *  \param string : the complete path to the plugin
  *  \param string : the function name to use in this plugin
  void setComputeFFunction(const std::string& , const std::string& );
  */

  // -- e --
  /** get the value of e
  *  \return plugged vector

  inline const Plugged_Vector_FTime getE() const { return *e; }
  */
  /** get e
  *  \return pointer on a plugged vector
  */
  inline SP::SiconosVector e() const
  {
    return _e;
  }

  //   /** set a specified function to compute the matrix B
  //    *  \param string : the complete path to the plugin
  //    *  \param string : the function name to use in this plugin
  //    */
  void setComputebFunction(const std::string& pluginPath, const std::string& functionName)
  {
    setComputeJacglambdaFunction(pluginPath,  functionName);
  }


  /** Function to compute matrix C
  */
  virtual void computeC(const double time, Interaction& inter);

  /** Function to compute matrix F
  */
  virtual void computeF(const double time, Interaction& inter);

  /** Function to compute matrix D
  */
  virtual void computeD(const double time, Interaction& inter);

  /** Function to compute vector e
  */
  virtual void computeE(const double time, Interaction& inter);

  /** Function to compute matrix B
  */
  virtual void computeb(const double time, Interaction& inter);

  /** initialize the relation (check sizes, memory allocation ...)
  \param inter Interaction: the interaction that owns this relation
  */
  virtual void initialize(Interaction & inter);

  /** default function to compute h
  *  \param double : current time
  */
  virtual void computeh(const double time, Interaction& inter);

  /** default function to compute g
  *  \param double : current time
  */
  virtual void computeg(const double time, Interaction& inter);

  /** default function to compute y
  *  \param double: not used
  *  \param unsigned int: not used
  */
  virtual void computeOutput(const double time, Interaction& inter, unsigned int = 0);

  /** default function to compute r
  *  \param double : not used
  *  \param unsigned int: not used
  */
  void computeInput(const double time, Interaction& inter, unsigned int = 0);

  /** copy the data of the Relation to the XML tree
  */
  void saveRelationToXML() const;

  /** print the data to the screen
  */
  void display() const;
  virtual void setComputeEFunction(FOVecPtr ptrFunct);
  virtual void setComputeEFunction(const std::string& pluginPath, const std::string& functionName);
  inline SP::SiconosMatrix F() const
  {
    return _F;
  }

  /** encapsulates an operation of dynamic casting. Needed by Python interface.
  *  \param Relation * : the relation which must be converted
  * \return a pointer on the relation if it is of the right type, NULL otherwise
  */
  static FirstOrderLinearR* convert(Relation *r);

  /**
  * return true if the relation is linear.
  */

  virtual bool isLinear()
  {
    return true;
  }
  ACCEPT_STD_VISITORS();

};

TYPEDEF_SPTR(FirstOrderLinearR)

#endif
