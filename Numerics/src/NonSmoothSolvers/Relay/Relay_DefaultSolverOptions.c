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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include "NumericsOptions.h"
#include "Relay_Solvers.h"
#include "NonSmoothDrivers.h"
#include "relay_cst.h"

int relay_setDefaultSolverOptions(RelayProblem* problem, SolverOptions* options, int solverId)
{

  int info = -1;
  switch (solverId)
  {
  case SICONOS_RELAY_PGS:
  {
    info =    relay_pgs_setDefaultSolverOptions(options);
    break;
  }
  case SICONOS_RELAY_LEMKE:
  {
    info =    relay_lexicolemke_setDefaultSolverOptions(options);
    break;
  }
  case SICONOS_RELAY_ENUM:
  {
    info =    relay_enum_setDefaultSolverOptions(problem, options);
    break;
  }
  case SICONOS_RELAY_PATH:
  {
    info =    relay_path_setDefaultSolverOptions(options);
    break;
  }
  case SICONOS_RELAY_AVI_CAOFERRIS:
  {
    info =    relay_avi_caoferris_setDefaultSolverOptions(options);
    break;
  }
  case SICONOS_RELAY_AVI_CAOFERRIS_TEST:
  {
    info =    relay_avi_caoferris_test_setDefaultSolverOptions(options);
    break;
  }
/* XXX: to implement ?
   case SICONOS_RELAY_LATIN:
  {
    info =    relay_latin_setDefaultSolverOptions(options);
    break;
  }
  case SICONOS_RELAY_NLGS:
  {
    info =    relay_nlgs_setDefaultSolverOptions(options);
    break;
  }
  */
  default:
  {
    numericsError("Relay_setDefaultSolverOptions", " Unknown Solver");
  }
  }

  return info;
}
