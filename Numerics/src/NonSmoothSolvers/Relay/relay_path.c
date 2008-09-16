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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LA.h"
#include <math.h>
#include "Relay_Solvers.h"

static int sVerbose = 0;
#ifdef HAVE_PATHFERRIS
#include "InterfaceToPathFerris/SimpleLCP.h"
#endif /*HAVE_PATHFERRIS*/

void relay_path(Relay_Problem* problem, double *z, double *w, int *info , Solver_Options* options)
{
  *info = 1;
#ifdef HAVE_PATHFERRIS
  /* matrix M/vector q of the relay */
  double * M = problem->M->matrix0;
  double * q = problem->q;
  double *a = problem->a;
  double *b = problem->b;

  int nnz, i, j, dim;

  /* size of the RELAY */
  int n = problem->size;

  double tol = options->dparam[0];
  MCP_Termination termination;

  nnz = nbNonNulElems(n, M, 1.0e-18);
  int * m_i = (int *)calloc(nnz + 1, sizeof(int));
  int * m_j = (int *)calloc(nnz + 1, sizeof(int));
  double * m_ij = (double *)calloc(nnz + 1, sizeof(double));
  double * lb = (double *)calloc(n + 1, sizeof(double));
  double * ub = (double *)calloc(n + 1, sizeof(double));
  double err, val;


  FortranToPathSparse(n, M, 1.0e-18, m_i, m_j, m_ij);
  for (i = 0; i < n; i++)
  {
    lb[i] = -;
    ub[i] = 1.e20;
  }
  SimpleLCP(n, nnz, m_i, m_j, m_ij, q, lb, ub,
            &termination, z);

  if (termination == MCP_Error)
  {
    *info = 1;
    if (verbose > 0)
      printf("PATH : Error in the solution.\n");
  }
  else if (termination == MCP_Solved)
  {
    for (i = 0; i < n; i++)
    {
      val = q[i];
      for (j = 0; j < n; j++)
      {
        val += M[i + j * n] * z[j];
      }
      w[i] = val;
    }
    *info = 0;
    /* **** Criterium convergence **** */
    //relay_compute_error(problem,z,w,tol,&err);

    if (verbose > 0)
      printf("PATH : RELAY Solved, error %10.7f.\n", err);
  }
  else
  {
    if (verbose > 0)
      printf("PATH : Other error: %d\n", termination);
  }
  free(m_i);
  free(m_j);
  free(m_ij);
  free(lb);
  free(ub);

#endif /*HAVE_PATHFERRIS*/


  return;
}
