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

/*! \file Interaction.hpp
  \brief Interaction class and related typedef
*/


#ifndef INTERACTION_H
#define INTERACTION_H

#include "DynamicalSystemsSet.hpp"
#include "RelationNamespace.hpp"

#include "SiconosPointers.hpp"
#include "SiconosFwd.hpp"
#include <vector>
typedef std::vector<SP::SiconosMemory> VectorOfMemories;

/**  An Interaction describes the non-smooth interactions
 *  several Dynamical Systems.
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) Apr 29, 2004
 *
 * An interaction represents the "link" between a set of Dynamical
 * Systems.
 * The state variables and inputs of the DynamicalSystem (x,R)
 * are related to the interaction variables (y,lambda) thanks to the
 * interaction with the help of the relation
 * The interaction completed by a non-smooth law that describes the type
 * of law between y and lambda.
 *
 * Thus, the interaction main members are:
 *
 * - relation: a pointer to a Relation object that determines the type
 *   of relation and so the way it is computed. Warning: there is only
 *   one Relation object (ie only one type of relation for an
 *   interaction) but there can be several "relations", in the sense
 *   of constraints equations between (y,lambda) and (x,r).
 *
 * - nslaw: the nonsmooth law
 *
 * - the local variable y  (its size is interactionSize).
 *   STL vectors are used and y[i] usually represents the
 *   i-eme derivative of variable y.
 *
 * - the local variable lambda  (its size is interactionSize).
 *   STL vectors are used and lambda[i] represents various level
 *   of multiplier involved in the nonsmooth law with y[i]
 *
 *   y (resp, lambda) is a container of SiconosVector.
 *
 */
class Interaction : public std11::enable_shared_from_this<Interaction >
{
private:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(Interaction);

  /**initialization flag */
  bool _initialized;

  /** number specific to each Interaction */
  unsigned int _number;

  /** relative degree of this interaction */
  unsigned int _relativeDegree;

  /** Minimum required 'level' for output y
   *  y will be initialized from
   *  y[_lowerLevelForOutput] to y[_upperLevelForOutput]
   */
  unsigned int _lowerLevelForOutput;

  /** Maximum required 'level' for output y
    *  y will be initialized from
    *  y[_lowerLevelForOutput] to y[_upperLevelForOutput]
    */
  unsigned int _upperLevelForOutput;

  /** Minimum required 'level' for input lambda
   *  lambda will be initialized from
   *  lambda[_lowerLevelForIntput] to lambda[_upperLevelForInput]
   */
  unsigned int _lowerLevelForInput;

  /** Maximum required 'level' for input lambda
   *  lambda will be initialized from
   *  lambda[_lowerLevelForIntput] to lambda[_upperLevelForInput]
   */
  unsigned int _upperLevelForInput;

  /** size of the interaction, ie size of y[i] and _lambda[i] */
  unsigned int _interactionSize;

  /** sum of all DS sizes, for DS involved in the interaction */
  unsigned int _sizeOfDS;

  /** Bool to check the number of DS concerned by this interaction
      (1 or 2 indeed)
      True if 2 DS.
      Note FP : usefull in NewtonEuler jacobians computation.
  */
  bool _has2Bodies;

  /** Absolute position in the "global" vector of constraints (for
      example, the one handled by lsodar) */
  unsigned int _absolutePosition;

  /** Absolute position in the "global" vector of constraints for the proj formulation. */
  unsigned int _absolutePositionProj;

  /** relation between constrained variables and states variables
   * vector of output derivatives
   * y[0] is y, y[1] is yDot and so on
   */
  VectorOfVectors _y;

  /** previous value of Newton iteration for y
   * \warning : VA 24/05/2013 this has to be put into the workspace vector
   *   or we have to use the _yMemory storage
   */
  VectorOfVectors _yOld;

  /** value of the previous time-step */
  VectorOfVectors _y_k;

  /** memory of previous coordinates of the system */
  VectorOfMemories _yMemory;

  /** memory of previous coordinates of the system */
  VectorOfMemories _lambdaMemory;

  /** Size (depth) of the Memory*/
  unsigned int _steps;

