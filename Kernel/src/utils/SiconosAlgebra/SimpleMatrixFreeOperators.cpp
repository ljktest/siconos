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

#include "SimpleMatrix.hpp"
#include "BlockMatrixIterators.hpp"
#include "BlockMatrix.hpp"

#include "SiconosAlgebra.hpp"

using namespace Siconos;


SP::SimpleMatrix operator * (const SP::SimpleMatrix A, const SP::SimpleMatrix B)
{
  SP::SimpleMatrix aux(new SimpleMatrix((DenseMat)prod(*(*A).dense() , *(*B).dense())));
  return aux;
}

const SimpleMatrix operator * (const SiconosMatrix & A, double a)
{
  // To compute B = a * A

  unsigned int numA = A.getNum();

  if (numA == 6) // if A = 0
  {
    //DenseMat p(zero_matrix(A.size(0),A.size(1)));
    //return p;
    return A;
  }
  else if (numA == 7)
  {
    return (DenseMat)(a**A.identity());
  }
  else if (numA == 0) // A block
  {
    SimpleMatrix tmp(A); // ... copy ...
    tmp *= a;
    return tmp;
  }
  else if (numA == 1) // dense)
    return (DenseMat)(a** A.dense());
  else if (numA == 2)
    return (TriangMat)(a ** A.triang());
  else if (numA == 3)
    return (SymMat)(a ** A.sym());
  else if (numA == 4)
    return (SparseMat)(a ** A.sparse());
  else //if(numA==5)
    return (BandedMat)(a ** A.banded());
}

SimpleMatrix operator * (double a, const SiconosMatrix & A)
{
  // To compute B = a * A

  unsigned int numA = A.getNum();

  if (numA == 6) // if A = 0
  {
    //DenseMat p(zero_matrix(A.size(0),A.size(1)));
    //return p;
    return A;
  }
  else if (numA == 7)
  {
    return (DenseMat)(a**A.identity());
  }
  else if (numA == 0) // A block
  {
    SimpleMatrix tmp(A); // ... copy ...
    tmp *= a;
    return tmp;
  }
  else if (numA == 1) // dense)
    return (DenseMat)(a** A.dense());
  else if (numA == 2)
    return (TriangMat)(a ** A.triang());
  else if (numA == 3)
    return (SymMat)(a ** A.sym());
  else if (numA == 4)
    return (SparseMat)(a ** A.sparse());
  else //if(numA==5)
    return (BandedMat)(a ** A.banded());
}

const SimpleMatrix operator / (const SiconosMatrix & A, double a)
{
  // To compute B = A/a

  if (a == 0.0)
    SiconosMatrixException::selfThrow(" Matrix, operator / , division by zero.");

  unsigned int numA = A.getNum();

  if (numA == 6) // if A = 0
  {
    //DenseMat p(zero_matrix(A.size(0),A.size(1)));
    //return p;
    return A;
  }
  else if (numA == 7)
  {
    return (DenseMat)(*A.identity() / a);
  }
  else if (numA == 0) // A block
  {
    SimpleMatrix tmp(A); // ... copy ...
    tmp /= a;
    return tmp;
  }
  else if (numA == 1) // dense)
    return (DenseMat)(*A.dense() / a);
  else if (numA == 2)
    return (TriangMat)(*A.triang() / a);
  else if (numA == 3)
    return (SymMat)(*A.sym() / a);
  else if (numA == 4)
    return (SparseMat)(*A.sparse() / a);
  else //if(numA==5)
    return (BandedMat)(*A.banded() / a);
}

// const SimpleMatrix operator + (const  SimpleMatrix& A, const  SimpleMatrix& B){
//   return (DenseMat)(*A.dense() + *B.dense());
// }
SimpleMatrix operator + (const  SimpleMatrix& A, const  SimpleMatrix& B)
{

  return (DenseMat)(*A.dense() + *B.dense());
}

void operator +=(SP::SiconosMatrix A, SP::SimpleMatrix B)
{
  *A += *B;
}


SP::SimpleMatrix operator +(const SP::SimpleMatrix A, const SP::SimpleMatrix B)
{
  return SP::SimpleMatrix(new SimpleMatrix(*A + *B));
}



