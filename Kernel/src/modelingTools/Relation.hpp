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

/*! \file Relation.h
  \brief General interface for relations.
*/

#ifndef RELATION_H
#define RELATION_H

#include "SimpleVector.hpp"
#include "SimpleMatrix.hpp"
#include "Interaction.hpp"
#include "RuntimeException.hpp"
#include "Tools.hpp"
#include "Plugin.hpp"
#include "SiconosPointers.hpp"
#include "PluginTypes.hpp"
#include "RelationNamespace.hpp"
#include "PluggedObject.hpp"
#include <boost/weak_ptr.hpp>

class SimpleVector;
class SimpleMatrix;

/** General Non Linear Relation (Virtual Base class for Relations).
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) Apr 27, 2004
 *
 *  A relation is a link between global variables of the Dynamical
 * Systems and some local ones, named y and lambda; belonging to one
 * and only one Interaction.
 *
 * The present class is an interface to all relations and provides
 * tools to define and describe them.
 *
 * Each relation must have the two following functions:
 *
 *  - computeOutput(...) to compute y using DS global variables.
 * - computeInput(...) to compute non-smooth DS part (r or p) using
 *   lambda.
 *
 * Depending on the DS class and the link type, various relations (ie
 * derived classes) are available:
 *   - FirstOrder, for FirstOrderDS and derived classes.
 *   - Lagrangian, for LagrangianDS and derived classes.
 *
 *  The specific type (Linear, Scleronomous ...) is then given by the
 *  "subType". \n
 *
 * The relation holds also:
 *  - a pointer to an xml object
 *  - a VectorMap to handle links to DS variables (no copy!!!). Filled
 *    in during initialize.
 *
 */
class Relation
{

protected:
  /** Plug-in to compute h(...)
  */
  SP::PluggedObject _pluginh;

  /** Plug-in to compute \f$ \nabla_x h(..)\f$
   */
  SP::PluggedObject _plunginJachx;
  /** Plug-in to compute \f$ \nabla_{\lambda} h(..)\f$
   */
  SP::PluggedObject _pluginJachlambda;

  /** Plug-in to compute g(...)
   */
  SP::PluggedObject _pluging;

  /** Plug-in to compute \f$ \nabla_\lambda g(lambda,t,z)\f$
   */
  SP::PluggedObject _pluginJacLg;
  /** Plug-in to compute f.$
   */
  SP::PluggedObject _pluginf;
  /** Plug-in to compute e.$
   */
  SP::PluggedObject _plugine;
  /** To initialize all the plugin functions with NULL.
   */
  void zeroPlugin();

protected:

  /** type of the Relation: FirstOrder or Lagrangian */
  RELATION::TYPES relationType;

  /** sub-type of the Relation (exple: LinearTIR or ScleronomousR ...) */
  RELATION::SUBTYPES subType;

  /** The Interaction linked to this Relation */
  boost::weak_ptr<Interaction> _interaction;

  /** Name of the plugin function used to compute h*/
  std::string hName;

  /** Name of the plugin function used to compute g*/
  std::string gName;

  /** A map of vectors, used to save links (pointers) to DS objects of
      the interaction */
  std::vector<SP::SiconosVector> data;

  /** the object linked this Relation to read XML data */
  SP::RelationXML relationxml;

  /** work vector for x */
  SP::SimpleVector _workR;

  /** work vector for x */
  SP::SimpleVector _workX;

  /** work vector for z */
  SP::SimpleVector _workZ;

  /** work vector for y */
  SP::SimpleVector _workY;

  /** work vector for lambda */
  SP::SimpleVector _workL;

  /** The residu y of the newton iterations*/
  SP::SiconosVector _Residuy;
  /*value of h at the current newton iteration*/
  SP::SiconosVector _h_alpha;

  SP::SiconosMatrix Jachlambda;


  /** basic constructor
   *  \param a string that gives the type of the relation
   *  \param a string that gives the subtype of the relation
   */
  Relation(RELATION::TYPES, RELATION::SUBTYPES);

  /** xml constructor
   *  \param RelationXML* : the XML object corresponding
   *  \param a string that gives the type of the relation
   *  \param a string that gives the subtype of the relation
   */
  Relation(SP::RelationXML, RELATION::TYPES, RELATION::SUBTYPES);

private:

