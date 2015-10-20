/* Siconos-Numerics, Copyright INRIA 2005-2012.
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


#include "fc3d_GlockerFischerBurmeister_functions.h"
#include "NonSmoothNewton.h"
#include "fc3d_Solvers.h"
#include "FischerBurmeister.h"
/* size of a block */
static int Fsize;

/* static void F_GlockerFischerBurmeister(int sizeF, double* reaction, double* FVector, int up2Date); */
/* static void jacobianF_GlockerFischerBurmeister(int sizeF, double* reaction, double* jacobianFMatrix, int up2Date); */



/** writes \f$ F(z) \f$ using Glocker formulation and the Fischer-Burmeister function.
 */
void F_GlockerFischerBurmeister(int sizeF, double* reaction, double* FVector, int up2Date)
{
  /* Glocker formulation */
  double* FGlocker = NULL;
  computeFGlocker(&FGlocker, up2Date);
  /* Note that FGlocker is a static var. in fc3d2NCP_Glocker and thus there is no memory allocation in
   the present file.
  */

  /* Call Fisher-Burmeister function => fill FVector */
  phi_FB(sizeF, reaction, FGlocker, FVector);
  FGlocker = NULL;
}

/** writes \f$ \nabla_z F(z) \f$  using Glocker formulation and the Fischer-Burmeister function.
 */
void jacobianF_GlockerFischerBurmeister(int sizeF, double* reaction, double* jacobianFMatrix, int up2Date)
{
  /* Glocker formulation */
  double* FGlocker = NULL, *jacobianFGlocker = NULL;
  computeFGlocker(&FGlocker, up2Date);
  computeJacobianFGlocker(&jacobianFGlocker, up2Date);
  /* Note that FGlocker and jacobianFGlocker are static var. in fc3d2NCP_Glocker and thus there is no memory allocation in
   the present file.
  */

  /* Call Fisher-Burmeister function => fill jacobianFMatrix */
  jacobianPhi_FB(sizeF, reaction, FGlocker, jacobianFGlocker, jacobianFMatrix);
  FGlocker = NULL;
  jacobianFGlocker = NULL;
}
