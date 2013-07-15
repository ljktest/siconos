/* Siconos-IO, Copyright INRIA 2005-2011.
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

#ifndef MechanicsIO_hpp
#define MechanicsIO_hpp


#include <SimpleMatrix.hpp>
  class SiconosVector;
  class Model;
  class SpaceFilter&
}
#endif

class MechanicsIO
{
protected:

  template<typename T, typename G>
  SP::SimpleMatrix visitAllVerticesForVector(const G& graph) const;

  template<typename T, typename G>
  SP::SiconosVector visitAllVerticesForDouble(const G& graph) const;

public:
  /** default constructor
   */
  MechanicsIO() {};
  
  /** get static objects identifications 
   * \param broadphase a SP::SpaceFilter
   * \return a SP::SiconosVector with ids
   */
  SP::SiconosVector staticIds(const SpaceFilter& broadphase) const;


  /** get dynamical systems identifications 
   * \param model a SP::Model
   * \return a SP::SiconosVector with ids
   */
  SP::SiconosVector dynamicIds(const Model& model) const;


  /** get all positions : translation (x,y,z) + orientation quaternion
   * (qw, qx, qy, qz)
   * \param model a SP::Model
   * \return a SP::SimpleMatrix where the columns are x, y, z, qw, qx, qy, qz 
   */
  SP::SimpleMatrix positions(const Model& model) const;

  /** get all velocities : translation (xdot, ydot, zdot) + orientation velocities 
      ox, oy, oz
      \return a SP::SimpleMatrix where the columns are xdot, ydot, zdot,
      ox, oy, oz
  */
  SP::SimpleMatrix velocities(const Model& model) const;

  /** get the coordinates of all contact points, normals, reactions and velocities
      \return a SP::SimpleMatrix where the columns are x y z, nx, ny, nz, rx, ry, rz, vx, vy, vz
  */
  SP::SimpleMatrix contactPoints(const Model& model) const;
};


#endif
