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
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdint.h>

#include "NumericsMatrix.h"
#include "SiconosLapack.h"
#include "misc.h"
#include "GlobalFrictionContact3D_AlartCurnier.h"

void prodNumericsMatrix(int sizeX, int sizeY, double alpha, NumericsMatrix* A, const double* const x, double beta, double* y)
{

  assert(A);
  assert(x);
  assert(y);
  assert(A->size0 == sizeY);
  assert(A->size1 == sizeX);

  int storage = A->storageType;

  /* double* storage */
  switch (storage)
  {
    case NM_DENSE:
      cblas_dgemv(CblasColMajor, CblasNoTrans, sizeY, sizeX, alpha, A->matrix0, sizeY, x, 1, beta, y, 1);
    break;
  /* SparseBlock storage */
    case NM_SPARSE_BLOCK:
      prodSBM(sizeX, sizeY, alpha, A->matrix1, x, beta, y);
    break;
  /* coordinate */
    case NM_TRIPLET:
      cs_aaxpy(alpha, NM_triplet(A), x, beta, y);
    break;

    default:
    fprintf(stderr, "Numerics, NumericsMatrix, product matrix - vector prod(A,x,y) failed, unknown storage type for A.\n");
    exit(EXIT_FAILURE);
  }

}

void prodNumericsMatrixNumericsMatrix(double alpha, const NumericsMatrix* const A, const NumericsMatrix* const B, double beta,  NumericsMatrix* C)
{

  assert(A);
  assert(B);
  assert(C);
  int astorage = A->storageType;
  int bstorage = B->storageType;
  int cstorage = C->storageType;
  assert(A->size1 == B->size0);
  assert(C->size0 == A->size0);
  assert(C->size1 == B->size1);


  /* double* storage */
  if ((astorage == NM_DENSE) && (bstorage == NM_DENSE) && (cstorage == NM_DENSE))
  {
    /*      cblas_dgemv(CblasColMajor,CblasNoTrans, sizeY, sizeX, alpha, A->matrix0, sizeY, x, 1, beta, y, 1); */
    cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, A->size0, B->size1, A->size1, alpha, A->matrix0, A->size0, B->matrix0, B->size0, beta, C->matrix0, C->size0);
  }
  /* SparseBlock storage */
  else if ((astorage == NM_SPARSE_BLOCK) && (bstorage == NM_SPARSE_BLOCK) && (cstorage == NM_SPARSE_BLOCK))
  {
    prodSBMSBM(alpha, A->matrix1, B->matrix1, beta, C->matrix1);
  }
  else
  {
    fprintf(stderr, "Numerics, NumericsMatrix, product matrix - matrix prod(A,B,C) not yet implemented.\n");
    exit(EXIT_FAILURE);
  }
}

void subRowProd(int sizeX, int sizeY, int currentRowNumber, const NumericsMatrix* A, const double* const x, double* y, int init)
{

  assert(A);
  assert(x);
  assert(y);
  assert(A->size0 >= sizeY);
  assert(A->size1 == sizeX);

  int storage = A->storageType;

  /* double* storage */
  if (storage == 0)
  {
    int incx = A->size0, incy = 1;
    double* mat = A->matrix0;
    if (init == 0) /* y += subAx */
    {
      for (int row = 0; row < sizeY; row++)
        y[row] += cblas_ddot(sizeX, &mat[currentRowNumber + row], incx, x, incy);
    }
    else
    {
      for (int row = 0; row < sizeY; row++)
        y[row] = cblas_ddot(sizeX, &mat[currentRowNumber + row], incx, x, incy);
    }

  }
  /* SparseBlock storage */
  else if (storage == 1)
    subRowProdSBM(sizeX, sizeY, currentRowNumber, A->matrix1, x, y, init);
  else
  {
    fprintf(stderr, "Numerics, NumericsMatrix, product matrix - vector subRowProd(A,x,y) failed, unknown storage type for A.\n");
    exit(EXIT_FAILURE);
  }

}

