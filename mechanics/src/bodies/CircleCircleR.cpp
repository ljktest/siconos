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
 * Foundation, Inc., 51 Franklin St, Fifth FLOOR, Boston, MA  02110-1301  USA
 *
 * Contact: Vincent ACARY, siconos-team@lists.gforge.inria.fr
 *
 */

#include <cmath>
#include "CircleCircleR.hpp"
#include <BlockVector.hpp>

CircleCircleR::CircleCircleR(double r, double rr): CircularR(r, rr)
{
}

double CircleCircleR::distance(double x1, double y1, double r1, double x2, double y2, double r2)
{

  return (fabs(r1 - r2) - hypot(x1 - x2, y1 - y2));

}

void CircleCircleR::computeh(SiconosVector& q, SiconosVector& z, SiconosVector& y)
{

  double q_0 = q(0);
  double q_1 = q(1);
  double q_3 = q(3);
  double q_4 = q(4);

  y(0) = distance(q_0, q_1, _r1, q_3, q_4, _r2);

}

void CircleCircleR::computeJachq(SiconosVector& q, SiconosVector& z)
{

  SimpleMatrix *g = (SimpleMatrix *) _jachq.get();

  double x1 = q(0);
  double y1 = q(1);
  double x2 = q(3);
  double y2 = q(4);

  double dx = x2 - x1;
  double dy = y2 - y1;

  double d = hypot(dx, dy);

  double dxsd = dx / d;
  double dysd = dy / d;

  (*g)(0, 0) = dxsd;
  (*g)(1, 0) = -dysd;
  (*g)(0, 1) = dysd;
  (*g)(1, 1) = dxsd;
  (*g)(0, 2) = 0.;
  (*g)(1, 2) = -_r1;
  (*g)(0, 3) = -dxsd;
  (*g)(1, 3) = dysd;
  (*g)(0, 4) = -dysd;
  (*g)(1, 4) = -dxsd;
  (*g)(0, 5) = 0.;
  (*g)(1, 5) = -_r2;
}