const SimpleMatrix operator + (const  SiconosMatrix& A, const  SiconosMatrix& B)
{
  // To compute C = A + B

  if ((A.size(0) != B.size(0)) || (A.size(1) != B.size(1)))
    SiconosMatrixException::selfThrow("Matrix operator +: inconsistent sizes");

  unsigned int numA = A.getNum();
  unsigned int numB = B.getNum();

  // == A or B equal to null ==
  if (numA == 6) // A = 0
  {
    if (numB == 6) // B = 0
      return SimpleMatrix(A.size(0), A.size(1));
    else
      return SimpleMatrix(B);
  }

  if (numB == 6)
    return SimpleMatrix(A);

  // == A and B different from 0 ==

  if (numA == numB && numA != 0) // all matrices are of the same type and NOT block
  {
    if (numA == 1)
      return (DenseMat)(*A.dense() + *B.dense());
    else if (numA == 2)
      return (TriangMat)(*A.triang() + *B.triang());
    else if (numA == 3)
      return (SymMat)(*A.sym() + *B.sym());
    else if (numA == 4)
    {
      SparseMat tmp(*A.sparse());
      tmp += *B.sparse();
      return tmp;
      // return (SparseMat)(*A.sparse() + *B.sparse());
    }
    else //if(numA==5)
    {
      BandedMat tmp(*A.banded());
      tmp += *B.banded();
      return tmp;
    }
  }
  else if (numA != 0 && numB != 0 && numA != numB) // A and B of different types and none is block
  {
    if (numA == 1)
    {
      if (numB == 2)
        return (DenseMat)(*A.dense() + *B.triang());
      else if (numB == 3)
        return (DenseMat)(*A.dense() + *B.sym());
      else if (numB == 4)
        return (DenseMat)(*A.dense() + *B.sparse());
      else if (numB == 5)
        return (DenseMat)(*A.dense() + *B.banded());
      else // if(numB ==7)
        return (DenseMat)(*A.dense() + *B.identity());
      SiconosMatrixException::selfThrow("Matrix operator +: invalid type of matrix");
    }
    else if (numA == 2)
    {
      if (numB == 1)
        return (DenseMat)(*A.triang() + *B.dense());
      else if (numB == 3)
        return (DenseMat)(*A.triang() + *B.sym());
      else if (numB == 4)
        return (DenseMat)(*A.triang() + *B.sparse());
      else if (numB == 5)
        return (DenseMat)(*A.triang() + *B.banded());
      else // if(numB ==7:
        return (DenseMat)(*A.triang() + *B.identity());
    }
    else if (numA == 3)
    {
      if (numB == 1)
        return (DenseMat)(*A.sym() + *B.dense());
      else if (numB == 2)
        return (DenseMat)(*A.sym() + *B.triang());
      else if (numB == 4)
        return (DenseMat)(*A.sym() + *B.sparse());
      else if (numB == 5)
        return (DenseMat)(*A.sym() + *B.banded());
      else // if(numB ==7)
        return (DenseMat)(*A.sym() + *B.identity());
    }
    else if (numA == 4)
    {
      if (numB == 1)
        return (DenseMat)(*A.sparse() + *B.dense());
      else if (numB == 2)
        return (DenseMat)(*A.sparse() + *B.triang());
      else if (numB == 3)
        return (DenseMat)(*A.sparse() + *B.sym());
      else if (numB == 5)
        return (DenseMat)(*A.sparse() + *B.banded());
      else // if(numB ==7)
        return (DenseMat)(*A.sparse() + *B.identity());
    }

    else if (numA == 5)
    {
      if (numB == 1)
        return (DenseMat)(*A.banded() + *B.dense());
      else if (numB == 2)
        return (DenseMat)(*A.banded() + *B.triang());
      else if (numB == 3)
        return (DenseMat)(*A.banded() + *B.sym());
      else if (numB == 4)
        return (DenseMat)(*A.banded() + *B.sparse());
      else //if(numB ==7)
        return (DenseMat)(*A.banded() + *B.identity());
    }

    else //if(numA==7)
    {
      if (numB == 1)
        return (DenseMat)(*A.identity() + *B.dense());
      else if (numB == 2)
        return (DenseMat)(*A.identity() + *B.triang());
      else if (numB == 3)
        return (DenseMat)(*A.identity() + *B.sym());
      else if (numB == 4)
        return (DenseMat)(*A.identity() + *B.sparse());
      else //if(numB ==5)
        return (DenseMat)(*A.identity() + *B.banded());
    }
  }
  else if (numB != 0) // B Simple, whatever is A
  {
    SimpleMatrix tmp(B);
    tmp += A;
    return tmp;
  }
  else // B Block, A simple or block
  {
    SimpleMatrix tmp(A);
    tmp += B;
    return tmp;
  }
}

