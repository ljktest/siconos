/* Siconos-Numerics, Copyright INRIA 2005-2014
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
 * You should have received a copy of the GNU General Public License
 * along with Siconos; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Contact: Vincent ACARY, siconos-team@lists.gforge.inria.fr
 */

#include "VariationalInequality_Solvers.h"
#include "VariationalInequality_computeError.h"
#include "Qi_merit.h"
#include "SiconosSets.h"
#include "Newton_Methods.h"
#include "VI_Newton.h"

void VI_compute_F(void* data_opaque, double* x, double* F)
{
  VariationalInequality* problem = (VariationalInequality*) data_opaque;
  problem->F(problem, problem->size, x, F);
}

void VI_compute_error_box(void* data_opaque, double* x, double* F, double* Jac_F_merit, double tol, double* err)
{
  VariationalInequality* problem = (VariationalInequality*) data_opaque;
  variationalInequality_compute_error_box(problem, x, F, tol, err);
}

void VI_compute_F_box_Qi(void* data_opaque, double* x, double* F, double* Fbox)
{
  VariationalInequality* problem = (VariationalInequality*) data_opaque;
  phi_Qi(problem->size, x, F, Fbox, ((box_constraints*) problem->set)->lb, ((box_constraints*) problem->set)->ub);
}

void VI_compute_H_box_Qi(void* data_opaque, double* x, double* F, double* workV1, double* workV2, NumericsMatrix* H)
{
  VariationalInequality* problem = (VariationalInequality*) data_opaque;
  problem->compute_nabla_F(problem, problem->size, x, problem->nabla_F);

  Jac_F_Qi(problem->size, x, F, workV1, workV2, problem->nabla_F, ((box_constraints*) problem->set)->lb, ((box_constraints*) problem->set)->ub, H);
}

void* vi_get_set(void* problem); /*XXX */

void variationalInequality_box_newton_QiLSA(VariationalInequality* problem, double *x, double *F, int* info, SolverOptions* options)
{
  functions_LSA functions_QiLSA;
  init_lsa_functions(&functions_QiLSA, &VI_compute_F, &VI_compute_F_box_Qi);
  functions_QiLSA.compute_H = &VI_compute_H_box_Qi;
  functions_QiLSA.compute_error = &VI_compute_error_box;
  functions_QiLSA.get_set_from_problem_data = &vi_get_set;
  options->iparam[SICONOS_IPARAM_LSA_FORCE_ARCSEARCH] = 1;

  set_lsa_params_data(options, problem->nabla_F);
  newton_LSA(problem->size, x, F, info, (void *)problem, options, &functions_QiLSA);
}
