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
#ifndef RELAY_PROBLEM_C
#define RELAY_PROBLEM_C


#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "RelayProblem.h"
#include "misc.h"

void Relay_display(RelayProblem* p)
{

  assert(p);
  int i, n = p->size;
  printf("RelayProblem Display :\n-------------\n");
  printf("size :%d \n", p->size);
  if (p->M)
  {
    printf("M matrix:\n");
    display(p->M);
  }
  else
    printf("No M matrix:\n");

  if (p->q)
  {
    printf("q vector:\n");
    for (i = 0; i < n; i++) printf("q[ %i ] = %12.8e\n", i, p->q[i]);
  }
  else
    printf("No q vector:\n");

  if (p->lb)
  {
    printf("lb vector:\n");
    for (i = 0; i < n; i++) printf("lb[ %i ] = %12.8e\n", i, p->lb[i]);
  }
  else
    printf("No lb vector:\n");

  if (p->ub)
  {
    printf("ub vector:\n");
    for (i = 0; i < n; i++) printf("ub[ %i ] = %12.8e\n", i, p->ub[i]);
  }
  else
    printf("No ub vector:\n");
}




int relay_printInFile(RelayProblem*  problem, FILE* file)
{
  if (! problem)
  {
    fprintf(stderr, "Numerics, RelayProblem printInFile failed, NULL input.\n");
    exit(EXIT_FAILURE);
  }
  int i;
  int n = problem->size;
  fprintf(file, "%d\n", n);
  printInFile(problem->M, file);
  for (i = 0; i < problem->M->size1; i++)
  {
    fprintf(file, "%32.24e ", problem->q[i]);
  }
  fprintf(file, "\n");
  for (i = 0; i < problem->M->size1; i++)
  {
    fprintf(file, "%32.24e ", problem->lb[i]);
  }
  fprintf(file, "\n");
  for (i = 0; i < problem->M->size1; i++)
  {
    fprintf(file, "%32.24e ", problem->ub[i]);
  }
  return 1;
}

int relay_newFromFile(RelayProblem* problem, FILE* file)
{
  int n = 0;
  int i;

  CHECK_IO(fscanf(file, "%d\n", &n));
  problem->size = n;
  problem->M = newNumericsMatrix();

  newFromFile(problem->M, file);

  problem->q = (double *) malloc(problem->M->size1 * sizeof(double));
  for (i = 0; i < problem->M->size1; i++)
  {
    CHECK_IO(fscanf(file, "%lf ", &(problem->q[i])));
  }

  problem->lb = (double *) malloc(problem->M->size1 * sizeof(double));
  for (i = 0; i < problem->M->size1; i++)
  {
    CHECK_IO(fscanf(file, "%lf ", &(problem->lb[i])));
  }

  problem->ub = (double *) malloc(problem->M->size1 * sizeof(double));
  for (i = 0; i < problem->M->size1; i++)
  {
    CHECK_IO(fscanf(file, "%lf ", &(problem->ub[i])));
  }
  return 1;
}

void freeRelay_problem(RelayProblem* problem)
{
  assert(problem);
  if (problem->M)
  {
    freeNumericsMatrix(problem->M);
    free(problem->M);
  }
  if (problem->q)  { free(problem->q); }
  if (problem->lb) { free(problem->lb); }
  if (problem->ub) { free(problem->ub); }
  free(problem);
}



#endif

