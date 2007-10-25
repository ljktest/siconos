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

#include "BlockVector.h"
#include "SimpleVector.h"
#include <boost/numeric/ublas/vector_proxy.hpp>  // for project
#include <vector>

// =================================================
//                CONSTRUCTORS
// =================================================
BlockVector::BlockVector(): SiconosVector(0), tabIndex(NULL), isBlockAllocatedIn(NULL)
{
  tabIndex = new Index();
  isBlockAllocatedIn = new std::vector<bool> ();
}

BlockVector::BlockVector(const std::string & file, bool ascii): SiconosVector(0), tabIndex(NULL), isBlockAllocatedIn(NULL)
{
  SiconosVectorException::selfThrow(" BlockVector::constructor from a file : read BlockVector is not implemented");
}

BlockVector::BlockVector(const BlockVector &v): SiconosVector(0), tabIndex(NULL), isBlockAllocatedIn(NULL)
{
  unsigned int nbBlocks = v.getNumberOfBlocks();
  tabIndex = new Index();
  tabIndex->reserve(nbBlocks);

  isBlockAllocatedIn = new std::vector<bool> ();
  isBlockAllocatedIn->reserve(nbBlocks);

  vect.reserve(nbBlocks);
  ConstBlockVectIterator it;
  for (it = v.begin(); it != v.end(); ++it)
  {
    if (!(*it)->isBlock())  // Call copy-constructor of SimpleVector
      vect.push_back(new SimpleVector(**it)) ;
    else
      vect.push_back(new BlockVector(**it)) ;
    isBlockAllocatedIn->push_back(true);
    sizeV += (*it)->size();
    tabIndex->push_back(sizeV);
  }
}

BlockVector::BlockVector(const SiconosVector &v): SiconosVector(0), tabIndex(NULL), isBlockAllocatedIn(NULL)
{
  unsigned int nbBlocks = v.getNumberOfBlocks();
  tabIndex = new Index();
  isBlockAllocatedIn = new std::vector<bool> ();
  tabIndex->reserve(nbBlocks);
  vect.reserve(nbBlocks);
  isBlockAllocatedIn->reserve(nbBlocks);
  if (v.isBlock())
  {
    ConstBlockVectIterator it;
    for (it = v.begin(); it != v.end(); ++it)
    {
      if (!(*it)->isBlock())  // Call copy-constructor of SimpleVector
        vect.push_back(new SimpleVector(**it)) ;
      else
        vect.push_back(new BlockVector(**it)) ;
      isBlockAllocatedIn->push_back(true);
      sizeV += (*it)->size();
      tabIndex->push_back(sizeV);
    }
  }
  else
  {
    // Call copy-constructor of SimpleVector
    vect.push_back(new SimpleVector(v));
    isBlockAllocatedIn->push_back(true);
    sizeV = v.size();
    tabIndex->push_back(sizeV);
  }
}

BlockVector::BlockVector(SiconosVector* v1, SiconosVector* v2): SiconosVector(0), tabIndex(NULL), isBlockAllocatedIn(NULL)
{
  // Insert the two vectors in the container
  // NO COPY !!
  if ((v1 == NULL) && (v2 == NULL))
    SiconosVectorException::selfThrow("BlockVector:constructor(SimpleVector*,SimpleVector*), both vectors are NULL.");

  tabIndex = new Index();
  isBlockAllocatedIn = new std::vector<bool> ();
  tabIndex->reserve(2);
  vect.reserve(2);
  isBlockAllocatedIn->reserve(2);

  if (v1 != NULL)
  {
    vect.push_back(v1);
    sizeV = v1->size();
    tabIndex->push_back(sizeV);
    isBlockAllocatedIn->push_back(false);
  }
  else
    // If first parameter is a NULL pointer, then set this(1) to a SimpleVector of the same size as v2, and equal to 0.
  {
    // This case is usefull to set xDot in LagrangianDS.
    sizeV = v2->size();
    vect.push_back(new SimpleVector(sizeV));
    tabIndex->push_back(sizeV);
    isBlockAllocatedIn->push_back(true);
  }
  if (v2 != NULL)
  {
    vect.push_back(v2);
    sizeV += v2->size();
    tabIndex->push_back(sizeV);
    isBlockAllocatedIn->push_back(false);
  }
  else // If second parameter is a NULL pointer, then set this(2) to a SimpleVector of the same size as v1, and equal to 0.
  {
    // This case is usefull to set xDot in LagrangianDS.
    vect.push_back(new SimpleVector(v1->size()));
    sizeV += v1->size();
    tabIndex->push_back(sizeV);
    isBlockAllocatedIn->push_back(true);
  }
}

