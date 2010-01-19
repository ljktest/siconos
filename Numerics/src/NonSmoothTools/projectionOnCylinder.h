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

#ifndef ProjectionOnCylinder_H
#define ProjectionOnCylinder_H
#ifdef __cplusplus
extern "C" {
#endif
  /** projectionOnCylinder Projection onto the positive Cylinder of radius R  \f$K \{ r, r_1 \geq 0, 0 \sqrt(r_2^2+r_3^2) \geq R \}
  \param[in-out] the vector to be projected
  \param[in] R the radius of the cone
  */
  void projectionOnCylinder(double* r, double  R);
#ifdef __cplusplus
}
#endif

#endif