void rowProdNoDiag(int sizeX, int sizeY, int currentRowNumber, const NumericsMatrix* A, const double* const x, double* y, int init)
{

  assert(A);
  assert(x);
  assert(y);
  assert(A->size0 >= sizeY);
  assert(A->size1 == sizeX);

  /* Checks storage type */
  int storage = A->storageType;


  /* double* storage */
  if (storage == 0)
  {
    double * xSave = (double*) malloc(sizeY * sizeof(double));
    double * xx = (double *)x; /*because of const*/
    for (int i = 0; i < sizeY; i++)
    {
      xSave[i] = x[currentRowNumber + i];
      xx[currentRowNumber + i] = 0;
    }
    double * MM = A->matrix0 + currentRowNumber;
    int incx = A->size0;
    int incy = 1;
    if (init)
    {
      for (int i = 0; i < sizeY; i++)
        y[i] = 0;
    }
    for (int i = 0; i < sizeY; i++)
    {
      y[i] += cblas_ddot(A->size0 , MM + i , incx , x , incy);
    }
    for (int i = 0; i < sizeY; i++)
    {
      xx[currentRowNumber + i] = xSave[i];
    }
    free(xSave);

  }
  else if (storage == 1)
    rowProdNoDiagSBM(sizeX, sizeY, currentRowNumber, A->matrix1, x, y, init);
  else
  {
    fprintf(stderr, "Numerics, NumericsMatrix, product matrix - vector rowProdNoDiag(A,x,y) failed, unknown storage type for A.\n");
    exit(EXIT_FAILURE);
  }
}


void freeNumericsMatrix(NumericsMatrix* m)
{
  if (m->storageType == 0)
  {
    if (m->matrix0)
      free(m->matrix0);
    m->matrix0 = NULL;
  }
  else
  {
    freeSBM(m->matrix1);
    free(m->matrix1);
    m->matrix1 = NULL;
    if (m->matrix2)
    {
      freeNumericsSparseMatrix(m->matrix2);
      free(m->matrix2);
      m->matrix2 = NULL;
    }
  }
  if (m->internalData)
  {
    if (m->internalData->iWork)
    {
      assert (m->internalData->iWorkSize > 0);
      free(m->internalData->iWork);
      m->internalData->iWork = NULL;
    }
    free(m->internalData);
    m->internalData = NULL;
  }
}


void displayMat(double * m, int nRow, int nCol, int lDim)
{
  int lin, col;
  if (lDim == 0)
    lDim = nRow;
  printf("Matrix of size\t%d\t%d =\n[", nRow, nCol);
  if (nRow == 0)
  {
    printf("]\n");
  }
  if (nCol == 0)
  {
    printf("]\n");
  }

  for (lin = 0; lin < nRow; lin++)
  {
    for (col = 0; col < nCol; col++)
    {
      printf(" %.15e", m[lin + col * lDim]);
      if (col != nCol - 1)
        printf(",");
    }
    if (lin != nRow - 1)
      printf(";\n");
    else
      printf("]\n");
  }

}

void display(const NumericsMatrix* const m)
{
  if (! m)
  {
    fprintf(stderr, "Numerics, NumericsMatrix display failed, NULL input.\n");
    exit(EXIT_FAILURE);
  }
  int storageType = m->storageType;
  if (storageType == 0)
  {
    printf("\n ========== Numerics Matrix\n");
    displayMat(m->matrix0, m->size0, m->size1, m->size0);
    /* printf("["); */
    /* for (int i = 0; i<m->size0; i++) */
    /* {     */
    /*   printf("["); */
    /*   for (int j = 0; j<m->size1; j++) */
    /*   { */
    /*     printf("%lf ",m->matrix0[i+j*m->size0]); */
    /*   } */
    /*   printf("]\n"); */
    /* }   */
  }
  else if (storageType == 1)
    printSBM(m->matrix1);
}
void displayRowbyRow(const NumericsMatrix* const m)
{
  if (! m)
  {
    fprintf(stderr, "Numerics, NumericsMatrix display failed, NULL input.\n");
    exit(EXIT_FAILURE);
  }
  int storageType = m->storageType;
  if (storageType == 0)
  {
    printf("\n ========== Numerics Matrix of dim %dX%d\n", m->size0, m->size1);
    for (int lin = 0; lin < m->size0; lin++)
    {
      for (int col = 0; col < m->size1; col++)
        printf("%lf ", m->matrix0[lin + col * m->size1]);
      printf("\n");
    }
  }
  else if (storageType == 1)
    printSBM(m->matrix1);
}