BlockVector::BlockVector(unsigned int numberOfBlocks, unsigned int dim): SiconosVector(0), tabIndex(NULL), isBlockAllocatedIn(NULL)
{
  tabIndex = new Index();
  isBlockAllocatedIn = new std::vector<bool> ();
  tabIndex->reserve(numberOfBlocks);
  vect.reserve(numberOfBlocks);
  isBlockAllocatedIn->reserve(numberOfBlocks);
  for (unsigned int i = 0; i < numberOfBlocks; ++i)
  {
    vect.push_back(new SimpleVector(dim));
    tabIndex->push_back(dim * (i + 1));
    isBlockAllocatedIn->push_back(true);
  }
  sizeV = dim * numberOfBlocks;
}

BlockVector::~BlockVector()
{
  purge(vect, *isBlockAllocatedIn);

  tabIndex->clear();
  delete tabIndex;
  isBlockAllocatedIn->clear();
  delete isBlockAllocatedIn;
}

// =================================================
//        get Ublas component (dense or sparse)
// =================================================

const DenseVect BlockVector::getDense(unsigned int i) const
{
  if (vect[i]->getNum() != 1)
    SiconosVectorException::selfThrow("BlockVector::getDense(unsigned int num) : the vector[num] is not a Dense vector");

  return (vect[i])->getDense();
}

const SparseVect BlockVector::getSparse(unsigned int i)const
{
  if (vect[i]->getNum() != 4)
    SiconosVectorException::selfThrow("BlockVector::getSparse(unsigned int num) : the vector[num] is not a Sparse vector");
  return (vect[i])->getSparse();
}

DenseVect* BlockVector::getDensePtr(unsigned int i) const
{
  if (vect[i]->getNum() != 1)
    SiconosVectorException::selfThrow("BlockVector::getDensePtr(unsigned int num) : the vector[num] is not a Dense vector");
  return (vect[i])->getDensePtr();
}

SparseVect* BlockVector::getSparsePtr(unsigned int i) const
{
  if (vect[i]->getNum() != 4)
    SiconosVectorException::selfThrow("BlockVector::getSparsePtr(unsigned int num) : the vector[num] is not a Sparse vector");
  return (vect[i])->getSparsePtr();
}

double* BlockVector::getArray(unsigned int i) const
{
  if (vect[i]->isBlock())
    SiconosVectorException::selfThrow("BlockVector::getArray(unsigned int num) : the vector[num] is a Block vector");
  return (vect[i])->getArray();
}

// ===========================
//       fill vector
// ===========================

void BlockVector::zero()
{
  BlockVectIterator it;
  for (it = vect.begin(); it != vect.end(); ++it)
    (*it)->zero();
}

void BlockVector::fill(double value)
{
  BlockVectIterator it;
  for (it = vect.begin(); it != vect.end(); ++it)
    if ((*it) != NULL)(*it)->fill(value);
}

//=======================
// set vector dimension
//=======================

void BlockVector::resize(unsigned int, bool)
{
  SiconosVectorException::selfThrow("BlockVector::resize, not allowed for block vectors.");
}

//=======================
//       get norm
//=======================

const double BlockVector::normInf() const
{
  double d = 0, tmp;
  ConstBlockVectIterator it;
  for (it = vect.begin(); it != vect.end(); ++it)
  {
    tmp = (*it)->normInf();
    if (tmp > d) d = tmp;
  }
  return d;
}

const double BlockVector::norm2() const
{
  double d = 0;
  ConstBlockVectIterator it;
  for (it = vect.begin(); it != vect.end(); ++it)
  {
    if ((*it) != NULL)
      d += pow((*it)->norm2(), 2);

    else
      SiconosVectorException::selfThrow("BlockVector::norm, one of the blocks is equal to NULL pointer.");
  }
  return sqrt(d);
}

//=====================
// screen display
//=====================

void BlockVector::display() const
{
  ConstBlockVectIterator it;
  std::cout << "=======> Block Vector Display (" << tabIndex->size() << " block(s)): " << std::endl;
  for (it = vect.begin(); it != vect.end(); ++it)
    (*it)->display();
}

