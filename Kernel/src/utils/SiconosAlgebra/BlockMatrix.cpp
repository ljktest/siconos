#include "BlockMatrix.h"
#include "SimpleMatrix.h"
#include "SimpleVector.h"
#include "SiconosMatrixException.h"
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

using std::cout;
using std::endl;

// =================================================
//                CONSTRUCTORS
// =================================================

// Default (private)
BlockMatrix::BlockMatrix(): SiconosMatrix(0), mat(NULL), tabRow(NULL), tabCol(NULL), isBlockAllocatedIn(NULL)
{}

BlockMatrix::BlockMatrix(const SiconosMatrix &m): SiconosMatrix(0), mat(NULL), tabRow(NULL), tabCol(NULL), isBlockAllocatedIn(NULL)
{
  tabRow = new Index();
  tabCol = new Index();
  if (m.isBlock())
  {
    unsigned int nbRows = m.getNumberOfBlocks(0);
    unsigned int nbCols = m.getNumberOfBlocks(1);
    tabRow->reserve(nbRows);
    tabCol->reserve(nbCols);

    // mat construction
    mat = new BlocksMat(nbRows, nbCols, nbRows * nbCols);
    isBlockAllocatedIn = new std::vector<bool>();
    isBlockAllocatedIn->reserve(nbRows * nbCols);

    unsigned int i, j;
    ConstBlockIterator1 it1;
    ConstBlockIterator2 it2;
    bool firstLoop = true;
    // We scan all the blocks of m ...
    for (it1 = m.begin(); it1 != m.end(); ++it1)
    {
      dimRow += (*(it1.begin()))->size(0);
      tabRow->push_back(dimRow);
      for (it2 = it1.begin(); it2 != it1.end(); ++it2)
      {
        i = it2.index1();
        j = it2.index2();
        if ((*it2)->isBlock())  // if the current matrix is a blockMatrix
          mat->insert_element(i, j, new BlockMatrix(**it2));
        else
          mat->insert_element(i, j, new SimpleMatrix(**it2));
        isBlockAllocatedIn->push_back(true);
        // dimCol must be incremented only at first "column-loop"
        if (firstLoop)
        {
          dimCol += (*it2)->size(1);
          tabCol->push_back(dimCol);
        }
      }
      firstLoop = false;
    }
  }
  else // if m is a SimpleMatrix
  {
    tabRow->reserve(1);
    tabCol->reserve(1);
    // mat construction
    mat = new BlocksMat(1, 1, 1);
    isBlockAllocatedIn = new std::vector<bool>();
    isBlockAllocatedIn->reserve(1);
    mat->insert_element(0, 0, new SimpleMatrix(m));
    isBlockAllocatedIn->push_back(true);
    dimRow = m.size(0);
    dimCol = m.size(1);
    tabRow->push_back(dimRow);
    tabCol->push_back(dimCol);
  }
}

BlockMatrix::BlockMatrix(const BlockMatrix &m): SiconosMatrix(0), mat(NULL), tabRow(NULL), tabCol(NULL), isBlockAllocatedIn(NULL)
{
  unsigned int nbRows = m.getNumberOfBlocks(0);
  unsigned int nbCols = m.getNumberOfBlocks(1);
  tabRow = new Index();
  tabCol = new Index();
  tabRow->reserve(nbRows);
  tabCol->reserve(nbCols);

  // mat construction
  mat = new BlocksMat(nbRows, nbCols, nbRows * nbCols);
  isBlockAllocatedIn = new std::vector<bool>();
  isBlockAllocatedIn->reserve(nbRows * nbCols);

  unsigned int i, j;
  // We scan all the blocks of m ...
  ConstBlockIterator1 it1;
  ConstBlockIterator2 it2;
  bool firstLoop = true;
  // We scan all the blocks of m ...
  for (it1 = m.begin(); it1 != m.end(); ++it1)
  {
    dimRow += (*(it1.begin()))->size(0);
    tabRow->push_back(dimRow);
    for (it2 = it1.begin(); it2 != it1.end(); ++it2)
    {
      i = it2.index1();
      j = it2.index2();
      if ((*it2)->isBlock())  // if the current matrix is a blockMatrix
        mat->insert_element(i, j, new BlockMatrix(**it2));
      else
        mat->insert_element(i, j, new SimpleMatrix(**it2));
      isBlockAllocatedIn->push_back(true);
      // dimCol must be incremented only at first "column-loop"
      if (firstLoop)
      {
        dimCol += (*it2)->size(1);
        tabCol->push_back(dimCol);
      }
    }
    firstLoop = false;
  }
}