void printInFile(const NumericsMatrix* const m, FILE* file)
{
  if (! m)
  {
    fprintf(stderr, "Numerics, NumericsMatrix printInFile failed, NULL input.\n");
    exit(EXIT_FAILURE);
  }
  int storageType = m->storageType;
  fprintf(file, "%d\n", m->storageType);
  fprintf(file, "%d\n", m->size0);
  fprintf(file, "%d\n", m->size1);

  if (storageType == 0)
  {
    fprintf(file, "%i\t%i\n", m->size0, m->size1);
    for (int i = 0; i < m->size1 * m->size0; i++)
    {
      fprintf(file, "%32.24e ", m->matrix0[i]);
      if ((i + 1) % m->size1 == 0)
        fprintf(file, "\n");
    }
  }
  else if (storageType == 1)
    printInFileSBM(m->matrix1, file);
}

void printInFileForScilab(const NumericsMatrix* const m, FILE* file)
{
  if (! m)
  {
    fprintf(stderr, "Numerics, NumericsMatrix printInFile failed, NULL input.\n");
    exit(EXIT_FAILURE);
  }
  int storageType = m->storageType;
  fprintf(file, "storageType = %d ; \n", m->storageType);
  fprintf(file, "size0 = %d; \n", m->size0);
  fprintf(file, "size1 = %d; \n", m->size1);

  if (storageType == 0)
  {
    fprintf(file, "data= [");
    for (int i = 0; i < m->size0; i++)
    {
      fprintf(file, "[");
      for (int j = 0; j < m->size1; j++)
      {
        fprintf(file, "%32.24e,\t ", m->matrix0[i + j * m->size0]);
      }
      fprintf(file, "];\n");
    }
    fprintf(file, "]");
  }
  else if (storageType == 1)
  {
    printInFileSBMForScilab(m->matrix1, file);
    /*       fprintf(stderr,"Numerics, NumericsMatrix printInFileForScilab. Not yet implemented fo storageType = %i.\n", storageType); */
    /*       exit(EXIT_FAILURE); */

  }
}

void printInFileName(const NumericsMatrix* const m, const char *filename)
{
  FILE* foutput = fopen(filename, "w");
  printInFile(m, foutput);
  fclose(foutput);
}

void readInFile(NumericsMatrix* const m, FILE *file)
{
  if (! m)
  {
    fprintf(stderr, "Numerics, NumericsMatrix readInFile failed, NULL input.\n");
    exit(EXIT_FAILURE);
  }
  CHECK_IO(fscanf(file, "%d", &(m->storageType)));
  CHECK_IO(fscanf(file, "%d", &(m->size0)));
  CHECK_IO(fscanf(file, "%d", &(m->size1)));
  int storageType = m->storageType;

  if (storageType == 0)
  {
    CHECK_IO(fscanf(file, "%d\t%d\n", &(m->size0), &(m->size1)));

    for (int i = 0; i < m->size1 * m->size0; i++)
    {
      CHECK_IO(fscanf(file, "%le ", &(m->matrix0[i])));
    }


  }
  else if (storageType == 1)
  {
    m->matrix0 = NULL;
    readInFileSBM(m->matrix1, file);
  }
}


