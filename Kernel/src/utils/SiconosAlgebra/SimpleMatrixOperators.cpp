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

#include "KernelConfig.h"

#include "SimpleMatrix.hpp"
#include "BlockMatrixIterators.hpp"
#include "BlockMatrix.hpp"

#include "SiconosAlgebra.hpp"

using namespace Siconos;


// =================================================
//                CONSTRUCTORS
// =================================================

using  std::cout;
using std::endl;

double& SimpleMatrix::operator()(unsigned int row, unsigned int col)
{
  if (row >= dimRow || col >= dimCol)
    SiconosMatrixException::selfThrow("SimpleMatrix:operator(): Index out of range");

  if (num == 1)
    return (*mat.Dense)(row, col);
  else if (num == 2)
    return (*mat.Triang)(row, col);
  else if (num == 3)
    return (*mat.Sym)(row, col);
  else if (num == 4)
  {
    double *d = (*mat.Sparse).find_element(row, col);
    if (d == NULL)
      SiconosMatrixException::selfThrow("SimpleMatrix:operator(): Index out of range");
    double & ref = *d;
    return ref;
  }
  else if (num == 5)
    return (*mat.Banded)(row, col);
  else if (num == 6)
    return const_cast<double&>((*mat.Zero)(row, col));
  else // i(num==7)
    return const_cast<double&>((*mat.Identity)(row, col));
}

double SimpleMatrix::operator()(unsigned int row, unsigned int col) const
{
  if (row >= dimRow || col >= dimCol)
    SiconosMatrixException::selfThrow("SimpleMatrix:operator(): Index out of range");

  if (num == 1)
    return (*mat.Dense)(row, col);
  else if (num == 2)
    return (*mat.Triang)(row, col);
  else if (num == 3)
    return (*mat.Sym)(row, col);
  else if (num == 4)
    return (*mat.Sparse)(row, col);
  else if (num == 5)
    return (*mat.Banded)(row, col);
  else if (num == 6)
    return 0.0;
  else // if (num == 7)
    return (row == col);
}

//=============
// Assignment
//=============

SimpleMatrix& SimpleMatrix::operator = (const SiconosMatrix& m)
{

  if (&m == this) return *this; // auto-assignment.

  unsigned int numM = m.getNum();

  if (dimRow != m.size(0) || dimCol != m.size(1))
  {
    resize(m.size(0), m.size(1));
  }
  // SiconosMatrixException::selfThrow("SimpleMatrix::operator = failed. Inconsistent sizes.");

  if (numM == 6) // m = zero matrix
  {
    zero();
    return *this;
  }

  if (numM == 7) // m = identity matrix
  {
    eye();
    return *this;
  }

  if (numM == 0) // if m is a BlockMatrix
  {
    const BlockMatrix& mB = static_cast<const BlockMatrix&>(m);
    ConstBlocksIterator1 it;
    ConstBlocksIterator2 it2;
    unsigned int posRow = 0;
    unsigned int posCol = 0;

    for (it = mB._mat->begin1(); it != mB._mat->end1(); ++it)
    {
      for (it2 = it.begin(); it2 != it.end(); ++it2)
      {
        setBlock(posRow, posCol, **it2);
        posCol += (*it2)->size(1);
      }
      posRow += (*it)->size(0);
      posCol = 0;
    }
  }
  else
  {
    switch (num)
    {
    case 1:
      switch (numM)
      {
      case 1:
        noalias(*(mat.Dense)) = *m.dense();
        break;
      case 2:
        noalias(*(mat.Dense)) = *m.triang();
        break;
      case 3:
        noalias(*(mat.Dense)) = *m.sym();
        break;
      case 4:
        noalias(*(mat.Dense)) = *m.sparse();
        break;
      case 5:
        noalias(*(mat.Dense)) = *m.banded();
        break;
      default:
        SiconosMatrixException::selfThrow("SimpleMatrix::op= (const SimpleMatrix): invalid type of matrix");
        break;
      }
      break;
    case 2:
      switch (numM)
      {
      case 2:
        noalias(*(mat.Triang)) = *m.triang();
        break;
      default:
        SiconosMatrixException::selfThrow("SimpleMatrix::assignment of a bad type of matrix into a triangular one.");
        break;
      }
      break;
    case 3:
      if (numM == 3)
        noalias(*(mat.Sym)) = *m.sym();
      else
        SiconosMatrixException::selfThrow("SimpleMatrix::bad assignment of matrix (symetric one = dense or ...)");
      break;
    case 4:
      switch (numM)
      {
      case 2:
        noalias(*(mat.Sparse)) = *m.triang();
        break;
      case 3:
        noalias(*(mat.Sparse)) = *m.sym();
        break;
      case 4:
        noalias(*(mat.Sparse)) = *m.sparse();
        break;
      case 5:
        noalias(*(mat.Sparse)) = *m.banded();
        break;
      default:
        SiconosMatrixException::selfThrow("SimpleMatrix::op= (const SimpleMatrix): invalid type of matrix");
        break;
      }
      break;
    case 5:
      switch (numM)
      {
      case 5:
        noalias(*(mat.Banded)) = *m.banded();
        break;
      default:
        SiconosMatrixException::selfThrow("SimpleMatrix::op= (const SimpleMatrix): invalid type of matrix");
        break;
      }
      break;
    default:
      SiconosMatrixException::selfThrow("SimpleMatrix::op= (const SimpleMatrix): invalid type of matrix");
      break;
    }
    resetLU();
  }
  return *this;
}

