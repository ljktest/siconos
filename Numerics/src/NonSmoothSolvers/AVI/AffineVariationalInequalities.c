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
#ifndef AVI_PROBLEM_C
#define AVI_PROBLEM_C


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include "AffineVariationalInequalities.h"


void AVI_display(AffineVariationalInequalities* problem)
{

  assert(problem);
  int i, n = problem->size;
  printf("AffineVariationalInequalities Display :\n-------------\n");
  printf("size :%d \n", problem->size);
  if (problem->M)
  {
    printf("M matrix:\n");
    display(problem->M);
  }
  else
    printf("No M matrix.\n");

  if (problem->q)
  {
    printf("q vector:\n");
    for (i = 0; i < n; i++) printf("q[ %i ] = %12.8e\n", i, problem->q[i]);
  }
  else
    printf("No q vector.\n");

  if (problem->d)
  {
    printf("d vector:\n");
    for (i = 0; i < n; i++) printf("d[ %i ] = %12.8e\n", i, problem->d[i]);
  }
  else
    printf("No d vector.\n");

}





int AVI_printInFile(AffineVariationalInequalities*  problem, FILE* file)
{
  if (! problem)
  {
    fprintf(stderr, "Numerics, AffineVariationalInequalities printInFile failed, NULL input.\n");
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
  return 1;
}

int AVI_newFromFile(AffineVariationalInequalities* problem, FILE* file)
{
  int n = 0;
  int i;

  int nread = fscanf(file, "%d\n", &n);
  problem->size = n;
  problem->M = (NumericsMatrix *)malloc(sizeof(NumericsMatrix));

  newFromFile(problem->M, file);

  problem->q = (double *) malloc(problem->M->size1 * sizeof(double));
  for (i = 0; i < problem->M->size1; i++)
  {
    nread = fscanf(file, "%lf ", &(problem->q[i]));
  }
  return 1;
}
int AVI_newFromFilename(AffineVariationalInequalities* problem, char* filename)
{
  int info = 0;
  FILE * file = fopen(filename, "r");

  info = AVI_newFromFile(problem, file);

  fclose(file);
  return info;
}

void freeAVI(AffineVariationalInequalities* problem)
{
  freeNumericsMatrix(problem->M);
  free(problem->M);
  free(problem->q);
  freePolyhedron(problem->poly);
  if (problem->d)
    free(problem->d);
  free(problem);
  problem = NULL;
}



#endif