BlockMatrix::BlockMatrix(const std::vector<SiconosMatrix* >& m, unsigned int row, unsigned int col):
  SiconosMatrix(0), mat(NULL), tabRow(NULL), tabCol(NULL), isBlockAllocatedIn(NULL)
{
  if (m.size() != (row * col))
    SiconosMatrixException::selfThrow("BlockMatrix constructor from a vector<SiconosMatrix*>, number of blocks inconsistent with provided dimensions.");

  tabRow = new Index();
  tabCol = new Index();
  tabRow->reserve(row);
  tabCol->reserve(col);

  // mat construction
  mat = new BlocksMat(row, col, row * col);
  isBlockAllocatedIn = new std::vector<bool>();
  isBlockAllocatedIn->reserve(row * col);

  BlockIterator1 it1;
  BlockIterator2 it2;

  unsigned int k = 0;
  bool firstRowLoop = true;
  bool firstColLoop = true;

  for (unsigned int i = 0; i < row; ++i)
  {
    for (unsigned int j = 0; j < col; ++j)
    {
      (*mat)(i, j) = m[k++];
      isBlockAllocatedIn->push_back(false);
      // dimCol must be incremented only at first "column-loop"
      if (firstColLoop)
      {
        dimCol += m[k - 1]->size(1);
        tabCol->push_back(dimCol);
      }
      if (firstRowLoop)
      {
        dimRow += m[k - 1]->size(0);
        tabRow->push_back(dimRow);
        firstRowLoop = false;
      }
    }
    firstColLoop = false;
    firstRowLoop = true;
  }
}

BlockMatrix::BlockMatrix(SiconosMatrix* A, SiconosMatrix* B, SiconosMatrix* C, SiconosMatrix* D):
  SiconosMatrix(0), mat(NULL), tabRow(NULL), tabCol(NULL), isBlockAllocatedIn(NULL)
{
  if (A->size(0) != B->size(0) || C->size(0) != D->size(0) ||  A->size(1) != C->size(1) ||  B->size(1) != D->size(1))
    SiconosMatrixException::selfThrow("BlockMatrix constructor(A,B,C,D), inconsistent sizes between A, B, C or D SiconosMatrices.");

  // mat = [ A B ]
  //       [ C D ]

  // mat construction
  mat = new BlocksMat(2, 2, 4);
  isBlockAllocatedIn = new std::vector<bool>();
  isBlockAllocatedIn->reserve(4);
  tabRow = new Index();
  tabCol = new Index();
  tabRow->reserve(2);
  tabCol->reserve(2);

  (*mat)(0, 0) = A;
  (*mat)(0, 1) = B;
  (*mat)(1, 0) = C;
  (*mat)(1, 1) = D;
  dimRow = A->size(0);
  tabRow->push_back(dimRow);
  dimRow += C->size(0);
  tabRow->push_back(dimRow);
  dimCol = A->size(1);
  tabCol->push_back(dimCol);
  dimCol += B->size(1);
  tabCol->push_back(dimCol);
  isBlockAllocatedIn->push_back(false);
  isBlockAllocatedIn->push_back(false);
  isBlockAllocatedIn->push_back(false);
  isBlockAllocatedIn->push_back(false);
}

BlockMatrix::~BlockMatrix()
{
  std::vector<bool>::iterator it = isBlockAllocatedIn->begin();
  BlockIterator1 it1;
  BlockIterator2 it2;

  for (it1 = mat->begin1(); it1 != mat->end1(); ++it1)
  {
    for (it2 = it1.begin(); it2 != it1.end(); ++it2)
    {
      if (*it++) delete(*it2);
      *it2 = NULL;
    }
  }

  mat->clear();
  delete mat;
  mat = NULL;
  tabRow->clear();
  tabCol->clear();
  delete tabRow;
  tabRow = NULL;
  delete tabCol;
  tabCol = NULL;
  isBlockAllocatedIn->clear();
  delete isBlockAllocatedIn;
  isBlockAllocatedIn = NULL;

}

// =================================================
//    get number of blocks
// =================================================

unsigned int BlockMatrix::getNumberOfBlocks(unsigned int dim) const
{
  if (dim == 0)
    return tabRow->size();
  else
    return tabCol->size();
}

// =================================================
//   iterators to begin/end of the ublas matrix
// =================================================

BlockIterator1 BlockMatrix::begin()
{
  return mat->begin1();
}

BlockIterator1 BlockMatrix::end()
{
  return mat->end1();
}

ConstBlockIterator1 BlockMatrix::begin() const
{
  return mat->begin1();
}

ConstBlockIterator1 BlockMatrix::end() const
{
  return mat->end1();
}

// =================================================
//        get Ublas component (dense ...)
// =================================================

// return the boost dense matrix of the block (i, j)
const DenseMat  BlockMatrix::getDense(unsigned int row, unsigned int col) const
{
  (*(*mat)(row, col)).display();

  if ((*(*mat)(row, col)).getNum() != 1)
    SiconosMatrixException::selfThrow("DenseMat BlockMatrix::getDense(unsigned int row, unsigned int col) : the matrix at (row, col) is not a Dense matrix");

  return ((*(*mat)(row, col)).getDense());
}

