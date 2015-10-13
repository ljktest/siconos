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
#include <stdlib.h>
#include <assert.h>
#include "SecondOrderConeLinearComplementarityProblem.h"
#include "misc.h"

//#define DEBUG_STDOUT
//#define DEBUG_MESSAGES
#include "debug.h"

void secondOrderConeLinearComplementarityProblem_display(SecondOrderConeLinearComplementarityProblem* problem)
{

  assert(problem);



  int i, n ;

  n=problem->n;

  printf("SecondOrderConeLinearComplementarityProblem Display :\n-------------\n");
  printf("nc:%d \n", problem->nc);

  if(problem->M)
  {
    printf("M matrix:\n");
    display(problem->M);
  }
  else
    printf("No M matrix:\n");

  if(problem->q)
  {
    printf("q vector:\n");
    for(i = 0; i < n; i++) printf("q[ %i ] = %12.8e\n", i, problem->q[i]);
  }
  else
    printf("No q vector:\n");

  if(problem->coneIndex)
  {
    printf("coneIndex vector:\n");
    for(i = 0; i < problem->nc+1; i++) printf("coneIndex[ %i ] = %i\n", i, problem->coneIndex[i]);
  }
  else
    printf("No mu vector:\n");
  if(problem->mu)
  {
    printf("mu vector:\n");
    for(i = 0; i < problem->nc; i++) printf("mu[ %i ] = %12.8e\n", i, problem->mu[i]);
  }
  else
    printf("No mu vector:\n");

}





int secondOrderConeLinearComplementarityProblem_printInFile(SecondOrderConeLinearComplementarityProblem*  problem, FILE* file)
{
  if(! problem)
  {
    fprintf(stderr, "Numerics, SecondOrderConeLinearComplementarityProblem printInFile failed, NULL input.\n");
    exit(EXIT_FAILURE);
  }
  int i;
  int n = problem->n;
  fprintf(file, "%d\n", n);
  int nc = problem->nc;
  fprintf(file, "%d\n", nc);
  printInFile(problem->M, file);
  for(i = 0; i < problem->M->size1; i++)
  {
    fprintf(file, "%32.24e ", problem->q[i]);
  }
  fprintf(file, "\n");
  for(i = 0; i < nc+1; i++)
  {
    fprintf(file, "%i ", problem->coneIndex[i]);
  }
  fprintf(file, "\n");

  for(i = 0; i < nc; i++)
  {
    fprintf(file, "%32.24e ", problem->mu[i]);
  }
  fprintf(file, "\n");
  return 0;
}

int secondOrderConeLinearComplementarityProblem_printInFilename(SecondOrderConeLinearComplementarityProblem* problem, char* filename)
{
  int info = 0;
  FILE * file = fopen(filename, "w");

  if(!file)
  {
    return errno;
  }

  info = secondOrderConeLinearComplementarityProblem_printInFile(problem, file);

  fclose(file);
  return info;
}

int secondOrderConeLinearComplementarityProblem_newFromFile(SecondOrderConeLinearComplementarityProblem* problem, FILE* file)
{
  DEBUG_PRINT("Start -- int secondOrderConeLinearComplementarityProblem_newFromFile(SecondOrderConeLinearComplementarityProblem* problem, FILE* file)\n");
  int n = 0;
  int nc = 0;
  int i;

  CHECK_IO(fscanf(file, "%d\n", &n));
  problem->n = n;
  CHECK_IO(fscanf(file, "%d\n", &nc));
  problem->nc = nc;
  problem->M = newNumericsMatrix();

  /* fix: problem->M->storageType unitialized ! */

  newFromFile(problem->M, file);

  problem->q = (double *) malloc(problem->M->size1 * sizeof(double));
  for(i = 0; i < problem->M->size1; i++)
  {
    CHECK_IO(fscanf(file, "%lf ", &(problem->q[i])));
  }
  problem->coneIndex = (unsigned int *) malloc((nc+1) * sizeof(unsigned int));
  for(i = 0; i < nc+1; i++)
  {
    CHECK_IO(fscanf(file, "%d ", &(problem->coneIndex[i])));
  }

  problem->mu = (double *) malloc(nc * sizeof(double));
  for(i = 0; i < nc; i++)
  {
    CHECK_IO(fscanf(file, "%lf ", &(problem->mu[i])));
  }
  DEBUG_PRINT("End --  int secondOrderConeLinearComplementarityProblem_newFromFile(SecondOrderConeLinearComplementarityProblem* problem, FILE* file)\n");

  return 0;
}

int secondOrderConeLinearComplementarityProblem_newFromFilename(SecondOrderConeLinearComplementarityProblem* problem, char* filename)
{
  int info = 0;
  FILE * file = fopen(filename, "r");

  if(!file)
  {
    return errno;
  }

  info = secondOrderConeLinearComplementarityProblem_newFromFile(problem, file);

  fclose(file);
  return info;
}

void freeSecondOrderConeLinearComplementarityProblem(SecondOrderConeLinearComplementarityProblem* problem)
{

  if (problem->M)
  {
    freeNumericsMatrix(problem->M);
    free(problem->M);
    problem->M = NULL;
  }
  if (problem->mu)
  {
    free(problem->mu);
    problem->mu = NULL;
  }
  if (problem->q)
  {
    free(problem->q);
    problem->q = NULL;
  }
  if (problem->coneIndex)
  {
    free(problem->coneIndex);
    problem->coneIndex = NULL;
  }
  free(problem);

}

SecondOrderConeLinearComplementarityProblem* secondOrderConeLinearComplementarityProblem_new(int n,  int nc, NumericsMatrix* M, double* q, unsigned int *coneIndex, double* mu)
{
  SecondOrderConeLinearComplementarityProblem* soclcp = (SecondOrderConeLinearComplementarityProblem*) malloc(sizeof(SecondOrderConeLinearComplementarityProblem));

  soclcp->n = n;
  soclcp->nc = nc;
  soclcp->M = M;
  soclcp->q = q;
  soclcp->coneIndex = coneIndex;
  soclcp->mu = mu;

  return soclcp;
}
