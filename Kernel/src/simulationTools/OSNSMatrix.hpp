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
/*! \file OSNSMatrix.hpp
  Specific storage for matrices used in OneStepNSProblem
*/

#ifndef OSNSM_H
#define OSNSM_H

#include "SiconosFwd.hpp"
#include "SimulationTypeDef.hpp"

enum SICONOS_STORAGE_TYPE
{
  SICONOS_DENSE = 0,
  SICONOS_SPARSE = 1
};

/** Interface to some specific storage types for matrices used in
 * OneStepNSProblem
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *  \date (Creation) 05/02/2010
 *
 * This class is used to define an interface for various storage used
 * for matrices in OneStepNSProblem. Its aim is to fill the
 * Numerics structure NumericsMatrix, required in many XXX_problem
 * structures of Numerics as input argument for drivers. \n
 *
 * The idea is to remove all matrix storage management problems from
 * OSNS classes (LCP ...) and to leave it into this class. \n
 *
 * Two main functions:
 * - fill(indexSet, interactionBlocks): fill the matrix using a list of
 *   "active" Interaction, in indexSet, and a
 *   MapOfMapOfInteractionMatrices, interactionBlocks, which determines
 *   which Interaction are connected or not (ie have common DynamicalSystem).
 *   - convert(): fill the NumericsMatrix structure (indeed only
 *   pointers links to the components of the present class)
 *
 * Note that OSNSMatrix are square.
 *
 *  For example, if in a LCP, constraints of interest are
 *  indexSet={inter2,inter3,inter8,inter12}, whith common DynamicalSystem between
 *  2 and 3, 2 and 8 and 8 and 12.

 *  interactionBlocks contains matrices for all (interi,interj) which have
 *  common DS, for (interi,interj) in I0, the set of all Interaction.

 *  (for details on how interactionBlocks is computed see OneStepNSProblem.h).
 *
 * We denote interactionBlocks[interi][interj] = mij \n Then, a call to
 * fill(indexSet, interactionBlock) results in a matrix which looks like:
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
 *
 *  - full matrix in a SiconosMatrix (storageType = 0). In this case,
 *  for each call to fill(), the SiconosMatrix M is resized
 *  according  to the sizes of the Interaction present in indexSet and then
 *  all the required interactionBlocks mij are COPIED into M.
 *
 *  - Sparse Block Storage (storageType = 1): corresponds to
 *  SparseBlockStructuredMatrix structure of Numerics. Only non-null
 *  interactionBlocks are saved in the matrix M and there is no copy of
 *  sub-interactionBlocks, only links thanks to pointers.
 *
 */
class OSNSMatrix
{
protected:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(OSNSMatrix);


  /** number of rows */
  unsigned int dimRow;

  /** number of columns */
  unsigned int dimColumn;

  /** Storage type used for the present matrix */
  int storageType;

  /** Numerics structure to be filled  */
  SP::NumericsMatrix numericsMat;

  /** Matrix used for default storage type (storageType = 0) */
  SP::SiconosMatrix M1;

  /** Matrix which corresponds to Numerics SparseBlockStructuredMatrix
      (storageType = 1) */
  SP::BlockCSRMatrix M2;

  /** For each Interaction in the graph, compute its absolute position
   * \param[out] dim the dimension of the problem (or size of the matrix), computed as the sum of the nslaw of all the Interaction in indexSet
     \param indexSet the index set of the active constraints
  */
  virtual void updateSizeAndPositions(unsigned int& dim, SP::InteractionsGraph indexSet);

private:
  /** Private copy constructor => no copy nor pass by value */
  OSNSMatrix(const OSNSMatrix&);

  /** Private assignment -> forbidden */
  OSNSMatrix& operator=(const OSNSMatrix&);

public:

  /** Default constructor -> empty matrix
   */
  OSNSMatrix();

  /** Constructor with dimRow. of the matrix
      \param n size of the square matrix
      \param stor storage type (0:dense, 1:sparse interactionBlock)
  */
  OSNSMatrix(unsigned int, int);

  /** Constructor with dimRow and DimColumn of the matrix
      \param n and m sizes of the rectangle matrix
      \param stor storage type (0:dense, 1:sparse interactionBlock)
  */
  OSNSMatrix(unsigned int, unsigned int, int);

  /** Constructor from index set and map
      \param InteractionsGraph* the index set of the active constraints
      \param storage type
  */
  OSNSMatrix(SP::InteractionsGraph, int);

  /** Constructor with copy of a SiconosMatrix => storageType = 0
      \param MSource matrix to be copied
  */
  OSNSMatrix(const SiconosMatrix&);

  /** destructor
   */
  virtual ~OSNSMatrix();

  /** get dimension of the square matrix */
  inline unsigned int size() const
  {
    return dimRow;
  };

  /** get dimension of the square matrix */
  inline unsigned int sizeColumn() const
  {
    return dimColumn;
  };

  /** get the type of storage for current matrix  */
  inline int getStorageType() const
  {
    return storageType;
  };

  /** set which type of storage will be used for current matrix
   * \param i the type of storage
   */
  inline void setStorageType(int i)
  {
    storageType = i;
  };

  /** get the absolute position of the interaction 
      \param inter the Interaction from which position is required
  */
  virtual unsigned int getPositionOfInteractionBlock(Interaction& inter) const;

  /** get the numerics-readable structure */
  inline SP::NumericsMatrix getNumericsMatrix()
  {
    return numericsMat;
  };

  /** get the matrix used for default storage */
  inline SP::SiconosMatrix defaultMatrix()
  {
    return M1;
  };

  /** fill the current class using an index set and a map of interactionBlocks
      \param indexSet the index set of the active constraints
      \param update if true update the size of the Matrix (default true)
  */
  virtual void fill(SP::InteractionsGraph indexSet, bool update = true);

  /** fill the numerics structure numericsMatSparse using MBlockCSR */
  void convert();

  /** display the current matrix
   */
  void display() const;
};

#endif