void newFromFile(NumericsMatrix* const m, FILE *file)
{
  if (! m)
  {
    fprintf(stderr, "Numerics, NumericsMatrix newFromFile failed, NULL input.\n");
    exit(EXIT_FAILURE);
  }

  int storageType;
  size_t size0;
  size_t size1;
  void* data = NULL;

  CHECK_IO(fscanf(file, "%d", &storageType));
  CHECK_IO(fscanf(file, "%zu",&size0));
  CHECK_IO(fscanf(file, "%zu", &size1));

  if (storageType == NM_DENSE)
  {
    CHECK_IO(fscanf(file, "%zu\t%zu\n", &size0, &size1));

    data = malloc(size1 * size0 * sizeof(double));
    double* data_d = (double*) data;

    for (size_t i = 0; i < size1 * size0; ++i)
    {
      CHECK_IO(fscanf(file, "%le ", &(data_d[i])));
    }
  }
  else if (storageType == NM_SPARSE_BLOCK)
  {
    data = malloc(sizeof(SparseBlockStructuredMatrix));
    newFromFileSBM((SparseBlockStructuredMatrix*)data, file);
  }

  fillNumericsMatrix(m, storageType, (int)size0, (int)size1, data);
}

void readInFileName(NumericsMatrix* const m, const char *filename)
{
  FILE* finput = fopen(filename, "r");
  printInFile(m, finput);
  fclose(finput);
}

void readInFileForScilab(NumericsMatrix* const M, FILE *file)
{
  fprintf(stderr, "Numerics, NumericsMatrix,readInFileForScilab");
  exit(EXIT_FAILURE);
}

void getDiagonalBlock(NumericsMatrix* m, int numBlockRow, int numRow, int size, double ** Bout)
{
  int storageType = m->storageType;
  if (storageType == 0)
  {
    double * MM = m->matrix0;
    double * elem = 0;
    /* The part of MM which corresponds to the current block is copied into MLocal */
    for (int i = 0; i < size; i++)
    {
      elem = MM + numRow + (numRow + i) * (m->size0);
      for (int j = 0; j < size; j++)
      {
        (*Bout)[j + i * size] = *elem;
        elem++;
      }
    }
  }
  else if (storageType == 1)
  {
    int diagPos = getDiagonalBlockPos(m->matrix1, numBlockRow);
    (*Bout) = m->matrix1->block[diagPos];

  }
}

NumericsMatrix* createNumericsMatrixFromData(int storageType, int size0, int size1, void* data)
{
  NumericsMatrix* M = (NumericsMatrix*) malloc(sizeof(NumericsMatrix));

  fillNumericsMatrix(M, storageType, size0, size1, data);

  return M;
}

NumericsMatrix* duplicateNumericsMatrix(NumericsMatrix* mat)
{
  NumericsMatrix* M = (NumericsMatrix*) malloc(sizeof(NumericsMatrix));

  void* data;
  int size0 = mat->size0;
  int size1 = mat->size1;

  M->storageType = mat->storageType;
  switch (mat->storageType)
  {
    case NM_DENSE:
      data = malloc(size0*size1*sizeof(double));
      break;
    case NM_SPARSE_BLOCK:
      data = malloc(sizeof(SparseBlockStructuredMatrix));
      break;
    case NM_TRIPLET:
      data = malloc(sizeof(CSparseMatrix));
      break;
    default:
      printf("createNumericsMatrix :: storageType value %d not implemented yet !", mat->storageType);
      exit(EXIT_FAILURE);
  }

  fillNumericsMatrix(M, mat->storageType, size0, size1, data);

  return M;
}


