/* Siconos-Kernel, Copyright INRIA 2005-2007.
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
 *
 * Note: the size of this non smooth law is one by default, but can be set to any value.
 * Size > 1 is useful when D matrix in the relation is not null and not diagonal, to write the y = CX +Dlambda with only one
 * interaction.
 */
/*! \file MixedComplementarityConditionNSL.hpp

*/
#ifndef MIXEDCOMPLEMENTARITYCONDITIONNSLAW_H
#define MIXEDCOMPLEMENTARITYCONDITIONNSLAW_H

#include "NonSmoothLaw.hpp"

#include "SiconosPointers.hpp"

class NonSmoothLaw;

/** Complementarity NonSmoothLaw
 *
 * \author SICONOS Development Team - copyright INRIA
 *  \version 2.1.1.
 *  \date (Creation) Apr 27, 2004
 *
 *
 **/
class MixedComplementarityConditionNSL : public NonSmoothLaw
{
private:
  /** serialization hooks
  */
  ACCEPT_SERIALIZATION(MixedComplementarityConditionNSL);

  /** default constructor
   */
  MixedComplementarityConditionNSL() {};
  unsigned int EqualitySize;

public:
  /** basic constructor
  *  \param: size of the non smooth law
  *  \param: size of the equality relation
  */
  MixedComplementarityConditionNSL(unsigned int newSize, unsigned int equalitySize);

  /** constructor with XML object of the parent class NonSmoothLaw
  *  \param nslawxml the corresponding XML object
  */
  MixedComplementarityConditionNSL(SP::NonSmoothLawXML nslawxml);

  /** Destructor */
  ~MixedComplementarityConditionNSL();


  /** print the data to the screen
  */
  inline void display()const {};

  /** copy the data of the NonSmoothLaw to the XML tree
  */
  inline void saveNonSmoothLawToXML() {};

  /** get the number of equality present in the MLCP
   *  \return an unsigned int
   */
  inline unsigned int getEqualitySize()
  {
    return EqualitySize;
  };

  /** Visitors hook
   */
  ACCEPT_STD_VISITORS();
};

#endif // MIXEDCOMPLEMENTARITYCONDITIONNSLAW_H