// return the boost triangular matrix of the block (i, j)
const TriangMat BlockMatrix::getTriang(unsigned int row, unsigned int col) const
{
  if ((*(*mat)(row, col)).getNum() != 2);
  SiconosMatrixException::selfThrow("TriangMat BlockMatrix::getTriang(unsigned int row, unsigned int col) : the matrix at (row, col) is not a Triangular matrix");
  return ((*(*mat)(row, col)).getTriang());
}

// return the boost symmetric matrix of the block (i, j)
const SymMat BlockMatrix::getSym(unsigned int row, unsigned int col) const
{

  if ((*(*mat)(row, col)).getNum() != 3);
  SiconosMatrixException::selfThrow("SymMat BlockMatrix::getSym(unsigned int row, unsigned int col) : the matrix at (row, col) is not a Symmmetric matrix");
  return ((*(*mat)(row, col)).getSym());
}

// return the boost sparse matrix of the block (i, j)
const SparseMat  BlockMatrix::getSparse(unsigned int row, unsigned int col) const
{

  if ((*(*mat)(row, col)).getNum() != 4);
  SiconosMatrixException::selfThrow("SparseMat BlockMatrix::getSparse(unsigned int row, unsigned int col) : the matrix at (row, col) is not a Sparse matrix");

  return ((*(*mat)(row, col)).getSparse());
}

// return the boost banded matrix of the block (i, j)
const BandedMat  BlockMatrix::getBanded(unsigned int row, unsigned int col) const
{

  if ((*(*mat)(row, col)).getNum() != 5);
  SiconosMatrixException::selfThrow("BandedMat BlockMatrix::getBanded(unsigned int row, unsigned int col) : the matrix at (row, col) is not a Banded matrix");

  return ((*(*mat)(row, col)).getBanded());
}

// return the boost zero matrix of the block (i, j)
const ZeroMat  BlockMatrix::getZero(unsigned int row, unsigned int col) const
{

  if ((*(*mat)(row, col)).getNum() != 5);
  SiconosMatrixException::selfThrow("ZeroMat BlockMatrix::getZero(unsigned int row, unsigned int col) : the matrix at (row, col) is not a Zero matrix");

  return ((*(*mat)(row, col)).getZero());
}

// return the boost identity matrix of the block (i, j)
const IdentityMat  BlockMatrix::getIdentity(unsigned int row, unsigned int col) const
{

  if ((*(*mat)(row, col)).getNum() != 5);
  SiconosMatrixException::selfThrow("IdentityMat BlockMatrix::getIdentity(unsigned int row, unsigned int col) : the matrix at (row, col) is not a Identity matrix");

  return ((*(*mat)(row, col)).getIdentity());
}

// The following functions return the corresponding pointers
DenseMat*  BlockMatrix::getDensePtr(unsigned int row, unsigned int col) const
{

  if ((*(*mat)(row, col)).getNum() != 1);
  SiconosMatrixException::selfThrow("DenseMat* BlockMatrix::getDensePtr(unsigned int row, unsigned int col) : the matrix at (row, col) is not a Dense matrix");

  return ((*(*mat)(row, col)).getDensePtr());
}

TriangMat* BlockMatrix::getTriangPtr(unsigned int row, unsigned int col) const
{

  if ((*(*mat)(row, col)).getNum() != 2);
  SiconosMatrixException::selfThrow("TriangMat* BlockMatrix::getTriangPtr(unsigned int row, unsigned int col) : the matrix at (row, col) is not a Triangular matrix");

  return ((*(*mat)(row, col)).getTriangPtr());
}
SymMat* BlockMatrix::getSymPtr(unsigned int row, unsigned int col) const
{

  if ((*(*mat)(row, col)).getNum() != 3);
  SiconosMatrixException::selfThrow("SymMat* BlockMatrix::getSymPtr(unsigned int row, unsigned int col) : the matrix at (row, col) is not a Symmmetric matrix");
  return ((*(*mat)(row, col)).getSymPtr());
}

SparseMat*  BlockMatrix::getSparsePtr(unsigned int row, unsigned int col) const
{

  if ((*(*mat)(row, col)).getNum() != 4);
  SiconosMatrixException::selfThrow("SparseMat* BlockMatrix::getSparsePtr(unsigned int row, unsigned int col) : the matrix at (row, col) is not a Sparse matrix");

  return ((*(*mat)(row, col)).getSparsePtr());
}

BandedMat*  BlockMatrix::getBandedPtr(unsigned int row, unsigned int col) const
{

  if ((*(*mat)(row, col)).getNum() != 5);
  SiconosMatrixException::selfThrow("BandedMat* BlockMatrix::getBandedPtr(unsigned int row, unsigned int col) : the matrix at (row, col) is not a Banded matrix");

  return ((*(*mat)(row, col)).getBandedPtr());
}

ZeroMat*  BlockMatrix::getZeroPtr(unsigned int row, unsigned int col) const
{

  if ((*(*mat)(row, col)).getNum() != 5);
  SiconosMatrixException::selfThrow("ZeroMat* BlockMatrix::getZeroPtr(unsigned int row, unsigned int col) : the matrix at (row, col) is not a Zero matrix");

  return ((*(*mat)(row, col)).getZeroPtr());
}

