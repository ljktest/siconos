/* Siconos-Example version 3.0.0, Copyright INRIA 2005-2008.
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
 * Contact: Vincent ACARY vincent.acary@inrialpes.fr
 *
 */

#include <math.h>
#include "SpherePlanR.hpp"

SpherePlanR::SpherePlanR(double r, double A, double B, double C, double D)
  : LagrangianScleronomousR(), r(r), A(A), B(B), C(C), D(D)
{
  nN = sqrt(A * A + B * B + C * C);

  n1 = A / nN;
  n2 = B / nN;
  n3 = C / nN;

  nU = sqrt((A + B) * (A + B) + (B - C) * (B - C) + (A + C) * (A + C));
  u1 = (B - C) / nU;
  u2 = -(A + C) / nU;
  u3 = (A + B) / nU;

  // v = n /\ u

  v1 = B * (A + B) / (nN * nU) - C * (-A - C) / (nN * nU);
  v2 = C * (B - C) / (nN * nU) - A * (A + B) / (nN * nU);
  v3 = A * (-A - C) / (nN * nU) - B * (B - C) / (nN * nU);

  // r*u & r *v

  ru1 = r * u1;
  ru2 = r * u2;
  ru3 = r * u3;

  rv1 = r * v1;
  rv2 = r * v2;
  rv3 = r * v3;

}

double SpherePlanR::distance(double x, double y, double z, double rad)
{

  return (fabs(A * x + B * y + C * z + D) / nN - rad);
}


void SpherePlanR::computeh(double)
{

  double q_0 = (*data[q0])(0);
  double q_1 = (*data[q0])(1);
  double q_2 = (*data[q0])(2);

  SP::SiconosVector y = interaction()->y(0);

  y->setValue(0, distance(q_0, q_1, q_2, r));

};

void normalize(SP::SiconosVector, unsigned int);

void SpherePlanR::computeJachq(double)
{
  SimpleMatrix *g = (SimpleMatrix *)_jachq.get();

  double theta = (*data[q0])(3);
  double phi   = (*data[q0])(4);
  double psi   = (*data[q0])(5);

  double cthe = cos(theta);
  double sthe = sin(theta);
  double cphi = cos(phi);
  double sphi = sin(phi);

  (*g)(0, 0) = n1;
  (*g)(1, 0) = u1;
  (*g)(2, 0) = v1;
  (*g)(0, 1) = n2;
  (*g)(1, 1) = u2;
  (*g)(2, 1) = v2;
  (*g)(0, 2) = n3;
  (*g)(1, 2) = u3;
  (*g)(2, 2) = v3;
  (*g)(0, 3) = 0;
  (*g)(1, 3) = -rv1 * cphi - rv2 * sphi;
  (*g)(2, 3) = ru1 * cphi + ru2 * sphi;
  (*g)(0, 4) = 0;
  (*g)(1, 4) = -rv3;
  (*g)(2, 4) = ru3;
  (*g)(0, 5) = 0;
  (*g)(1, 5) = -rv3 * cthe + rv2 * cphi * sthe - rv1 * sphi * sthe;
  (*g)(2, 5) = ru3 * cthe + ru1 * sphi * sthe - ru2 * cphi * sthe;

}