NumericsMatrix* createNumericsMatrix(int storageType, int size0, int size1)
{
  NumericsMatrix* M = (NumericsMatrix*) malloc(sizeof(NumericsMatrix));

  void* data;

  switch (storageType)
  {
    case NM_DENSE:
      data = malloc(size0*size1*sizeof(double));
      break;
    case NM_SPARSE_BLOCK:
      data = malloc(sizeof(SparseBlockStructuredMatrix));
      break;
    case NM_TRIPLET:
      data = malloc(sizeof(CSparseMatrix));
      break;
    default:
      printf("createNumericsMatrix :: storageType value %d not implemented yet !", storageType);
      exit(EXIT_FAILURE);
  }

  fillNumericsMatrix(M, storageType, size0, size1, data);

  return M;
}


void fillNumericsMatrix(NumericsMatrix* M, int storageType, int size0, int size1, void* data)
{
  M->storageType = storageType;
  M->size0 = size0;
  M->size1 = size1;

  M->matrix0 = NULL;
  M->matrix1 = NULL;
  M->matrix2 = NULL;
  M->internalData = NULL;

  if (data)
  {
    switch (storageType)
    {
      case NM_DENSE:
        M->matrix0 = (double*) data;
        break;
      case NM_SPARSE_BLOCK:
        M->matrix1 = (SparseBlockStructuredMatrix*) data;
        break;
      case NM_TRIPLET:
        M->matrix2 = (NumericsSparseMatrix*) data;
        break;

      default:
        printf("fillNumericsMatrix :: storageType value %d not implemented yet !", storageType);
        exit(EXIT_FAILURE);
    }
  }
}

NumericsMatrix* newSparseNumericsMatrix(int size0, int size1, SparseBlockStructuredMatrix* m1)
{
  return createNumericsMatrixFromData(NM_SPARSE_BLOCK, size0, size1, (void*)m1);
}

NumericsSparseMatrix* NM_sparse(NumericsMatrix* A)
{
  if(!A->matrix2)
  {
    A->matrix2 = newNumericsSparseMatrix();
  }
  return A->matrix2;
}

NumericsSparseLinearSolverParams* NM_linearSolverParams(NumericsMatrix* A)
{
  if(!NM_sparse(A)->linearSolverParams)
  {
    NM_sparse(A)->linearSolverParams = newNumericsSparseLinearSolverParams();
  }
  return NM_sparse(A)->linearSolverParams;
}


/* NumericsMatrix : initialize csc storage from sparse block storage */
CSparseMatrix* NM_triplet(NumericsMatrix* A)
{
  if(!NM_sparse(A)->triplet)
  {
    assert(A->matrix1);
    A->matrix2->triplet = cs_spalloc(0,0,1,1,1);

    /* iteration on row, cr : current row */
    for(unsigned int cr = 0; cr < A->matrix1->filled1-1; ++cr)
    {
      for(size_t bn = A->matrix1->index1_data[cr];
          bn < A->matrix1->index1_data[cr + 1]; ++bn)
      {
        /* cc : current column */
        size_t cc = A->matrix1->index2_data[bn];
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
            CHECK_RETURN(cs_zentry(A->matrix2->triplet, i + roffset, j + coffset,
                                   A->matrix1->block[bn][i + j*inbr]));
          }
        }
      }
    }
  }
  return A->matrix2->triplet;
}

CSparseMatrix* NM_csc(NumericsMatrix *A)
{
  if(!NM_sparse(A)->csc)
  {
    assert(A->matrix2);
    A->matrix2->csc = cs_triplet(NM_triplet(A)); /* triplet -> csc */
  }
  return A->matrix2->csc;
}

CSparseMatrix* NM_csc_trans(NumericsMatrix* A)
{
  if(!NM_sparse(A)->trans_csc)
  {
    assert(A->matrix2);
    A->matrix2->trans_csc = cs_transpose(NM_csc(A), 1); /* value = 1 -> allocation */
  }
  return A->matrix2->trans_csc;
}

