/* Siconos-Numerics version 2.1.1, Copyright INRIA 2005-2006.
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
/*!
 ******************************************************************************
 *
 * This subroutine allows the resolution of LCP (Linear Complementary Problem).\n
 *
 * Try \f$(z,w)\f$ such that:\n
 * \f$
 *  \left\lbrace
 *   \begin{array}{l}
 *    w - M z = q\\
 *    0 \le z \perp w \ge 0\\
 *   \end{array}
 *  \right.
 * \f$
 *
 * where M is an (n x n)-matrix, q , w and z n-vectors.\n
 *
 *  This system of equations and inequalities is solved thanks to lcp solvers\n
 *
 *        lcp_nlgs( n , M , q , z , w , info , iparam , dparam )
 *        lcp_cpg ( n , M , q , z , w , info , iparam , dparam )
 *        lcp_latin ( n , M , q , z , w , info , iparam , dparam )
 *        lcp_latin_w ( n , M , q , z , w , info , iparam , dparam )
 *        lcp_lexicolemke( n , M , q , z , w , info , iparam , dparam )
 *        lcp_qp( n , M , q , z , w , info , iparam , dparam )
 *        lcp_nsqp( n , M , q , z , w , info , iparam , dparam )
 *
 *  where info shows the termination result (0 for success) and iparam and dparam are respectivelly
 *  pointer over integer and pointer over double which contain specific parameters of each solver.
 *
 *  The solver's call is performed via the function lcp_solver:
 *
 *  int lcp_solver( double *vec , double *q , int *nn , method *pt , double *z , double *w , int *it_end , double *res )
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NSSpack.h"
#include "blaslapack.h"

#define BAVARD

/*
 ******************************************************************************
 */

