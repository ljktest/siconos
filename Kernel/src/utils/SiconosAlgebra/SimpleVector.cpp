/* Siconos-Kernel version 2.0.1, Copyright INRIA 2005-2006.
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

#include "SimpleVector.h"
#include "SimpleMatrix.h"
#include "ioVector.h"
#include "SiconosVectorException.h"
#include <boost/numeric/ublas/io.hpp>            // for >> 
#include <boost/numeric/ublas/vector_proxy.hpp>  // for project
#include "boost/numeric/bindings/atlas/cblas1.hpp"

namespace atlas = boost::numeric::bindings::atlas;

// parameters: dimension and type.
SimpleVector::SimpleVector(unsigned int row, TYP typ): SiconosVector(false, row), num(1)
{
  if (typ == SPARSE)
  {
    vect.Sparse = new SparseVect(row);
    num = 4;
  }
  else if (typ == DENSE)
  {
    vect.Dense = new DenseVect(row);
    // num = 1; // set by default
  }
  else
  {
    SiconosVectorException::selfThrow("SimpleVector::constructor(TYP, unsigned int) failed, invalid type given");
  }
}

// parameters: dimension, default value for all components and type.
SimpleVector::SimpleVector(unsigned int row, double val, TYP typ): SiconosVector(false, row), num(1)
{
  if (typ == SPARSE)
  {
    vect.Sparse = new SparseVect(row);
    num = 4;
  }
  else if (typ == DENSE)
  {
    vect.Dense = new DenseVect(row);
    // num = 1; // set by default
  }
  else
  {
    SiconosVectorException::selfThrow("SimpleVector::constructor(TYP, unsigned int) : invalid type given");
  }
  fill(val);
}

// parameters: a vector (stl) of double and the type.
SimpleVector::SimpleVector(const std::vector<double> v, TYP typ): SiconosVector(false, v.size()), num(1)
{
  if (typ != DENSE)
    SiconosVectorException::selfThrow("SimpleVector::constructor(TYP, std::vector<double>, unsigned int) : invalid type given");

  vect.Dense = new DenseVect(sizeV, v);
}

// Copy
SimpleVector::SimpleVector(const SimpleVector &svect): SiconosVector(false, svect.size()), num(svect.getNum())
{
  if (num == 1)
    vect.Dense = new DenseVect(*svect.getDensePtr());

  else if (num == 4)
    vect.Sparse = new SparseVect(*svect.getSparsePtr());

  else
    SiconosVectorException::selfThrow("SimpleVector:constructor(const SimpleVector) : invalid type given");
}

// Copy
SimpleVector::SimpleVector(const SiconosVector &v): SiconosVector(false, v.size()), num(1)
{
  if (v.isBlock())
  {
    vect.Dense = new DenseVect(sizeV);
    for (unsigned int i = 0; i < sizeV; ++i)
      (*vect.Dense)(i) = v(i);
  }
  else
  {
    num = v.getNum();
    if (num == 1)
      vect.Dense = new DenseVect(*v.getDensePtr());

    else if (num == 4)
      vect.Sparse = new SparseVect(*v.getSparsePtr());

    else
      SiconosVectorException::selfThrow("SimpleVector:constructor(const SiconosVector) : invalid type given");
  }
}

SimpleVector::SimpleVector(const DenseVect& m): SiconosVector(false, m.size()), num(1)
{
  vect.Dense = new DenseVect(m);
}

SimpleVector::SimpleVector(const SparseVect& m): SiconosVector(false, m.size()), num(4)
{
  vect.Sparse = new SparseVect(m);
}

SimpleVector::SimpleVector(const std::string &file, bool ascii): SiconosVector(false), num(1)
{
  vect.Dense = new DenseVect();
  if (ascii)
  {
    ioVector io(file, "ascii");
    io.read(*this);
  }
  else
  {
    ioVector io(file, "binary");
    io.read(*this);
  }
  sizeV = (vect.Dense)->size();
}

SimpleVector::~SimpleVector(void)
{
  if (num == 1)
    delete(vect.Dense);
  else if (num == 4)
    delete(vect.Sparse);
}

const DenseVect SimpleVector::getDense(unsigned int)const
{

  if (num != 1)
    SiconosVectorException::selfThrow("SimpleVector::getDense(unsigned int row, unsigned int col) : the current vector is not a Dense vector");

  return *vect.Dense;
}

const SparseVect SimpleVector::getSparse(unsigned int)const
{

  if (num != 4)
    SiconosVectorException::selfThrow("SimpleVector::getSparse(unsigned int row, unsigned int col) : the current vector is not a Sparse vector");

  return *vect.Sparse;
}

DenseVect* SimpleVector::getDensePtr(unsigned int) const
{
  if (num != 1)
    SiconosVectorException::selfThrow("SimpleVector::getDensePtr(unsigned int row, unsigned int col) : the current vector is not a Dense vector");

  return vect.Dense;
}

SparseVect* SimpleVector::getSparsePtr(unsigned int)const
{

  if (num != 4)
    SiconosVectorException::selfThrow("SimpleVector::getSparsePtr(unsigned int row, unsigned int col) : the current vector is not a Sparse vector");

  return vect.Sparse;
}

double* SimpleVector::getArray(unsigned int) const
{
  if (num == 4)
    SiconosVectorException::selfThrow("SimpleVector::getArray() : not yet implemented for sparse vector.");

  return &(((*vect.Dense).data())[0]);
}

void SimpleVector::getBlock(unsigned int pos, SiconosVector& vOut) const
{
  unsigned int end = vOut.size();
  if ((pos + end) > sizeV)
    SiconosVectorException::selfThrow("SimpleVector::getBlock(pos,vOut): vOut size+pos is out of range.");

  if (vOut.isBlock())
    SiconosVectorException::selfThrow("SimpleVector::getBlock(pos,vOut) failed: not yet implemented for vOut being a BlockVector.");

  if (num == 1)
    *(vOut.getDensePtr()) = project((*vect.Dense), ublas::range(pos, pos + end));
  else
    *(vOut.getSparsePtr()) = project((*vect.Sparse), ublas::range(pos, pos + end));
}

void SimpleVector::zero()
{
  if (num == 1)
    atlas::set(0.0, *vect.Dense);

  else //if(num==4)
    *vect.Sparse *= 0.0;
}

void SimpleVector::resize(unsigned int n, bool preserve)
{
  sizeV = n;
  if (num == 1)
    (vect.Dense)->resize(n, preserve);
  if (num == 4)
    (vect.Sparse)->resize(n, preserve);
}

const double SimpleVector::normInf() const
{
  if (num == 1)
    return norm_inf(*vect.Dense);
  else //if(num==4)
    return norm_inf(*vect.Sparse);
}


const double SimpleVector::norm2() const
{
  if (num == 1)
    return ublas::norm_2(*vect.Dense);
  else //if(num==4)
    return ublas::norm_2(*vect.Sparse);
}

void SimpleVector::display()const
{
  std::cout << "vect: " ;
  if (num == 1)
    std::cout << *vect.Dense << std::endl;
  else if (num == 4)
    std::cout << *vect.Sparse << std::endl;
}

void SimpleVector::setVector(unsigned int, const SiconosVector& newV)
{
  if (newV.size() != sizeV)
    SiconosVectorException::selfThrow("SimpleVector::setVector(num,v), unconsistent sizes.");

  *this = newV ;
}

void SimpleVector::setVectorPtr(unsigned int, SiconosVector*)
{
  SiconosVectorException::selfThrow("SimpleVector::setVectorPtr(num,v), not allowed for SimpleVector.");
}

void SimpleVector::fill(double value)
{
  if (num == 4)
  {
    for (unsigned int i = 0; i < (vect.Sparse)->size(); ++i)
      (vect.Sparse)->push_back(i, value);
  }
  else
    atlas::set(value, *vect.Dense);
}

const std::string SimpleVector::toString() const
{
  std::stringstream sstr;
  std::string s;
  if (num == 1)
    sstr << *vect.Dense;
  else
    sstr << *vect.Sparse;
  sstr >> s;
  s = s.substr(4, s.size() - 5); // Remove "[size](" at the beginning of the string
  std::string::size_type pos;
  while ((pos = s.find(",")) != std::string::npos) // Replace "," by " " in the string
    s[pos] = ' ';
  return s;
}

const double SimpleVector::getValue(unsigned int row)
{
  if (num == 1)
    return (*vect.Dense)(row);
  else
    return (*vect.Sparse)(row);
}

void SimpleVector::setValue(unsigned int row, double value)
{
  if (num == 1)
    (*vect.Dense)(row) = value ;
  else
    (*vect.Sparse)(row) = value;
}

void SimpleVector::setBlock(unsigned int index, const SiconosVector& m)
{
  if (num != 1) SiconosVectorException::selfThrow("SimpleVector::setBlock : vector should be dense");
  if ((index + m.size()) > size()) SiconosVectorException::selfThrow("SimpleVector::setBlock : invalid ranges");

  if (!m.isBlock()) // m a SimpleVector ...
  {
    if (m.getNum() != 1) SiconosVectorException::selfThrow("SimpleVector::setBlock : vectors should be dense");
    ublas::subrange(*vect.Dense, index, index + m.size()) = *m.getDensePtr();
  }
  else // m a BlockVector
  {
    ConstBlockVectIterator it;
    unsigned int pos = index;
    for (it = m.begin(); it != m.end(); ++it)
    {
      setBlock(pos, **it);
      pos += (*it)->size();
    }
  }
}

void SimpleVector::addBlock(unsigned int index, const SiconosVector& m)
{
  if (num != 1) SiconosVectorException::selfThrow("SimpleVector::addBlock : vector should be dense");
  if ((index + m.size()) > size()) SiconosVectorException::selfThrow("SimpleVector::addBlock : invalid ranges");

  if (!m.isBlock()) // m a SimpleVector ...
  {
    if (m.getNum() != 1) SiconosVectorException::selfThrow("SimpleVector::addBlock : vectors should be dense");
    ublas::subrange(*vect.Dense, index, index + m.size()) += *m.getDensePtr();
  }
  else // m a BlockVector
  {
    ConstBlockVectIterator it;
    unsigned int pos = index;
    for (it = m.begin(); it != m.end(); ++it)
    {
      addBlock(pos, **it);
      pos += (*it)->size();
    }
  }
}

void SimpleVector::subBlock(unsigned int index, const SiconosVector& m)
{
  if (num != 1) SiconosVectorException::selfThrow("SimpleVector::subBlock : vector should be dense");
  if ((index + m.size()) > size()) SiconosVectorException::selfThrow("SimpleVector::subBlock : invalid ranges");

  if (!m.isBlock()) // m a SimpleVector ...
  {
    if (m.getNum() != 1) SiconosVectorException::selfThrow("SimpleVector::subBlock : vectors should be dense");
    ublas::subrange(*vect.Dense, index, index + m.size()) -= *m.getDensePtr();
  }
  else // m a BlockVector
  {
    ConstBlockVectIterator it;
    unsigned int pos = index;
    for (it = m.begin(); it != m.end(); ++it)
    {
      subBlock(pos, **it);
      pos += (*it)->size();
    }
  }
}

double& SimpleVector::operator()(unsigned int row)
{
  if (num == 1)
    return (*vect.Dense)(row);
  else
    return (*vect.Sparse)(row).ref();
}

const double SimpleVector::operator()(unsigned int row)const
{
  double d = 0;
  switch (num)
  {
  case 1:
    d = (*vect.Dense)(row);
    break;
  case 2:
    d = ((*vect.Sparse)(row)).ref();
    break;
  default:
    SiconosVectorException::selfThrow("operator() (unsigned int) : invalid type given");
    break;
  }
  return d;
}

SimpleVector& SimpleVector::operator = (const SiconosVector& m)
{
  if (!m.isBlock())
  {
    switch (num)
    {
    case 1:
      switch (m.getNum())
      {
      case 1:
        atlas::copy(*m.getDensePtr(), *vect.Dense);
        break;
      case 4:
        *vect.Dense = *m.getSparsePtr();
        break;
      default:
        SiconosVectorException::selfThrow("SimpleVector::operator = : invalid type given");
        break;
      }
      break;
    case 4:
      if (m.getNum() == 4)
        *vect.Sparse = *m.getSparsePtr();
      else
        SiconosVectorException::selfThrow("SimpleVector::operator = : can not set sparse = dense.");
      break;
    default:
      SiconosVectorException::selfThrow("SimpleVector::operator = : invalid type given");
      break;
    }
  }
  else // m a BlockVector: element by element copy ...
  {
    if (sizeV != m.size())
      SiconosVectorException::selfThrow("SimpleVector::operator = failed: inconsistent sizes.");

    ConstBlockVectIterator it;
    unsigned int pos = 0;
    for (it = m.begin(); it != m.end(); ++it)
    {
      setBlock(pos, **it);
      pos += (*it)->size();
    }
  }
  return *this;
}

SimpleVector& SimpleVector::operator = (const SimpleVector& m)
{
  switch (num)
  {
  case 1:
    switch (m.getNum())
    {
    case 1:
      atlas::copy(*m.getDensePtr(), *vect.Dense);
      break;
    case 4:
      *vect.Dense = *m.getSparsePtr();
      break;
    default:
      SiconosVectorException::selfThrow("SimpleVector::operator = : invalid type given");
      break;
    }
    break;
  case 4:
    if (m.getNum() == 4)
      *vect.Sparse = *m.getSparsePtr();
    else
      SiconosVectorException::selfThrow("SimpleVector::operator = : can not set sparse = dense.");
    break;
  }
  return *this;
}

SimpleVector& SimpleVector::operator = (const DenseVect& d)
{
  if (num != 1)
    SiconosVectorException::selfThrow("SimpleVector::operator = DenseVect : current vector is not dense.");
  if (d.size() != sizeV)
    SiconosVectorException::selfThrow("SimpleVector::operator = DenseVect : inconsistent size.");

  atlas::copy(d, *vect.Dense);
  return *this;
}

SimpleVector& SimpleVector::operator = (const SparseVect& sp)
{
  if (num != 4)
    SiconosVectorException::selfThrow("SimpleVector::operator = SparseVect : current vector is not sparse.");
  if (sp.size() != sizeV)
    SiconosVectorException::selfThrow("SimpleVector::operator = SparseVect : inconsistent size.");

  *vect.Sparse = sp;

  return *this;
}

bool operator == (const SiconosVector &m, const SiconosVector &x)
{
  assert(m.isBlock() == false && x.isBlock() == false);
  return ((m - x).norm2() < tolerance);
}

SimpleVector& SimpleVector::operator += (const SiconosVector& m)
{
  if (!m.isBlock()) // if m is not a BlockVector
  {
    unsigned int numM = m.getNum();
    switch (num)
    {
    case 1:
      switch (numM)
      {
      case 1:
        *vect.Dense += *m.getDensePtr();
        break;
      case 4:
        *vect.Dense += *m.getSparsePtr();
        break;
      default:
        SiconosVectorException::selfThrow("SimpleVector::operator += : invalid type given");
        break;
      }
      break;
    case 4:
      if (numM == 4)
        *vect.Sparse += *m.getSparsePtr();
      else SiconosVectorException::selfThrow("SimpleVector::operator += : can not add a dense to a sparse.");
      break;
    default:
      SiconosVectorException::selfThrow("SimpleVector::operator += : invalid type given");
      break;
    }
  }
  else // m a BlockVector: element by element copy ...
  {
    if (sizeV != m.size())
      SiconosVectorException::selfThrow("SimpleVector::operator += failed: inconsistent sizes.");

    ConstBlockVectIterator it;
    unsigned int pos = 0;
    for (it = m.begin(); it != m.end(); ++it)
    {
      addBlock(pos, **it);
      pos += (*it)->size();
    }
  }
  return *this;
}

SimpleVector& SimpleVector::operator -= (const SiconosVector& m)
{
  if (!m.isBlock()) // if m is not a BlockVector
  {
    unsigned int numM = m.getNum();
    switch (num)
    {
    case 1:
      switch (numM)
      {
      case 1:
        *vect.Dense -= *m.getDensePtr();
        break;
      case 4:
        *vect.Dense -= *m.getSparsePtr();
        break;
      default:
        SiconosVectorException::selfThrow("SimpleVector::operator -= : invalid type given");
        break;
      }
      break;
    case 4:
      if (numM == 4)
        *vect.Sparse -= *m.getSparsePtr();
      else SiconosVectorException::selfThrow("SimpleVector::operator -= : can not sub a dense to a sparse.");
      break;
    default:
      SiconosVectorException::selfThrow("SimpleVector::operator -= : invalid type given");
      break;
    }
  }
  else // m a BlockVector: element by element copy ...
  {
    if (sizeV != m.size())
      SiconosVectorException::selfThrow("SimpleVector::operator -= failed: inconsistent sizes.");

    ConstBlockVectIterator it;
    unsigned int pos = 0;
    for (it = m.begin(); it != m.end(); ++it)
    {
      subBlock(pos, **it);
      pos += (*it)->size();
    }
  }
  return *this;
}

SimpleVector& SimpleVector::operator *= (double m)
{
  switch (num)
  {
  case 1:
    *vect.Dense *= m;
    break;
  case 4:
    *vect.Sparse *= m;
    break;
  default:
    SiconosVectorException::selfThrow("SimpleVector::operator *=  : invalid type given");
    break;
  }
  return *this;
}

SimpleVector& SimpleVector::operator *= (int m)
{
  switch (num)
  {
  case 1:
    *vect.Dense *= m;
    break;
  case 4:
    *vect.Sparse *= m;
    break;
  default:
    SiconosVectorException::selfThrow("SimpleVector::operator *=  : invalid type given");
    break;
  }
  return *this;
}

SimpleVector& SimpleVector::operator /= (double m)
{
  switch (num)
  {
  case 1:
    *vect.Dense /= m;
    break;
  case 4:
    *vect.Sparse /= m;
    break;
  default:
    SiconosVectorException::selfThrow("SimpleVector::operator /=  : invalid type given");
    break;
  }
  return *this;
}

SimpleVector& SimpleVector::operator /= (int m)
{
  switch (num)
  {
  case 1:
    *vect.Dense /= m;
    break;
  case 4:
    *vect.Sparse /= m;
    break;
  default:
    SiconosVectorException::selfThrow("SimpleVector::operator /=  : invalid type given");
    break;
  }
  return *this;
}

const double inner_prod(const SiconosVector &x, const SiconosVector &m)
{
  if (x.isBlock() || m.isBlock())
    SiconosVectorException::selfThrow("inner_prod: not implemented for BlockVectors.");

  if (x.size() != m.size())
    SiconosVectorException::selfThrow("inner_prod: inconsistent sizes");

  unsigned int numM = m.getNum();
  unsigned int numX = x.getNum();
  if (numX == numM)
  {
    if (numM == 1)
      return atlas::dot(*x.getDensePtr(), *m.getDensePtr());
    else
      return inner_prod(*x.getSparsePtr(), *m.getSparsePtr());
  }
  else if (numM == 1)
    return inner_prod(*x.getSparsePtr(), *m.getDensePtr());
  else
    return inner_prod(*x.getDensePtr(), *m.getSparsePtr());
}

SimpleVector operator + (const SimpleVector &x, const SimpleVector &m)
{
  if (x.size() != m.size())
    SiconosVectorException::selfThrow("SimpleVector::Vector addition: inconsistent sizes");

  unsigned int numX = x.getNum();
  unsigned int numM = m.getNum();

  if (numX == numM)
  {
    if (numX == 1)
    {
      DenseVect p = *m.getDensePtr();
      atlas::xpy(*x.getDensePtr(), p);
      return p;
    }
    else
    {
      SparseVect s = *x.getSparsePtr() + *m.getSparsePtr();
      return s;
    }
  }
  else
  {
    DenseVect p;
    if (numX == 1)
    {
      p = *x.getDensePtr() + *m.getSparsePtr();
      return p;
    }
    else
    {
      p = *x.getSparsePtr() + *m.getDensePtr();
      return p;
    }
  }
}

SimpleVector operator - (const SimpleVector &x, const SimpleVector &m)
{
  if (x.size() != m.size())
    SiconosVectorException::selfThrow("SimpleVector::Vector subtraction: inconsistent sizes");

  unsigned int numX = x.getNum();
  unsigned int numM = m.getNum();
  if (numX == numM)
  {
    if (numX == 1)
    {
      // Copy m into p, scal p = -p and call xpy(x,p): p = x+p.
      DenseVect p = *m.getDensePtr();
      p *= -1.0;
      atlas::xpy(*x.getDensePtr(), p);
      return p;
    }
    else
    {
      SparseVect s = *x.getSparsePtr() - *m.getSparsePtr();
      return s;
    }
  }
  else
  {
    DenseVect p;
    if (numX == 1)
    {
      p = *x.getDensePtr() - *m.getSparsePtr();
      return p;
    }
    else
    {
      p = *x.getSparsePtr() - *m.getDensePtr();
      return p;
    }
  }
}

// outer_prod(v,w) = trans(v)*w
SimpleMatrix outer_prod(const SiconosVector &x, const SiconosVector& m)
{
  if (x.isBlock() || m.isBlock())
    SiconosVectorException::selfThrow("outer_prod: not implemented for BlockVectors.");
  unsigned int numM = m.getNum();
  unsigned int numX = x.getNum();
  if (numM == 1)
  {
    if (numX == 1)
    {
      DenseMat p = outer_prod(*x.getDensePtr(), *m.getDensePtr());
      return p;
    }
    else// if(numX == 4)
    {
      DenseMat p = outer_prod(*x.getSparsePtr(), *m.getDensePtr());
      return p;
    }
  }
  else // if(numM == 4)
  {
    if (numX == 1)
    {
      DenseMat p = outer_prod(*x.getDensePtr(), *m.getSparsePtr());
      return p;
    }
    else //if(numX == 4)
    {
      DenseMat p = outer_prod(*x.getSparsePtr(), *m.getSparsePtr());
      return p;
    }
  }
}

SimpleVector operator * (const SimpleVector &m, double d)
{
  if (m.getNum() != 1 && m.getNum() != 4)
    SiconosVectorException::selfThrow("opertor * (const SimpleVector&, double) : invalid type of vector");

  if (m.getNum() == 1)
  {
    // Copy m into p and call atlas::scal(d,p), p = d*p.
    DenseVect p = *m.getDensePtr();
    atlas::scal(d, p);
    return p;
  }
  else// if(m.getNum()==4)
  {
    SparseVect s = *m.getSparsePtr() * d;
    return s;
  }
}

SimpleVector operator * (const SimpleVector &m, int d)
{
  if (m.getNum() != 1 && m.getNum() != 4)
    SiconosVectorException::selfThrow("opertor * (const SimpleVector&, int) : invalid type of vector");

  if (m.getNum() == 1)
  {
    // Copy m into p and call atlas::scal(d,p), p = d*p.
    DenseVect p = *m.getDensePtr();
    atlas::scal((double)d, p);
    return p;
  }
  else // if(m.getNum()==4){
  {
    SparseVect s = *m.getSparsePtr() * d;
    return s;
  }
}

SimpleVector operator * (double d, const SimpleVector &m)
{
  if (m.getNum() != 1 && m.getNum() != 4)
    SiconosVectorException::selfThrow("opertor * (double, const SimpleVector&) : invalid type of vector");

  if (m.getNum() == 1)
  {
    // Copy m into p and call atlas::scal(d,p), p = d*p.
    DenseVect p = *m.getDensePtr();
    atlas::scal(d, p);
    return p;
  }
  else // if(m.getNum()==4){
  {
    SparseVect s = d * *m.getSparsePtr();
    return s;
  }
}

SimpleVector operator * (int d, const SimpleVector &m)
{
  if (m.getNum() != 1 && m.getNum() != 4)
    SiconosVectorException::selfThrow("opertor * (int, const SimpleVector&) : invalid type of vector");

  if (m.getNum() == 1)
  {
    // Copy m into p and call atlas::scal(d,p), p = d*p.
    DenseVect p = *m.getDensePtr();
    atlas::scal((double)d, p);
    return p;
  }
  else
  {
    // if(m.getNum()==4){
    SparseVect s = d * *m.getSparsePtr();
    return s;
  }
}

SimpleVector operator / (const SimpleVector &m, double d)
{
  if (m.getNum() != 1 && m.getNum() != 4)
    SiconosVectorException::selfThrow("opertor / (const SimpleVector&, double) : invalid type of vector");

  if (m.getNum() == 1)
  {
    // Copy m into p and call atlas::scal(d,p), p = (1/d)*p.
    DenseVect p = *m.getDensePtr();
    atlas::scal(1.0 / d, p);
    return p;
  }
  else // if(m.getNum()==4){
  {
    SparseVect s = *m.getSparsePtr() / d;
    return s;
  }
}

SimpleVector operator / (const SimpleVector &m, int d)
{
  if (m.getNum() != 1 && m.getNum() != 4)
    SiconosVectorException::selfThrow("opertor / (const SimpleVector&, int) : invalid type of vector");

  if (m.getNum() == 1)
  {
    // Copy m into p and call atlas::scal(d,p), p = (1/d)*p.
    DenseVect p = *m.getDensePtr();
    atlas::scal(1.0 / d, p);
    return p;
  }
  else // if(m.getNum()==4){
  {
    SparseVect s = *m.getSparsePtr() / d;
    return s;
  }
}

void SimpleVector::xpy(const SiconosVector &x, const SiconosVector &m)
{
  *this = m;
  *this += x;
  //   // Check sizes.
  //   if (x.size () != m.size () || x.size()!= size())
  //     SiconosVectorException::selfThrow("SiconosVector::Vector addition: inconsistent sizes");

  //   unsigned int numX = x.getNum();
  //   unsigned int numM = m.getNum();

  //   if(numX == numM && num == numX)
  //     {
  //       if(numX == 1) // If all are dense ...
  //  {
  //    // m is copied into this
  //    atlas::copy(*m.getDensePtr(),*vect.Dense);
  //    // xpy(x,this) => this = x + this.
  //    atlas::xpy(*x.getDensePtr(),*vect.Dense);
  //  }
  //       else
  //  *vect.Sparse = *x.getSparsePtr() + *m.getSparsePtr();
  //     }
  //   else
  //     {
  //       // If this is sparse, then error.
  //       if (num==4)
  //  SiconosVectorException::selfThrow("SiconosVector::xpy, sparse = sparse + dense not allowed.");
  //       if(numX == 1)
  //  *vect.Dense = *x.getDensePtr() + *m.getSparsePtr();
  //       else
  //  *vect.Dense = *x.getSparsePtr() + *m.getDensePtr();
  //     }
}

void SimpleVector::axpby(double a, const SiconosVector& x, double b, const SiconosVector& y)
{
  unsigned int numX = x.getNum();
  unsigned int numY = y.getNum();

  if (numX == numY && num == numX)
  {
    if (numX == 1)  // If all are dense ...
    {
      *this = y;
      atlas::axpby(a, *x.getDensePtr(), b, *vect.Dense);
    }
    else // all sparse
      *vect.Sparse = a**x.getSparsePtr() + b**y.getSparsePtr();
  }
  else
  {
    // If this is sparse, then error.
    if (num == 4)
      SiconosVectorException::selfThrow("SiconosVector::axpby sparse = sparse + dense not allowed.");
    if (numX == 1)
    {
      // this = ax
      atlas::axpy(a, *x.getDensePtr(), *vect.Dense);
      // this += by
      *vect.Dense += b**y.getSparsePtr();
    }
    else
    {
      // this = by
      atlas::axpy(b, *y.getDensePtr(), *vect.Dense);
      // this += by
      *vect.Dense += a**x.getSparsePtr();
    }
  }
}

void SimpleVector::scal(double a, const SiconosVector& x)
{
  if (x.getNum() == num)
  {
    if (num == 1)
      axpby(a, x, 0.0, *this);
    else
      *vect.Sparse = a**x.getSparsePtr();
  }
  else
    SiconosVectorException::selfThrow("SiconosVector::scal, mix sparse-dense not allowed.");
}

void SimpleVector::axpy(double a, const SiconosVector& x, const SiconosVector& y)
{
  unsigned int numX = x.getNum();
  unsigned int numY = y.getNum();

  if (numX == numY && num == numX)
  {
    if (numX == 1)  // If all are dense ...
    {
      *this = y;
      atlas::axpy(a, *x.getDensePtr(), *vect.Dense);
    }
    else // all sparse
      *vect.Sparse = a**x.getSparsePtr() + *y.getSparsePtr();
  }
  else
  {
    // If this is sparse, then error.
    if (num == 4)
      SiconosVectorException::selfThrow("SiconosVector::axpy sparse = sparse + dense not allowed.");
    if (numX == 1)
    {
      // this = ax
      atlas::axpy(a, *x.getDensePtr(), *vect.Dense);
      // this += by
      *vect.Dense += *y.getSparsePtr();
    }
    else
    {
      // this = by
      atlas::xpy(*y.getDensePtr(), *vect.Dense);
      // this += by
      *vect.Dense += a**x.getSparsePtr();
    }
  }
}

void axpby(double a, const SiconosVector& x, double b, SiconosVector& y)
{
  unsigned int numX = x.getNum();
  unsigned int numY = y.getNum();

  if (numX == numY)
  {
    if (numX == 1)  // If all are dense ...
      atlas::axpby(a, *x.getDensePtr(), b, *y.getDensePtr());

    else // all sparse
    {
      *y.getSparsePtr() *= b;
      *y.getSparsePtr() += a**x.getSparsePtr();
    }
  }
  else
  {
    y *= b;
    if (numX == 1)
      *y.getSparsePtr() += a**x.getDensePtr();
    else
      *y.getDensePtr() +=  a**x.getSparsePtr();
  }
}

void axpy(double a, const SiconosVector& x, SiconosVector& y)
{
  unsigned int numX = x.getNum();
  unsigned int numY = y.getNum();

  if (numX == numY)
  {
    if (numX == 1)  // If all are dense ...
      atlas::axpy(a, *x.getDensePtr(), *y.getDensePtr());
    else // all sparse
      *y.getSparsePtr() += a**x.getSparsePtr();
  }
  else
  {
    if (numX == 1)
      *y.getSparsePtr() += a**x.getDensePtr();
    else
      *y.getDensePtr() +=  a**x.getSparsePtr();
  }
}

void swap(SiconosVector& x, SiconosVector& y)
{
  unsigned int numX = x.getNum();
  unsigned int numY = y.getNum();

  if (numX == numY)
  {
    if (numX == 1) // ublas swap seems to be much more efficient than atlas::swap (bindings) ...
      swap(*x.getDensePtr(), *y.getDensePtr());
    else
      swap(*x.getSparsePtr(), *y.getSparsePtr());
  }
  else
    SiconosVectorException::selfThrow("Swap(x,y): can not swap two vectors of different types.");
}