IdentityMat*  BlockMatrix::getIdentityPtr(unsigned int row, unsigned int col) const
{

  if ((*(*mat)(row, col)).getNum() != 5);
  SiconosMatrixException::selfThrow("IdentityMat* BlockMatrix::getIdentityPtr(unsigned int row, unsigned int col) : the matrix at (row, col) is not a Identity matrix");

  return ((*(*mat)(row, col)).getIdentityPtr());
}

double* BlockMatrix::getArray(unsigned int i, unsigned int j) const
{
  return (*(*mat)(i, j)).getArray();
}

// ===========================
//       fill matrix
// ===========================

void BlockMatrix::zero()
{
  BlocksMat::iterator1 it;
  BlocksMat::iterator2 it2;
  for (it = mat->begin1(); it != mat->end1(); ++it)
  {
    for (it2 = it.begin(); it2 != it.end(); ++it2)
    {
      (*it2)->zero();
    }
  }
}

void BlockMatrix::eye()
{
  BlocksMat::iterator1 it;
  BlocksMat::iterator2 it2;

  for (it = mat->begin1(); it != mat->end1(); ++it)
  {
    for (it2 = it.begin(); it2 != it.end(); ++it2)
    {
      if (it2.index1() == it2.index2())
        (*it2)->eye();
      else
        (*it2)->zero();
    }
  }
}

//=======================
// set matrix dimensions
//=======================

void BlockMatrix::resize(unsigned int, unsigned int, unsigned int, unsigned int, bool)
{
  SiconosMatrixException::selfThrow("BlockMatrix::resize : forbidden for block matrices.");
}

//=======================
//       get norm
//=======================

const double BlockMatrix::normInf()const
{
  double sum = 0, norm = 0;
  for (unsigned int i = 0; i < size(0); i++)
  {
    for (unsigned int j = 0; j < size(1); j++)
    {
      sum += (*this)(i, j);
    }
    if (fabs(sum) > norm) norm = fabs(sum);
    sum = 0;
  }
  return norm;
}

//=====================
// screen display
//=====================

void BlockMatrix::display(void)const
{
  std::cout << "==========> BlockMatrix (" << getNumberOfBlocks(0) << " X " << getNumberOfBlocks(1) << " blocks): " << std::endl;
  BlocksMat::iterator1 it;
  BlocksMat::iterator2 it2;
  for (it = mat->begin1(); it != mat->end1(); ++it)
  {
    for (it2 = it.begin(); it2 != it.end(); ++it2)
    {
      (*it2)->display();
    }
  }
  std::cout << "===========================================================================================" << std::endl;
}

//=============================
// Elements access (get or set)
//=============================

double& BlockMatrix::operator()(unsigned int row, unsigned int col)
{
  unsigned int nbRow = 0;
  unsigned int nbCol = 0;

  while (row >= (*tabRow)[nbRow] && nbRow < tabRow->size())
    nbRow ++;

  while (col >= (*tabCol)[nbCol] && nbCol < tabCol->size())
    nbCol ++;

  unsigned int posRow = row;
  unsigned int posCol = col;

  if (nbRow != 0)
    posRow -= (*tabRow)[nbRow - 1];
  if (nbCol != 0)
    posCol -= (*tabCol)[nbCol - 1];

  return (*(*mat)(nbRow, nbCol))(posRow, posCol);
}

const double BlockMatrix::operator()(unsigned int row, unsigned int col) const
{

  unsigned int nbRow = 0;
  unsigned int nbCol = 0;

  while (row >= (*tabRow)[nbRow] && nbRow < tabRow->size())
    nbRow ++;

  while (col >= (*tabCol)[nbCol] && nbCol < tabCol->size())
    nbCol ++;

  unsigned int posRow = row;
  unsigned int posCol = col;

  if (nbRow != 0)
    posRow -= (*tabRow)[nbRow - 1];
  if (nbCol != 0)
    posCol -= (*tabCol)[nbCol - 1];

  return (*(*mat)(nbRow, nbCol))(posRow, posCol);
}

const double BlockMatrix::getValue(unsigned int row, unsigned int col) const
{
  unsigned int nbRow = 0;
  unsigned int nbCol = 0;

  while (row >= (*tabRow)[nbRow] && nbRow < tabRow->size())
    nbRow ++;

  while (col >= (*tabCol)[nbCol] && nbCol < tabCol->size())
    nbCol ++;

  unsigned int posRow = row;
  unsigned int posCol = col;

  if (nbRow != 0)
    posRow -= (*tabRow)[nbRow - 1];
  if (nbCol != 0)
    posCol -= (*tabCol)[nbCol - 1];

  return (*(*mat)(nbRow, nbCol))(posRow, posCol);
}

