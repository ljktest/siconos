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

/*! \file BlockVector.hpp
  \brief Object to handle vectors of vectors ( ... of vectors)
*/

#ifndef BLOCKVECTOR_H
#define BLOCKVECTOR_H

#include "SiconosAlgebraTypeDef.hpp"

/** Object to handle block-vectors (ie list of SP::SiconosVector)
 *
 *  \author SICONOS Development Team - copyright INRIA
 *  \version 3.0.0.
 *
 * A block vector is a stl vector that handles pointers to SiconosVector.
 *
 * Insertion of NULL SP::SiconosVector is not allowed.
 *
 */
class BlockVector
{
private:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(BlockVector);


  /** Size (ie total number of scalar elements, not number of blocks) */
  unsigned int _sizeV;

  /** A container of pointers on SiconosVector. */
  VectorOfVectors vect;

  /** tabindex[i] = tabindex[i-1] + ni, ni being the size of svref[i]. */
  SP::Index _tabIndex;

public:

  void setBlock(const SiconosVector&, unsigned int, unsigned int, unsigned int);

  /** default contructor
   */
  BlockVector();

  /** copy contructor
   *  \param v BlockVector& 
   */
  BlockVector(const BlockVector& v);

  /** contructor with a 2 SiconosVectors
   *  \param v1 first vector
   *  \param v2 second vector
   */
  BlockVector(SP::SiconosVector v1, SP::SiconosVector v2);

  BlockVector(unsigned int numberOfBlocks, unsigned int dim);

  /** destructor
   */
  ~BlockVector();

  /** get the vector size, ie the total number of (double) elements in
     *  the vector
   *  \return unsigned int
   */
  unsigned int size() const
  {
    return _sizeV;
  };


  /** iterator equal to vect.begin
      \return a VectorOfVectors::iterator
  */
  inline VectorOfVectors::iterator begin()
  {
    return vect.begin();
  };

  /** iterator equal to vect.end
      \return a VectorOfVectors::iterator
  */
  inline VectorOfVectors::iterator end()
  {
    return vect.end();
  };

  /** const iterator equal to vect.begin
      \return a VectorOfVectors::iterator
  */
  inline VectorOfVectors::const_iterator begin() const
  {
    return vect.begin();
  };

  /** const iterator equal to vect.end
      \return a VectorOfVectors::iterator
  */
  inline VectorOfVectors::const_iterator end() const
  {
    return vect.end();
  } ;

  /** get vect, ie all the vectors of the object
   * \return a VectorOfVectors
   */
  inline VectorOfVectors getAllVect() const
  {
    return vect;
  }

  /** get the number of Blocks
   *  \return unsigned int
   */
  inline unsigned int getNumberOfBlocks() const
  {
    return _tabIndex->size();
  };



  /** sets all the values of the vector to 0.0
   */
  void zero();

  /** set all values of the vector component to value.
   * \param a double
   */
  void fill(double a);

  /** display data on standard output
   */
  void display(void) const;

  /** return the element vector[i]
   *  \param i an unsigned int
   *  \return a double
   */
  double getValue(unsigned int i) const;

  /** set the element vector[i]
   *  \param i an unsigned int
   *  \param value
   */
  void setValue(unsigned int i, double value);

  /** get the element at position i (warning: absolute position.)
   *  \param i an unsigned integer
   *  \return a reference to a double
   */
  double& operator()(unsigned int i) ;

  /** get the element at position i (warning: absolute position.)
   *  \param  i an unsigned integer
   *  \return a double
   */
  double operator()(unsigned int i) const;

  /** return i-eme SiconosVector of vect
   * \param pos block number
   * \return a pointer to a SiconosVector
   */
  inline SP::SiconosVector vector(unsigned int pos)
  {
    return vect[pos];
  };

  /** return i-eme SiconosVector of vect
   * \param pos block number
   * \return a pointer to a SiconosVector
   */
  inline SPC::SiconosVector vector(unsigned int pos) const
  {
    return vect[pos];
  };

  /** set i-eme SiconosVector of vect (copy)
   * \param pos block number
   * \param v a SiconosVector
   */
  void setVector(unsigned int pos, const SiconosVector& v);

  /** set i-eme SiconosVector of vect (pointer link)
   * \param pos block number
   * \param v a SiconosVector
   */
  void setVectorPtr(unsigned int pos, SP::SiconosVector v);

  /** get the vector at position i(ie this for Simple and block i for BlockVector)
   *  \param pos block number
   *  \return a SP::SiconosVector
   */
  SP::SiconosVector operator [](unsigned int pos) ;

  /** get the vector at position i(ie this for Simple and block i for BlockVector)
   *  \param pos block number
   *  \return a SP::SiconosVector
   */
  SPC::SiconosVector operator [](unsigned int pos) const;

  /** get the index tab
   * \return a standard vector of int
   */
  inline Index getTabIndex() const
  {
    return *_tabIndex;
  }

  /** get a pointer to the index tab
   * \return SP::Index
   */
  inline const SP::Index tabIndex() const
  {
    return _tabIndex;
  }

  /** get an iterator that points to the first element of tabIndex
   * \return an Index::iterator
   */
  inline Index::iterator tabIndexBegin()
  {
    return _tabIndex->begin();
  }

  /** get an iterator that points to tabIndex.end()
   * \return an Index::iterator
   */
  inline Index::iterator tabIndexEnd()
  {
    return _tabIndex->end();
  }

  /** get an iterator that points to the first element of tabIndex
   * \return an Index::iterator
   */
  inline Index::const_iterator tabIndexBegin() const
  {
    return _tabIndex->begin();
  }

  /** get an iterator that points to tabIndex.end()
   * \return an Index::iterator
   */
  inline Index::const_iterator tabIndexEnd() const
  {
    return _tabIndex->end();
  }

  /** get the number of the vector that handles element at position "pos"
   *  \param pos unsigned int, position of the element
   *  \return unsigned int number of the searched vector
   */
  unsigned int getNumVectorAtPos(unsigned int pos) const;

  /** operator =
  *  \param vIn the vector to be copied
  * \return  BlockVector&
  */
  BlockVector& operator =(const BlockVector& vIn);

  /** Equality operator with raw double* data on the right-hand side
   *  \param data data to put in the BlockVector
   * \return  BlockVector&
   */
  BlockVector& operator = (const double* data);

  BlockVector& operator -=(const BlockVector&);
  BlockVector& operator +=(const BlockVector&);

  /** operator =
   *  \param vIn the vector to be copied
   * \return  BlockVector&
   */
  BlockVector& operator =(const SiconosVector& vIn);

  BlockVector& operator *= (double s);

  BlockVector& operator /= (double s);

  /** Insert a subvector in this vector: allocation and copy
  *  \param v SiconosVector& v : the vector to be inserted
  */
  void insert(const SiconosVector& v) ;

  /** Insert a pointer to a subvector in this vector: no reallocation nor copy.
   *  \param v a SiconosVector
   */
  void insertPtr(SP::SiconosVector v);

  bool isComparableTo(const BlockVector& v1, const BlockVector& v2);

  double norm2() const;

  /** compute the infinite norm of the vector
   *  \return a double
   */
  double normInf() const;

  BlockVector& operator += (const SiconosVector& vIn);
  BlockVector& operator -= (const SiconosVector& vIn);

  ACCEPT_NONVIRTUAL_VISITORS();

};

#endif
