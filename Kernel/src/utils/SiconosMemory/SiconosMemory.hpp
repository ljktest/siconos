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

/*! \file SiconosMemory.hpp
    \brief class SiconosMemory

*/


#ifndef SICONOSMEMORY_H
#define SICONOSMEMORY_H

#include "SiconosMemoryException.hpp"
#include "SiconosPointers.hpp"
#include <deque>
#include "SiconosAlgebraTypeDef.hpp"

/** Container used to save vectors in SiconosMemory */
typedef std::deque<SP::SiconosVector> MemoryContainer;
TYPEDEF_SPTR(MemoryContainer)

/** This class is a backup for vectors of previous time step
    \author SICONOS Development Team - copyright INRIA
    \version 3.0.0.
    \date (Creation) 07/06/2004

    There is a max number of saved vector (memorySize) and all the vector (simple or block)
    should have the same size.

*/
class SiconosMemory
{
private:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(SiconosMemory);


  /** the maximum size of the memory (i.e the max numbers of SiconosVectors it can store) */
  unsigned int _size;

  /** the real number of SiconosVectors saved in the Memory (ie the ones for which memory has been allocated) */
  unsigned int _nbVectorsInMemory;

  /** the stl deque which contains the SiconosVectors kept in memory */
  SP::MemoryContainer _vectorMemory;

  /** index to avoid removal and creation of vectors */
  unsigned int _indx;

  /** default constructor, private. */
  SiconosMemory() {};

  /** Assignment, private */
  const SiconosMemory& operator=(const SiconosMemory&);

public:

  /** constructor with size parameter.
   * \param size size of the MemoryContainer
   * \param vectorSize the size of the SiconosVector to store
   */
  SiconosMemory(const unsigned int size, const unsigned int vectorSize);

  /** constructor with deque parameter.
   * \param MemoryContainer, the deque of siconosVector which must be stored
   * _size is set to the size of the deque given in parameters
   */
  SiconosMemory(const MemoryContainer&);

  /** constructor with size and deque parameter.
   * \param int : the size of the memory
   * \param  MemoryContainer, the deque of siconosVector which must be stored
   * this constructor is useful if the deque given in parameters has a size lower than the normal size of the memory
   */
  SiconosMemory(const unsigned int, const MemoryContainer&);

  /** Copy constructor
   * \param Mem a SiconosMemory
   */
  SiconosMemory(const SiconosMemory& Mem);

  /** destructor */
  ~SiconosMemory();

  /*************************************************************************/

  /** fill the memory with a vector of siconosVector
   * \param MemoryContainer
   * _size is set to the size of the deque given in parameters
   */
  void setVectorMemory(const MemoryContainer&);

  /** To get SiconosVector number i of the memory
   * \param int i: the position in the memory of the wanted SiconosVector
   * \return a SP::SiconosVector
   */
  SP::SiconosVector getSiconosVector(const unsigned int) const;

  /** gives the size of the memory
   * \return int >= 0
   */
  inline unsigned int getMemorySize() const
  {
    return _size;
  };

  /** set the max size of the SiconosMemory
   * \param int : the max size for this SiconosMemory
   */
  inline void setSiconosMemorySize(const unsigned int max)
  {
    _size = max;
  }

  /** gives the numbers of SiconosVectors currently stored in the memory
   * \return int >= 0
   */
  inline unsigned int nbVectorsInMemory() const
  {
    return _nbVectorsInMemory;
  };

  /** gives the vector of SiconosVectors of the memory
   * \return stl deque of  SiconosVector
   */
  inline SP::MemoryContainer vectorMemory() const
  {
    return _vectorMemory;
  };

  /** puts a SiconosVector into the memory
   * \param v the SiconosVector we want to put in memory
   */
  void swap(const SiconosVector& v);

  /** displays the data of the memory object
   */
  void display() const;

};
#endif