  /** default constructor => private, no copy nor pass-by-value
   */
  Relation();

  /** copy constructor => private, no copy nor pass-by-value.
   */
  Relation(const Relation&);

  /** Assignment  => private, forbidden
   */
  Relation& operator=(const Relation&);

public:


  /** destructor
   */
  virtual ~Relation();

  /** To get the pointer to the Interaction linked to the present
   *  Relation
   *  \return a pointer to Interaction.
   */
  inline SP::Interaction interaction()
  {
    assert(!_interaction.expired());
    return _interaction.lock();
  }

  /** To set the pointer to the Interaction linked to the present
   *  Relation
   *  \param a pointer to Interaction.
   */
  inline void setInteractionPtr(SP::Interaction newInter)
  {
    _interaction = boost::weak_ptr<Interaction>(newInter);
  }

  /** To get the RelationXML* of the Relation
   *  \return a pointer on the RelationXML of the Relation
   */
  inline SP::RelationXML getRelationXML()
  {
    return relationxml;
  }

  /** To set the RelationXML* of the Relation
   *  \param RelationXML* : the pointer to set
   */
  inline void setRelationXML(SP::RelationXML rxml)
  {
    relationxml = rxml;
  }

  /** To get the type of the Relation (FirstOrder or Lagrangian)
   *  \return the type of the Relation
   */
  inline const RELATION::TYPES  getType() const
  {
    return relationType;
  }

  /** To get the subType of the Relation
   *  \return the sub-type of the Relation
   */
  inline const RELATION::SUBTYPES  getSubType() const
  {
    return subType;
  }

  /** To get the name of h plugin
   *  \return a string
   */
  inline const std::string gethName() const
  {
    return hName;
  }

  /** To get the name of g plugin
   *  \return a string
   */
  inline const std::string getgName() const
  {
    return gName;
  }

  /** To get the name of Jach[i] plugin
   *  \return a string
   */
  virtual const std::string getJachName(unsigned int) const
  {
    return "unamed";
  }

  /** To get the name of Jacg[i] plugin
   *  \return a string
   */
  virtual const std::string getJacgName(unsigned int) const
  {
    return "unamed";
  }

  /** true if h is plugged
   *  \return a bool

  inline const bool isHPlugged() const {return output;}
  */
  /** true if g is plugged
   *  \return a bool

  inline const bool isGPlugged() const {return input;}
  */
  /** true if Jach[i] is plugged
   *  \return a bool
  virtual const bool isJachPlugged(unsigned int) const {return false;}
   */

  /** true if Jacg[i] is plugged
   *  \return a bool
  virtual const bool isJacgPlugged(unsigned int) const {return false;}
   */


  /**
  * return a SP on the C matrix.
  * The matrix C in the linear case, else it returns Jacobian of the output with respect to x.
  *
  */
  virtual SP::SiconosMatrix C() = 0;
  /**
   * return a SP on the D matrix.
   * The matrix D in the linear case, else it returns Jacobian of the output with respect to lambda.
   */
  virtual SP::SiconosMatrix D() = 0;
  /**
   * return a SP on the B matrix.
   * The matrix B in the linear case, else it returns Jacobian of the input with respect to lambda.
   */
  virtual SP::SiconosMatrix B() = 0;



  /** get matrix Jach[index]
   *  \return a SimpleMatrix
  virtual const SimpleMatrix getJach(unsigned int  index = 0) const = 0;
   */

  /** get a pointer on matrix Jach[index]
   *  \return a pointer on a SiconosMatrix
  virtual SP::SiconosMatrix jachX() const = 0;
  virtual SP::SiconosMatrix jachlambda() const = 0;
   */

  /** get matrix Jacg[index]
   *  \return a SimpleMatrix
  virtual const SimpleMatrix getJacg(unsigned int  index = 0) const
  {
    RuntimeException::selfThrow("Relation::getJacg() - not implemented for this type of relation (probably Lagrangian): "+getType());
    return SimpleMatrix(0,0);
  }
   */

  /** get a pointer on matrix Jacg[index]
   *  \return a pointer on a SiconosMatrix
   */
  virtual SP::SiconosMatrix jacglambda() const
  {
    RuntimeException::selfThrow("Relation::jacg() - not implemented for this type of relation (probably Lagrangian): " + getType());
    return SP::SiconosMatrix();
  }