void BlockMatrix::setValue(unsigned int row, unsigned int col, double value)
{
  unsigned int nbRow = 0;
  unsigned int nbCol = 0;

  while (row >= (*tabRow)[nbRow] && nbRow < tabRow->size())
    nbRow ++;

  while (col >= (*tabCol)[nbCol] && nbCol < tabCol->size())
    nbCol ++;

  unsigned int posRow = row;
  unsigned int posCol = col;

  if (nbRow != 0)
    posRow -= (*tabRow)[nbRow - 1];
  if (nbCol != 0)
    posCol -= (*tabCol)[nbCol - 1];

  (*(*mat)(nbRow, nbCol))(posRow, posCol) = value;
}

//============================================
// Access (get or set) to blocks of elements
//============================================

// void BlockMatrix::getBlock (unsigned int row, unsigned int col, SiconosMatrix * m) const
// {
//   SiconosMatrixException::selfThrow("BlockMatrix::getBlock, not yet implemented or useless for BlockMatrices.");
// }

// void BlockMatrix::setBlock(unsigned int row, unsigned int col, const SiconosMatrix *m)
// {
//   // Set current matrix elements, starting from row row_min and column col_min, with the values of the matrix m.
//   // m may be a BlockMatrix.

//   if(m == this)
//     SiconosMatrixException::selfThrow("BlockMatrix::setBlock(pos,..., m): m = this.");

//   if(m->isBlock ())
//     SiconosMatrixException::selfThrow("BlockMatrix::setBlock of a block into an other block is forbidden.");

//   if(row > dimRow || col > dimCol )
//     SiconosMatrixException::selfThrow("BlockMatrix::setBlock(i,j,m), i or j is out of range.");

//   // Check dim
//   if( (*(*mat)(row,col)).size(0)!=m->size(0) || (*(*mat)(row,col)).size(1) != m->size(1) )
//     SiconosMatrixException::selfThrow("BlockMatrix::setBlock(x,y,m), block(x,y) of current matrix and m have inconsistent sizes.");
//   *((*mat)(row,col)) = *m; // copy
// }

void BlockMatrix::getRow(unsigned int r, SiconosVector &v) const
{
  unsigned int numRow = 0, posRow = r, start = 0, stop = 0;

  if (r > dimRow)
    SiconosMatrixException::selfThrow("BlockMatrix:getRow : row number is out of range");

  // Verification of the size of the result vector
  if (v.size() != dimCol)
    SiconosMatrixException::selfThrow("BlockMatrix:getRow : inconsistent sizes");

  // Find the row-block number where "r" is
  while (r >= (*tabRow)[numRow] && numRow < tabRow->size())
    numRow ++;

  // Computation of the value of the index row into this block
  if (numRow != 0)
    posRow -= (*tabRow)[numRow - 1];

  for (unsigned int j = 0; j < tabCol->size(); j++)
  {
    start = stop;
    stop += (*(*mat)(numRow, j)).size(1);
    ublas::subrange(*(v.getDensePtr()), start, stop) = ublas::row(*((*(*mat)(numRow, j)).getDensePtr()), posRow);
  }
}

void BlockMatrix::getCol(unsigned int c, SiconosVector &v) const
{
  unsigned int numCol = 0, posCol = c, start = 0, stop = 0;

  if (c > dimCol)
    SiconosMatrixException::selfThrow("BlockMatrix:getCol : column number is out of range");

  // Verification of the size of the result vector
  if (v.size() != dimRow)
    SiconosMatrixException::selfThrow("BlockMatrix:getcol : inconsistent sizes");

  // Find the column-block number where "c" is
  while (c >= (*tabCol)[numCol] && numCol < tabCol->size())
    numCol ++;

  // Computation of the value of the index column into this block
  if (numCol != 0)
    posCol -= (*tabCol)[numCol - 1];

  for (unsigned int i = 0; i < tabRow->size(); i++)
  {
    start = stop;
    stop += (*(*mat)(i, numCol)).size(0);
    ublas::subrange(*(v.getDensePtr()), start, stop) = ublas::column((*(*mat)(i, numCol)).getDense(), posCol);
  }
}

void BlockMatrix::setRow(unsigned int r, const SiconosVector &v)
{

  unsigned int numRow = 0, posRow = r, start = 0, stop = 0;

  if (v.size() != dimCol)
    SiconosMatrixException::selfThrow("BlockMatrix:setRow : inconsistent sizes");

  while (r >= (*tabRow)[numRow] && numRow < tabRow->size())
    numRow ++;

  if (numRow != 0)
    posRow -= (*tabRow)[numRow - 1];

  for (unsigned int j = 0; j < tabCol->size(); j++)
  {
    start = stop;
    stop += (* (*mat)(numRow, j)).size(1);
    ublas::row(*((*(*mat)(numRow, j)).getDensePtr()), posRow) = ublas::subrange(*(v.getDensePtr()), start, stop);
  }
}