SimpleMatrix& SimpleMatrix::operator = (const SimpleMatrix& m)
{

  if (&m == this) return *this; // auto-assignment.

  unsigned int numM = m.getNum();

  if (dimRow != m.size(0) || dimCol != m.size(1))
    resize(m.size(0), m.size(1));

  //    SiconosMatrixException::selfThrow("SimpleMatrix::operator = failed. Inconsistent sizes.");

  if (numM == 6) // m = zero matrix
  {
    zero();
    return *this;
  }
  else if (numM == 7) // m = identity matrix
  {
    eye();
    return *this;
  }

  switch (num)
  {
  case 1:
    switch (numM)
    {
    case 1:
      noalias(*(mat.Dense)) = *m.dense();
      break;
    case 2:
      noalias(*(mat.Dense)) = *m.triang();
      break;
    case 3:
      noalias(*(mat.Dense)) = *m.sym();
      break;
    case 4:
      noalias(*(mat.Dense)) = *m.sparse();
      break;
    case 5:
      noalias(*(mat.Dense)) = *m.banded();
      break;
    default:
      SiconosMatrixException::selfThrow("SimpleMatrix::op= (const SimpleMatrix): invalid type of matrix");
      break;
    }
    break;
  case 2:
    switch (numM)
    {
    case 2:
      noalias(*(mat.Triang)) = *m.triang();
      break;
    default:
      SiconosMatrixException::selfThrow("SimpleMatrix::assignment of a bad type of matrix into a triangular one.");
      break;
    }
    break;
  case 3:
    if (numM == 3)
      noalias(*(mat.Sym)) = *m.sym();
    else
      SiconosMatrixException::selfThrow("SimpleMatrix::bad assignment of matrix (symetric one = dense or ...)");
    break;
  case 4:
    switch (numM)
    {
    case 2:
      noalias(*(mat.Sparse)) = *m.triang();
      break;
    case 3:
      noalias(*(mat.Sparse)) = *m.sym();
      break;
    case 4:
      noalias(*(mat.Sparse)) = *m.sparse();
      break;
    case 5:
      noalias(*(mat.Sparse)) = *m.banded();
      break;
    default:
      SiconosMatrixException::selfThrow("SimpleMatrix::op= (const SimpleMatrix): invalid type of matrix");
      break;
    }
    break;
  case 5:
    switch (numM)
    {
    case 5:
      noalias(*(mat.Banded)) = *m.banded();
      break;
    default:
      SiconosMatrixException::selfThrow("SimpleMatrix::op= (const SimpleMatrix): invalid type of matrix");
      break;
    }
    break;
  default:
    SiconosMatrixException::selfThrow("SimpleMatrix::op= (const SimpleMatrix): invalid type of matrix");
    break;
  }
  resetLU();
  return *this;
}

SimpleMatrix& SimpleMatrix::operator = (const DenseMat& m)
{
  if (num != 1)
    SiconosMatrixException::selfThrow("SimpleMatrix::operator = DenseMat : forbidden: the current matrix is not dense.");

  if (dimRow != m.size1() || dimCol != m.size2())
    SiconosMatrixException::selfThrow("SimpleMatrix::operator = DenseMat failed. Inconsistent sizes.");

  noalias(*(mat.Dense)) = m;

  resetLU();
  return *this;
}