void test_lcp_series(int n , double *vec , double *q)
{

  int i, j;
  int nonsymmetric;
  int incx = 1, incy = 1;
  int info1 = -1, info2 = -1, info3 = -1, info4 = -1, info5 = -1, info6 = -1, info7 = -1, info8 = -1;

  double comp, diff, alpha, beta;

  double *z1, *z2, *z3, *z4, *z5, *z6, *z7, *z8;
  double *w1, *w2, *w3, *w4, *w5, *w6, *w7, *w8;

  char NT = 'N';

  z1 = malloc(n * sizeof(double));
  w1 = malloc(n * sizeof(double));
  z2 = malloc(n * sizeof(double));
  w2 = malloc(n * sizeof(double));
  z3 = malloc(n * sizeof(double));
  w3 = malloc(n * sizeof(double));
  z4 = malloc(n * sizeof(double));
  w4 = malloc(n * sizeof(double));
  z5 = malloc(n * sizeof(double));
  w5 = malloc(n * sizeof(double));
  z6 = malloc(n * sizeof(double));
  w6 = malloc(n * sizeof(double));
  z7 = malloc(n * sizeof(double));
  w7 = malloc(n * sizeof(double));
  z8 = malloc(n * sizeof(double));
  w8 = malloc(n * sizeof(double));

  /* Method definition */

  static method_lcp method_lcp1 = { "NLGS"       , 1001 , 1e-8 , 0.6 , 1.0 , 0 , "N2" , 0 , 0.0 };
  static method_lcp method_lcp2 = { "CPG"        , 1000 , 1e-8 , 0.6 , 1.0 , 0 , "N2" , 0 , 0.0 };
  static method_lcp method_lcp3 = { "Latin"      , 1000 , 1e-6 , 0.3 , 1.0 , 0 , "N2" , 0 , 0.0 };
  static method_lcp method_lcp8 = { "Latin_w"    , 1000 , 1e-6 , 0.3 , 1.0 , 0.75 , "N2" , 0 , 0.0 };
  static method_lcp method_lcp4 = { "QP"         , 1000 , 1e-8 , 0.7 , 1.0 , 0 , "N2" , 0 , 0.0 };
  static method_lcp method_lcp5 = { "NSQP"       , 1000 , 1e-8 , 0.7 , 1.0 , 0 , "N2" , 0 , 0.0 };
  static method_lcp method_lcp6 = { "LexicoLemke", 1000 , 1e-8 , 0.7 , 1.0 , 0 , "N2" , 0 , 0.0 };
  static method_lcp method_lcp7 = { "NewtonMin",   10   , 1e-8 , 0.7 , 1.0 , 0 , "N2" , 0 , 0.0 };

  nonsymmetric = 0;

  /* Is M symmetric ? */

  for (i = 0 ; i < n ; ++i)
  {
    for (j = 0 ; j < i ; ++j)
    {
      if (abs(vec[i * n + j] - vec[j * n + i]) > 1e-16)
      {
        nonsymmetric = 1;
        break;
      }
    }
  }

#ifdef BAVARD
  if (nonsymmetric) printf("\n !! WARNING !!\n M is a non symmetric matrix \n");
  else printf(" M is a symmetric matrix \n");
#endif

  /* #1 NLGS TEST */
#ifdef BAVARD
  printf("**** NLGS TEST ****\n");
#endif
  for (i = 0 ; i < n ; ++i)
  {
    z1[i] = 0.0;
    w1[i] = 0.0;
  }

  info1 = lcp_solver(vec , q , &n , &method_lcp1 , z1 , w1);

  /* #2 CPG TEST */
#ifdef BAVARD
  printf("**** CPG TEST *****\n");
#endif
  for (i = 0 ; i < n ; ++i)
  {
    z2[i] = 0.0;
    w2[i] = 0.0;
  }

  info2 = lcp_solver(vec , q , &n , &method_lcp2 , z2 , w2);

  /* #4 QP TEST */
#ifdef BAVARD
  printf("**** QP TEST ******\n");
#endif
  for (i = 0 ; i < n ; ++i)
  {
    z4[i] = 0.0;
    w4[i] = 0.0;
  }

  info4 = lcp_solver(vec , q , &n , &method_lcp4 , z4 , w4);

  /* #5 NSQP TEST */
#ifdef BAVARD
  printf("**** NSQP TEST ****\n");
#endif
  for (i = 0 ; i < n ; ++i)
  {
    z5[i] = 0.0;
    w5[i] = 0.0;
  }

  info5 = lcp_solver(vec , q , &n , &method_lcp5 , z5 , w5);

  /* #6 LEXICO LEMKE TEST */
#ifdef BAVARD
  printf("**** Lemke TEST ***\n");
#endif
  for (i = 0 ; i < n ; ++i)
  {
    z6[i] = 0.0;
    w6[i] = 0.0;
  }

  info6 = lcp_solver(vec , q , &n , &method_lcp6 , z6 , w6);

  /* #7 NEWTONMIN TEST */
#ifdef BAVARD
  printf("**** Newton TEST **\n");
#endif
  for (i = 0 ; i < n ; ++i)
  {
    z7[i] = 0.0;
    w7[i] = 0.0;
  }

  info7 = lcp_solver(vec , q , &n , &method_lcp7 , z7 , w7);

  /* #3 LATIN TEST */
#ifdef BAVARD
  printf("**** LATIN TEST ***\n");
#endif
  for (i = 0 ; i < n ; ++i)
  {
    z3[i] = 0.0;
    w3[i] = 0.0;
  }

  info3 = lcp_solver(vec , q , &n , &method_lcp3 , z3 , w3);


  /* #8 LATIN_W TEST */
#ifdef BAVARD
  printf("**** LATIN_W TEST ***\n");
#endif
  for (i = 0 ; i < n ; ++i)
  {
    z8[i] = 0.0;
    w8[i] = 0.0;
  }

  info8 = lcp_solver(vec , q , &n , &method_lcp8 , z8 , w8);


#ifdef BAVARD
  printf(" *** ************************************** ***\n");

  for (i = 0 ; i < n ; i++)
    printf("NLGS: %14.7e    CPG: %14.7e   LATIN: %14.7e    LATIN_w %14.7e \n", z1[i], z2[i], z3[i], z8[i]);

  for (i = 0 ; i < n ; i++)
    printf("\n QP: %14.7e     NSQP: %14.7e   Lemke: %14.7e    Newton: %14.7e ", z4[i], z5[i], z6[i], z7[i]);

  printf("\n\n");
  printf(" INFO RESULT\n");

  alpha = -1;
  beta  = 1;

  printf(" -- SOLVEUR ------ ITER/PIVOT ----- ERR ----- COMP ----- SOL?");

  comp = ddot_(&n , z1 , &incx , w1 , &incy);
  daxpy_(&n , &alpha , q , &incx , w1 , &incy);
  dgemv_(&NT , &n , &n , &beta , vec , &n , z1 , &incx , &alpha , w1 , &incy);
  diff = dnrm2_(&n , w1 , &incx);

  printf("\n    NLGS   (LOG:%1d)|      %5d | %10.4g | %10.4g | %10.4g |", info1, method_lcp1.iter, method_lcp1.err, comp, diff);

  comp = ddot_(&n , z2 , &incx , w2 , &incy);
  daxpy_(&n , &alpha , q , &incx , w2 , &incy);
  dgemv_(&NT , &n , &n , &beta , vec , &n , z2 , &incx , &alpha , w2 , &incy);
  diff = dnrm2_(&n , w2 , &incx);

  printf("\n    CPG    (LOG:%1d)|      %5d | %10.4g | %10.4g | %10.4g |", info2, method_lcp2.iter, method_lcp2.err, comp, diff);

  comp = ddot_(&n , z3 , &incx , w3 , &incy);
  daxpy_(&n , &alpha , q , &incx , w3 , &incy);
  dgemv_(&NT , &n , &n , &beta , vec , &n , z3 , &incx , &alpha , w3 , &incy);
  diff = dnrm2_(&n , w3 , &incx);

  printf("\n    LATIN  (LOG:%1d)|      %5d | %10.4g | %10.4g | %10.4g |", info3, method_lcp3.iter, method_lcp3.err, comp, diff);




  comp = ddot_(&n , z8 , &incx , w8 , &incy);
  daxpy_(&n , &alpha , q , &incx , w8 , &incy);
  dgemv_(&NT , &n , &n , &beta , vec , &n , z8 , &incx , &alpha , w8 , &incy);
  diff = dnrm2_(&n , w8 , &incx);

  printf("\n  LATIN_W  (LOG:%1d)|      %5d | %10.4g | %10.4g | %10.4g |", info8, method_lcp8.iter, method_lcp8.err, comp, diff);

  comp = ddot_(&n , z4 , &incx , w4 , &incy);
  daxpy_(&n , &alpha , q , &incx , w4 , &incy);
  dgemv_(&NT , &n , &n , &beta , vec , &n , z4 , &incx , &alpha , w4 , &incy);
  diff = dnrm2_(&n , w4 , &incx);


  printf("\n    QP     (LOG:%1d)|      %5d | %10.4g | %10.4g | %10.4g |", info4, method_lcp4.iter, method_lcp4.err, comp, diff);

  comp = ddot_(&n , z5 , &incx , w5 , &incy);
  daxpy_(&n , &alpha , q , &incx , w5 , &incy);
  dgemv_(&NT , &n , &n , &beta , vec , &n , z5 , &incx , &alpha , w5 , &incy);
  diff = dnrm2_(&n , w5 , &incx);

  printf("\n    NSQP   (LOG:%1d)|      %5d | %10.4g | %10.4g | %10.4g |", info5, method_lcp5.iter, method_lcp5.err, comp, diff);

  comp = ddot_(&n , z6 , &incx , w6 , &incy);
  daxpy_(&n , &alpha , q , &incx , w6 , &incy);
  dgemv_(&NT , &n , &n , &beta , vec , &n , z6 , &incx , &alpha , w6 , &incy);
  diff = dnrm2_(&n , w6 , &incx);

  printf("\n    Lemke  (LOG:%1d)|      %5d | %10.4g | %10.4g | %10.4g |", info6, method_lcp6.iter, method_lcp6.err, comp, diff);

  comp = ddot_(&n , z7 , &incx , w7 , &incy);
  daxpy_(&n , &alpha , q , &incx , w7 , &incy);
  dgemv_(&NT , &n , &n , &beta , vec , &n , z7 , &incx , &alpha , w7 , &incy);
  diff = dnrm2_(&n , w7 , &incx);

  printf("\n    Newton (LOG:%1d)|      %5d | %10.4g | %10.4g | %10.4g | \n \n ", info7, method_lcp7.iter, method_lcp7.err, comp, diff);

#endif

  free(z1);
  free(w1);
  free(z2);
  free(w2);
  free(z3);
  free(w3);
  free(z4);
  free(w4);
  free(z5);
  free(w5);
  free(z6);
  free(w6);
  free(z7);
  free(w7);
  free(z8);
  free(w8);


}
/*
 ******************************************************************************
 */

