/* Siconos-Kernel, Copyright INRIA 2005-2015
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
#include "AVI.hpp"
#include <assert.h>
#include "Simulation.hpp"
#include "NormalConeNSL.hpp"
#include "OSNSMatrix.hpp"

//#include <AVI_Solvers.h>
#include <NonSmoothDrivers.h>

#include <limits>
#include <cstdlib>
#include <iostream>
#include <cassert>

/*****************************************************
 * START visitor for nslaw
*/
#if 0
struct AVI::_BoundsNSLEffect : public SiconosVisitor
{

  using SiconosVisitor::visit;

  AVI* _parent;
  SP::Interaction _inter;
  unsigned int _pos;


  _BoundsNSLEffect(AVI *p, SP::Interaction inter, unsigned int pos) :
    _parent(p), _inter(inter), _pos(pos) {};

  void visit(const NormalConeNSL& nslaw)
  {
    if (_pos > 0)
    {
      S
    }
    // take the 
    SiconosVector& K = nslaw.K();
    SimpleMatrix& H = nslaw.H();
    _numerics_problem->size = nslaw.size();
    _numerics_problem->d = NULL;
    _numerics_problem->poly->id = SICONOS_SET_POLYHEDRON;
    _numerics_problem->poly->size_ineq = K.size();
    _numerics_problem->poly->size_eq = 0;
    _numerics_problem->poly->H = H.getArray();
    _numerics_problem->poly->K = K.getArray();
    _numerics_problem->poly->Heq = NULL;
    _numerics_problem->poly->Keq = NULL;
  }

  void visit(const RelayNSL& nslaw)
  {
    Siconos

  }

  void visit(const ComplementarityConditionNSL& nslaw)
  {
  }

};
#endif
/*****************************************************
 * END visitor for nslaw
*/

AVI::AVI(int numericsSolverId): LinearOSNS(numericsSolverId)
{
  _numerics_problem.reset(new AffineVariationalInequalities);
  _numerics_problem->poly = new polyhedron;
  set_SolverOptions(_numerics_solver_options.get(), numericsSolverId);
}

void AVI::initialize(SP::Simulation sim)
{
  LinearOSNS::initialize(sim);

  // right now we support only one (1) NonsmoothLaw associated with this AVI
  // It is not clear whether having multiple NonsmoothLaw would be beneficial given the exponential complexity of most solvers
  // TODO We should support RelayNSL with generic rectangles -- xhub
  InteractionsGraph& indexSet = *simulation()->indexSet(indexSetLevel());
  InteractionsGraph::VIterator ui, uiend;
  unsigned nbInter = 0;
  for (std11::tie(ui, uiend) = indexSet.vertices(); ui != uiend; ++ui)
  {
    NormalConeNSL& nc = static_cast<NormalConeNSL&>(*indexSet.bundle(*ui)->nonSmoothLaw());
    assert(Type::value(nc) == Type::NormalConeNSL &&
        "AVI::initialize :: found a NonSmoothLaw that is not of the NormalConeNSL type! This is currently not supported");
    SiconosVector& K = nc.K();
    SimpleMatrix& H = nc.H();
    _numerics_problem->size = nc.size();
    _numerics_problem->d = NULL;
    _numerics_problem->poly->id = SICONOS_SET_POLYHEDRON;
    _numerics_problem->poly->size_ineq = K.size();
    _numerics_problem->poly->size_eq = 0;
    _numerics_problem->poly->H = H.getArray();
    _numerics_problem->poly->K = K.getArray();
    _numerics_problem->poly->Heq = NULL;
    _numerics_problem->poly->Keq= NULL;

    // we do not support more than one interaction
    assert(nbInter++ == 0 &&
        "AVI::initialize :: more than one Interactions for this OneStepNSProblem is not support ATM!");
  }

}


int AVI::compute(double time)
{
  int info = 0;
  // --- Prepare data for AVI computing ---
  bool cont = preCompute(time);
  if (!cont)
    return info;

  if (_numerics_problem->size != _sizeOutput)
  {
    std::cout << "AVI::compute - size mismatch between AVI size and and the current size" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // --- Call Numerics driver ---
  // Inputs:
  // - the problem (M,q ...)
  // - the unknowns (z,w)
  // - the options for the solver (name, max iteration number ...)
  // - the global options for Numerics (verbose mode ...)

  if (_sizeOutput != 0)
  {
    // The AVI in Numerics format
    _numerics_problem->M = _M->getNumericsMatrix().get();
    _numerics_problem->q = _q->getArray();

    info = avi_driver(_numerics_problem.get(), _z->getArray() , _w->getArray() ,
                      _numerics_solver_options.get(), _numerics_options.get());

    if (info != 0)
    {
      std::cout << "Warning : Problem in AVI resolution" <<std::endl;
    }

    // --- Recovering of the desired variables from AVI output ---
    postCompute();
  }

  return info;
}

void AVI::display() const
{
  std::cout << "======= AVI of size " << _sizeOutput << " with: " <<std::endl;
  LinearOSNS::display();
}

void AVI::setSolverId(int solverId)
{
  // clear previous Solveroptions
  deleteSolverOptions(_numerics_solver_options.get());
  set_SolverOptions(_numerics_solver_options.get(), solverId);
}


AVI::~AVI()
{
  deleteSolverOptions(&*_numerics_solver_options);
  delete _numerics_problem->poly;
}