//=================================
// Op. and assignment (+=, -= ... )
//=================================

SimpleMatrix& SimpleMatrix::operator +=(const SiconosMatrix& m)
{

  unsigned int numM = m.getNum();
  if (numM == 6) // m = 0
    return *this;

  if (&m == this) // auto-assignment
  {
    switch (num)
    {
    case 1:
      *mat.Dense += *mat.Dense;
      break;
    case 2:
      *mat.Triang += *mat.Triang;
      break;
    case 3:
      *mat.Sym += *mat.Sym;
      break;
    case 4:
      *mat.Sparse += *mat.Sparse;
      break;
    case 5:
      *mat.Banded += *mat.Banded;
      break;
    default:
      SiconosMatrixException::selfThrow("SimpleMatrix op+= invalid type of matrix");
    }
    resetLU();
    return *this;
  }

  if (dimRow != m.size(0) || dimCol != m.size(1))
    resize(m.size(0), m.size(1));

  //  SiconosMatrixException::selfThrow("SimpleMatrix op+= inconsistent sizes.");

  if (numM == 0) // m is a BlockMatrix
  {
    const BlockMatrix& mB = static_cast<const BlockMatrix&>(m);
    ConstBlocksIterator1 it1;
    ConstBlocksIterator2 it2;
    unsigned int posRow = 0;
    unsigned int posCol = 0;
    // We scan all the blocks of m ...
    for (it1 = mB._mat->begin1(); it1 != mB._mat->end1(); ++it1)
    {
      for (it2 = it1.begin(); it2 != it1.end(); ++it2)
      {
        addBlock(posRow, posCol, **it2); // Each block of m is added into this.
        posCol += (*it2)->size(1);
      }
      posRow += (*it1)->size(0);
      posCol = 0;
    }
  }
  else // if m is a SimpleMatrix
  {
    switch (num)
    {
    case 1:
      switch (numM)
      {
      case 1:
        noalias(*(mat.Dense)) += *m.dense();
        break;
      case 2:
        noalias(*(mat.Dense)) += *m.triang();
        break;
      case 3:
        noalias(*(mat.Dense)) += *m.sym();
        break;
      case 4:
        noalias(*(mat.Dense)) += *m.sparse();
        break;
      case 5:
        noalias(*(mat.Dense)) += *m.banded();
        break;
      case 7:
        noalias(*(mat.Dense)) += *m.identity();
        break;
      default:
        SiconosMatrixException::selfThrow("SimpleMatrix::op+= (const SimpleMatrix): invalid type of matrix");
        break;
      }
      break;
    case 2:
      switch (numM)
      {
      case 2:
        noalias(*(mat.Triang)) += *m.triang();
        break;
      case 7:
        noalias(*(mat.Triang)) += *m.identity();
        break;
      default:
        SiconosMatrixException::selfThrow("SimpleMatrix::op+= of a bad type of matrix into a triangular one.");
        break;
      }
      break;
    case 3:
      if (numM == 3)
        noalias(*(mat.Sym)) += *m.sym();
      else if (numM == 7)
        noalias(*(mat.Sym)) += *m.identity();
      else
        SiconosMatrixException::selfThrow("SimpleMatrix::op+= bad assignment of matrix (symetric one = dense or ...)");
      break;
    case 4:
      switch (numM)
      {
      case 2:
        noalias(*(mat.Sparse)) += *m.triang();
        break;
      case 3:
        noalias(*(mat.Sparse)) += *m.sym();
        break;
      case 4:
        noalias(*(mat.Sparse)) += *m.sparse();
        break;
      case 5:
        noalias(*(mat.Sparse)) += *m.banded();
        break;
      case 7:
        noalias(*(mat.Sparse)) += *m.identity();
        break;
      default:
        SiconosMatrixException::selfThrow("SimpleMatrix::op+=: invalid type of matrix");
        break;
      }
      break;
    case 5:
      switch (numM)
      {
      case 5:
        noalias(*(mat.Banded)) += *m.banded();
        break;
      case 7:
        noalias(*(mat.Banded)) += *m.identity();
        break;
      default:
        SiconosMatrixException::selfThrow("SimpleMatrix::op+= : invalid type of matrix");
        break;
      }
      break;
    default:
      SiconosMatrixException::selfThrow("SimpleMatrix::op+= : invalid type of matrix");
      break;
    }
    resetLU();
  }
  return *this;
}

