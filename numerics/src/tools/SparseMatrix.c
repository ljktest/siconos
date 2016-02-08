#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "SparseMatrix.h"
#include <math.h>
#include <float.h>

#include "SiconosCompat.h"

//#define VERBOSE_DEBUG

/* add an entry to triplet matrix only if value is not (nearly) null */
csi cs_zentry(CSparseMatrix *T, csi i, csi j, double x)
{
  if(fabs(x) >= DBL_EPSILON)
  {
    return cs_entry(T, i, j, x);
  }
  else
  {
    return 1;
  }
}

int cs_lu_factorization(csi order, const cs *A, double tol, NumericsSparseLinearSolverParams* p)
{
  assert(A);
  assert(p);

  cs_lu_factors* cs_lu_A = (cs_lu_factors*) malloc(sizeof(cs_lu_factors));
  cs_lu_A->n = A->n;
  css* S = cs_sqr (order, A, 0);
  cs_lu_A->S = S;
  cs_lu_A->N = cs_lu(A, S, tol);

  p->solver_data = cs_lu_A;
  return (S && cs_lu_A->N);
}

void NM_csparse_free(void* p)
{
  assert(p);

  cs_lu_factors* cs_lu_A = NM_csparse_lu_factors(p);
  if (cs_lu_A)
  {
    cs_lu_A->n = -1;

    cs_sfree(cs_lu_A->S);
    cs_lu_A->S = NULL;

    cs_nfree(cs_lu_A->N);
    cs_lu_A->N = NULL;

    free(cs_lu_A);
  }
}

/* Solve Ax = b with the factorization of A stored in the cs_lu_A
 * This is extracted from cs_lusol, you need to synchronize any changes! */
csi cs_solve (cs_lu_factors* cs_lu_A, double* x, double *b)
{
  assert(cs_lu_A);

  csi ok;
  csi n = cs_lu_A->n;
  css* S = cs_lu_A->S;
  csn* N = cs_lu_A->N;
  ok = (S && N && x) ;
  if (ok)
  {
    cs_ipvec (N->pinv, b, x, n) ;       /* x = b(p) */
    cs_lsolve (N->L, x) ;               /* x = L\x */
    cs_usolve (N->U, x) ;               /* x = U\x */
    cs_ipvec (S->q, x, b, n) ;          /* b(q) = x */
  }
  return (ok);
}

int cs_check_triplet(CSparseMatrix *T)
{
  if (T->nz < 0)
  {
    fprintf(stderr, "cs_check_triplet :: given CSparseMatrix is not in a triplet form: nz = " SN_PTRDIFF_T_F, T->nz);
    return 1;
  }
  csi nb_row = T->m;
  csi nb_col = T->n;
  csi* Ti = T->i;
  csi* Tp = T->p;
  int info = 0;
  for (csi indx = 0; indx < T->nz; ++indx)
  {
    if (Ti[indx] >= nb_row)
    {
      printf("cs_check_triplet :: matrix element " SN_PTRDIFF_T_F " has a row number " SN_PTRDIFF_T_F "  > " SN_PTRDIFF_T_F " the number of rows\n", indx, Ti[indx], nb_row);
      info = 1;
    }
    if (Tp[indx] >= nb_col)
    {
      printf("cs_check_triplet :: matrix element " SN_PTRDIFF_T_F " has a row number " SN_PTRDIFF_T_F " > " SN_PTRDIFF_T_F " the number of rows\n", indx, Tp[indx], nb_col);
      info = 1;
    }
  }
  return info;
}

/* from sparse to dense */
double* cs_dense(CSparseMatrix *A)
{
  csi m = A->m;
  csi n = A->n;
  csi *Ap = A->p;
  csi *Ai = A->i;
  double *Ax = A->x;
  csi nzmax = A->nzmax;
  csi nz = A->nz;
  double *r = (double*) malloc(A->m * A->n * sizeof(double));
  for(int i = 0; i<m*n; ++i)
  {
    r[i] = 0.;
  }
  if(nz < 0)
  {
    for(int j = 0 ; j < n ; j++)
    {
      printf("    col %d : locations " SN_PTRDIFF_T_F " to " SN_PTRDIFF_T_F "\n", j, Ap [j], Ap [j+1]-1);
      for(csi p = Ap [j] ; p < Ap [j+1] ; p++)
      {
        printf("      " SN_PTRDIFF_T_F " : %g\n", Ai [p], Ax ? Ax [p] : 1) ;
        r[Ai[p] + j*m] = Ax ? Ax [p] : 1;
      }
    }
  }
  else
  {
    printf("triplet: " SN_PTRDIFF_T_F "-by-" SN_PTRDIFF_T_F ", nzmax: " \
        SN_PTRDIFF_T_F " nnz: " SN_PTRDIFF_T_F "\n", m, n, nzmax, nz) ;
    for(int p = 0 ; p < nz ; p++)
    {
      printf("    " SN_PTRDIFF_T_F " " SN_PTRDIFF_T_F " : %g\n", Ai [p], Ap [p], Ax ? Ax [p] : 1) ;
      r[Ai[p] + Ap[p] * m] = Ax ? Ax[p] : 1;
    }
  }
  return r;
}