void BlockMatrix::setCol(unsigned int col, const SiconosVector &v)
{

  unsigned int numCol = 0, posCol = col, start = 0, stop = 0;

  if (v.size() != dimRow)
    SiconosMatrixException::selfThrow("BlockMatrix:setCol : inconsistent sizes");

  while (col >= (*tabCol)[numCol] && numCol < tabCol->size())
    numCol ++;

  if (numCol != 0)
    posCol -= (*tabCol)[numCol - 1];

  for (unsigned int i = 0; i < tabRow->size(); i++)
  {
    start = stop;
    stop += (*(*mat)(i, numCol)).size(0);
    ublas::column(*((*(*mat)(i, numCol)).getDensePtr()), posCol) = ublas::subrange(*(v.getDensePtr()), start, stop);
  }
}

void BlockMatrix::addSimple(unsigned int& indRow, unsigned int& indCol, const SiconosMatrix& m)
{
  // Add a part of m (starting from (indRow,indCol) to the current matrix.
  // m must be a SimpleMatrix.

  // At the end of the present function, indRow (resp. indCol) is equal to indRow + the corresponding dimension of the added sub-matrix.

  unsigned int row = m.size(0) - indRow; // number of rows of the block to be added.
  unsigned int col = m.size(1) - indCol; // number of columns of the block to be added.
  unsigned int initCol = indCol;

  if (row > dimRow || col > dimCol) SiconosMatrixException::selfThrow("BlockMatrix::addSimple : invalid ranges");

  unsigned int numM = m.getNum();

  // iterators through this
  BlocksMat::iterator1 it1;
  BlocksMat::iterator2 it2;
  unsigned int currentRow = 0 , currentCol = 0, currentNum;
  for (it1 = mat->begin1(); it1 != mat->end1(); ++it1)
  {
    for (it2 = it1.begin(); it2 != it1.end(); ++it2)
    {
      if ((*it2)->isBlock())  // if the sub-block is also a BlockMatrix ...
        (static_cast<BlockMatrix*>(*it2))->addSimple(indRow, indCol, m);

      else
      {
        currentCol = (*it2)->size(1);
        currentRow = (*it2)->size(0);
        currentNum = (*it2)->getNum();
        if (numM != currentNum) SiconosMatrixException::selfThrow("BlockMatrix::addSimple : inconsistent types.");

        if (numM == 1)
          noalias(*(*it2)->getDensePtr()) += ublas::subrange(*m.getDensePtr(), indRow, indRow + currentRow, indCol, indCol + currentCol);
        else if (numM == 2)
          noalias(*(*it2)->getTriangPtr()) += ublas::subrange(*m.getTriangPtr(), indRow, indRow + currentRow, indCol, indCol + currentCol);
        else if (numM == 3)
          noalias(*(*it2)->getSymPtr()) += ublas::subrange(*m.getSymPtr(), indRow, indRow + currentRow, indCol, indCol + currentCol);
        else if (numM == 4)
          noalias(*(*it2)->getSparsePtr()) += ublas::subrange(*m.getSparsePtr(), indRow, indRow + currentRow, indCol, indCol + currentCol);
        else if (numM == 5)
          noalias(*(*it2)->getBandedPtr()) += ublas::subrange(*m.getBandedPtr(), indRow, indRow + currentRow, indCol, indCol + currentCol);
        else if (numM == 6) {}
        else
          SiconosMatrixException::selfThrow("BlockMatrix::addSimple : inconsistent types.");
      }
      indCol += currentCol;
    }
    indRow += currentRow;
    indCol = initCol;
  }
}

void BlockMatrix::subSimple(unsigned int& indRow, unsigned int& indCol, const SiconosMatrix& m)
{
  // subtract a part of m (starting from (indRow,indCol) to the current matrix.
  // m must be a SimpleMatrix.

  // At the end of the present function, indRow (resp. indCol) is equal to indRow + the corresponding dimension of the subtracted sub-matrix.

  unsigned int row = m.size(0) - indRow; // number of rows of the block to be added.
  unsigned int col = m.size(1) - indCol; // number of columns of the block to be added.
  unsigned int initCol = indCol;
  if (row > dimRow || col > dimCol) SiconosMatrixException::selfThrow("BlockMatrix::addSimple : invalid ranges");

  unsigned int numM = m.getNum();

  // iterators through this
  BlocksMat::iterator1 it1;
  BlocksMat::iterator2 it2;
  unsigned int currentRow = 0, currentCol = 0, currentNum;
  for (it1 = mat->begin1(); it1 != mat->end1(); ++it1)
  {
    for (it2 = it1.begin(); it2 != it1.end(); ++it2)
    {
      if ((*it2)->isBlock())  // if the sub-block is also a BlockMatrix ...
        (static_cast<BlockMatrix*>(*it2))->subSimple(indRow, indCol, m);

      else
      {
        currentCol = (*it2)->size(1);
        currentRow = (*it2)->size(0);
        currentNum = (*it2)->getNum();
        if (numM != currentNum) SiconosMatrixException::selfThrow("BlockMatrix::addSimple : inconsistent types.");

        if (numM == 1)
          noalias(*(*it2)->getDensePtr()) -= ublas::subrange(*m.getDensePtr(), indRow, indRow + currentRow, indCol, indCol + currentCol);
        else if (numM == 2)
          noalias(*(*it2)->getTriangPtr()) -= ublas::subrange(*m.getTriangPtr(), indRow, indRow + currentRow, indCol, indCol + currentCol);
        else if (numM == 3)
          noalias(*(*it2)->getSymPtr()) -= ublas::subrange(*m.getSymPtr(), indRow, indRow + currentRow, indCol, indCol + currentCol);
        else if (numM == 4)
          noalias(*(*it2)->getSparsePtr()) -= ublas::subrange(*m.getSparsePtr(), indRow, indRow + currentRow, indCol, indCol + currentCol);
        else if (numM == 5)
          noalias(*(*it2)->getBandedPtr()) -= ublas::subrange(*m.getBandedPtr(), indRow, indRow + currentRow, indCol, indCol + currentCol);
        else if (numM == 6) {}
        else
          SiconosMatrixException::selfThrow("BlockMatrix::addSimple : inconsistent types.");
      }
      indCol += currentCol;
    }
    indRow += currentRow;
    indCol = initCol;
  }
}


