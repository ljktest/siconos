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

/*!\file lcp_avi_caoferris.c
 \brief Solve an LCP by reformulating it as an AVI and the solver by Cao and
Ferris solves the subsequent AVI.
 \author Olivier Huber
*/

#include "AVI_Solvers.h"
#include "Relay_Solvers.h"
#include "avi_caoferris.h"
#include "relay_cst.h"

void relay_avi_caoferris(RelayProblem* problem, double *z, double *w, int *info, SolverOptions* options)
{
  unsigned int n = problem->size;
  unsigned int s = 2*n;

  /* Copy the data from Relay problem */
  AffineVariationalInequalities avi_pb;
  avi_pb.size = s;
  NumericsMatrix num_mat;
  avi_pb.M = &num_mat;
  num_mat.size0 = s;
  num_mat.size1 = s;
  num_mat.storageType = 0;
  num_mat.matrix1 = NULL;
  num_mat.matrix2 = NULL;
  num_mat.matrix3 = NULL;
  num_mat.matrix0 = (double*)calloc(s*s, sizeof(double));

  avi_pb.q = (double *)malloc(s*sizeof(double));
  double* b_bar = (double *)malloc(s*sizeof(double));

  /* We can always choose the extreme point such that the matrix of active
   * constrains is the identity and the other matrix is minus the identity.
   * B_A = Id, B_I = -Id, b_A = lb, b_I = -ub
   * TODO: implement the case when the user gives an hint about the solution
   *
   * Siconos/Kernel is giving us column-major matrix and the solver is
   * expecting matrix in this format
   * */

  double tmp;
  for (unsigned int i = 0; i < n; ++i)
  {
    tmp = 0.0;
    avi_pb.M->matrix0[i*(s+1)] = 1.0;
    avi_pb.M->matrix0[(i + n)*(s+1)] = -1.0;
    avi_pb.q[i] = problem->q[i];
    avi_pb.q[i+n] = - problem->lb[i] + problem->ub[i];
    for (unsigned int j = 0; j < n; ++j)
    {
      avi_pb.M->matrix0[i + (j+n)*s] = problem->M->matrix0[i + j*n];
      tmp += problem->M->matrix0[i + j*n]*problem->lb[j];
    }
    /* \bar{a} =  -\tilde{a} + Ay_e */
    avi_pb.q[i] += tmp;
  }
  avi_pb.d = (double *)malloc(s*sizeof(double));
  for (unsigned int i = 0; i<n; ++i)
  {
    avi_pb.d[i] = -1;
    avi_pb.d[i+n] = 0;
  }
  avi_pb.poly = NULL;

  /* Set of active constraint is trivial */
  unsigned int * A = (unsigned int *)malloc(n*sizeof(unsigned int));
  for (unsigned int i = 0; i<n; ++i) A[i] = i+1;

  double* u_vec = (double *)calloc(s, sizeof(double));
  double* s_vec = (double *)calloc(s, sizeof(double));
  /* Call directly the 3rd stage 
   * Here w is used as u and z as s in the AVI */
  avi_caoferris_stage3(&avi_pb, u_vec, s_vec, n, A, info, options);

  /* Update z  */
  /* XXX why no w ?  */
  for (unsigned int i = 0; i<n; ++i) z[i] = s_vec[A[i]-1] + problem->lb[i];
  /* free allocated stuff */
  free(u_vec);
  free(s_vec);
  free(A);
  free(avi_pb.d);
  freeNumericsMatrix(avi_pb.M);
  free(avi_pb.q);
}

int relay_avi_caoferris_setDefaultSolverOptions(SolverOptions* options)
{
  int ret = avi_caoferris_setDefaultSolverOptions(options);
  options->solverId = SICONOS_RELAY_AVI_CAOFERRIS;
  return ret;
}