//============================
// Convert vector to a string
//============================

const std::string BlockVector::toString() const
{
  SiconosVectorException::selfThrow("BlockVector::toString, not yet implemented.");
  return "BlockVector";
}

//=============================
// Elements access (get or set)
//=============================

const double BlockVector::getValue(unsigned int pos) const
{
  unsigned int blockNum = 0;

  while (pos >= (*tabIndex)[blockNum] && blockNum < tabIndex->size())
    blockNum ++;

  unsigned int relativePos = pos;

  if (blockNum != 0)
    relativePos -= (*tabIndex)[blockNum - 1];

  return (*vect[blockNum])(relativePos);
}

void BlockVector::setValue(unsigned int pos, double value)
{
  unsigned int blockNum = 0;

  while (pos >= (*tabIndex)[blockNum] && blockNum < tabIndex->size())
    blockNum ++;

  unsigned int relativePos = pos;

  if (blockNum != 0)
    relativePos -= (*tabIndex)[blockNum - 1];

  (*vect[blockNum])(relativePos) = value;
}

double& BlockVector::operator()(unsigned int pos)
{
  unsigned int blockNum = 0;

  while (pos >= (*tabIndex)[blockNum] && blockNum < tabIndex->size())
    blockNum ++;

  unsigned int relativePos = pos;

  if (blockNum != 0)
    relativePos -= (*tabIndex)[blockNum - 1];

  return (*vect[blockNum])(relativePos);
}

const double BlockVector::operator()(unsigned int pos) const
{
  unsigned int blockNum = 0;

  while (pos >= (*tabIndex)[blockNum] && blockNum < tabIndex->size())
    blockNum ++;
  unsigned int relativePos = pos;

  if (blockNum != 0)
    relativePos -= (*tabIndex)[blockNum - 1];

  return (*vect[blockNum])(relativePos);
}

//============================================
// Access (get or set) to blocks of elements
//============================================

SimpleVector BlockVector::getVector(unsigned int pos) const
{

  if (vect[pos] == NULL)
    SiconosVectorException::selfThrow("BlockVector::getVector(pos), vector[pos] == NULL pointer.");

  if (vect[pos]->isBlock())
    SiconosVectorException::selfThrow("BlockVector::getVector(pos), vector[pos] is a Block. Use getVectorPtr()");

  return *(vect[pos]);
}

void BlockVector::setVector(unsigned int pos, const SiconosVector& newV)
{
  if (vect[pos] == NULL)
    SiconosVectorException::selfThrow("BlockVector::setVector(pos,v), this[pos] == NULL pointer.");

  if (newV.size() != (vect[pos])->size())
    SiconosVectorException::selfThrow("BlockVector::setVector(pos,v), this[pos] and v have unconsistent sizes.");

  *vect[pos] = newV ;
}

void BlockVector::setVectorPtr(unsigned int pos, SiconosVector* newV)
{
  if (newV->size() != (vect[pos])->size())
    SiconosVectorException::selfThrow("BlockVector::setVectorPtr(pos,v), this[pos] and v have unconsistent sizes.");

  if ((*isBlockAllocatedIn)[pos]) delete vect[pos];
  vect[pos] = newV;
  (*isBlockAllocatedIn)[pos] = false;
}

SiconosVector* BlockVector::operator [](unsigned int pos)
{
  return  vect[pos];
}

const SiconosVector* BlockVector::operator [](unsigned int pos) const
{
  return  vect[pos];
}

unsigned int BlockVector::getNumVectorAtPos(unsigned int pos) const
{
  unsigned int blockNum = 0;

  while (pos >= (*tabIndex)[blockNum] && blockNum < tabIndex->size() - 1)
    blockNum ++;
  return blockNum;
}

