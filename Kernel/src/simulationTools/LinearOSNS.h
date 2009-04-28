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
/*! \file LinearOSNS.h
  \brief Linear Complementarity Problem formulation and solving
*/

#ifndef LinearOSNS_H
#define LinearOSNS_H

#include "OneStepNSProblem.h"
#include "SimpleVector.h"
#include "SimpleMatrix.h"
#include "BlockCSRMatrix.h"
#include <sys/time.h>

#include "SiconosPointers.hpp"

class OneStepNSProblem;

/** stl vector of double */
typedef std::vector<double> MuStorage;
TYPEDEF_SPTR(MuStorage);

/** Base (abstract) class for linear non-smooth problems

    \author SICONOS Development Team - copyright INRIA
    \version 3.0.0.
    \date (Creation) November 13, 2008

    Base class for linear non-smooth problems, usually in the form:


    \f[
    w =  q + M z
    \f]
    where
    - \f$ w \in R^{n} \f$  and \f$z \in R^{n} \f$ are the unknowns,
    - \f$ M \in R^{n \times n } \f$  and \f$q \in R^{n} \f$

    examples: LCP, FrictionContact ...

*/
class LinearOSNS : public OneStepNSProblem
{
protected:

  /** contains the vector w of a LinearOSNS system */
  SP::SiconosVector _w;

  /** contains the vector z of a LinearOSNS system */
  SP::SiconosVector _z;

  /** contains the matrix M of a LinearOSNS system */
  SP::OSNSMatrix M;

  /** contains the vector q of a LinearOSNS system */
  SP::SiconosVector q;

  /** Storage type for M - 0: SiconosMatrix (dense), 1: Sparse Storage
      (embedded into OSNSMatrix) */
  int MStorageType;

  /** a boolean to decide if _w and _z vectors are initialized with
      previous values of Y and Lambda when a change occurs in problem
      size */
  bool keepLambdaAndYState;

  /** default constructor (private)
   */
  LinearOSNS() {};

public:

  /** xml constructor
      \param SP::OneStepNSProblemXML : the XML linked-object
      \param string, ns problem type            \
  */
  LinearOSNS(SP::OneStepNSProblemXML, const std::string&);

  /** constructor from data
      \param string, ns problem type    \
      \param Solver* pointer to object that contains solver algorithm
      and formulation \n (optional, default = NULL => read .opt file
      in Numerics)
      \param String: id of the problem (default = "unamed")
  */
  LinearOSNS(const std::string&,
             SP::NonSmoothSolver = SP::NonSmoothSolver(),
             const std::string& = "unamed_lcp");

  /** destructor
   */
  ~LinearOSNS() {};

  // --- W ---
  /** get the value of w, the initial state of the DynamicalSystem
   *  \return SimpleVector
   *  \warning: SiconosVector is an abstract class => can not be an
   *  lvalue => return SimpleVector
   */
  inline const SimpleVector getW() const
  {
    return *_w;
  }

  /** get w, the initial state of the DynamicalSystem
   *  \return pointer on a SimpleVector
   */
  inline SP::SiconosVector getWPtr() const
  {
    return _w;
  }

  /** set the value of w to newValue
   *  \param SiconosVector newValue
   */
  void setW(const SiconosVector&);

  /** set w to pointer newPtr
   *  \param SP::SiconosVector  newPtr
   */
  inline void setWPtr(SP::SiconosVector newPtr)
  {
    _w = newPtr;
  }

  // --- Z ---
  /** get the value of z, the initial state of the DynamicalSystem
   *  \return SimpleVector
   *  \warning: SimpleVector is an abstract class => can not be an
   *  lvalue => return SimpleVector
   */
  inline const SimpleVector getZ() const
  {
    return *_z;
  }

  /** get z, the initial state of the DynamicalSystem
   *  \return pointer on a SiconosVector
   */
  inline SP::SiconosVector getZPtr() const
  {
    return _z;
  }

  /** set the value of z to newValue
   *  \param SiconosVector newValue
   */
  void setZ(const SiconosVector&);

  /** set z to pointer newPtr
   *  \param SP::SiconosVector  newPtr
   */
  inline void setZPtr(SP::SiconosVector newPtr)
  {
    _z = newPtr;
  }

  // --- M ---

  /** get M
   *  \return pointer on a OSNSMatrix
   */
  inline SP::OSNSMatrix getMPtr() const
  {
    return M;
  }

  /** set the value of M to newValue
   *  \param newValue OSNSMatrix
   */
  void setM(const OSNSMatrix&);

  /** set M to pointer newPtr
   *  \param newPtr OSNSMatrix*
   */
  inline void setMPtr(SP::OSNSMatrix newPtr)
  {
    M = newPtr;
  }

  // --- Q ---
  /** get the value of q, the initial state of the DynamicalSystem
   *  \return SimpleVector
   *  \warning: SimpleVector is an abstract class => can not be an
   *  lvalue => return SimpleVector
   */
  inline const SimpleVector getQ() const
  {
    return *q;
  }

  /** get q, the initial state of the DynamicalSystem
   *  \return pointer on a SiconosVector
   */
  inline SP::SiconosVector getQPtr() const
  {
    return q;
  }

  /** set the value of q to newValue
   *  \param SiconosVector newValue
   */
  void setQ(const SiconosVector&);

  /** set q to pointer newPtr
   *  \param SP::SiconosVector  newPtr
   */
  inline void setQPtr(SP::SiconosVector newPtr)
  {
    q = newPtr;
  }

  /** get the type of storage for M */
  inline const int getMStorageType() const
  {
    return MStorageType;
  };

  /** set which type of storage will be used for M - Note that this function does not
      allocate any memory for M, it just sets an indicator for future use */
  inline void setMStorageType(int i)
  {
    MStorageType = i;
  };

  /** Memory allocation or resizing for z,w,q */
  void initVectorsMemory();

  /** Build or reinit M */
  virtual void updateM();

  /** To initialize the LinearOSNS problem(computes topology ...)
      \param the simulation, owner of this OSNSPB
  */
  void initialize(SP::Simulation);

  /** computes extra diagonal unitaryBlock-matrix that corresponds to UR1 and UR2
   *  Move this to Unitary Relation class?
   *  \param a pointer to UnitaryRelation
   *  \param a pointer to UnitaryRelation
   */
  virtual void computeUnitaryBlock(SP::UnitaryRelation, SP::UnitaryRelation);

  /** To compute a part of the "q" vector of the OSNS
      \param SP::UnitaryRelation, the UR which corresponds to the considered block
      \param unsigned int, the position of the first element of yOut to be set
  */
  void computeQBlock(SP::UnitaryRelation, unsigned int);

  /** compute vector q
   *  \param double : current time
   */
  void computeQ(double);

  /** pre-treatment for LinearOSNS
   *  \param double : current time
   *  \return void
   */
  void preCompute(double);

  /** Compute the unknown z and w and update the Interaction (y and lambda )
   *  \param double : current time
   *  \return int, information about the solver convergence.
   */
  virtual int compute(double) = 0;

  /** post-treatment for LinearOSNS
   */
  void postCompute() ;

  /** print the data to the screen
   */
  void display() const;

  /** copy the data of the OneStepNSProblem to the XML tree
   *  \exception RuntimeException
   */
  void saveNSProblemToXML();

  /** set if if _w and _z vectors are initialized with
      previous values of Y and Lambda when a change occurs in problem
      size */
  void setKeepLambdaAndYState(bool val)
  {
    keepLambdaAndYState = val ;
  }

};

TYPEDEF_SPTR(LinearOSNS);

#endif // LinearOSNS_H
