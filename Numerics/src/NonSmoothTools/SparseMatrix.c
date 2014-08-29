#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "SparseMatrix.h"
#include <math.h>

//#define VERBOSE_DEBUG

void printSparse(const SparseMatrix* const m)
{
  if (! m)
  {
    fprintf(stderr, "Numerics, SparseMatrix display failed, NULL input.\n");
    exit(EXIT_FAILURE);
  }
  assert(m->p);
  assert(m->i);
  assert(m->x);

  printf("Sparse matrix of size %dX%d\n", m->m, m->n);
  printf("maximum number of entries %i\n", m->nzmax);
  printf("storage (nz) %li\n", (long int)(m->nz));

  printf("pointer (p) of size %li= {", (long int)(m->m + 1));
  for (int i = 0 ; i < (m->m + 1); i++)
  {
    printf("%li,  ", (long int)(m->p[i]));
  }
  printf("}\n");
  printf("index (i) of size %li= {", (long int)m->nzmax);
  for (int i = 0 ; i < m->nzmax; i++) printf("%li,  ", (long int)(m->i[i]));
  printf("}\n");

  printf("data (x) of size %li = [\n", (long int)m->nzmax);
  for (int i = 0 ; i < m->m; i++)
  {
    for (int j = m->p[i]; j < m->p[i + 1] ; j++)
    {
      assert(m->nzmax > j);
      printf("%12.8e,  ", m->x[j]);
    }
    printf("\n");
  }
  printf("]\n");


}

void freeSparse(const SparseMatrix* M)
{

  assert(M);
  if (M->p)
  {
    free(M->p);
    M->p = NULL;
  }
  if (M->i)
  {
    free(M->i);
    M->i = NULL;
  }
  if (M->x)
  {
    free(M->x);
    M->x = NULL;
  }
}