void BlockVector::addSimple(unsigned int& index, const SiconosVector& vIn)
{
  // Add a part of vIn (starting from index) to the current vector.
  // vIn must be a SimpleVector.

  // At the end of the present function, index is equal to index + the dim. of the added sub-vector.

  unsigned int dim = vIn.size() - index; // size of the block to be added.
  if (dim > sizeV) SiconosVectorException::selfThrow("BlockVector::addSimple : invalid ranges");

  ConstBlockVectIterator it;
  unsigned int numVIn = vIn.getNum();
  unsigned int currentSize, currentNum;

  for (it = vect.begin(); it != vect.end(); ++it)
  {
    if ((*it)->isBlock())
      (static_cast<BlockVector*>(*it))->addSimple(index, vIn);

    else // the current block is a SimpleVector
    {
      currentSize = (*it)->size();
      currentNum = (*it)->getNum();
      if (numVIn != currentNum) SiconosVectorException::selfThrow("BlockVector::addSimple : inconsistent types.");
      if (numVIn == 1)
        noalias(*(*it)->getDensePtr()) +=  ublas::subrange(*vIn.getDensePtr(), index, index + currentSize) ;
      else
        noalias(*(*it)->getSparsePtr()) +=  ublas::subrange(*vIn.getSparsePtr(), index, index + currentSize) ;
      index += currentSize;
    }
  }
}

void BlockVector::subSimple(unsigned int& index, const SiconosVector& vIn)
{
  // subtract a part of vIn (starting from index) to the current vector.
  // vIn must be a SimpleVector.

  // At the end of the present function, index is equal to index + the dim. of the added sub-vector.

  unsigned int dim = vIn.size() - index; // size of the block to be added.
  if (dim > sizeV) SiconosVectorException::selfThrow("BlockVector::addSimple : invalid ranges");

  ConstBlockVectIterator it;
  unsigned int numVIn = vIn.getNum();
  unsigned int currentSize, currentNum;

  for (it = vect.begin(); it != vect.end(); ++it)
  {
    if ((*it)->isBlock())
      (static_cast<BlockVector*>(*it))->subSimple(index, vIn);

    else // the current block is a SimpleVector
    {
      currentSize = (*it)->size();
      currentNum = (*it)->getNum();
      if (numVIn != currentNum) SiconosVectorException::selfThrow("BlockVector::addSimple : inconsistent types.");
      if (numVIn == 1)
        noalias(*(*it)->getDensePtr()) -=  ublas::subrange(*vIn.getDensePtr(), index, index + currentSize) ;
      else
        noalias(*(*it)->getSparsePtr()) -=  ublas::subrange(*vIn.getSparsePtr(), index, index + currentSize) ;
      index += currentSize;
    }
  }
}

//===============
//  Assignment
//===============

BlockVector& BlockVector::operator = (const SiconosVector& vIn)
{
  if (&vIn == this) return *this;

  if (vIn.size() != sizeV)
    SiconosVectorException::selfThrow("BlockVector:operator = vIn, inconsistent size between this and vIn.");

  if (vIn.isBlock())
  {
    if (isComparableTo(this, &vIn)) // if vIn and this are "block-consistent"
    {
      BlockVectIterator it1;
      ConstBlockVectIterator it2 = vIn.begin();

      for (it1 = vect.begin(); it1 != vect.end(); ++it1)
      {
        (**it1) = (**it2);
        it2++;
      }
    }
    else // use of a temporary SimpleVector... bad way, to be improved. But this case happens rarely ...
    {
      //    std::cout << "WARNING: BlockVector::operator = BlockVector, v=w, with v and w having blocks of different sizes. This operation may be time-consuming and inappropriate." << std::endl;
      //    SiconosVector * tmp = new SimpleVector(vIn);
      //    *this = *tmp;
      //    delete tmp;

      // component by component copy ...
      for (unsigned int i = 0; i < sizeV; ++i)
        (*this)(i) = vIn(i);

    }
  }
  else // if vIn is a SimpleVector
  {
    setBlock(&vIn, this, sizeV, 0, 0);
    //       unsigned int pos = 0;
    //       BlockVectIterator it1;
    //       for(it1=vect.begin(); it1!=vect.end();++it1)
    //  {

    //    vIn.getBlock(pos,*it1);
    //    pos += (*it1)->size();
    //  }
  }
  return *this;
}