  /** result of the computeInput function */
  VectorOfVectors _lambda;

  /** previous step values for _lambda
   * \warning : VA 24/05/2013 this has to be put into the workspace vector
   *   or we have to use the _yMemory storage
   */
  VectorOfVectors _lambdaOld;

  /** the Non-smooth Law of the interaction*/
  SP::NonSmoothLaw _nslaw;

  /** the type of Relation of the interaction */
  SP::Relation _relation;

  /** The residu y of the newton iterations*/
  SP::SiconosVector _residuY;

  /*value of h at the current newton iteration*/
  SP::SiconosVector _h_alpha;

  /* work vector to compute qblock, XXX maybe it shouldn't exist */
  SP::SiconosVector _yForNSsolver;

  // === PRIVATE FUNCTIONS ===

  /** copy constructor => private, no copy nor pass-by-value.
   */
  Interaction(const Interaction& inter);

public:

  /** default constructor */
  Interaction():_initialized(false), _number(0), _interactionSize(0), _sizeOfDS(0), _has2Bodies(false), _y(2)
  {};

   /** constructor with no data
   *  \param interactionSize size of the interaction, i.e. the size of the input and output
   *  \param NSL pointer to the NonSmoothLaw
   *  \param rel a pointer to the Relation
   *  \param number the number of this Interaction (default 0)
   */
  Interaction(unsigned int interactionSize, SP::NonSmoothLaw NSL, SP::Relation rel, unsigned int number = 0);

  /** destructor
   */
  ~Interaction() {};

  /** initialize this relation
      \param time time for initialization.
      \param DSlink set of BlockVector to link to the data in DynamicalSystem
      \param workVInter set of work vectors for this Interaction
      \param workMInter set of work matrices for this Interaction
      \param osnsMInter matrix used in the OSNS for this Interaction
      \param ds1 first ds linked to this Interaction (i.e IG->vertex.source)
      \param workV1 work vectors of ds1
      \param ds2 second ds linked to this Interaction (i.e IG->vertex.target) ds1 == ds2 is allowed.
      \param workV2 work vectors of ds2
      \param computeResiduY if true allocate vectors for the computation of the residu on y
      \param computeResiduR if true allocate vectors for the computation of the residu on r
   */
  void initialize(double time, VectorOfBlockVectors& DSlink, VectorOfVectors& workVInter, VectorOfSMatrices& workMInter, SiconosMatrix& osnsMInter, DynamicalSystem& ds1, VectorOfVectors& workV1, DynamicalSystem& ds2, VectorOfVectors& workV2, bool computeResiduY = false, bool computeResiduR = false);

  /** check if Interaction is initialized
   * \return true if it is initialized
   */
  bool isInitialized() const
  {
    return _initialized;
  }

  /** set all lambda to zero
   */
  void resetAllLambda() ;

  /** set lambda to zero for a given level
   * \param level
   */
  void resetLambda(unsigned int level);

  /** build y and \f$\lambda\f$ vectors
   * \param computeResiduY if true the residu on y is computed and memory allocation is done for _residuY and _h_alpha
  */
  void initializeMemory(bool computeResiduY);

  // === GETTERS/SETTERS ===
  /** get the value of number
   *  \return the value of number
   */
  inline int number() const
  {
    return _number;
  }

  /** set number
  *  \param int number : the value to set number
  */
  inline void setNumber(const int newNumber)
  {
    _number = newNumber;
  }


  /** get the relative degree
   * \return an unsigned int
   */
  inline unsigned int getRelativeDegree() const
  {
    return _relativeDegree;
  };

  /** set the relative degree
   * \param an unsigned int
   */
  inline void setRelativeDegree(const unsigned int newVal)
  {
    _relativeDegree = newVal;
  };


  /** set the lower level for output y
   * \param an unsigned int
   */
  inline void setLowerLevelForOutput(const unsigned int newVal)
  {
    _lowerLevelForOutput = newVal;
  };

  /** set the upper level for output y
   * \param an unsigned int
   */
  inline void setUpperLevelForOutput(const unsigned int newVal)
  {
    _upperLevelForOutput = newVal;
  };