//===============
//  Assignment
//===============

BlockMatrix& BlockMatrix::operator = (const SiconosMatrix &m)
{
  if (&m == this) return *this; // auto-assignment.

  if (m.size(0) != dimRow || m.size(1) != dimCol)
    SiconosMatrixException::selfThrow("operator = (const SiconosMatrix&): Left and Right values have inconsistent sizes.");

  // Warning: we do not reallocate the blocks, but only copy the values. This means that
  // all blocks are already allocated and that dim of m and mat are to be consistent.
  // Thus, tabRow and tabCol remains unchanged.
  // If m and mat are not "block-consistent", we use the () operator for a component-wise copy.

  if (m.isBlock())
  {
    if (isComparableTo(this, &m))
    {
      // iterators through this
      BlocksMat::iterator1 it1;
      BlocksMat::iterator2 it2;
      // iterators through m
      BlocksMat::const_iterator1 itM1 = m.begin();
      BlocksMat::const_iterator2 itM2;

      for (it1 = mat->begin1(); it1 != mat->end1(); ++it1)
      {
        itM2 = itM1.begin();
        for (it2 = it1.begin(); it2 != it1.end(); ++it2)
        {
          (**it2) = (**itM2);
          itM2++; // increment column pos. in m.
        }
        itM1++; // increment row pos. in m.
      }
    }
    else
    {
      for (unsigned int i = 0; i < dimRow; ++i)
        for (unsigned int j = 0; j < dimCol; ++j)
          (*this)(i, j) = m(i, j);
    }
  }
  else // if m is a SimpleMatrix
  {
    BlockIterator1 it;
    BlockIterator2 it2;
    unsigned int posRow = 0;
    unsigned int posCol = 0;
    Index subDim(2);
    Index subPos(4);

    for (it = mat->begin1(); it != mat->end1(); ++it)
    {
      for (it2 = it.begin(); it2 != it.end(); ++it2)
      {
        // a sub-block of m is copied into this
        subDim[0] = (*it2)->size(0);
        subDim[1] = (*it2)->size(1);
        subPos[0] = posRow;
        subPos[1] = posCol;
        subPos[2] = 0;
        subPos[3] = 0;
        setBlock(&m, *it2, subDim, subPos);
        posCol += subDim[1];
      }
      posRow += (*it)->size(0);
      posCol = 0;
    }
  }

  return *this;
}

BlockMatrix& BlockMatrix::operator = (const BlockMatrix &m)
{
  if (&m == this) return *this; // auto-assignment.

  if (m.size(0) != dimRow || m.size(1) != dimCol)
    SiconosMatrixException::selfThrow("operator = (const SiconosMatrix&): Left and Right values have inconsistent sizes.");

  // Warning: we do not reallocate the blocks, but only copy the values. This means that
  // all blocks are already allocated and that dim of m and mat are to be consistent.
  // Thus, tabRow and tabCol remains unchanged.
  // If m and mat are not "block-consistent", we use the () operator for a componet-wise copy.

  if (isComparableTo(this, &m))
  {
    // iterators through this
    BlocksMat::iterator1 it1;
    BlocksMat::iterator2 it2;
    // iterators through m
    BlocksMat::const_iterator1 itM1 = m.begin();
    BlocksMat::const_iterator2 itM2;

    for (it1 = mat->begin1(); it1 != mat->end1(); ++it1)
    {
      itM2 = itM1.begin();
      for (it2 = it1.begin(); it2 != it1.end(); ++it2)
      {
        (**it2) = (**itM2);
        itM2++; // increment column pos. in m.
      }
      itM1++; // increment row pos. in m.
    }
  }
  else
  {
    for (unsigned int i = 0; i < dimRow; ++i)
      for (unsigned int j = 0; j < dimCol; ++j)
        (*this)(i, j) = m(i, j);
  }
  return *this;
}

