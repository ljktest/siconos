/* Siconos-IO, Copyright INRIA 2005-2015
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

#include "IOConfig.h"
#ifdef WITH_SERIALIZATION
#include "SiconosFull.hpp"

#include "RegisterModel.hpp"

#include <boost/numeric/bindings/ublas/matrix.hpp>
#include <boost/numeric/bindings/ublas/vector.hpp>
#include <boost/numeric/bindings/ublas/vector_sparse.hpp>
#include <boost/numeric/bindings/ublas/matrix_sparse.hpp>

#include <boost/archive/binary_oarchive.hpp>

void RegisterModelObin(std::ofstream& ofs, SP::Model& model)
{
  boost::archive::binary_oarchive ar(ofs);
  siconos_io_register_Numerics(ar);
  siconos_io_register_Kernel(ar);
  ar << NVP(model);
}
#endif
