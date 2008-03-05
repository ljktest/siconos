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
#ifndef MLCP_PROBLEM_C
#define MLCP_PROBLEM_C


#include <stdio.h>
#include "MixedLinearComplementarity_Problem.h"

void displayMat(double * M, int Nblin, int Nbcol)
{
  int lin, col;
  for (lin = 0; lin < Nblin; lin++)
  {
    for (col = 0; col < Nbcol; col++)
      printf("%lf", M[lin + col * Nblin]);
    printf("\n");
  }

}

void displayMLCP(MixedLinearComplementarity_Problem* p)
{
  int n = p->n;
  int m = p->m;
  printf("MLCP DISPLAY:\n-------------\n");
  printf("n :%d m: %d\n", p->n, p->m);
  printf(p->problemType ? "using (M)\n" : "using (A,B,C,D)\n");
  if (p->M)
  {
    printf("M matrix:\n");
    displayRawbyRaw(p->M);
  }
  else
    printf("No M matrix:\n");
  if (p->q)
  {
    printf("q matrix:\n");
    displayMat(p->q, n + m, 1);
  }
  else
    printf("No b matrix:\n");


  if (p->A)
  {
    printf("A matrix:\n");
    displayMat(p->A, n, n);
  }
  else
    printf("No A matrix:\n");

  if (p->C)
  {
    printf("B matrix:\n");
    displayMat(p->C, n, m);
  }
  else
    printf("No B matrix:\n");

  if (p->D)
  {
    printf("D matrix:\n");
    displayMat(p->D, m, n);
  }
  else
    printf("No D matrix:\n");

  if (p->B)
  {
    printf("B matrix:\n");
    displayMat(p->B, m, m);
  }
  else
    printf("No B matrix:\n");

  if (p->a)
  {
    printf("a matrix:\n");
    displayMat(p->a, n, 1);
  }
  else
    printf("No a matrix:\n");
  if (p->b)
  {
    printf("b matrix:\n");
    displayMat(p->b, m, 1);
  }
  else
    printf("No b matrix:\n");

}
#endif