SimpleMatrix& SimpleMatrix::operator -= (const SiconosMatrix& m)
{

  unsigned int numM = m.getNum();
  if (numM == 6) // m = 0
    return *this;

  if (&m == this) // auto-assignment
  {
    switch (num)
    {
    case 1:
      *mat.Dense -= *mat.Dense;
      break;
    case 2:
      *mat.Triang -= *mat.Triang;
      break;
    case 3:
      *mat.Sym -= *mat.Sym;
      break;
    case 4:
      *mat.Sparse -= *mat.Sparse;
      break;
    case 5:
      *mat.Banded -= *mat.Banded;
      break;
    default:
      SiconosMatrixException::selfThrow("SimpleMatrix op-= invalid type of matrix");
    }
    resetLU();
    return *this;
  }
  if (dimRow != m.size(0) || dimCol != m.size(1))
    SiconosMatrixException::selfThrow("SimpleMatrix op-= inconsistent sizes.");

  if (numM == 0) // m is a BlockMatrix
  {
    const BlockMatrix& mB = static_cast<const BlockMatrix&>(m);
    ConstBlocksIterator1 it1;
    ConstBlocksIterator2 it2;
    unsigned int posRow = 0;
    unsigned int posCol = 0;
    // We scan all the blocks of m ...
    for (it1 = mB._mat->begin1(); it1 != mB._mat->end1(); ++it1)
    {
      for (it2 = it1.begin(); it2 != it1.end(); ++it2)
      {
        subBlock(posRow, posCol, **it2); // Each block of m is added into this.
        posCol += (*it2)->size(1);
      }
      posRow += (*it1)->size(0);
      posCol = 0;
    }
  }
  else // if m is a SimpleMatrix
  {
    switch (num)
    {
    case 1:
      switch (numM)
      {
      case 1:
        noalias(*(mat.Dense)) -= *m.dense();
        break;
      case 2:
        noalias(*(mat.Dense)) -= *m.triang();
        break;
      case 3:
        noalias(*(mat.Dense)) -= *m.sym();
        break;
      case 4:
        noalias(*(mat.Dense)) -= *m.sparse();
        break;
      case 5:
        noalias(*(mat.Dense)) -= *m.banded();
        break;
      case 7:
        noalias(*(mat.Dense)) -= *m.identity();
        break;
      default:
        SiconosMatrixException::selfThrow("SimpleMatrix::op-= (const SimpleMatrix): invalid type of matrix");
        break;
      }
      break;
    case 2:
      switch (numM)
      {
      case 2:
        noalias(*(mat.Triang)) -= *m.triang();
        break;
      case 7:
        noalias(*(mat.Triang)) -= *m.identity();
        break;
      default:
        SiconosMatrixException::selfThrow("SimpleMatrix::op-= of a bad type of matrix into a triangular one.");
        break;
      }
      break;
    case 3:
      if (numM == 3)
        noalias(*(mat.Sym)) -= *m.sym();
      else if (numM == 7)
        noalias(*(mat.Sym)) -= *m.identity();
      else
        SiconosMatrixException::selfThrow("SimpleMatrix::op-= bad assignment of matrix (symetric one = dense or ...)");
      break;
    case 4:
      switch (numM)
      {
      case 2:
        noalias(*(mat.Sparse)) -= *m.triang();
        break;
      case 3:
        noalias(*(mat.Sparse)) -= *m.sym();
        break;
      case 4:
        noalias(*(mat.Sparse)) -= *m.sparse();
        break;
      case 5:
        noalias(*(mat.Sparse)) -= *m.banded();
        break;
      case 7:
        noalias(*(mat.Sparse)) -= *m.identity();
        break;
      default:
        SiconosMatrixException::selfThrow("SimpleMatrix::op-=: invalid type of matrix");
        break;
      }
      break;
    case 5:
      switch (numM)
      {
      case 5:
        noalias(*(mat.Banded)) -= *m.banded();
        break;
      case 7:
        noalias(*(mat.Banded)) -= *m.identity();
        break;
      default:
        SiconosMatrixException::selfThrow("SimpleMatrix::op-= : invalid type of matrix");
        break;
      }
      break;
    default:
      SiconosMatrixException::selfThrow("SimpleMatrix::op-= : invalid type of matrix");
      break;
    }
    resetLU();
  }
  return *this;

}