  /** get the lower level for output y
   */
  inline unsigned int lowerLevelForOutput()
  {
    return _lowerLevelForOutput;
  };

  /** get the upper level for output y
   */
  inline unsigned int  upperLevelForOutput()
  {
    return _upperLevelForOutput;
  };

  /** set the lower level for input Lambda
   * \param an unsigned int
   */
  inline void setLowerLevelForInput(const unsigned int newVal)
  {
    _lowerLevelForInput = newVal;
  };

  /** set the upper level for input Lambda
   * \param an unsigned int
   */
  inline void setUpperLevelForInput(const unsigned int newVal)
  {
    _upperLevelForInput = newVal;
  };


  /** get the lower level for input Lambda
   */
  inline unsigned int lowerLevelForInput()
  {
    return _lowerLevelForInput ;
  };

  /** get the upper level for input Lambda
   */
  inline unsigned int upperLevelForInput()
  {
    return _upperLevelForInput;
  };


  /** get the dimension of the interaction (y and _lambda size)
  *  \return an unsigned int
  */
  inline unsigned int getSizeOfY() const
  {
    return _interactionSize;
  }

  /** set the dimension of the Interaction
  *  \param an unsigned int
  */
  inline void setInteractionSize(const unsigned int newVal)
  {
    _interactionSize = newVal;
  }

  //  /** get the number of relations in the interaction
  //  *  \return an unsigned int
  //  */
  // inline unsigned int numberOfRelations() const {return _numberOfRelations;}

  /** get the sum of DS sizes, for DS involved in interaction
   *  \return an unsigned int
   */
  inline unsigned int getSizeOfDS() const
  {
    return _sizeOfDS;
  }

  /** Set the number of dynamical systems concerned by
      this interaction. Warning FP: this function is supposed
      to be called only during topology->link(inter, ds1, ds2) call.
      \param bool : true if two ds, else false
   */
  void setHas2Bodies(bool val) {_has2Bodies = val;}

  /** Check the number of dynamical systems concerned by
      this interaction
      \return bool : true if two ds, else false
   */
  bool has2Bodies() const {return _has2Bodies;}

  unsigned int absolutePosition()
  {
    return _absolutePosition;
  };

  void setAbsolutePosition(unsigned int v)
  {
    _absolutePosition = v;
  };

  unsigned int absolutePositionProj()
  {
    return _absolutePositionProj;
  };

  void setAbsolutePositionProj(unsigned int v)
  {
    _absolutePositionProj = v;
  };

  // -- y --

  /** get y[i], derivative number i of output
   *  \return BlockVector
   */
  inline const SiconosVector getCopyOfy(const unsigned int i) const
  {
    return *(_y[i]);
  }

  /** get y[i], derivative number i of output
   *  \return BlockVector
   */
  inline const SiconosVector getCopyOfyOld(const unsigned int i) const
  {
    return *(_yOld[i]);
  }


  /** get vector of output derivatives
  *  \return a VectorOfVectors
  */
  inline const VectorOfVectors y() const
  {
    return _y;
  }


  /** get y[i], derivative number i of output
  *  \return pointer on a SiconosVector
  */
  inline SP::SiconosVector y(const unsigned int i) const
  {
    return _y[i];
  }

  /** set the output vector y to newVector with copy of the y[i] (ie
      memory allocation)
  *  \param VectorOfVectors
  */
  void setY(const VectorOfVectors&);

  /** set the output vector y to newVector with direct pointer
  *  equality for the y[i]
  * \param VectorOfVectors
  */
  void setYPtr(const VectorOfVectors&);

  /** set y[i] to newValue
  *  \param a SiconosVector and an unsigned int
  */
  void setY(const unsigned int , const SiconosVector&);

  /** set y[i] to pointer newPtr
  *  \param a SP::SiconosVector  and an unsigned int
  */
  void setYPtr(const unsigned int , SP::SiconosVector newPtr);

  // -- yOld --

  /** get vector of output derivatives
  *  \return a VectorOfVectors
  */
  inline const VectorOfVectors getYOld() const
  {
    return _yOld;
  }

