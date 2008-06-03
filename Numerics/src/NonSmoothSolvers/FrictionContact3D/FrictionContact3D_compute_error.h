/* Siconos-Numerics version 3.0.0, Copyright INRIA 2005-2008.
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
 * Contact: Vincent ACARY vincent.acary@inrialpes.fr
 */

#ifndef FrictionContact3D_compute_error_H
#define FrictionContact3D_compute_error_H

/*!\file NCP_block_compute_error.h
  \brief Computation of the NCP error fpr a block formulation

  \author Vincent Acary, 26/05/2008

*/

#ifdef __cplusplus
extern "C" {
#endif

  /** Computation of the error

   */
  void FrictionContact3D_compute_error(FrictionContact_Problem* problem, double *z , double *w, double tolerance, double * error);

#ifdef __cplusplus
}
#endif

#endif
