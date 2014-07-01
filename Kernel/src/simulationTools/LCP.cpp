/* Siconos-Kernel, Copyright INRIA 2005-2012.
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
#include "LCP.hpp"
#include "OSNSMatrix.hpp"
// #define DEBUG_STDOUT
// #define DEBUG_MESSAGES 1
#include "debug.h"

#include <LCP_Solvers.h>

using namespace RELATION;

LCP::LCP(int numericsSolverId):
  LinearOSNS(numericsSolverId)
{
  _numerics_problem.reset(new LinearComplementarityProblem);
  linearComplementarity_setDefaultSolverOptions(NULL, &*_numerics_solver_options, _numerics_solver_id);
}

int LCP::compute(double time)
{

  int info = 0;

  // --- Prepare data for LCP computing ---
  // And check if there is something to be done
  bool cont = preCompute(time);
  if (!cont)
    return info;

  // --- Call Numerics driver ---
  // Inputs:
  // - the problem (M,q ...)
  // - the unknowns (z,w)
  // - the options for the solver (name, max iteration number ...)
  // - the global options for Numerics (verbose mode ...)
  DEBUG_PRINTF("LCP : sizeOutput=%d\n", _sizeOutput);
  DEBUG_PRINTF("_indexSetLevel = %i\n", _indexSetLevel);
  DEBUG_EXPR(display(););


  if (_sizeOutput != 0)
  {

    // The LCP in Numerics format
    _numerics_problem->M = &*_M->getNumericsMatrix();
    _numerics_problem->q = _q->getArray();
    _numerics_problem->size = _sizeOutput;

    //const char * name = &*_numerics_solver_options->solverName;
    if (_numerics_solver_options->solverId == SICONOS_LCP_ENUM)
    {
      lcp_enum_init(&*_numerics_problem, &*_numerics_solver_options, 1);


    }
    info = linearComplementarity_driver(&*_numerics_problem, _z->getArray() , _w->getArray() ,
                                        &*_numerics_solver_options, &*_numerics_options);

    if (_numerics_solver_options->solverId == SICONOS_LCP_ENUM)
    {
      lcp_enum_reset(&*_numerics_problem, &*_numerics_solver_options, 1);


    }

    // --- Recovering of the desired variables from LCP output ---
    postCompute();


    DEBUG_EXPR(display());

  }

  return info;
}

void LCP::display() const
{
  std::cout << "======= LCP of size " << _sizeOutput << " with: " <<std::endl;
  LinearOSNS::display();
}

void LCP::initialize(SP::Simulation sim)
{
  // General initialize for LinearOSNS
  LinearOSNS::initialize(sim);
}

LCP::~LCP()
{
  deleteSolverOptions(&*_numerics_solver_options);
}