/* y = alpha*A*x+beta*y */
int cs_aaxpy(const double alpha, const cs *A, const double *x,
             const double beta, double *y)
{
  csi p, n, *Ap, *Ai ;
  int j;
  double *Ax ;
  if(!A || !x || !y) return (0) ;	     /* check inputs */
  n = A->n ;
  Ap = A->p ;
  Ai = A->i ;
  Ax = A->x ;
  assert(fabs(beta-1.) < 100*DBL_EPSILON && "cs_aaxpy is broken for beta != 1, if you don't see why, ask Olivier");
  for(j = 0 ; j < n ; j++)
  {
    for(p = Ap [j] ; p < Ap [j+1] ; p++)
    {
      y [Ai [p]] *= beta;
      y [Ai [p]] += alpha * Ax [p] * x [j] ;
    }
  }
  return (1) ;
}

CSparseMatrix* cs_spfree_on_stack(CSparseMatrix* A)
{
  if(!A) return (NULL) ;	/* do nothing if A already NULL */
  cs_free(A->p) ;
  cs_free(A->i) ;
  cs_free(A->x) ;
  return NULL;
}

NumericsSparseLinearSolverParams* newNumericsSparseLinearSolverParams(void)
{
  NumericsSparseLinearSolverParams* p = (NumericsSparseLinearSolverParams*)
    malloc(sizeof(NumericsSparseLinearSolverParams));

  p->solver = NS_CS_LUSOL;

  p->iparam = NULL;
  p->dparam = NULL;
  p->iWork = NULL;
  p->dWork = NULL;

  p->solver_data = NULL;
  p->solver_free_hook = NULL;

  p->iSize = 0;
  p->dSize = 0;
  p->iWorkSize = 0;
  p->dWorkSize = 0;

  return p;
}

NumericsSparseLinearSolverParams* freeNumericsSparseLinearSolverParams(NumericsSparseLinearSolverParams* p)
{
  if (p->solver_free_hook)
    (*p->solver_free_hook)(p);
  p->solver_free_hook = NULL;

  if (p->iparam)
  {
    assert(p->iSize>0);
    free(p->iparam);
    p->iparam = NULL;
  }
  if (p->dparam)
  {
    assert(p->dSize>0);
    free(p->dparam);
    p->dparam = NULL;
  }
  if (p->iWork)
  {
    assert(p->iWorkSize>0);
    free(p->iWork);
    p->iWork = NULL;
  }
  if (p->dWork)
  {
    assert(p->dWorkSize>0);
    free(p->dWork);
    p->dWork = NULL;
  }
  if (p->solver_data)
  {
    free(p->solver_data);
    p->solver_data = NULL;
  }

  free(p);
  return NULL;
}

NumericsSparseMatrix* newNumericsSparseMatrix(void)
{
  NumericsSparseMatrix* p = (NumericsSparseMatrix*)
    malloc(sizeof(NumericsSparseMatrix));

  p->linearSolverParams = newNumericsSparseLinearSolverParams();

  p->triplet = NULL;
  p->csc = NULL;
  p->trans_csc = NULL;

  return p;
}

NumericsSparseMatrix* freeNumericsSparseMatrix(NumericsSparseMatrix* A)
{
  if (A->linearSolverParams)
  {
    freeNumericsSparseLinearSolverParams(A->linearSolverParams);
    A->linearSolverParams = NULL;
  }
  if (A->triplet)
  {
    cs_spfree(A->triplet);
    A->triplet = NULL;
  }
  if (A->csc)
  {
    cs_spfree(A->csc);
    A->csc = NULL;
  }
  if (A->trans_csc)
  {
    cs_spfree(A->trans_csc);
    A->trans_csc = NULL;
  }
  return NULL;
}

