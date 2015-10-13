/* Siconos-Numerics, Copyright INRIA 2005-2011.
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

#ifndef SiconosBlas_H
#define SiconosBlas_H

#include "SiconosConfig.h"

#if defined(__cplusplus)
#include <algorithm>
using std::min;
using std::max;
#else
#define min(a,b) ((a)>(b)?(b):(a))
#define max(a,b) ((a) >= (b) ? (a) : (b))
#endif

#if defined(__cplusplus) 
extern "C"
{
#endif

#if defined(HAS_MKL_CBLAS) 
#include <mkl_cblas.h>
#elif defined(HAS_ATLAS_CBLAS)
#include <cblas.h>
#elif defined(HAS_ACCELERATE)
#include <Accelerate.h>
#else
#include <cblas.h>
#endif 

#ifdef __cplusplus
}
#endif


#include "matrix_la.h"

#endif // SiconosBlas_H


