
#include "NM_conversions.h"
#include <assert.h>
#include <stdio.h>

CSparseMatrix* NM_csc_to_triplet(CSparseMatrix* csc)
{
  assert(csc);
  CSparseMatrix* triplet = cs_spalloc(csc->m, csc->n, csc->p[csc->n], 1, 1);

  csi* Ap = csc->p;
  csi* Ai = csc->i;
  double* val = csc->x;
  for (csi j = 0; j < csc->n; ++j)
  {
    for (csi i = Ap[j]; i < Ap[j+1]; ++i)
    {
      cs_zentry(triplet, Ai[i], j, val[i]);
    }
  }
  return triplet;
}

CSparseMatrix* NM_triplet_to_csr(CSparseMatrix* triplet)
{
#ifdef WITH_MKL
  assert(triplet);
  if (triplet->m != triplet->n)
  {
    fprintf(stderr, "NM_triplet_to_csr :: the matrix has to be square\n");
    exit(EXIT_FAILURE);
  }
  CSparseMatrix* csr = cs_spalloc(NS_NROW_CSR(triplet), NS_NCOL_CSR(triplet), triplet->nz, 1, 0);
  assert(csr);
  csr->nz = -2;

  csi n = csr->n;
  csi job[6] = {0};
  csi info = 0;
  job[0] = 2;
  mkl_dcsrcoo(job, &n, csr->x, csr->i, csr->p, &(triplet->nz), triplet->x, triplet->i, triplet->p, &info);

  return csr;
#else
  fprintf(stderr, "NM_triplet_to_csr :: MKL not enabled\n");
  exit(EXIT_FAILURE);
#endif
}

CSparseMatrix* NM_csr_to_triplet(CSparseMatrix* csr)
{
#ifdef WITH_MKL
  assert(csr);
  if (csr->m != csr->n)
  {
    fprintf(stderr, "NM_csr_to_triplet :: the matrix has to be square\n");
    exit(EXIT_FAILURE);
  }
  CSparseMatrix* triplet = cs_spalloc(csr->m, csr->n, csr->p[csr->m], 1, 1);
  assert(triplet);

  csi n = csr->n;
  csi job[6] = {0};
  csi info = 0;
  mkl_dcsrcoo(job, &n, csr->x, csr->i, csr->p, &(csr->p[csr->m]), triplet->x, triplet->i, triplet->p, &info);

  return triplet;
#else
  fprintf(stderr, "NM_csr_to_triplet :: MKL not enabled\n");
  exit(EXIT_FAILURE);
#endif
}

CSparseMatrix* NM_csc_to_csr(CSparseMatrix* csc)
{
#ifdef WITH_MKL
  assert(csc);
  if (csc->m != csc->n)
  {
    fprintf(stderr, "NM_csc_to_csr :: the matrix has to be square\n");
    exit(EXIT_FAILURE);
  }
  CSparseMatrix* csr = cs_spalloc(NS_NROW_CSR(csc), NS_NCOL_CSR(csc), csc->p[csc->n], 1, 0);
  assert(csr);
  csr->nz = -2;

  csi n = csr->n;
  csi job[6] = {0};
  csi info = 0;
  job[0] = 2;
  mkl_dcsrcsc(job, &n, csr->x, csr->i, csr->p, &(csc->p[csc->n]), csc->x, csc->i, csc->p, &info);

  return csr;
#else
  fprintf(stderr, "NM_csc_to_csr :: MKL not enabled\n");
  exit(EXIT_FAILURE);
#endif
}

CSparseMatrix* NM_csr_to_csc(CSparseMatrix* csr)
{
#ifdef WITH_MKL
  if (csr->m != csr->n)
  {
    fprintf(stderr, "NM_csr_to_csc :: the matrix has to be square\n");
    exit(EXIT_FAILURE);
  }
  assert(csr);
  CSparseMatrix* csc = cs_spalloc(csr->m, csr->n, csr->nzmax, 1, 0);
  assert(csc);

  csi n = csr->n;
  csi job[6] = {0};
  csi info = 0;
  mkl_dcsrcsc(job, &n, csr->x, csr->i, csr->p, &(csr->p[csr->m]), csc->x, csc->i, csc->p, &info);

  return csc;
#else
  fprintf(stderr, "NM_csr_to_csc :: MKL not enabled\n");
  exit(EXIT_FAILURE);
#endif
}