/* Numerics Matrix wrapper  for y <- alpha A x + beta y */
void NM_gemv(const double alpha, NumericsMatrix* A, const double *x,
             const double beta, double *y)
{
  switch (A->storageType)
  {
  case NM_DENSE:
    cblas_dgemv(CblasColMajor, CblasNoTrans, A->size0, A->size1,
                alpha, A->matrix0, A->size0, x, 1, beta, y, 1);
    break;
  case NM_SPARSE_BLOCK:
    prodSBM(A->size1, A->size0, alpha, A->matrix1, x, beta, y);
    break;

  default:
    assert(A->storageType == NM_TRIPLET);
    CHECK_RETURN(cs_aaxpy(alpha, NM_csc(A), x, beta, y));
  }
}

/* Numerics Matrix wrapper  for y <- alpha trans(A) x + beta y */
void NM_tgemv(const double alpha, NumericsMatrix* A, const double *x,
              const double beta, double *y)
{
  switch (A->storageType)
  {
    case NM_DENSE:
      {
        cblas_dgemv(CblasColMajor, CblasTrans, A->size0, A->size1,
            alpha, A->matrix0, A->size0, x, 1, beta, y, 1);
        break;
      }
    case NM_SPARSE_BLOCK:
    case NM_TRIPLET:
      {
        CHECK_RETURN(cs_aaxpy(alpha, NM_csc_trans(A), x, beta, y));
        break;
      }
    default:
      {
        assert(0 && "NM_tgemv unknown storageType");
      }
  }
}

NumericsMatrixInternalData* NM_internalData(NumericsMatrix* A)
{
  if (!A->internalData)
  {
    NM_alloc_internalData(A);
  }
  A->internalData->iWork = NULL;
  A->internalData->iWorkSize = 0;

  return A->internalData;
}

int* NM_iWork(NumericsMatrix* A, int size)
{
  if (!NM_internalData(A)->iWork)
  {
    assert(A->internalData);

    assert(A->internalData->iWorkSize == 0);
    A->internalData->iWork = (int *) malloc(size * sizeof(int));
    A->internalData->iWorkSize = size;
  }
  else
  {
    assert(A->internalData);

    if (size > A->internalData->iWorkSize)
    {
      A->internalData->iWork = (int *) realloc(A->internalData->iWork, size * sizeof(int));
      A->internalData->iWorkSize = size;
    }
  }

  assert(A->internalData->iWork);
  assert(A->internalData->iWorkSize >= size);

  return A->internalData->iWork;
}

#ifdef WITH_MUMPS

MPI_Comm NM_MPI_com(NumericsMatrix* A)
{
  if (NM_linearSolverParams(A)->mpi_com == MPI_COMM_NULL)
  {
    int myid;
    int argc = 0;
    char **argv;
    CHECK_MPI(MPI_Init(&argc, &argv));
    CHECK_MPI(MPI_Comm_rank(MPI_COMM_WORLD, &myid));
    NM_linearSolverParams(A)->mpi_com = MPI_COMM_WORLD;
  }
  return NM_linearSolverParams(A)->mpi_com;
}


int* NM_MUMPS_irn(NumericsMatrix* A)
{
  NumericsSparseLinearSolverParams* params = NM_linearSolverParams(A);

  if(!params->iWork)
  {
    CSparseMatrix* triplet = NM_triplet(A);
    int nz = triplet->nz;

    params->iWork = (int *) malloc(2 * nz * sizeof(int));
    params->iWorkSize = 2 * nz;

    int *p_mumps_irn = params->iWork;
    int *p_A_irn = triplet->i;

    for (int i=0; i<nz ; ++i, *p_mumps_irn++ = *p_A_irn++ + 1);
  }
  return params->iWork;
}

int* NM_MUMPS_jcn(NumericsMatrix* A)
{
  NumericsSparseLinearSolverParams* params = NM_linearSolverParams(A);

  CSparseMatrix* triplet = NM_triplet(A);
  int nz = triplet->nz;

  if(!params->iWork)
  {
    params->iWork = (int *) malloc(2 * nz * sizeof(int));
    params->iWorkSize = 2 * nz;

    int *p_mumps_jcn = params->iWork + nz;
    int *p_A_jcn = triplet->p;

    for (int i=0; i<nz ; ++i, *p_mumps_jcn++ = *p_A_jcn++ + 1);
  }
  return params->iWork + nz;
}


