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


/* /!\ work in progress */

#include "NumericsConfig.h"

#include "GlobalFrictionContact3D_AlartCurnier.h"
#include "GlobalFrictionContact3D_Solvers.h"
#include "GlobalFrictionContact3D_compute_error.h"

#include "op3x3.h"
#include "SparseBlockMatrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "SparseMatrix.h"
#include "FrictionContact3D_localAlartCurnier.h"

#include "GlobalFrictionContact3D_compute_error.h"
#include "SiconosBlas.h"
#ifdef WITH_MUMPS
#include <mpi.h>
#include <dmumps_c.h>

#define JOB_INIT -1
#define JOB_END -2
#define USE_COMM_WORLD -987654
#define ICNTL(I) icntl[(I)-1]
#define CNTL(I) cntl[(I)-1]
#endif

/* check return code of a function */
#define CHECK(EXPR)                                                     \
  do                                                                    \
  {                                                                     \
    if (!EXPR)                                                          \
    {                                                                   \
      fprintf (stderr, "Siconos Numerics: Warning %s failed, %s:%d ",   \
               #EXPR, __FILE__, __LINE__);                              \
    }                                                                   \
  } while (0)

/* from sparse to dense */
double* cs_dense(SparseMatrix *A)
{
  int m = A->m;
  int n = A->n;
  int *Ap = A->p;
  int *Ai = A->i;
  double *Ax = A->x;
  int nzmax = A->nzmax;
  int nz = A->nz;
  double *r = (double*) malloc(A->m * A->n * sizeof(double));
  for(unsigned int i = 0; i<m*n; ++i)
  {
    r[i] = 0.;
  }
  if(nz < 0)
  {
    for(unsigned int j = 0 ; j < n ; j++)
    {
      printf("    col %d : locations %d to %d\n", j, Ap [j], Ap [j+1]-1);
      for(unsigned int p = Ap [j] ; p < Ap [j+1] ; p++)
      {
        printf("      %d : %g\n", Ai [p], Ax ? Ax [p] : 1) ;
        r[Ai[p] + j*m] = Ax ? Ax [p] : 1;
      }
    }
  }
  else
  {
    printf("triplet: %d-by-%d, nzmax: %d nnz: %d\n", m, n, nzmax, nz) ;
    for(unsigned int p = 0 ; p < nz ; p++)
    {
      printf("    %d %d : %g\n", Ai [p], Ap [p], Ax ? Ax [p] : 1) ;
      r[Ai[p] + Ap[p] * m] = Ax ? Ax[p] : 1;
    }
  }
  return r;
}

/* add an entry to triplet matrix only if value is not (nearly) null */
int cs_zentry(SparseMatrix *T, int i, int j, double x)
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

/* y = alpha*A*x+beta*y */
int cs_aaxpy(const double alpha, const cs *A, const double *x, double *y)
{
  int p, j, n, *Ap, *Ai ;
  double *Ax ;
  if(!A || !x || !y) return (0) ;	     /* check inputs */
  n = A->n ;
  Ap = A->p ;
  Ai = A->i ;
  Ax = A->x ;
  for(j = 0 ; j < n ; j++)
  {
    for(p = Ap [j] ; p < Ap [j+1] ; p++)
    {
      y [Ai [p]] += alpha * Ax [p] * x [j] ;
    }
  }
  return (1) ;
}

/* NumericsMatrix : create compress column storage from triplet storage */
SparseMatrix* NM_csc(NumericsMatrix *A)
{
  if(!A->matrix3)
  {
    assert(A->matrix2);
    A->matrix3 = cs_triplet(A->matrix2); /* triplet -> csc */
  }
  return A->matrix3;
}

/* NumericsMatrix : create transposed compress column storage from
 * compress column storage */
SparseMatrix* NM_trans(NumericsMatrix* A)
{
  if(!A->matrix4)
  {
    A->matrix4 = cs_transpose(NM_csc(A), 1); /* value = 1 -> allocation */
  }
  return A->matrix4;
}


/* Numerics Matrix wrapper  for y += alpha A x + y */
void NM_aaxpy(const double alpha, NumericsMatrix* A, const double *x,
              double *y)
{
  CHECK(cs_aaxpy(alpha, NM_csc(A), x, y));
}

/* Numerics Matrix wrapper for y += alpha transpose(A) x + y */
void NM_aatxpy(const double alpha, NumericsMatrix* A, const double *x,
               double *y)
{
  CHECK(cs_aaxpy(alpha, NM_trans(A), x, y));
}

/* NumericsMatrix : initialize csc storage from sparse block storage */
void NM_setup(NumericsMatrix* A)
{
  if(!A->matrix2)
  {
    assert(A->matrix1);
    A->matrix2 = cs_spalloc(0,0,1,1,1);

    /* iteration on row, cr : current row */
    for(unsigned int cr = 0; cr < A->matrix1->filled1-1; ++cr)
    {
      for(unsigned int bn = A->matrix1->index1_data[cr];
          bn < A->matrix1->index1_data[cr + 1]; ++bn)
      {
        /* cc : current column */
        unsigned int cc = A->matrix1->index2_data[bn];
        unsigned int inbr = A->matrix1->blocksize0[cr];
        unsigned int roffset = 0;
        unsigned int coffset = 0;
        if(cr != 0)
        {
          roffset = A->matrix1->blocksize0[cr - 1];
          inbr -= roffset;
        }
        unsigned int inbc = A->matrix1->blocksize1[cc];
        if(cc != 0)
        {
          coffset = A->matrix1->blocksize1[cc - 1];
          inbc -= coffset;
        }
        for(unsigned j = 0; j < inbc; ++j)
        {
          for(unsigned i = 0; i < inbr; ++i)
          {
            CHECK(cs_zentry(A->matrix2, i + roffset, j + coffset,
                            A->matrix1->block[bn][i + j*inbr]));
          }
        }
      }
    }
  }
}

/* size of whole problem */
unsigned int sizeOfPsi(
  SparseMatrix* M,
  SparseMatrix* H)
{
  return M->n + H->n + H->m;
}

/* compute psi function */
void ACPsi(GlobalFrictionContactProblem* problem,
           double *globalVelocity,
           double *reaction,
           double *velocity,
           double *rho,
           double *psi)
{

  assert(problem->H->size1 == problem->dimension * problem->numberOfContacts);

  unsigned int localProblemSize = problem->H->size1;

  unsigned int ACProblemSize = sizeOfPsi(problem->M->matrix2,
                                         problem->H->matrix2);

  unsigned int globalProblemSize = problem->M->size0;


  /* psi <-
       compute -problem->M * globalVelocity + problem->H * reaction + problem->q
     ... */
  cblas_dscal(ACProblemSize, 0., psi, 1);
  cblas_dcopy(globalProblemSize, problem->q, 1, psi, 1);
  NM_aaxpy(1., problem->H, reaction, psi);
  NM_aaxpy(-1., problem->M, globalVelocity, psi);


  /* psi + globalProblemSize <-
     compute -velocity + trans(problem->H) * globalVelocity + problem->b
   ... */
  cblas_daxpy(localProblemSize, -1., velocity, 1, psi + globalProblemSize, 1);
  cblas_daxpy(localProblemSize, 1, problem->b, 1, psi + globalProblemSize, 1);
  NM_aatxpy(1., problem->H, globalVelocity, psi + globalProblemSize);



  /* compute AC function */
  frictionContact3D_AlartCurnierFunction(localProblemSize,
      reaction,
      velocity, problem->mu, rho,
      psi+globalProblemSize+
      problem->H->size1,
      NULL, NULL);

}





/* init memory for jacobian */
int initACPsiJacobian(
  SparseMatrix* M,
  SparseMatrix* H,
  SparseMatrix *A,
  SparseMatrix *B,
  SparseMatrix *J)
{
  /* only triplet matrix */
  assert(M->nz>=0);
  assert(H->nz>=0);
  assert(A->nz>=0);
  assert(B->nz>=0);

  /* M square */
  assert(M->m == M->n);

  /* A & B squares */
  assert(A->m == A->n);
  assert(B->m == B->n);

  assert(A->nz == B->nz);

  /* - M */
  for(unsigned int e = 0; e < M->nz; ++e)
  {
    CHECK(cs_zentry(J, M->i[e], M->p[e], - M->x[e]));
  }

  /* H */
  assert(M->n == H->m);
  for(unsigned int e = 0; e < H->nz; ++e)
  {
    CHECK(cs_zentry(J, H->i[e], H->p[e] + M->n + A->n, H->x[e]));
  }

  /* Ht */
  for(unsigned int e = 0; e < H->nz; ++e)
  {
    CHECK(cs_zentry(J, H->p[e] + M->m, H->i[e], H->x[e]));
  }

  /* -I */
  for(unsigned int e = 0; e < A->m; ++e)
  {
    CHECK(cs_zentry(J, e + M->m, e + M->n, -1.));
  }

  /* keep A start indice for update */
  int Astart = J->nz;

  /* A */
  for(unsigned int e = 0; e < A->nz; ++e)
  {
    CHECK(cs_zentry(J, A->i[e] + M->m + H->n, A->p[e] + M->n, A->x[e]));
  }

  /* B */
  for(unsigned int e = 0; e < B->nz; ++e)
  {
    CHECK(cs_zentry(J, B->i[e] + M->m + H->n, B->p[e] + M->n + A->n, B->x[e]));
  }

  return Astart;
}

/* update J with new A and B */
void updateACPsiJacobian(
  SparseMatrix* M,
  SparseMatrix* H,
  SparseMatrix *A,
  SparseMatrix *B,
  SparseMatrix *J,
  int Astart)
{
  /* only triplet matrix */
  assert(M->nz>=0);
  assert(H->nz>=0);
  assert(A->nz>=0);
  assert(B->nz>=0);

  /* M square */
  assert(M->m == M->n);

  /* A & B squares */
  assert(A->m == A->n);
  assert(B->m == B->n);


  assert(J->n == M->n + A->m + B->m);
  assert(J->m == M->m + A->n + B->n);

  assert(J->p);
  assert(J->i);
  assert(J->x);

  if(((Astart + A->nz + B->nz) > J->nzmax))
  {
    CHECK(cs_sprealloc(J, Astart + A->nz + B->nz));
  }

  /* A */
  J->nz = Astart;

  for(unsigned int e = 0; e < A->nz; ++e)
  {
    if(fabs(A->x[e]) >= DBL_EPSILON)
    {

      J->i[J->nz] = A->i[e] + M->m + H->n;
      J->p[J->nz] = A->p[e] + M->n;
      J->x[J->nz] = A->x[e];
      J->nz++;

      assert(J->nz <= J->nzmax);

    }
  }

  /* B */
  for(unsigned int e = 0; e < B->nz; ++e)
  {
    if(fabs(B->x[e]) > DBL_EPSILON)
    {
      J->i[J->nz] = B->i[e] + M->m + H->n;
      J->p[J->nz] = B->p[e] + M->n + A->n;
      J->x[J->nz] = B->x[e];
      J->nz++;

      assert(J->nz <= J->nzmax);

    }
  }
}

/* 3D lists blocks => sparse matrix */
void init3x3DiagBlocks(int nc, double* P, SparseMatrix* R)
{

  R->m = 3 * nc;
  R->n = 3 * nc;
  R->x = P;
  R->nz = 9*nc;
  R->nzmax = R->nz;

  for(unsigned int ib = 0; ib < nc; ++ib)
  {
    for(unsigned int j = 0; j < 3; ++j)
    {
      for(unsigned int i = 0; i < 3; ++i)
      {
        R->i[9*ib+i+3*j] = ib * 3 + i;
        R->p[9*ib+i+3*j] = ib * 3 + j;
      }
    }
  }
}

/* the true global line search
   (globalAlartCurnier should be renamed in localAlartCurnier)
*/
int _globalLineSearchSparseGP(
  GlobalFrictionContactProblem *problem,
  double *solution,
  double *direction,
  double *globalVelocity,
  double *reaction,
  double *velocity,
  double *mu,
  double *rho,
  double *F,
  double *psi,
  SparseMatrix *J,
  double *tmp,
  double alpha[1],
  unsigned int maxiter_ls)
{
  double inf = 1e10;
  double alphamin = 0.0;
  double alphamax = inf;

  double m1 = 0.01, m2 = 0.99;

  unsigned int ACProblemSize = sizeOfPsi(problem->M->matrix2,
                                         problem->H->matrix2);

  // Computation of q(t) and q'(t) for t =0

  double q0 = 0.5 * cblas_ddot(ACProblemSize, psi, 1, psi, 1);

  //  tmp <- J * direction
  cblas_dscal(ACProblemSize, 0., tmp, 1);
  cs_gaxpy(J, direction, tmp);

  double dqdt0 = cblas_ddot(ACProblemSize, psi, 1, tmp, 1);

  for(unsigned int iter = 0; iter < maxiter_ls; ++iter)
  {

    // tmp <- alpha*direction+solution
    cblas_dcopy(ACProblemSize, solution, 1, tmp, 1);
    cblas_daxpy(ACProblemSize, alpha[0], direction, 1, tmp, 1);

    ACPsi(problem,
          tmp,  /* v */
          tmp+problem->M->size0+problem->H->size1, /* P */
          tmp+problem->M->size0, /* U */
          rho, psi);

    double q  = 0.5 * cblas_ddot(ACProblemSize, psi, 1, psi, 1);

    assert(q >= 0);

    double slope = (q - q0) / alpha[0];

    int C1 = (slope >= m2 * dqdt0);
    int C2 = (slope <= m1 * dqdt0);

    if(C1 && C2)
    {
      if(verbose > 1)
      {
        printf("global line search success.\n");
        printf("Number of iteration = %i  alpha = %.10e, q = %.10e\n",
               iter,
               alpha[0], q);
      }

      return 0;

    }
    else if(!C1)
    {
      alphamin = alpha[0];
    }
    else
    {
      // not(C2)
      alphamax = alpha[0];
    }

    if(alpha[0] < inf)
    {
      alpha[0] = 0.5 * (alphamin + alphamax);
    }
    else
    {
      alpha[0] = alphamin;
    }

  }
  if(verbose > 1)
  {
    printf("global line search failed.\n");
    printf("max number of iteration reached  = %i  with alpha = %.10e \n",
           maxiter_ls, alpha[0]);
  }

  return -1;
}

int globalFrictionContact3D_AlartCurnier_setDefaultSolverOptions(
  SolverOptions* options)
{
  if(verbose > 0)
  {
    printf("Set the default solver options for the GLOBALAC Solver\n");
  }

  options->solverId = SICONOS_FRICTION_3D_GLOBAL_AC;
  options->numberOfInternalSolvers = 0;
  options->isSet = 1;
  options->filterOn = 1;
  options->iSize = 10;
  options->dSize = 10;
  options->iparam = (int *) malloc(options->iSize * sizeof(int));
  options->dparam = (double *) malloc(options->dSize * sizeof(double));
  options->dWork = NULL;
  options->iWork = NULL;   options->callback = NULL; options->numericsOptions = NULL;
  for(unsigned int i = 0; i < 10; i++)
  {
    options->iparam[i] = 0;
    options->dparam[i] = 0.0;
  }
  options->iparam[0] = 200;    /* input :  itermax */
  options->iparam[1] = 1;      /* output : #iter */
  options->iparam[2] = 0;      /* unused */
  options->iparam[3] = 100000; /* nzmax*/
  options->iparam[4] = 0;      /* unused */
  options->iparam[5] = 1;      /* unused */
  options->iparam[7] = 1;      /* erritermax */
  options->iparam[8] = -1;     /* mpi com fortran */

  options->iparam[9] = 0;      /* > 0 memory is allocated */

  options->dparam[0] = 1e-10;


  options->internalSolvers = NULL;

  return 0;
}

void globalFrictionContact3D_sparseGlobalAlartCurnierInit(
  SolverOptions *SO)
{
#ifdef WITH_MUMPS
  DMUMPS_STRUC_C* mumps_id = malloc(sizeof(DMUMPS_STRUC_C));

  // SO with void pointers ?
  SO->dparam[7] = (long) mumps_id;

  // Initialize a MUMPS instance. Use MPI_COMM_WORLD.
  mumps_id->job = JOB_INIT;
  mumps_id->par = 1;
  mumps_id->sym = 0;
  mumps_id->comm_fortran = USE_COMM_WORLD;
  dmumps_c(mumps_id);

  if(verbose > 0)
  {
    mumps_id->ICNTL(4) = 0;
    mumps_id->ICNTL(10) = 1;
    mumps_id->ICNTL(11) = 1;
  }
  else
  {
    mumps_id->ICNTL(1) = -1;
    mumps_id->ICNTL(2) = -1;
    mumps_id->ICNTL(3) = -1;
  }

  mumps_id->ICNTL(24) = 1; /* Null pivot row detection see also
                              CNTL(3) & CNTL(5) */

#endif
}

/* Alart & Curnier solver for sparse global problem */
void globalFrictionContact3D_AlartCurnier(
  GlobalFrictionContactProblem* problem,
  double *reaction,
  double *velocity,
  double *globalVelocity,
  int *info,
  SolverOptions* options)
{

  assert(problem);
  assert(reaction);
  assert(velocity);
  assert(info);
  assert(options);

  assert(problem->dimension == 3);

  assert(options->iparam);
  assert(options->dparam);

  assert(problem->q);
  assert(problem->mu);
  assert(problem->M);
  assert(problem->H);

  assert(!problem->M->matrix0);
  assert(problem->M->matrix1);

  assert(!options->iparam[4]); // only host

  /* M is square */
  assert(problem->M->size0 == problem->M->size1);

  assert(problem->M->size0 = problem->H->size1);

  unsigned int iter = 0;
  unsigned int itermax = options->iparam[0];
  unsigned int erritermax = options->iparam[7];

#ifdef WITH_MUMPS
  DMUMPS_STRUC_C* mumps_id = (DMUMPS_STRUC_C*)(long) options->dparam[7];
  int mumps_com = options->iparam[8];

  if(mumps_com<0)
  {
    /* we suppose mpi init has not been done */
    int ierr MAYBE_UNUSED;
    int myid;
    int argc = 0;
    char **argv;
    ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    globalFrictionContact3D_sparseGlobalAlartCurnierInit(options);
    mumps_id = (DMUMPS_STRUC_C*)(long) options->dparam[7];
  }
  else /* an mpi communicator is passed through iparam[8] so mpi init
        * has been done */
  {
    globalFrictionContact3D_sparseGlobalAlartCurnierInit(options);
    mumps_id = (DMUMPS_STRUC_C*)(long) options->dparam[7];
    mumps_id->comm_fortran = mumps_com;
  }

  assert(mumps_id);
#endif

  assert(itermax > 0);
  assert(options->iparam[3] > 0);

  double tolerance = options->dparam[0];
  assert(tolerance > 0);

  /* sparse triplet storage */
  NM_setup(problem->M);
  NM_setup(problem->H);

  unsigned int ACProblemSize = sizeOfPsi(problem->M->matrix2,
                                         problem->H->matrix2);

  unsigned int globalProblemSize = problem->M->matrix2->m;

  unsigned int localProblemSize = problem->H->size1;

  assert(localProblemSize == problem->numberOfContacts * problem->dimension);

  assert(globalProblemSize == problem->H->size0); /* size(velocity) ==
                                                   * Htrans*globalVelocity */
  if(options->iparam[9] == 0)
  {
    /* allocate memory */
    assert(options->dWork == NULL);
    assert(options->iWork == NULL);
    options->dWork = (double *) malloc(
                       (localProblemSize + /* F */
                        3 * localProblemSize + /* A */
                        3 * localProblemSize + /* B */
                        localProblemSize + /* rho */
                        ACProblemSize + /* psi */
                        ACProblemSize + /* rhs */
                        ACProblemSize + /* tmp2 */
                        ACProblemSize   /* solution */) *  sizeof(double));

    options->iWork = (int *) malloc(
                       (3 * localProblemSize + /* iA */
                        3 * localProblemSize + /* iB */
                        3 * localProblemSize + /* pA */
                        3 * localProblemSize)  /* pB */
                       * sizeof(int));

    options->iparam[9] = 1;

  }

  assert(options->dWork != NULL);
  assert(options->iWork != NULL);

  double *F = options->dWork;
  double *A = F +   localProblemSize;
  double *B = A +   3 * localProblemSize;
  double *rho = B + 3 * localProblemSize;

  double * psi = rho + localProblemSize;
  double * rhs = psi + ACProblemSize;
  double * tmp2 = rhs + ACProblemSize;
  double * solution = tmp2 + ACProblemSize;

  int * iA = options->iWork;
  int * iB = iA + 3 * localProblemSize;
  int * pA = iB + 3 * localProblemSize;
  int * pB = pA + 3 * localProblemSize;

  SparseMatrix A_;
  SparseMatrix B_;
  SparseMatrix *J;

  A_.p = pA;
  B_.p = pB;
  A_.i = iA;
  B_.i = iB;

  init3x3DiagBlocks(problem->numberOfContacts, A, &A_);
  init3x3DiagBlocks(problem->numberOfContacts, B, &B_);

  J = cs_spalloc(problem->M->matrix2->n + A_.m + B_.m,
                 problem->M->matrix2->n + A_.m + B_.m,
                 problem->M->matrix2->nzmax + 2*problem->H->matrix2->nzmax +
                 2*A_.n + A_.nzmax + B_.nzmax, 1, 1);

  assert(A_.n == problem->H->size1);
  assert(A_.nz == problem->numberOfContacts * 9);
  assert(B_.n == problem->H->size1);
  assert(B_.nz == problem->numberOfContacts * 9);

  frictionContact3D_AlartCurnierFunction(localProblemSize,
      reaction, velocity,
      problem->mu, rho,
      F, A, B);

  int Astart = initACPsiJacobian(problem->M->matrix2,
                                 problem->H->matrix2,
                                 &A_, &B_, J);

  assert(Astart > 0);

  assert(A_.m == A_.n);
  assert(B_.m == B_.n);

  assert(A_.m == problem->H->size1);

  // compute rho here
  for(unsigned int i = 0; i < localProblemSize; ++i) rho[i] = 1.;

  // direction
  for(unsigned int i = 0; i < ACProblemSize; ++i) rhs[i] = 0.;


#ifdef WITH_MUMPS
  mumps_id->n = J->m;
  mumps_id->nz = J->nz;
  mumps_id->irn = J->i;
  mumps_id->jcn = J->p;
  mumps_id->a = J->x;
  mumps_id->rhs = rhs;

  mumps_id->job = 6;
#endif

  info[0] = 1;

  /* update local velocity from global velocity */
  /* an assertion ? */
  cblas_dcopy(localProblemSize, problem->b, 1, velocity, 1);
  NM_aatxpy(1., problem->H, globalVelocity, velocity);

  while(iter++ < itermax)
  {

    /* compute psi */
    ACPsi(problem, globalVelocity, reaction, velocity, rho, psi);

    /* compute A & B */
    frictionContact3D_AlartCurnierFunction(localProblemSize,
        reaction, velocity,
        problem->mu, rho,
        F, A, B);
    /* update J */
    updateACPsiJacobian(problem->M->matrix2,
                        problem->H->matrix2,
                        &A_, &B_, J, Astart);

    /* rhs = -psi */
    cblas_dcopy(ACProblemSize, psi, 1, rhs, 1);
    cblas_dscal(ACProblemSize, -1., rhs, 1);

    /* get compress column storage for linear ops */
    SparseMatrix* Jcsc = cs_triplet(J);

    /* Solve: J X = -psi */

#ifdef WITH_MUMPS
    mumps_id->n = J->m;
    mumps_id->nz = J->nz;

    /* set fortran indices in place so we do not have to bother with
     * yet another heap allocation */
    for(unsigned int e = 0 ; e < J->nz ; ++e)
    {
      J->i[e]++;
      J->p[e]++;
    }

    dmumps_c(mumps_id);

    /* come back to C indices */
    for(unsigned int e = 0 ; e < J->nz ; ++e)
    {
      J->i[e]--;
      J->p[e]--;
    }

    assert(mumps_id->info[0] >= 0);

    if(mumps_id->info[0] > 0)
      /*if (verbose>0)*/
      printf("GLOBALAC: MUMPS warning : info(1)=%d, info(2)=%d\n",
             mumps_id->info[0], mumps_id->info[1]);


    if(verbose > 0)
    {

      printf("mumps : condition number %g\n", mumps_id->rinfog[9]);
      printf("mumps : component wise scaled residual %g\n",
             mumps_id->rinfog[6]);
      printf("mumps : \n");

    }
#else
    /* use csparse LU factorization */
    CHECK(cs_lusol(Jcsc, rhs, 1, DBL_EPSILON));

#endif

    /* line search */
    double alpha = 1;

    /* set current solution */
    for(unsigned int i = 0; i < globalProblemSize; ++i)
    {
      solution[i] = globalVelocity[i];
    }
    for(unsigned int i = 0; i < localProblemSize; ++i)
    {
      solution[i+globalProblemSize] = velocity[i];
      solution[i+globalProblemSize+localProblemSize] = reaction[i];
    }

    int info_ls = _globalLineSearchSparseGP(problem,
                                            solution,
                                            rhs,
                                            globalVelocity,
                                            reaction, velocity,
                                            problem->mu, rho, F, psi, Jcsc,
                                            tmp2, &alpha, 100);


    cs_spfree(Jcsc);

    if(!info_ls)
    {
      cblas_daxpy(ACProblemSize, alpha, rhs, 1, solution, 1);
    }
    else
    {
      cblas_daxpy(ACProblemSize, 1, rhs, 1., solution, 1);
    }

    for(unsigned int e = 0 ; e < globalProblemSize; ++e)
    {
      globalVelocity[e] = solution[e];
    }

    for(unsigned int e = 0 ; e < localProblemSize; ++e)
    {
      velocity[e] = solution[e+globalProblemSize];
    }

    for(unsigned int e = 0; e < localProblemSize; ++e)
    {
      reaction[e] = solution[e+globalProblemSize+localProblemSize];
    }

    options->dparam[1] = INFINITY;

    if(!(iter % erritermax))
    {

      GlobalFrictionContact3D_compute_error(problem,
                                            reaction, velocity, globalVelocity,
                                            tolerance,
                                            &(options->dparam[1]));

    }

    if(verbose > 0)
      printf("GLOBALAC: iteration %d : error=%g\n", iter, options->dparam[1]);

    if(options->dparam[1] < tolerance)
    {
      info[0] = 0;
      break;
    }


  }

  if(verbose > 0)
  {
    if(!info[0])
      printf("GLOBALAC: convergence after %d iterations, error : %g\n",
             iter, options->dparam[1]);
    else
    {
      printf("GLOBALAC: no convergence after %d iterations, error : %g\n",
             iter, options->dparam[1]);
    }
  }

  options->iparam[1] = iter;

#ifdef DUMP_PROBLEM
  if(info[0])
  {
    static int file_counter = 0;
    char filename[64];
    printf("GLOBALAC: dumping problem\n");
    sprintf(filename, "GLOBALAC_failure%d.dat", file_counter++);
    FILE* file = fopen(filename, "w");
    frictionContact_printInFile(problem, file);
    fclose(file);
  }
#endif

  {
    cs_spfree(J);
  }
}