  /** get yOld[i], derivative number i of output
  *  \return BlockVector
  */
  inline const SiconosVector getYOld(const unsigned int i) const
  {
    return *(_yOld[i]);
  }

  /** get yOld[i], derivative number i of output
  *  \return pointer on a SiconosVector
  */
  inline SP::SiconosVector yOld(const unsigned int i) const
  {
    return _yOld[i];
  }
  /*get y_k[i]
    \return pointer on a SiconosVector
   */
  inline SP::SiconosVector y_k(const unsigned int i) const
  {
    return _y_k[i];
  }

  /** set the output vector yOld to newVector
  *  \param VectorOfVectors
  */
  void setYOld(const VectorOfVectors&);

  /** set vector yOld to newVector with direct pointer equality for
  *  the yOld[i]
  * \param VectorOfVectors
  */
  void setYOldPtr(const VectorOfVectors&);

  /** set yOld[i] to newValue
  *  \param a SiconosVector and an unsigned int
  */
  void setYOld(const unsigned int , const SiconosVector&);

  /** set yOld[i] to pointer newPtr
  *  \param a SP::SiconosVector  and an unsigned int
  */
  void setYOldPtr(const unsigned int , SP::SiconosVector newPtr);


  /** set yOld[i] to pointer newPtr
   *  \param a SP::SiconosVector  and an unsigned int
   */
  void setSteps(unsigned int newval)
  {
    _steps = newval;
  };


  /** get all the values of the state vector y stored in memory
   *  \return a memory
   */
  inline SP::SiconosMemory yMemory(unsigned int level) const
  {
    return _yMemory[level];
  }

  /** get all the values of the multiplier lambda stored in memory
   *  \return a memory
   */
  inline SP::SiconosMemory lambdaMemory(unsigned int level) const
  {
    return _lambdaMemory[level];
  }





  // -- _lambda --

  /** get vector of input derivatives
  *  \return a VectorOfVectors
  */
  inline const VectorOfVectors getLambda() const
  {
    return _lambda;
  }

  /** get _lambda[i], derivative number i of input
  *  \return SiconosVector
  */
  inline const SiconosVector getLambda(const unsigned int i) const
  {
    assert(_lambda[i]);
    return *(_lambda[i]);
  }

  /** get _lambda[i], derivative number i of input
  *  \return pointer on a SiconosVector
  */
  inline SP::SiconosVector lambda(const unsigned int i) const
  {
    assert(_lambda[i]);
    return _lambda[i];
  }

  /** set the input vector _lambda to newVector
  *  \param VectorOfVectors
  */
  void setLambda(const VectorOfVectors&);

  /** set vector _lambda to newVector with direct pointer equality for the _lambda[i]
  *  \param VectorOfVectors
  */
  void setLambdaPtr(const VectorOfVectors&);

  /** set _lambda[i] to newValue
  *  \param a SiconosVector and an unsigned int
  */
  void setLambda(const unsigned int , const SiconosVector&);

  /** set _lambda[i] to pointer newPtr
  *  \param a SP::SiconosVector  and an unsigned int
  */
  void setLambdaPtr(const unsigned int , SP::SiconosVector newPtr);

  // -- _lambdaOld --

  /** get vector of input derivatives
  *  \return a VectorOfVectors
  */
  inline const VectorOfVectors getLambdaOld() const
  {
    return _lambdaOld;
  }

  /** get _lambdaOld[i], derivative number i of input
  *  \return SiconosVector
  */
  inline const SiconosVector getLambdaOld(const unsigned int i) const
  {
    return *(_lambdaOld[i]);
  }

  /** get _lambdaOld[i], derivative number i of input
  *  \return pointer on a SiconosVector
  */
  inline SP::SiconosVector lambdaOld(const unsigned int i) const
  {
    return _lambdaOld[i];
  }

  /** set the input vector _lambdaOld to newVector
  *  \param VectorOfVectors
  */
  void setLambdaOld(const VectorOfVectors&);