BlockVector& BlockVector::operator = (const BlockVector& vIn)
{
  if (&vIn == this) return *this;

  if (vIn.size() != sizeV)
    SiconosVectorException::selfThrow("BlockVector:operator = vIn, inconsistent size between this and vIn.");

  if (isComparableTo(this, &vIn)) // if vIn and this are "block-consistent"
  {
    BlockVectIterator it1;
    ConstBlockVectIterator it2 = vIn.begin();

    for (it1 = vect.begin(); it1 != vect.end(); ++it1)
    {
      **it1 = **it2;
      it2++;
    }
  }
  else // use of a temporary SimpleVector... bad way, to be improved. But this case happens rarely ...
  {
    //       std::cout << "WARNING: BlockVector::operator = BlockVector, v=w, with v and w having blocks of different sizes. This operation may be time-consuming and inappropriate." << std::endl;
    //       SiconosVector * tmp = new SimpleVector(vIn);
    //       *this = *tmp;
    //       delete tmp;

    // component by component copy ...
    for (unsigned int i = 0; i < sizeV; ++i)
      (*this)(i) = vIn(i);

  }
  return *this;
}

BlockVector& BlockVector::operator =(const DenseVect&)
{
  SiconosVectorException::selfThrow("BlockVector:operator = DenseVect - Not implemented.");
  return *this;
}

BlockVector& BlockVector::operator =(const SparseVect&)
{
  SiconosVectorException::selfThrow("BlockVector:operator = SparseVect - Not implemented.");
  return *this;
}

//=================================
// Op. and assignment (+=, -= ... )
//=================================

BlockVector& BlockVector::operator += (const SiconosVector& vIn)
{
  if (&vIn == this)
  {
    BlockVectIterator it1;
    for (it1 = vect.begin(); it1 != vect.end(); ++it1)
    {
      **it1 += **it1;
    }
    return *this;
  }

  if (vIn.size() != sizeV)
    SiconosVectorException::selfThrow("BlockVector:operator += vIn, inconsistent size between this and vIn.");

  if (vIn.isBlock())
  {
    if (isComparableTo(this, &vIn)) // if vIn and this are "block-consistent"
    {
      BlockVectIterator it1;
      ConstBlockVectIterator it2 = vIn.begin();

      for (it1 = vect.begin(); it1 != vect.end(); ++it1)
      {
        **it1 += **it2;
        it2++;
      }
    }
    else // use of a temporary SimpleVector... bad way, to be improved. But this case happens rarely ...
    {
      //      SiconosVector * tmp = new SimpleVector(vIn);
      //      *this += *tmp;
      //      delete tmp;
      for (unsigned int i = 0; i < sizeV; ++i)
        (*this)(i) += vIn(i);
    }
  }
  else // if vIn is a Simple
  {
    unsigned int index = 0;
    addSimple(index, vIn);
  }
  // a call to a subfunction is required since the current vector
  // may be a Block of Block (...of Blocks...)

  return *this;
}

BlockVector& BlockVector::operator -= (const SiconosVector& vIn)
{
  if (&vIn == this)
  {
    this->zero();
    return *this;
  }
  if (vIn.size() != sizeV)
    SiconosVectorException::selfThrow("BlockVector:operator -= vIn, inconsistent size between this and vIn.");
  if (vIn.isBlock())
  {
    if (isComparableTo(this, &vIn)) // if vIn and this are "block-consistent"
    {
      unsigned int i = 0;
      BlockVectIterator it1;

      for (it1 = vect.begin(); it1 != vect.end(); ++it1)
        **it1 -= *(vIn[i++]);
    }
    else // use of a temporary SimpleVector... bad way, to be improved. But this case happens rarely ...
    {
      //    SiconosVector * tmp = new SimpleVector(vIn);
      //    *this -= *tmp;
      //    delete tmp;
      for (unsigned int i = 0; i < sizeV; ++i)
        (*this)(i) -= vIn(i);
    }
  }
  else // if vIn is a Simple
  {
    unsigned int index = 0;
    subSimple(index, vIn);
  }
  return *this;
}

void BlockVector::insert(const  SiconosVector& v)
{
  sizeV += v.size();

  if (!v.isBlock())
    vect.push_back(new SimpleVector(v)); // Copy
  else
    vect.push_back(new BlockVector(v)); // Copy

  isBlockAllocatedIn->push_back(true);
  tabIndex->push_back(sizeV);
}

void BlockVector::insertPtr(SiconosVector* v)
{
  if (v == NULL)
    SiconosVectorException::selfThrow("BlockVector:insertPtr(v), v is a NULL vector.");

  sizeV += v->size();
  vect.push_back(v);
  isBlockAllocatedIn->push_back(false);
  tabIndex->push_back(sizeV);
}