  /** To set a plug-in function to compute output function h
   *  \param string : the complete path to the plugin
   *  \param string : the function name to use in this plugin
   */
  virtual void setComputehFunction(const std::string& pluginPath, const std::string& functionName);

  /** To set a plug-in function to compute  \f$ \nabla_x h(..)\f$
   *  \param string : the complete path to the plugin
   *  \param string : the function name to use in this plugin
   */
  virtual void setComputeJachxFunction(const std::string& pluginPath, const std::string& functionName);
  /** To set a plug-in function to compute  \f$ \nabla_{\lambda} h(..)\f$
   *  \param string : the complete path to the plugin
   *  \param string : the function name to use in this plugin
   */
  virtual void setComputeJachlambdaFunction(const std::string& pluginPath, const std::string& functionName);

  /** To set a plug-in function to compute input function g
   *  \param string : the complete path to the plugin
   *  \param string : the function name to use in this plugin
   */
  virtual void setComputegFunction(const std::string& pluginPath, const std::string& functionName);
  /** To set a plug-in function to compute input function F
   *  \param string : the complete path to the plugin
   *  \param string : the function name to use in this plugin
   */
  virtual void setComputeFFunction(const std::string& pluginPath, const std::string& functionName);
  virtual void setComputeEFunction(const std::string& pluginPath, const std::string& functionName);

  /** To set a plug-in function to compute the jacobian according to x of the input
   *  \param string : the complete path to the plugin
   *  \param string : the function name to use in this plugin
   *  \param index for jacobian (0: jacobian according to x, 1 according to lambda)
   */
  virtual void setComputeJacglambdaFunction(const std::string& pluginPath, const std::string& functionName);

  /** initialize the relation (check sizes, memory allocation ...)
      \param SP to Interaction: the interaction that owns this relation
  */
  virtual void initialize(SP::Interaction) = 0;
  virtual void initializeMemory() ;

  /** default function to compute h
   *  \param double : current time
   */
  virtual void computeh(double) = 0;

  /** default function to compute g
   *  \param double : current time
   */
  virtual void computeg(double t);

  /** default function to compute jacobianH
   *  \param double : current time
   *  \param index for jacobian (0: jacobian according to x, 1 according to lambda)

  virtual void computeJachx(double) = 0;
  virtual void computeJachlambda(double) = 0;
  */
  /** default function to compute jacobianG according to lambda
   *  \param double : current time
   *  \param index for jacobian: at the time only one possible
   *  jacobian => i = 0 is the default value .
   */
  virtual void computeJacglambda(double)
  {
    ;//RuntimeException::selfThrow("Relation::computeJacg() - not implemented for this type of relation (probably Lagrangian): "+getType());
  }

  /* compute all the H Jacobian */
  virtual void computeJach(double) = 0;
  /* compute all the G Jacobian */
  virtual void computeJacg(double) = 0;


  /** default function to compute y
   *  \param double : current time
   *  \param unsigned int: number of the derivative to compute,
   *  optional, default = 0.
   */
  virtual void computeOutput(double, unsigned int = 0) = 0;

  /** default function to compute r
   *  \param double : current time
   *  \param unsigned int: "derivative" order of lambda used to
   *  compute input
   */
  virtual void computeInput(double, unsigned int = 0) = 0;
  virtual inline SP::SiconosMatrix jachlambda() const
  {
    return Jachlambda;
  }



  // --- Residu y functions

  inline const SP::SiconosVector getResiduY()
  {
    return _Residuy;
  }
  /*
   * Compute the residuY.
   *
   *
   */
  virtual void computeResiduY(double t);

  /*
   * Return H_alpha
   *
   */
  virtual const SP::SiconosVector Halpha()
  {
    return _h_alpha;
  };

  /*
   * Do the computation needed by for a Iteration.
   *
   */
  virtual void preparNewtonIteration()
  {
    ;
  };

  /**
   * return true if the relation is linear.
   */

  virtual bool isLinear()
  {
    return true;
  }

  /** main relation members display
   */
  virtual void display() const;

  /** copy the data of the Relation to the XML tree
   */
  virtual void saveRelationToXML() const;


  /** visitors hook
   */
  VIRTUAL_ACCEPT_VISITORS(Relation)
};

#endif // RELATION_H