  /** set vector _lambdaOld to newVector with direct pointer equality for the _lambdaOld[i]
  *  \param VectorOfVectors
  */
  void setLambdaOldPtr(const VectorOfVectors&);

  /** set _lambdaOld[i] to newValue
  *  \param a SiconosVector and an unsigned int
  */
  void setLambdaOld(const unsigned int , const SiconosVector&);

  /** set _lambdaOld[i] to pointer newPtr
  *  \param a SP::SiconosVector  and an unsigned int
  */
  void setLambdaOldPtr(const unsigned int , SP::SiconosVector newPtr);

  /** get the Relation of this Interaction
   *  \return a pointer on this Relation
   */
  inline SP::Relation relation() const
  {
    return _relation;
  }

  /** get the NonSmoothLaw of this Interaction
  *  \return a pointer on this NonSmoothLaw
  */
  inline SP::NonSmoothLaw nonSmoothLaw() const
  {
    return _nslaw;
  }
  inline SP::NonSmoothLaw nslaw() const
  {
    return _nslaw;
  }

  /** function used to sort Interaction in SiconosSet<SP::Interaction>
   *  \return a double* (warning: must be const, despite intel compilers warning, because of SiconosSet Cmp function arguments)
   */
  inline double* getSort() const
  {
    return (double*)this;
  }

  // --- OTHER FUNCTIONS ---

  /** set interaction 'ds-dimension', i.e. sum of all sizes of the dynamical systems linked
      by the current interaction. This must be done by topology during call to link(inter, ds, ...).
      \param int sum of ds sizes
      \param int sum of sizes of z components of the ds.
  */
  inline void setDSSizes(unsigned int s1, unsigned int s2)
  {
    _sizeOfDS = s1;

  }

  /**   put values of y into yOld, the same for _lambda
  */
  void swapInOldVariables();

  /** Must be call to fill _y_k. (after convergence of the Newton iterations)
   */
  void swapInMemory();

  /** print the data to the screen
  */
  void display() const;

  /** Computes output y; depends on the relation type.
   *  \param time current time
   *  \param derivativeNumber number of the derivative to compute,
   *  optional, default = 0.
   */
  void computeOutput(double time, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM, SiconosMatrix& osnsM, unsigned int derivativeNumber = 0);

  /** Compute input r of all Dynamical Systems involved in the present
   *   Interaction.
   *  \param time current time
   *  \param level order of _lambda used to compute input.
   */
  void computeInput(double time, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM, SiconosMatrix& osnsM, unsigned int level = 0);

  /** Get the _yForNssolver vector
   * \return the y part used in the solver
   */
  inline SP::SiconosVector yForNSsolver() const
  {
    return _yForNSsolver;
  }

  /** gets the matrix used in interactionBlock computation, (left * W * rigth), depends on the relation type (ex, LinearTIR, left = C, right = B).
   *         We get only the part corresponding to one ds.
   *  \param int, relative position of the beginning of the required block in relation matrix.
   *  \param a pointer to SiconosMatrix (in-out parameter): the resulting interactionBlock matrix
   */
  void getLeftInteractionBlockForDS(unsigned int pos, SP::SiconosMatrix InteractionBlock, VectorOfSMatrices& workM) const;

  /** gets the matrix used in interactionBlock computation. Used only for the formulation projecting on the constraints.
   *         We get only the part corresponding to ds.
   *  \param int, relative position of the beginning of the required block in relation matrix.
   *  \param a pointer to SiconosMatrix (in-out parameter): the resulting interactionBlock matrix
   */
  void getLeftInteractionBlockForDSProjectOnConstraints(unsigned int, SP::SiconosMatrix InteractionBlock) const;

  /** gets the matrix used in interactionBlock computation, (left * W * rigth), depends on the relation type (ex, LinearTIR, left = C, right = B).
   *         We get only the part corresponding to ds.
   *  \param int, relative position of the beginning of the required block in relation matrix.
   *  \param a pointer to SiconosMatrix (in-out parameter): the resulting interactionBlock matrix
   */
  void getRightInteractionBlockForDS(unsigned int pos, SP::SiconosMatrix InteractionBlock, VectorOfSMatrices& workM) const;