DMUMPS_STRUC_C* NM_MUMPS_id(NumericsMatrix* A)
{
  NumericsSparseLinearSolverParams* params = NM_linearSolverParams(A);

  if (!params->solver_data)
  {
    params->solver_data = malloc(sizeof(DMUMPS_STRUC_C));

    DMUMPS_STRUC_C* mumps_id = (DMUMPS_STRUC_C*) params->solver_data;

    // Initialize a MUMPS instance. Use MPI_COMM_WORLD.
    mumps_id->job = JOB_INIT;
    mumps_id->par = 1;
    mumps_id->sym = 0;

    if (NM_MPI_com(A) == MPI_COMM_WORLD)
    {
      mumps_id->comm_fortran = USE_COMM_WORLD;
    }
    else
    {
      mumps_id->comm_fortran = MPI_Comm_c2f(NM_MPI_com(A));
    }

    dmumps_c(mumps_id);

    if (verbose > 1)
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

    mumps_id->ICNTL(24) = 1; // Null pivot row detection see also CNTL(3) & CNTL(5)
    // ok for a cube on a plane & four contact points
    // computeAlartCurnierSTD != generated in this case...

    //mumps_id->CNTL(3) = ...;
    //mumps_id->CNTL(5) = ...;

    mumps_id->n = NM_triplet(A)->n;
    mumps_id->nz = NM_triplet(A)->nz;
    mumps_id->irn = NM_MUMPS_irn(A);
    mumps_id->jcn = NM_MUMPS_jcn(A);
    mumps_id->a = NM_triplet(A)->x;

  }
  return (DMUMPS_STRUC_C*) params->solver_data;
}
#endif

int NM_gesv(NumericsMatrix* A, double *b)
{
  assert(A->size0 == A->size1);

  int info = 1;

  switch (A->storageType)
  {
  case NM_DENSE:
  {
    assert(A->matrix0);
    DGESV(A->size0, 1, A->matrix0, A->size0, NM_iWork(A, A->size0), b,
          A->size0, &info);
    break;
  }

  case NM_SPARSE_BLOCK: /* sparse block -> triplet */
  case NM_TRIPLET:
  {
    switch (NM_sparse(A)->linearSolver)
    {
    case NS_CS_LUSOL:
      info = cs_lusol(NM_triplet(A), b, 1, DBL_EPSILON);
      break;

#ifdef WITH_MUMPS
    case NS_MUMPS:
    {
      DMUMPS_STRUC_C* mumps_id = NM_MUMPS_id(A);
      mumps_id->rhs = b;
      mumps_id->job = 6;

      dmumps_c(mumps_id);

      info = mumps_id->info[0];

      if (mumps_id->info[0] > 0)
      {
        if (verbose > 0)
        {
          printf("NM_gesv: MUMPS fails : info(1)=%d, info(2)=%d\n", mumps_id->info[0], mumps_id->info[1]);
        }
      }
      if (verbose > 0)
      {
        printf("MUMPS : condition number %g\n", mumps_id->rinfog[9]);
        printf("MUMPS : component wise scaled residual %g\n", mumps_id->rinfog[6]);
        printf("MUMPS : \n");
      }
      break;
    }
#endif
    default:
    {
      fprintf(stderr, "NM_gesv: unknown sparse linearsolver : %d\n", NM_sparse(A)->linearSolver);
      exit(EXIT_FAILURE);
    }
    }
    break;
  }

  default:
    assert (0 && "NM_gesv unknown storageType");
  }

  /* some time we cannot find a solution to a linear system, and its fine, for
   * instance with the minFBLSA. Therefore, we should not check here for
   * problems, but the calling function has to check the return code.*/
//  CHECK_RETURN(info);
  return info;
}
