/* Siconos-Numerics, Copyright INRIA 2005-2010.
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

#ifndef NUMERICSGENERICMECHANICALPROBLEM_H
#define NUMERICSGENERICMECHANICALPROBLEM_H

#include "NumericsMatrix.h"

/* void * solverFC3D; */
/* void * solverEquality; */
/* void * solverLCP; */
/* void * solverMLCP; */

/** GenericMechanicalProblem
    \param numberOfBlockLine The number of line of blocks.
    \param M A sparse blocks matrix.
    \param q A dense vector.
    \param problems An array of pointer on Numerics problem, either FrictionContactProblem, LinearSystemProblem, MixedLinearComplementarityProblem or LinearComplementarityProblem. The size is numberOfBlockLine.
    \param problemsType
 */

/** Remark:

    The M and q contains de matrices of the problem. The sub problems (problems) has also a M and q member because it is usfull for the computation of the local error.


 */

typedef struct
{
  /*Number of line of blocks.*/
  int numberOfBlockLine;
  NumericsMatrix* M;
  double* q;
  /*Array[numberOfBlockLine] of pointer on Numerics problem, either FrictionContactProblem, LinearSystemProblem, MixedLinearComplementarityProblem or LinearComplementarityProblem.*/
  void * * problems;
  /*Array[numberOfBlockLine] type of numerics problem.*/
  int * problemsType;
} GenericMechanicalProblem;





#endif