  /** gets extra interactionBlock corresponding to the present Interaction (see the
   *  top of this files for extra interactionBlock meaning)
   * \param[in,out] InteractionBlock SP::SiconosMatrix
   */
  void getExtraInteractionBlock(SP::SiconosMatrix InteractionBlock, VectorOfSMatrices& workM) const;

  void computeKhat(SiconosMatrix& m, VectorOfSMatrices& workM, double h) const;

  inline double getYRef(unsigned int i) const
  {
    // get the single value used to build indexSets Warning: the
    // relativePosition depends on NsLawSize and/or type.  This means
    // that at the time, for the interactionBlock of y that corresponds to
    // the present relation, the first scalar value is used.  For
    // example, for friction, normal part is in first position, followed
    // by the tangential parts.
    return (*_y[i])(0);
  }

  inline double getLambdaRef(unsigned int i) const
  {
    // get the single value used to build indexSets
    return (*_lambda[i])(0);
  }

  // --- Residu functions

  inline SP::SiconosVector residuY() const
  {
    return _residuY;
  }

  inline SP::SiconosVector Halpha() const
  {
    return _h_alpha;
  }

  /*
   *  Compute the residuY.
   *
   *
   */
  void computeResiduY(double time);

  /* Compute the residuR.
   * \param DSlink the container of the link to DynamicalSystem attributes
   * \param the work vectors of the DynamicalSystem
   */
  void computeResiduR(double time, VectorOfBlockVectors& DSlink, VectorOfVectors& workV) ;


  /** Instantiate the link with the DynamicalSystem
   * \param DSlink the container of the link to DynamicalSystem attributes
   */
  void initData(VectorOfBlockVectors& DSlink);

  /** Initialize the link with the DynamicalSystem
   * \param ds a DynamicalSystem concerned by this Interaction
   * \param workVDS the work vectors of the DynamicalSystem
   * \param DSlink the container of the link to DynamicalSystem attributes
   */
  void initDSData(DynamicalSystem& ds, VectorOfVectors& workVDS, VectorOfBlockVectors& DSlink);
  /** Instantiate the link with the DynamicalSystem, FirstOrderDS variant
   * \param DSlink the container of the link to DynamicalSystem attributes
   */
  void initDataFirstOrder(VectorOfBlockVectors& DSlink);

  /** Initialize the link with the DynamicalSystem, FirstOrderDS variant
   * \param ds a DynamicalSystem concerned by this Interaction
   * \param workVDS the work vectors of the DynamicalSystem
   * \param DSlink the container of the link to DynamicalSystem attributes
   */
  void initDSDataFirstOrder(DynamicalSystem& ds, VectorOfVectors& workVDS, VectorOfBlockVectors& DSlink);

  /** Instantiate the link with the DynamicalSystem, LagrangianDS variant
   * \param DSlink the container of the link to DynamicalSystem attributes
   */
  void initDataLagrangian(VectorOfBlockVectors& DSlink);

  /** Initialize the link with the DynamicalSystem, LagrangianDS variant
   * \param ds a DynamicalSystem concerned by this Interaction
   * \param workVDS the work vectors of the DynamicalSystem
   * \param DSlink the container of the link to DynamicalSystem attributes
   */
  void initDSDataLagrangian(DynamicalSystem& ds, VectorOfVectors& workVDS, VectorOfBlockVectors& DSlink);

  /** Instantiate the link with the DynamicalSystem, NewtonEulerDS variant
   * \param DSlink the container of the link to DynamicalSystem attributes
   */
  void initDataNewtonEuler(VectorOfBlockVectors& DSlink);

  /** Initialize the link with the DynamicalSystem, NewtonEulerDS variant
   * \param ds a DynamicalSystem concerned by this Interaction
   * \param workVDS the work vectors of the DynamicalSystem
   * \param DSlink the container of the link to DynamicalSystem attributes
   */
  void initDSDataNewtonEuler(DynamicalSystem& ds, VectorOfVectors& workVDS, VectorOfBlockVectors& DSlink);

};

#endif // INTERACTION_H