void test_lcp_block_series(int dn , int db , int *inb , int * iid , double *vecM , double *q)
{

  int i, dim;
  int incx = 1, incy = 1;
  int info1 = -1, info2 = -1, info3 = -1, info4 = -1, info5 = -1, info6 = -1, info7 = -1;
  int iter1, iter2, iter3, iter4, iter5, iter6, iter7;
  int titer1, titer2, titer3, titer4, titer5, titer6, titer7;

  double comp;
  double err1, err2, err3, err4, err5, err6, err7;

  double *z1, *z2, *z3, *z4, *z5, *z6, *z7;
  double *w1, *w2, *w3, *w4, *w5, *w6, *w7;

  dim = dn * db;

  z1 = malloc(dim * sizeof(double));
  w1 = malloc(dim * sizeof(double));
  z2 = malloc(dim * sizeof(double));
  w2 = malloc(dim * sizeof(double));
  z3 = malloc(dim * sizeof(double));
  w3 = malloc(dim * sizeof(double));
  z4 = malloc(dim * sizeof(double));
  w4 = malloc(dim * sizeof(double));
  z5 = malloc(dim * sizeof(double));
  w5 = malloc(dim * sizeof(double));
  z6 = malloc(dim * sizeof(double));
  w6 = malloc(dim * sizeof(double));
  z7 = malloc(dim * sizeof(double));
  w7 = malloc(dim * sizeof(double));

  /* Method definition */

  static method_lcp method_lcp1 = { "NLGS"       , 1001 , 1e-8 , 0.6 , 1.0 , 0 , "N2" , 0 , 0.0 };
  static method_lcp method_lcp2 = { "CPG"        , 1000 , 1e-8 , 0.6 , 1.0 , 0 , "N2" , 0 , 0.0 };
  static method_lcp method_lcp3 = { "Latin"      , 1000 , 1e-8 , 0.7 , 1.0 , 0 , "N2" , 0 , 0.0 };
  static method_lcp method_lcp4 = { "QP"         , 1000 , 1e-8 , 0.7 , 1.0 , 0 , "N2" , 0 , 0.0 };
  static method_lcp method_lcp5 = { "NSQP"       , 1000 , 1e-8 , 0.7 , 1.0 , 0 , "N2" , 0 , 0.0 };
  static method_lcp method_lcp6 = { "LexicoLemke", 1000 , 1e-8 , 0.7 , 1.0 , 0 , "N2" , 0 , 0.0 };
  static method_lcp method_lcp7 = { "NewtonMin"  , 10   , 1e-8 , 0.7 , 1.0 , 0 , "N2" , 0 , 0.0 };


  /* #1 NLGS TEST */
#ifdef BAVARD
  printf("**** NLGS TEST ****\n");
#endif

  for (i = 0 ; i < dim ; ++i) z1[i] = 0.0;

  info1 = lcp_solver_block(inb , iid , vecM , q , &dn , &db , &method_lcp1 , z1 , w1 , &iter1 , &titer1 , &err1);

  /* #2 CPG TEST */
#ifdef BAVARD
  printf("**** CPG TEST *****\n");
#endif
  for (i = 0 ; i < dim ; ++i) z2[i] = 0.0;

  info2 = lcp_solver_block(inb , iid , vecM , q , &dn , &db , &method_lcp2 , z2 , w2 , &iter2 , &titer2 , &err2);

  /* #3 QP TEST */
#ifdef BAVARD
  printf("**** QP TEST ******\n");
#endif
  for (i = 0 ; i < dim ; ++i) z4[i] = 0.0;

  info4 = lcp_solver_block(inb , iid , vecM , q , &dn , &db , &method_lcp4 , z4 , w4 , &iter4 , &titer4 , &err4);

  /* #4 NSQP TEST */
#ifdef BAVARD
  printf("**** NSQP TEST ****\n");
#endif
  for (i = 0 ; i < dim ; ++i) z5[i] = 0.0;

  info5 = lcp_solver_block(inb , iid , vecM , q , &dn , &db , &method_lcp5 , z5 , w5 , &iter5 , &titer5 , &err5);

  /* #5 LEXICO LEMKE TEST */
#ifdef BAVARD
  printf("**** Lemke TEST ***\n");
#endif
  for (i = 0 ; i < dim ; ++i)
  {
    z6[i] = 0.0;
    w6[i] = 0.0;
  }

  iter6 = 0;
  titer6 = 0;
  err6 = 0.;

  info6 = lcp_solver_block(inb , iid , vecM , q , &dn , &db , &method_lcp6 , z6 , w6 , &iter6 , &titer6 , &err6);

  /* #7 NEWTONMIN TEST */
#ifdef BAVARD
  printf("**** Newton TEST **\n");
#endif
  for (i = 0 ; i < dim ; ++i) z7[i] = 0.0;

  info7 = lcp_solver_block(inb , iid , vecM , q , &dn , &db , &method_lcp7 , z7 , w7 , &iter7 , &titer7 , &err7);

  /* #8 LATIN TEST */
#ifdef BAVARD
  printf("**** LATIN TEST ***\n");
#endif
  for (i = 0 ; i < dim ; ++i) z3[i] = 0.0;

  info3 = lcp_solver_block(inb , iid , vecM , q , &dn , &db , &method_lcp3 , z3 , w3 , &iter3 , &titer3 , &err3);

#ifdef BAVARD
  printf(" *** ************************************** ***\n");
  printf("\n   NLGS RESULT : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z1[i]);
  printf("\n    CPG RESULT : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z2[i]);
  printf("\n     QP RESULT : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z4[i]);
  printf("\n   NSQP RESULT : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z5[i]);
  printf("\n  Lemke RESULT : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z6[i]);
  printf("\n Newton RESULT : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z7[i]);
  printf("\n  LATIN RESULT : ");
  for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , z3[i]);
  printf("\n\n");
  printf(" INFO RESULT\n");

  printf(" -- SOLVEUR ------ BLOCK I/P ----- TOTAL I/P ----- ERR ----- COMP ");

  comp = ddot_(&dim , z1 , &incx , w1 , &incy);

  printf("\n    NLGS   (LOG:%1d)|      %5d | %5d | %10.4g | %10.4g |", info1, iter1, titer1, err1, comp);

  comp = ddot_(&dim , z2 , &incx , w2 , &incy);

  printf("\n    CPG    (LOG:%1d)|      %5d | %5d | %10.4g | %10.4g |", info2, iter2, titer2, err2, comp);

  comp = ddot_(&dim , z4 , &incx , w4 , &incy);

  printf("\n    QP     (LOG:%1d)|      %5d | %5d | %10.4g | %10.4g |", info4, iter4, titer4, err4, comp);

  comp = ddot_(&dim , z5 , &incx , w5 , &incy);

  printf("\n    NSQP   (LOG:%1d)|      %5d | %5d | %10.4g | %10.4g |", info5, iter5, titer5, err5, comp);

  comp = ddot_(&dim , z6 , &incx , w6 , &incy);

  /*  printf("\n    Lemke  (LOG:%1d)|",info6);
  printf("\n    Lemke    %5d ",iter6);
  printf("\n    Lemke %5d |",titer6);
  printf("\n    Lemke %10.4g",err6);
  printf("\n    Lemke %10.4g |",comp);*/
  printf("\n    Lemke  (LOG:%1d)|      %5d | %5d | %10.4g | %10.4g |", info6, iter6, titer6, err6, comp);

  comp = ddot_(&dim , z7 , &incx , w7 , &incy);

  printf("\n    Newton (LOG:%1d)|      %5d | %5d | %10.4g | %10.4g |", info7, iter7, titer7, err7, comp);

  comp = ddot_(&dim , z3 , &incx , w3 , &incy);

  printf("\n    LATIN  (LOG:%1d)|      %5d | %5d | %10.4g | %10.4g |", info3, iter3, titer3, err3, comp);

#endif

  free(z1);
  free(w1);
  free(z2);
  free(w2);
  free(z3);
  free(w3);
  free(z4);
  free(w4);
  free(z5);
  free(w5);
  free(z6);
  free(w6);
  free(z7);
  free(w7);
}
/*
 ******************************************************************************
 */

void test_mmc(void)
{

  FILE *f1, *f2;

  int i, nl, nc, n;

  double *q, *vecM;
  double qi, Mij;

  char val[20], vall[20];

  printf("* *** ******************** *** * \n");
  printf("* ***        TEST MMC      *** * \n");
  printf("* *** ******************** *** * \n");

  // computation of the size of the problem

  n = 0;

  if ((f1 = fopen("DATA/M_rectangle1.dat", "r")) == NULL)
  {
    perror("fopen 1");
    exit(1);
  }

  while (!feof(f1))
  {
    fscanf(f1, "%d", &nl);
    fscanf(f1, "%d", &nc);
    fscanf(f1, "%s", val);
    n = nl;
  }

  printf("\n SIZE OF THE PROBLEM : %d \n", n);
  fclose(f1);

  vecM = (double *)malloc(n * n * sizeof(double));

  /* Data loading of M and q */

  if ((f1 = fopen("DATA/M_rectangle1.dat", "r")) == NULL)
  {
    perror("fopen 1");
    exit(1);
  }

  if ((f2 = fopen("DATA/q_rectangle1.dat", "r")) == NULL)
  {
    perror("fopen 2");
    exit(2);
  }

  for (i = 0 ; i < n * n ; ++i) vecM[i] = 0.0;

  while (!feof(f1))
  {
    fscanf(f1, "%d", &nl);
    fscanf(f1, "%d", &nc);
    fscanf(f1, "%s", val);

    Mij = atof(val);
    vecM[(nc - 1)*n + nl - 1 ] = Mij;
  }


  q = (double *)malloc(n * sizeof(double));

  for (i = 0 ; i < n ; ++i) q[i] = 0.0;

  while (!feof(f2))
  {
    fscanf(f2, "%d", &nl);
    fscanf(f2, "%s", vall);
    qi = atof(vall);

    q[nl - 1] = qi;
  }

  fclose(f2);
  fclose(f1);

  test_lcp_series(n , vecM , q);

  free(vecM);
  free(q);

}

void test_matrix(void)
{

  FILE *LCPfile;

  int i, j, itest, NBTEST;
  int isol;
  int dim , dim2;

  double *q, *sol;
  double *vecM;

  char val[20];

  int iter;
  double criteria;

  iter  = 0;
  criteria = 0.0;

  NBTEST = 8;

  /****************************************************************/
#ifdef BAVARD
  printf("\n ********** BENCHMARK FOR LCP_SOLVER ********** \n\n");
#endif
  /****************************************************************/

  for (itest = 0 ; itest < NBTEST ; ++itest)
  {

    switch (itest)
    {
    case 0:
      printf("\n\n 2x2 LCP ");
      if ((LCPfile = fopen("MATRIX/deudeu.dat", "r")) == NULL)
      {
        perror("fopen LCPfile: deudeu.dat");
        exit(1);
      }
      break;
    case 1:
      printf("\n\n DIAGONAL LCP ");
      if ((LCPfile = fopen("MATRIX/trivial.dat", "r")) == NULL)
      {
        perror("fopen LCPfile: trivial.dat");
        exit(1);
      }
      break;
    case 2:
      printf("\n\n ORTIZ LCP ");
      if ((LCPfile = fopen("MATRIX/ortiz.dat", "r")) == NULL)
      {
        perror("fopen LCPfile: ortiz.dat");
        exit(1);
      }
      break;
    case 3:
      printf("\n\n PANG LCP ");
      if ((LCPfile = fopen("MATRIX/pang.dat", "r")) == NULL)
      {
        perror("fopen LCPfile: pang.dat");
        exit(1);
      }
      break;
    case 4:
      printf("\n\n DIODES RLC LCP ");
      if ((LCPfile = fopen("MATRIX/diodes.dat", "r")) == NULL)
      {
        perror("fopen LCPfile: diodes.dat");
        exit(1);
      }
      break;
    case 5:
      printf("\n\n MURTY1 LCP ");
      if ((LCPfile = fopen("MATRIX/murty1.dat", "r")) == NULL)
      {
        perror("fopen LCPfile: murty1.dat");
        exit(1);
      }
      break;
    case 6:
      printf("\n\n APPROXIMATED FRICTION CONE LCP ");
      if ((LCPfile = fopen("MATRIX/confeti.dat", "r")) == NULL)
      {
        perror("fopen LCPfile: confeti.dat");
        exit(1);
      }
      break;
    case 7:
      printf("\n\n MATHIEU2 LCP ");
      if ((LCPfile = fopen("MATRIX/mathieu2.dat", "r")) == NULL)
      {
        perror("fopen LCPfile: mathieu2.dat");
        exit(1);
      }
      break;
    }

    fscanf(LCPfile , "%d" , &dim);

    dim2 = dim * dim;
    isol = 1;

    vecM = (double*)malloc(dim2 * sizeof(double));
    q    = (double*)malloc(dim * sizeof(double));
    sol  = (double*)malloc(dim * sizeof(double));

    for (i = 0 ; i < dim ; ++i)  q[i]    = 0.0;
    for (i = 0 ; i < dim ; ++i)  sol[i]  = 0.0;
    for (i = 0 ; i < dim2 ; ++i) vecM[i] = 0.0;

    for (i = 0 ; i < dim ; ++i)
    {
      for (j = 0 ; j < dim ; ++j)
      {
        fscanf(LCPfile, "%s", val);
        vecM[ dim * j + i ] = atof(val);
      }
    }

    for (i = 0 ; i < dim ; ++i)
    {
      fscanf(LCPfile , "%s" , val);
      q[i] = atof(val);
    }

    fscanf(LCPfile , "%s" , val);

    if (!feof(LCPfile))
    {

      sol[0] = atof(val);

      for (i = 1 ; i < dim ; ++i)
      {
        fscanf(LCPfile , "%s" , val);
        sol[i] = atof(val);
      }
    }
    else
    {
      isol = 0;
      for (i = 0 ; i < dim ; ++i) sol[i] = 0.0;
    }

    fclose(LCPfile);

#ifdef BAVARD
    printf("\n exact solution : ");
    if (isol) for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , sol[i]);
    else printf(" unknown ");
    printf("\n");
#endif

    test_lcp_series(dim , vecM , q);

    free(sol);
    free(vecM);
    free(q);
  }
}

/*
 ******************************************************************************
 */

void test_blockmatrix(void)
{

  FILE *LCPfile;

  int i, j, isol, NBTEST, itest;
  int dim , dim2;
  int dn, db, db2, nblock, itmp;

  int *inb, *iid;
  double *q , *sol;
  double *vecM;

  char val[20];

  int iter, titer;
  double criteria;

  iter  = 0;
  titer = 0;
  criteria = 0.0;

  NBTEST = 2;

  /****************************************************************/
#ifdef BAVARD
  printf("\n\n ******** BENCHMARK FOR LCP_SOLVER_BLOCK ******** \n\n");
#endif
  /****************************************************************/

  for (itest = 0 ; itest < NBTEST ; ++itest)
  {

    switch (itest)
    {
    case 0:
      printf("\n\n MATHIEU2 LCP ");
      if ((LCPfile = fopen("BLOCKMATRIX/mathieu1.dat", "r")) == NULL)
      {
        perror("fopen LCPfile: mathieu1.dat");
        exit(1);
      }
      break;
    case 1:
      printf("\n\n MATHIEU3 LCP ");
      if ((LCPfile = fopen("BLOCKMATRIX/mathieu2.dat", "r")) == NULL)
      {
        perror("fopen LCPfile: mathieu2.dat");
        exit(1);
      }
      break;
    }

    fscanf(LCPfile , "%d" , &dn);
    fscanf(LCPfile , "%d" , &db);

    nblock = 0;

    inb  = (int*)malloc(dn * sizeof(int));

    for (i = 0 ; i < dn ; ++i)
    {
      fscanf(LCPfile , "%d" , &itmp);
      inb[i] = itmp;
      nblock += itmp;
    }

    iid  = (int*)malloc(nblock * sizeof(int));

    for (i = 0 ; i < nblock ; ++i)
    {
      fscanf(LCPfile , "%d" , &itmp);
      iid[i] = itmp;
    }

    dim  = db * dn;
    db2 = db * db;
    dim2 = nblock * db2;

    q    = (double*)malloc(dim * sizeof(double));
    sol  = (double*)malloc(dim * sizeof(double));
    vecM = (double*)malloc(dim2 * sizeof(double));

    for (i = 0 ; i < dim ; ++i)  q[i]    = 0.0;
    for (i = 0 ; i < dim ; ++i)  sol[i]  = 0.0;
    for (i = 0 ; i < dim2 ; ++i) vecM[i] = 0.0;


    for (i = 0 ; i < nblock ; ++i)
    {
      for (j = 0 ; j < db2 ; ++j)
      {
        fscanf(LCPfile, "%s", val);
        vecM[i * db2 + j] = atof(val);
      }
    }

    for (i = 0 ; i < dim ; ++i)
    {
      fscanf(LCPfile , "%s" , val);
      q[i] = atof(val);
    }

    fscanf(LCPfile , "%s" , val);

    if (!feof(LCPfile))
    {

      sol[0] = atof(val);
      isol = 1;
      for (i = 1 ; i < dim ; ++i)
      {
        fscanf(LCPfile , "%s" , val);
        sol[i] = atof(val);
      }
    }
    else
    {
      isol = 0;
      for (i = 0 ; i < dim ; ++i) sol[i] = 0.0;
    }

    fclose(LCPfile);

#ifdef BAVARD
    printf("\n exact solution : ");
    if (isol) for (i = 0 ; i < dim ; ++i) printf(" %10.4g " , sol[i]);
    else printf(" unknown ");
    printf("\n");
#endif

    test_lcp_block_series(dn , db , inb , iid , vecM , q);

    free(sol);
    free(vecM);
    free(q);
    free(iid);
    free(inb);
  }
}

/*
 ******************************************************************************
 */

int main(void)
{

  test_mmc();
  test_matrix();
  test_blockmatrix();

  return 0;
}

