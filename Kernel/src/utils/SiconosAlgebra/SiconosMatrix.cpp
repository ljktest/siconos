#include "SiconosMatrix.h"

// Constructor with the type-number
SiconosMatrix::SiconosMatrix(unsigned int newNum): dimRow(0), dimCol(0), num(newNum)
{}

// Constructor with the dimensions and the type-number
SiconosMatrix::SiconosMatrix(unsigned int newNum, unsigned int row, unsigned int col): dimRow(row), dimCol(col), num(newNum)
{}

BlockIterator1 SiconosMatrix::begin()
{
  SiconosMatrixException::selfThrow("SiconosMatrix::begin(): reserved to BlockMatrix");
  BlockIterator1 it;
  return it;
};

BlockIterator1 SiconosMatrix::end()
{
  SiconosMatrixException::selfThrow("SiconosMatrix::end(): reserved to BlockMatrix");
  BlockIterator1 it;
  return it;
};

ConstBlockIterator1 SiconosMatrix::begin() const
{
  SiconosMatrixException::selfThrow("SiconosMatrix::begin(): reserved to BlockMatrix");
  ConstBlockIterator1 it;
  return it;
};

ConstBlockIterator1 SiconosMatrix::end() const
{
  SiconosMatrixException::selfThrow("SiconosMatrix::end(): reserved to BlockMatrix");
  ConstBlockIterator1 it;
  return it;
};

const Index * SiconosMatrix::getTabRowPtr() const
{
  SiconosMatrixException::selfThrow("SiconosMatrix::getTabRowPtr() : not implemented for this type of matrix (Simple?) reserved to BlockMatrix.");
  // fake to avoid error on warning.
  Index * tmp = NULL;
  return tmp;
}

const Index * SiconosMatrix::getTabColPtr() const
{
  SiconosMatrixException::selfThrow("SiconosMatrix::getTabColPtr() : not implemented for this type of matrix (Simple?) reserved to BlockMatrix.");
  // fake to avoid error on warning.
  Index * tmp = NULL;
  return tmp;
}

//=====================
// matrices comparison
//=====================
const bool isComparableTo(const  SiconosMatrix* m1, const  SiconosMatrix* m2)
{
  // return:
  // - true if one of the matrices is a Simple and if they have the same dimensions.
  // - true if both are block but with blocks which are facing each other of the same size.
  // - false in other cases

  if ((!m1->isBlock() || !m2->isBlock()) && (m1->size(0) == m2->size(0)) && (m1->size(1) == m2->size(1)))
    return true;

  const Index * I1R = m1->getTabRowPtr();
  const Index * I2R = m2->getTabRowPtr();
  const Index * I1C = m1->getTabColPtr();
  const Index * I2C = m2->getTabColPtr();

  return ((*I1R == *I2R) && (*I1C == *I2C));
}