BlockMatrix& BlockMatrix::operator = (const DenseMat &m)
{
  SiconosMatrixException::selfThrow("BlockMatrix:operator = DenseMat - Not yet implemented.");
  return *this;
}

//=================================
// Op. and assignment (+=, -= ... )
//=================================

BlockMatrix& BlockMatrix::operator += (const SiconosMatrix &m)
{
  if (&m == this)
  {
    BlocksMat::iterator1 it1;
    BlocksMat::iterator2 it2;
    for (it1 = mat->begin1(); it1 != mat->end1(); ++it1)
    {
      for (it2 = it1.begin(); it2 != it1.end(); ++it2)
      {
        **it2 += **it2;
      }
    }
    return *this;
  }

  if (m.size(0) != dimRow || m.size(1) != dimCol)
    SiconosMatrixException::selfThrow("BlockMatrix::operator += Left and Right values have inconsistent sizes.");

  if (m.isBlock())
  {
    if (isComparableTo(&m, this))
    {
      // iterators through this
      BlocksMat::iterator1 it1;
      BlocksMat::iterator2 it2;
      // iterators through m
      BlocksMat::const_iterator1 itM1 = m.begin();
      BlocksMat::const_iterator2 itM2;

      for (it1 = mat->begin1(); it1 != mat->end1(); ++it1)
      {
        itM2 = itM1.begin();
        for (it2 = it1.begin(); it2 != it1.end(); ++it2)
        {
          (**it2) += (**itM2);
          itM2++; // increment column pos. in m.
        }
        itM1++; // increment row pos. in m.
      }
    }
    else
    {
      for (unsigned int i = 0; i < dimRow; ++i)
        for (unsigned int j = 0; j < dimCol; ++j)
          (*this)(i, j) += m(i, j);
    }
  }
  else // if m is a SimpleMatrix
  {
    unsigned int indRow = 0, indCol = 0;
    addSimple(indRow, indCol, m); // a sub-block of m is added to each block of this.
  }
  return *this;
}

BlockMatrix& BlockMatrix::operator -= (const SiconosMatrix &m)
{
  if (&m == this)
  {
    BlocksMat::iterator1 it1;
    BlocksMat::iterator2 it2;
    for (it1 = mat->begin1(); it1 != mat->end1(); ++it1)
    {
      for (it2 = it1.begin(); it2 != it1.end(); ++it2)
      {
        **it2 -= **it2;
      }
    }
    return *this;
  }

  if (m.size(0) != dimRow || m.size(1) != dimCol)
    SiconosMatrixException::selfThrow("BlockMatrix::operator += Left and Right values have inconsistent sizes.");

  if (m.isBlock())
  {
    if (isComparableTo(&m, this))
    {
      // iterators through this
      BlocksMat::iterator1 it1;
      BlocksMat::iterator2 it2;
      // iterators through m
      BlocksMat::const_iterator1 itM1 = m.begin();
      BlocksMat::const_iterator2 itM2;

      for (it1 = mat->begin1(); it1 != mat->end1(); ++it1)
      {
        itM2 = itM1.begin();
        for (it2 = it1.begin(); it2 != it1.end(); ++it2)
        {
          (**it2) -= (**itM2);
          itM2++; // increment column pos. in m.
        }
        itM1++; // increment row pos. in m.
      }
    }
    else
    {
      for (unsigned int i = 0; i < dimRow; ++i)
        for (unsigned int j = 0; j < dimCol; ++j)
          (*this)(i, j) -= m(i, j);
    }
  }
  else // if m is a SimpleMatrix
  {
    unsigned int indRow = 0, indCol = 0;
    subSimple(indRow, indCol, m); // a sub-block of m is subtracted to each block of this.
  }
  return *this;
}

void BlockMatrix::trans()
{
  SiconosMatrixException::selfThrow("BlockMatrix::trans(): not yet implemented.");
}

void BlockMatrix::trans(const SiconosMatrix &m)
{
  SiconosMatrixException::selfThrow("BlockMatrix::trans(M): not yet implemented.");
}

void BlockMatrix::PLUFactorizationInPlace()
{
  SiconosMatrixException::selfThrow(" BlockMatrix::PLUFactorizationInPlace: not yet implemented for Block Matrices.");
}

void BlockMatrix::PLUInverseInPlace()
{
  SiconosMatrixException::selfThrow(" BlockMatrix::PLUInverseInPlace: not yet implemented for Block Matrices.");
}

void BlockMatrix::PLUForwardBackwardInPlace(SiconosMatrix &B)
{
  SiconosMatrixException::selfThrow(" BlockMatrix::PLUForwardBackwardInPlace: not yet implemented for Block Matrices.");
}

void BlockMatrix::PLUForwardBackwardInPlace(SiconosVector &B)
{
  SiconosMatrixException::selfThrow(" BlockMatrix::PLUForwardBackwardInPlace: not yet implemented for Block Matrices.");
}
