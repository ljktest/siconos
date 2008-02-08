/* Siconos-Kernel version 2.1.1, Copyright INRIA 2005-2007.
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
/*! \file OSNSMatrix.h
Specific storage for matrices used in OneStepNSProblem
*/

#ifndef OSNSM_H
#define OSNSM_H

#include "SimpleMatrix.h"
#include "SiconosNumerics.h"
#include "SimulationTypeDef.h"

class SparseBlockMatrix;

/** Interface to some specific storage types for matrices used in OneStepNSProblem
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 2.1.1.
 *  \date (Creation) 05/02/2008
 *
 * This class is used to define an interface for various storage used for matrices in OneStepNSProblem. \n
 * Its aim is to fill the Numerics structure NumericsMatrix, required in many XXX_problem structures of Numerics \n
 * as input argument for drivers. \n
 *
 * The idea is to remove all matrix storage management problems from OSNS classes (LCP ...) and to leave it into this class. \n
 *
 * Two main functions:
 * - fill(indexSet, blocks): fill the matrix using a list of "active" UnitaryRelation, in indexSet, and a MapOfMapOfUnitaryMatrices, blocks, \n
 *   which determines which UR are connected or not (ie have common DynamicalSystem).
 * - convert(): fill the NumericsMatrix structure (indeed only pointers links to the components of the present class)
 *
 * Note that OSNSMatrix are square.
 *
 *  For example, if in a LCP, constraints of interest are indexSet={UR2,UR3,UR8,UR12}, whith common DynamicalSystem between 2 and 3, 2 and 8 and 8 and 12. \n
 *  blocks contains matrices for all (URi,URj) which have common DS, for (URi,URj) in I0, the set of all UnitaryRelation.\n
 *  (for details on how blocks is computed see OneStepNSProblem.h). \n
 * We denote blocks[URi][URj] = mij \n
 * Then, a call to fill(indexSet, block) results in a matrix which looks like:
 *
 * \f{eqnarray*}
   M=\left\lbrace\begin{array}{cccc}
    m22 & m23 & m28 &  0 \\
    m32 & m33 & 0   &  0 \\
     0  &  0  & m88 & m812 \\
     0  &  0  & m128& m1212
    \end{array}\right.
    \f}
 *
 *
 * Note: at the time the available storage types are:
 *  - full matrix in a SiconosMatrix (storageType = 0). In this case, for each call to fill(), the SiconosMatrix M is resized according\n
 *  to the sizes of the UR present in indexSet and then all the required blocks mij are COPIED into M.
 *  - Sparse Block Storage (storageType = 1): corresponds to SparseBlockStructuredMatrix structure of Numerics. Only non-null blocks are saved in the matrix M \n
 *  and there is no copy of sub-blocks, only links thanks to pointers.
 *
 */
class OSNSMatrix
{
protected:

  /** number of rows/columns */
  unsigned int dim;

  /** Storage type used for the present matrix */
  int storageType;

  /** map that links each UnitaryRelation with an int that gives the position (in number of scalar elements, not blocks) \n
   * of the corresponding block matrix in the full matrix (M in LCP case) - Warning: it depends on the considered index set \n
   * (ie on which constraints are "active")
   */
  UR_int* blocksPositions;

  /** Numerics structure to be filled  */
  NumericsMatrix* numericsMat;

  /** Matrix used for default storage type (storageType = 0) */
  SiconosMatrix * M1;

  /** Matrix which corresponds to Numerics SparseBlockStructuredMatrix (storageType = 1) */
  SparseBlockMatrix * M2;

  /** Private copy constructor => no copy nor pass by value */
  OSNSMatrix(const OSNSMatrix&);

  /** Private assignment -> forbidden */
  OSNSMatrix& operator=(const OSNSMatrix&);

  /** To update dim and blocksPositions for a new set of UnitaryRelation
      \param UnitaryRelationsSet* the index set of the active constraints
  */
  void updateSizeAndPositions(UnitaryRelationsSet*);

public:

  /** Default constructor -> empty matrix
   */
  OSNSMatrix();

  /** Constructor with dim. of the matrix
      \param n size of the square matrix
      \param stor storage type (0:dense, 1:sparse block)
  */
  OSNSMatrix(unsigned int, int);

  /** Constructor from index set and map
      \param UnitaryRelationsSet* the index set of the active constraints
      \param MapOfMapOfUnitaryMatrices the list of matrices linked to a couple of UR*
      \param storage type
  */
  OSNSMatrix(UnitaryRelationsSet*, MapOfMapOfUnitaryMatrices&, int);

  /** Constructor with copy of a SiconosMatrix => storageType = 0
      \param MSource matrix to be copied
  */
  OSNSMatrix(const SiconosMatrix&);

  /** destructor
   */
  ~OSNSMatrix();

  /** get dimension of the square matrix */
  inline const unsigned int size() const
  {
    return dim;
  };

  /** get the type of storage for current matrix  */
  inline const int getStorageType() const
  {
    return storageType;
  };

  /** set which type of storage will be used for current matrix */
  inline void setStorageType(int i)
  {
    storageType = i;
  };

  /** get the position (real, not block) of the first element of the block which corresponds to UR
      \param UR UnitaryRelation from which position is required
  */
  unsigned int getPositionOfBlock(UnitaryRelation*) const;

  /** get the numerics-readable structure */
  inline NumericsMatrix* getNumericsMatrix()
  {
    return numericsMat;
  };

  /** get the matrix used for default storage */
  inline SiconosMatrix* getDefaultMatrixPtr()
  {
    return M1;
  };

  /** fill the current class using an index set and a map of blocks
      \param UnitaryRelationsSet*, the index set of the active constraints
      \param MapOfMapOfUnitaryMatrices, the list of matrices linked to a couple of UR*
  */
  void fill(UnitaryRelationsSet*, MapOfMapOfUnitaryMatrices&);

  /** fill the numerics structure numericsMatSparse using MSparseBlock */
  void convert();

  /** display the current matrix
   */
  void display() const;
};

#endif
