/* Siconos-Kernel, Copyright INRIA 2005-2010.
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

/*! \file PluginTypes.hpp
  \brief list of typedef for pointers to functions used in plugin mechanism.
*/

#ifndef PLUGINTYPES_HPP
#define PLUGINTYPES_HPP

/** Pointer to function used for plug-in for matrix-type operators that depends only on time */
typedef void (*MatrixFunctionOfTime)(double, unsigned int, unsigned int, double*, unsigned int, double*);

/** Pointer to function used for plug-in for vector-type operators that depends only on time */
typedef void (*VectorFunctionOfTime)(double, unsigned int, double*, unsigned int, double*);

#include "SimpleMatrix.hpp"

/** */
typedef void (*FPtr1)(double, unsigned int, const double*, double*, unsigned int, double*);

/** */
typedef void (*FPtr2)(unsigned int, const double*, unsigned int, const double*, double*, unsigned int, double*);

/** */
typedef void (*FPtr3)(unsigned int, const double*, unsigned int, double*, unsigned int, double*);
typedef void (*FPtr4bis)(unsigned int, const double*, unsigned int, double*, unsigned int, double*, unsigned int, double*);

/** */
typedef void (*FPtr4)(unsigned int, const double*, double, unsigned int, double*, unsigned int, double*);

/** */
typedef void (*FPtr5)(unsigned int, const double*, const double*, double*, unsigned int, double*);

/** */
typedef void (*FPtr6)(double, unsigned int, const double*, const double*, double*, unsigned int, double*);

/** */
typedef void (*FPtr7)(unsigned int, const double*, double*, unsigned int, double*);

#endif
