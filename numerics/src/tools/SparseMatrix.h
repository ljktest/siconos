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

#ifndef SparseMatrix_H
#define SparseMatrix_H

/*! \page SparseMatrixPage Sparse Matrix Storage in Numerics

Documentation to be done

*/


/*!\file SparseMatrix.h
  \brief Structure definition and functions related to sparse matrix storage in Numerics
*/

#include "SiconosConfig.h"

#ifdef HAVE_MPI
#include <mpi.h>
#endif

#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
extern "C"
{
#endif
#include "csparse.h"
#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
}
#endif

/*  we use csparse from Timothy Davis

    Timothy Davis,
    Direct Methods for Sparse Linear Systems,
    SIAM, 2006,
    ISBN: 0898716136,
    LC: QA188.D386.

   matrix in compressed row/column or triplet form :
{
int nzmax ;   : maximum number of entries 
int m  ;      : number of rows 
int n ;       : number of columns 
int *p ;      : compressed: row (size m+1) or column (size n+1) pointers; triplet: row indices (size nz) 
int *i ;      : compressed: column or row indices, size nzmax; triplet: column indices (size nz) 
double *x ;   :  numerical values, size nzmax 
int nz ;      : # of entries in triplet matrix;
-1 for compressed columns;
-2 for compressed rows 

} */


#define CSparseMatrix struct cs_sparse

#define NS_UNKNOWN_ERR(func, orig) \
fprintf(stderr, #func ": unknown origin %d for sparse matrix\n", orig);



#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
extern "C"
{
#endif

  /** \struct cs_lu_factors SparseMatrix.h
   * Information used and produced by CSparse for an LU factorization*/
  typedef struct {
    csi n;       /**< size of linear system */
    css* S;      /**< symbolic analysis */
    csn* N;      /**< numerics factorization */
  } cs_lu_factors;

  typedef void (*freeNSLSP)(void* p);

  typedef enum { NS_CS_LUSOL, NS_MUMPS, NS_UMFPACK, NS_PARDISO } NumericsSparseLinearSolver;

  /** \struct NumericsSparseLinearSolverParams SparseMatrix.h
   * solver-specific parameters*/
  typedef struct
  {
    NumericsSparseLinearSolver solver;

    int* iparam;
    double iSize;
    double* dparam;
    double dSize;

    void* solver_data; /**< solver-specific data (or workspace) */
    freeNSLSP solver_free_hook; /**< solver-specific hook to free solver_data  */

    int* iWork; /**< integer work vector array (internal) */
    int iWorkSize; /**< size of integer work vector array */
    double* dWork;
    int dWorkSize;
  } NumericsSparseLinearSolverParams;

  typedef enum { NS_UNKNOWN, NS_TRIPLET, NS_CSC, NS_CSR } NumericsSparseOrigin;

  /** \struct NumericsSparseMatrix SparseMatrix.h
   * Sparse matrix representation in Numerics. The supported format are:
   * triplet (aka coordinate, COO), CSC (via CSparse) and CSR if MKL is used */
  typedef struct
  {
    NumericsSparseLinearSolverParams* linearSolverParams;
                               /**< solver-specific parameters */
    CSparseMatrix* triplet;    /**< triplet format, aka coordinate */
    CSparseMatrix* csc;        /**< csc matrix */
    CSparseMatrix* trans_csc;  /**< transpose of a csc matrix (used by CSparse) */
    CSparseMatrix* csr;        /**< csr matrix, only supported with mkl */
    unsigned       origin;     /**< original format of the matrix */
  } NumericsSparseMatrix;


  /** Add an entry to a triplet matrix only if the absolute value is
   * greater than DBL_EPSILON.
   * \param T the sparse matrix
   * \param i row index
   * \param j column index
   * \param x the value
   * \return integer value : 1 if the absolute value is less than
   * DBL_EPSILON, otherwise the return value of cs_entry.
   */
  csi cs_zentry(CSparseMatrix *T, csi i, csi j, double x);

  /** Check if the given triplet matrix is properly constructed (col and row indices are correct)
   * \param T the sparse matrix to check
   * \return 0 if the matrix is fine, 1 otherwise
   * */
  int cs_check_triplet(CSparseMatrix *T);

  /** Create dense matrix from a CSparseMatrix.
   * \param A the sparse matrix
   * \return a pointer on A->m * A->n allocated storage
   */
  double* cs_dense(CSparseMatrix *A);

  /** Matrix vector multiplication : y = alpha*A*x+beta*y
   * \param[in] alpha matrix coefficient
   * \param[in] A the sparse matrix
   * \param[in] x pointer on a dense vector of size A->n
   * \param[in] beta vector coefficient
   * \param[in, out] y pointer on a dense vector of size A->n
   * \return 0 if A x or y is NULL else 1
   */
  int cs_aaxpy(const double alpha, const cs *A, const double *x,
               const double beta, double *y);

  /** Free space allocated for a SparseMatrix. note : cs_spfree also
   *  free the cs_struct this fails when the struct is allocated on
   *  the stack.
   * \param A the sparse matrix
   * \return NULL on success
  */
  CSparseMatrix* cs_spfree_on_stack(CSparseMatrix* A);

  /** New and empty NumericsSparseLinearSolverParams.
   * \return a pointer on the allocated space.
   */
  NumericsSparseLinearSolverParams* newNumericsSparseLinearSolverParams(void);

  /** New and empty NumericsSparseMatrix with correctly initialized fields.
   * \return a pointer on the allocated space.
   */
  NumericsSparseMatrix* newNumericsSparseMatrix(void);

  /** Free allocated space for NumericsSparseLinearSolverParams.
   * \param p a NumericsSparseLinearSolverParams
   * \return NULL on success
   */
  NumericsSparseLinearSolverParams* freeNumericsSparseLinearSolverParams(NumericsSparseLinearSolverParams* p);

  /** Free allocated space for a NumericsSparseMatrix.
   * \param A a NumericsSparseMatrix
   * \return NULL on success
   */
  NumericsSparseMatrix* freeNumericsSparseMatrix(NumericsSparseMatrix* A);

  /** reuse a LU factorization (stored in the cs_lu_A) to solve a linear system Ax = b
   * \param cs_lu_A contains the LU factors of A, permutation information
   * \param x workspace
   * \param[in,out] b on input RHS of the linear system; on output the solution
   * \return 0 if failed, 1 otherwise*/
  csi cs_solve (cs_lu_factors* cs_lu_A, double* x, double *b);

  /** compute a LU factorization of A and store it in a workspace
   * \param order control if ordering is used
   * \param A the sparse matrix
   * \param tol the tolerance
   * \param p the parameter structure that eventually holds the factors
   * \return 1 if the factorization was successful, 1 otherwise
   */
  int cs_lu_factorization(csi order, const cs *A, double tol, NumericsSparseLinearSolverParams* p);

  /** Free a workspace related to a LU factorization
   * \param p the structure to free
   */
  void NM_csparse_free(void* p);

  /** Get the LU factors for cs_lusol
   * \param p the structure holding the data for the solver
   */
  static inline cs_lu_factors* NM_csparse_lu_factors(NumericsSparseLinearSolverParams* p)
  {
    return (cs_lu_factors*)p->solver_data;
  }

  /** Get the workspace for the sparse solver
   * \param p the structure holding the data for the solver
   */
  static inline double* NM_csparse_workspace(NumericsSparseLinearSolverParams* p)
  {
    return p->dWork;
  }


  /** Initialize the fields of a NumericsSparseMatrix
   * \param A the sparse matrix
   */
  void NM_sparse_null(NumericsSparseMatrix* A);

#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
}
#endif

#endif