const SimpleMatrix operator - (const  SiconosMatrix& A, const  SiconosMatrix& B)
{
  // To compute C = A - B

  if ((A.size(0) != B.size(0)) || (A.size(1) != B.size(1)))
    SiconosMatrixException::selfThrow("Matrix operator +: inconsistent sizes");

  unsigned int numA = A.getNum();
  unsigned int numB = B.getNum();

  // == B equal to null ==
  if (numB == 6)
    return SimpleMatrix(A);

  // == B different from 0 ==

  if (numA == numB && numA != 0) // all matrices are of the same type and NOT block
  {
    if (numA == 1)
      return (DenseMat)(*A.dense() - *B.dense());
    else if (numA == 2)
      return (TriangMat)(*A.triang() - *B.triang());
    else if (numA == 3)
      return (SymMat)(*A.sym() - *B.sym());
    else if (numA == 4)
    {
      SparseMat tmp(*A.sparse());
      tmp -= *B.sparse();
      return tmp;
      //return (SparseMat)(*A.sparse() - *B.sparse());
    }
    else //if(numA==5)
    {
      BandedMat tmp(*A.banded());
      tmp -= *B.banded();
      return tmp;
      //return (BandedMat)(*A.banded() - *B.banded());
    }
  }
  else if (numA != 0 && numB != 0 && numA != numB) // A and B of different types and none is block
  {
    if (numA == 1)
    {
      if (numB == 2)
        return (DenseMat)(*A.dense() - *B.triang());
      else if (numB == 3)
        return (DenseMat)(*A.dense() - *B.sym());
      else if (numB == 4)
        return (DenseMat)(*A.dense() - *B.sparse());
      else if (numB == 5)
        return (DenseMat)(*A.dense() - *B.banded());
      else // if(numB ==7)
        return (DenseMat)(*A.dense() - *B.identity());
      SiconosMatrixException::selfThrow("Matrix operator +: invalid type of matrix");
    }
    else if (numA == 2)
    {
      if (numB == 1)
        return (DenseMat)(*A.triang() - *B.dense());
      else if (numB == 3)
        return (DenseMat)(*A.triang() - *B.sym());
      else if (numB == 4)
        return (DenseMat)(*A.triang() - *B.sparse());
      else if (numB == 5)
        return (DenseMat)(*A.triang() - *B.banded());
      else // if(numB ==7:
        return (DenseMat)(*A.triang() - *B.identity());
    }
    else if (numA == 3)
    {
      if (numB == 1)
        return (DenseMat)(*A.sym() - *B.dense());
      else if (numB == 2)
        return (DenseMat)(*A.sym() - *B.triang());
      else if (numB == 4)
        return (DenseMat)(*A.sym() - *B.sparse());
      else if (numB == 5)
        return (DenseMat)(*A.sym() - *B.banded());
      else // if(numB ==7)
        return (DenseMat)(*A.sym() - *B.identity());
    }
    else if (numA == 4)
    {
      if (numB == 1)
        return (DenseMat)(*A.sparse() - *B.dense());
      else if (numB == 2)
        return (DenseMat)(*A.sparse() - *B.triang());
      else if (numB == 3)
        return (DenseMat)(*A.sparse() - *B.sym());
      else if (numB == 5)
        return (DenseMat)(*A.sparse() - *B.banded());
      else // if(numB ==7)
        return (DenseMat)(*A.sparse() - *B.identity());
    }

    else if (numA == 5)
    {
      if (numB == 1)
        return (DenseMat)(*A.banded() - *B.dense());
      else if (numB == 2)
        return (DenseMat)(*A.banded() - *B.triang());
      else if (numB == 3)
        return (DenseMat)(*A.banded() - *B.sym());
      else if (numB == 4)
        return (DenseMat)(*A.banded() - *B.sparse());
      else //if(numB ==7)
        return (DenseMat)(*A.banded() - *B.identity());
    }

    else if (numA == 6)
    {
      if (numB == 1)
        return (DenseMat)(*A.zero() - *B.dense());
      else if (numB == 2)
        return (DenseMat)(*A.zero() - *B.triang());
      else if (numB == 3)
        return (DenseMat)(*A.zero() - *B.sym());
      else if (numB == 4)
        return (DenseMat)(*A.zero() - *B.sparse());
      else //if(numB ==7)
        return (DenseMat)(*A.zero() - *B.identity());
    }
    else //if(numA==7)
    {
      if (numB == 1)
        return (DenseMat)(*A.identity() - *B.dense());
      else if (numB == 2)
        return (DenseMat)(*A.identity() - *B.triang());
      else if (numB == 3)
        return (DenseMat)(*A.identity() - *B.sym());
      else if (numB == 4)
        return (DenseMat)(*A.identity() - *B.sparse());
      else //if(numB ==5)
        return (DenseMat)(*A.identity() - *B.banded());
    }
  }
  else // A and/or B are/is Block
  {
    SimpleMatrix tmp(A);
    tmp -= B;
    return tmp;
  }
}

//========================
// Matrices comparison
//========================

bool operator == (const SiconosMatrix &m, const SiconosMatrix &x)
{
  //  if( ! isComparableTo( m, x))
  //    return false;
  // Warning : two block matrices may be "equal" but have blocks of different sizes.
  double norm = (m - x).normInf();
  return (norm < tolerance);
}


