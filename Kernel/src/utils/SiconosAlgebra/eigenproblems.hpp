/* Siconos-Kernel, Copyright INRIA 2005-2013.
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
#ifndef EIGENPROBLEMS_HPP
#define EIGENPROBLEMS_HPP
/*! \file EigenProblems.hpp
  Functions to compute eigenvalues/vectors of a matrix.
  Based on boost ublas bindings to lapack. 
*/

#include "SiconosMatrix.hpp"

namespace Siconos {
  namespace eigenproblems {

/** Compute eigenvalues and eigenvectors of the matrix 
    \param[in,out] eigenval : eigenvalues of the matrix
    \param[in,out] eigenvec : each column of the matrix is an 
    eigenvector. 
    \return int : return value from lapack routine
**/
    int syev(SiconosVector& eigenval, SiconosMatrix& eigenvec);

/** Compute eigenvalues and eigenvectors of the matrix 
    \param[in,out] eigenval : eigenvalues of the matrix
    \param[in,out] eigenvec : each column of the matrix is an 
    eigenvector. 
    \return int : return value from lapack routine
**/
    int geev(complex_matrix& input_mat, complex_vector& eigenval, complex_matrix& left_eigenvec, complex_matrix& right_eigenvec);

  } // namespace eigenproblems
} // namespace Siconos


#endif
