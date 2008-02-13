/* Siconos-Numerics version 2.1.1, Copyright INRIA 2005-2007.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NonSmoothDrivers.h"
#include "LA.h"

#define BAVARD
#define PATH_DRIVER

#ifdef PATH_DRIVER
const unsigned short int *__ctype_b;
const __int32_t *__ctype_tolower ;
#endif /*PATH_DRIVER*/
/*
******************************************************************************
*/

void test_lcp_series(LinearComplementarity_Problem * problem)
{

  int i, j;
  int nonsymmetric;
  int incx = 1, incy = 1;
  int info1 = -1, info2 = -1, info3 = -1, info4 = -1, info5 = -1, info6 = -1, info7 = -1, info8 = -1, info9 = -1, info10 = -1;

  double comp, diff, alpha, beta;

  double *z1, *z2, *z3, *z4, *z5, *z6, *z7, *z8, *z9, *z10;
  double *w1, *w2, *w3, *w4, *w5, *w6, *w7, *w8, *w9, *w10;

  int n = problem->size;
  double * vec = problem->M->matrix0;

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
  z9 = malloc(n * sizeof(double));
  w9 = malloc(n * sizeof(double));
  z10 = malloc(n * sizeof(double));
  w10 = malloc(n * sizeof(double));

  /* Solvers definitions */
  int maxIter = 1001;
  double tolerance = 1e-8;
  /* PGS Solver*/
  Solver_Options optionsPGS;
  strcpy(optionsPGS.solverName, "PGS");
  int iparam1[2] = {maxIter, 0};
  double dparam1[2] = {tolerance, 0.0};
  optionsPGS.iparam = iparam1;
  optionsPGS.dparam = dparam1;
  optionsPGS.isSet = 1;
  optionsPGS.storageType = 0;

  /* CPG Solver*/
  Solver_Options optionsCPG;
  strcpy(optionsCPG.solverName, "CPG");
  int iparam2[2] = {maxIter, 0};
  double dparam2[2] = {tolerance, 0.0};
  optionsCPG.iparam = iparam2;
  optionsCPG.dparam = dparam2;
  optionsCPG.isSet = 1;
  optionsCPG.storageType = 0;

  /* Latin */
  Solver_Options optionsLatin;
  strcpy(optionsLatin.solverName, "Latin");
  int iparam3[2] = {maxIter, 0};
  double dparam3[3] = {tolerance, 0.0, 0.3};
  optionsLatin.iparam = iparam3;
  optionsLatin.dparam = dparam3;
  optionsLatin.isSet = 1;
  optionsLatin.storageType = 0;

  /* Latin_w */
  Solver_Options optionsLatin_w;
  strcpy(optionsLatin_w.solverName, "Latin_w");
  int iparam4[2] = {maxIter, 0};
  double dparam4[4] = {tolerance, 0.0, 0.3, 1.0};
  optionsLatin_w.iparam = iparam4;
  optionsLatin_w.dparam = dparam4;
  optionsLatin_w.isSet = 1;
  optionsLatin_w.storageType = 0;

  /* QP */
  Solver_Options optionsQP;
  strcpy(optionsQP.solverName, "QP");
  double dparam5[1] = {tolerance};
  optionsQP.dparam = dparam5;
  optionsQP.isSet = 1;
  optionsQP.storageType = 0;

  /* NSQP */
  Solver_Options optionsNSQP;
  strcpy(optionsNSQP.solverName, "NSQP");
  double dparam6[1] = {tolerance};
  optionsNSQP.dparam = dparam6;
  optionsNSQP.isSet = 1;
  optionsNSQP.storageType = 0;

  /* Lemke */
  Solver_Options optionsLemke;
  strcpy(optionsLemke.solverName, "LexicoLemke");
  int iparam7[2] = {maxIter, 0};
  optionsLemke.iparam = iparam7;
  optionsLemke.isSet = 1;
  optionsLemke.storageType = 0;

  /* Newton Min */
  Solver_Options optionsNewtonMin;
  strcpy(optionsNewtonMin.solverName, "NewtonMin");
  int iparam8[2] = {10, 0};
  double dparam8[2] = {tolerance, 0};
  optionsNewtonMin.iparam = iparam8;
  optionsNewtonMin.dparam = dparam8;
  optionsNewtonMin.isSet = 1;
  optionsNewtonMin.storageType = 0;

  /* Newton FB */
  Solver_Options optionsNewtonFB;
  strcpy(optionsNewtonFB.solverName, "NewtonFB");
  int iparam9[2] = {10, 0};
  double dparam9[2] = {tolerance, 0};
  optionsNewtonFB.iparam = iparam9;
  optionsNewtonFB.dparam = dparam9;
  optionsNewtonFB.isSet = 1;
  optionsNewtonFB.storageType = 0;

  /* RPGS */
  Solver_Options optionsRPGS;
  strcpy(optionsRPGS.solverName, "RPGS");
  int iparam10[2] = {maxIter, 0};
  double dparam10[3] = {tolerance, 0.0, 1.0};
  optionsRPGS.iparam = iparam10;
  optionsRPGS.dparam = dparam10;
  optionsRPGS.isSet = 1;
  optionsRPGS.storageType = 0;

  /* Path */
  Solver_Options optionsPath;
  strcpy(optionsPath.solverName, "path");
  double dparam11[1] = {tolerance};
  optionsPath.dparam = dparam11;
  optionsPath.isSet = 1;
  optionsPath.storageType = 0;

  Numerics_Options global_options;
  global_options.verboseMode = 0;

  nonsymmetric = 0;

  /* Is M symmetric ? */

  for (i = 0 ; i < n ; ++i)
  {
    for (j = 0 ; j < i ; ++j)
    {
      if (abs(problem->M->matrix0[i * n + j] - problem->M->matrix0[j * n + i]) > 1e-16)
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

  /* #1 PGS TEST */
#ifdef BAVARD
  printf("**** PGS TEST ****\n");
#endif
  for (i = 0 ; i < n ; ++i)
  {
    z1[i] = 0.0;
    w1[i] = 0.0;
  }
  info1 = lcp_driver(problem, z1 , w1, &optionsPGS, &global_options);

  /* #2 CPG TEST */
#ifdef BAVARD
  printf("**** CPG TEST *****\n");
#endif
  for (i = 0 ; i < n ; ++i)
  {
    z2[i] = 0.0;
    w2[i] = 0.0;
  }
  info2 = lcp_driver(problem, z2 , w2, &optionsCPG, &global_options);

  /* #4 QP TEST */
#ifdef BAVARD
  printf("**** QP TEST ******\n");
#endif
  for (i = 0 ; i < n ; ++i)
  {
    z4[i] = 0.0;
    w4[i] = 0.0;
  }
  info4 = lcp_driver(problem, z4 , w4, &optionsQP, &global_options);

  /* #5 NSQP TEST */
#ifdef BAVARD
  printf("**** NSQP TEST ****\n");
#endif
  for (i = 0 ; i < n ; ++i)
  {
    z5[i] = 0.0;
    w5[i] = 0.0;
  }
  info5 = lcp_driver(problem, z5 , w5, &optionsNSQP, &global_options);

  /* #6 LEXICO LEMKE TEST */
#ifdef BAVARD
  printf("**** Lemke TEST ***\n");
#endif
  for (i = 0 ; i < n ; ++i)
  {
    z6[i] = 0.0;
    w6[i] = 0.0;
  }
  info6 = lcp_driver(problem, z6 , w6, &optionsLemke, &global_options);

  /* #7 NEWTONMIN TEST */
#ifdef BAVARD
  printf("**** Newton TEST **\n");
#endif
  for (i = 0 ; i < n ; ++i)
  {
    z7[i] = 0.0;
    w7[i] = 0.0;
  }
  info7 = lcp_driver(problem, z7 , w7, &optionsNewtonMin, &global_options);


  /* #3 LATIN TEST */
#ifdef BAVARD
  printf("**** LATIN TEST ***\n");
#endif
  for (i = 0 ; i < n ; ++i)
  {
    z3[i] = 0.0;
    w3[i] = 0.0;
  }

  info3 = lcp_driver(problem, z3 , w3, &optionsLatin, &global_options);

  /* #8 LATIN_W TEST */
#ifdef BAVARD
  printf("**** LATIN_W TEST ***\n");
#endif
  for (i = 0 ; i < n ; ++i)
  {
    z8[i] = 0.0;
    w8[i] = 0.0;
  }
  info8 = lcp_driver(problem, z8 , w8, &optionsLatin_w, &global_options);

  /* #9 RPGS TEST */
#ifdef BAVARD
  printf("**** RPGS TEST ****\n");
#endif
  for (i = 0 ; i < n ; ++i)
  {
    z9[i] = 0.0;
    w9[i] = 0.0;
  }
  info9 = lcp_driver(problem, z9 , w9, &optionsRPGS, &global_options);

#ifdef PATH_DRIVER
  /* #8 PATH TEST */
#ifdef BAVARD
  printf("**** PATH TEST ***\n");
#endif
  for (i = 0 ; i < n ; ++i)
  {
    z10[i] = 0.0;
    w10[i] = 0.0;
  }
  info10 = lcp_driver(problem, z10 , w10, &optionsPath, &global_options);

#endif /*PATH_DRIVER*/
#ifdef BAVARD
  /*  printf(" *** ************************************** ***\n"); */

  printf(" ****** z = ********************************\n");
  for (i = 0 ; i < n ; i++)
    printf("PGS: %14.7e    CPG: %14.7e   LATIN: %14.7e    LATIN_w %14.7e    PATH %14.7e \n", z1[i], z2[i], z3[i], z8[i], z10[i]);

  printf(" ****** w = ********************************\n");
  for (i = 0 ; i < n ; i++)
    printf("PGS: %14.7e    CPG: %14.7e   LATIN: %14.7e    LATIN_w %14.7e    PATH %14.7e \n", w1[i], w2[i], w3[i], w8[i], w10[i]);

  printf(" ****** z = ********************************\n");
  for (i = 0 ; i < n ; i++)
    printf("QP: %14.7e     NSQP: %14.7e   Lemke: %14.7e    Newton: %14.7e    RPGS: %14.7e    PATH %14.7e \n", z4[i], z5[i], z6[i], z7[i], z9[i], z10[i]);

  printf(" ****** w = ********************************\n");
  for (i = 0 ; i < n ; i++)
    printf("QP: %14.7e     NSQP: %14.7e   Lemke: %14.7e    Newton: %14.7e    RPGS: %14.7e    PATH %14.7e \n", w4[i], w5[i], w6[i], w7[i], w9[i], w10[i]);

  printf("\n\n");
  printf(" INFO RESULT\n");

  alpha = -1;
  beta  = 1;

  printf(" -- SOLVEUR ------ ITER/PIVOT ----- ERR ----- COMP ----- SOL?");

  comp = DDOT(n , z1 , incx , w1 , incy);
  DAXPY(n , alpha , problem->q , incx , w1 , incy);
  DGEMV(LA_NOTRANS , n , n , beta , vec , n , z1 , incx , alpha , w1 , incy);
  diff = DNRM2(n , w1 , incx);

  printf("\n    PGS   (LOG:%1d)|      %5d | %10.4g | %10.4g | %10.4g |", info1, optionsPGS.iparam[1], optionsPGS.dparam[1], comp, diff);

  comp = DDOT(n , z2 , incx , w2 , incy);
  DAXPY(n , alpha , problem->q , incx , w2 , incy);
  DGEMV(LA_NOTRANS , n , n , beta , vec , n , z2 , incx , alpha , w2 , incy);
  diff = DNRM2(n , w2 , incx);

  printf("\n    CPG    (LOG:%1d)|      %5d | %10.4g | %10.4g | %10.4g |", info2, optionsCPG.iparam[1], optionsCPG.dparam[1], comp, diff);

  comp = DDOT(n , z3 , incx , w3 , incy);
  DAXPY(n , alpha , problem->q , incx , w3 , incy);
  DGEMV(LA_NOTRANS , n , n , beta , vec , n , z3 , incx , alpha , w3 , incy);
  diff = DNRM2(n , w3 , incx);

  printf("\n    LATIN  (LOG:%1d)|      %5d | %10.4g | %10.4g | %10.4g |", info3, optionsLatin.iparam[1], optionsLatin.dparam[1], comp, diff);




  comp = DDOT(n , z8 , incx , w8 , incy);
  DAXPY(n , alpha , problem->q , incx , w8 , incy);
  DGEMV(LA_NOTRANS , n , n , beta , vec , n , z8 , incx , alpha , w8 , incy);
  diff = DNRM2(n , w8 , incx);

  printf("\n  LATIN_W  (LOG:%1d)|      %5d | %10.4g | %10.4g | %10.4g |", info8, optionsLatin_w.iparam[1], optionsLatin_w.dparam[1], comp, diff);

  comp = DDOT(n , z4 , incx , w4 , incy);
  DAXPY(n , alpha , problem->q , incx , w4 , incy);
  DGEMV(LA_NOTRANS , n , n , beta , vec , n , z4 , incx , alpha , w4 , incy);
  diff = DNRM2(n , w4 , incx);


  printf("\n    QP     (LOG:%1d)|      %5d | %10.4g | %10.4g | %10.4g |", info4, optionsQP.iparam[1], optionsQP.dparam[1], comp, diff);

  comp = DDOT(n , z5 , incx , w5 , incy);
  DAXPY(n , alpha , problem->q , incx , w5 , incy);
  DGEMV(LA_NOTRANS , n , n , beta , vec , n , z5 , incx , alpha , w5 , incy);
  diff = DNRM2(n , w5 , incx);

  printf("\n    NSQP   (LOG:%1d)|      %5d | %10.4g | %10.4g | %10.4g |", info5, optionsNSQP.iparam[1], optionsNSQP.dparam[1], comp, diff);

  comp = DDOT(n , z6 , incx , w6 , incy);
  DAXPY(n , alpha , problem->q , incx , w6 , incy);
  DGEMV(LA_NOTRANS , n , n , beta , vec , n , z6 , incx , alpha , w6 , incy);
  diff = DNRM2(n , w6 , incx);

  printf("\n    Lemke  (LOG:%1d)|      %5d | %10.4g | %10.4g | %10.4g |", info6, optionsLemke.iparam[1], optionsLemke.dparam[1], comp, diff);

  comp = DDOT(n , z7 , incx , w7 , incy);
  DAXPY(n , alpha , problem->q , incx , w7 , incy);
  DGEMV(LA_NOTRANS , n , n , beta , vec , n , z7 , incx , alpha , w7 , incy);
  diff = DNRM2(n , w7 , incx);

  printf("\n    Newton (LOG:%1d)|      %5d | %10.4g | %10.4g | %10.4g |", info7, optionsNewtonMin.iparam[1], optionsNewtonMin.dparam[1], comp, diff);

  comp = DDOT(n , z9 , incx , w9 , incy);
  DAXPY(n , alpha , problem->q , incx , w9 , incy);
  DGEMV(LA_NOTRANS , n , n , beta , vec , n , z9 , incx , alpha , w9 , incy);
  diff = DNRM2(n , w9 , incx);

  printf("\n    RPGS   (LOG:%1d)|      %5d | %10.4g | %10.4g | %10.4g |", info9, optionsRPGS.iparam[1], optionsRPGS.dparam[1], comp, diff);

#ifdef PATH_DRIVER
  comp = DDOT(n , z10 , incx , w10 , incy);
  DAXPY(n , alpha , problem->q , incx , w10 , incy);
  DGEMV(LA_NOTRANS , n , n , beta , vec , n , z10 , incx , alpha , w10 , incy);
  diff = DNRM2(n , w10 , incx);

  printf("\n    PATH (LOG:%1d)|      %5d | %10.4g | %10.4g | %10.4g | \n \n ", info10, optionsPath.iparam[1], optionsPath.dparam[1], comp, diff);
#endif /*PATH_DRIVER*/

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
  free(z9);
  free(w9);
  free(z10);
  free(w10);


}
/*
******************************************************************************
*/

void test_lcp_block_series(SparseBlockStructuredMatrix *blmat , double *q)
{

  /*   int i,dim; */
  /*   int incx=1,incy=1; */
  /*   int info1=-1,info2=-1,info3=-1,info4=-1,info5=-1,info6=-1,info7=-1,info8=-1; */
  /*   int iter1,iter2,iter3,iter4,iter5,iter6,iter7,iter8; */
  /*   int titer1,titer2,titer3,titer4,titer5,titer6,titer7,titer8; */

  /*   double comp; */
  /*   double err1,err2,err3,err4,err5,err6,err7,err8; */

  /*   double *z1,*z2,*z3,*z4,*z5,*z6,*z7,*z8; */
  /*   double *w1,*w2,*w3,*w4,*w5,*w6,*w7,*w8; */

  /*   dim = blmat->blocksize[blmat->size-1]; */

  /*   z1 = malloc( dim*sizeof( double ) ); */
  /*   w1 = malloc( dim*sizeof( double ) ); */
  /*   z2 = malloc( dim*sizeof( double ) ); */
  /*   w2 = malloc( dim*sizeof( double ) ); */
  /*   z3 = malloc( dim*sizeof( double ) ); */
  /*   w3 = malloc( dim*sizeof( double ) ); */
  /*   z4 = malloc( dim*sizeof( double ) ); */
  /*   w4 = malloc( dim*sizeof( double ) ); */
  /*   z5 = malloc( dim*sizeof( double ) ); */
  /*   w5 = malloc( dim*sizeof( double ) ); */
  /*   z6 = malloc( dim*sizeof( double ) ); */
  /*   w6 = malloc( dim*sizeof( double ) ); */
  /*   z7 = malloc( dim*sizeof( double ) ); */
  /*   w7 = malloc( dim*sizeof( double ) ); */
  /*   z8 = malloc( dim*sizeof( double ) ); */
  /*   w8 = malloc( dim*sizeof( double ) ); */

  /*   /\* Method definition *\/ */

  /*   static method_lcp method_lcp1 = { "PGS"       , 1001 , 1e-8 , 0.6 , 1.0 , 1.0 , 1 , 0 , 0.0 }; */
  /*   static method_lcp method_lcp2 = { "CPG"        , 1000 , 1e-8 , 0.6 , 1.0 , 1.0 , 1 , 0 , 0.0 }; */
  /*   static method_lcp method_lcp3 = { "Latin"      , 1000 , 1e-8 , 0.7 , 1.0 , 1.0 , 1 , 0 , 0.0 }; */
  /*   static method_lcp method_lcp4 = { "QP"         , 1000 , 1e-8 , 0.7 , 1.0 , 1.0 , 1 , 0 , 0.0 }; */
  /*   static method_lcp method_lcp5 = { "NSQP"       , 1000 , 1e-8 , 0.7 , 1.0 , 1.0 , 1 , 0 , 0.0 }; */
  /*   static method_lcp method_lcp6 = { "LexicoLemke", 1000 , 1e-8 , 0.7 , 1.0 , 1.0 , 1 , 0 , 0.0 }; */
  /*   static method_lcp method_lcp7 = { "NewtonMin"  , 10   , 1e-8 , 0.7 , 1.0 , 1.0 , 1 , 0 , 0.0 }; */
  /*   static method_lcp method_lcp8 = { "RPGS"       , 1001 , 1e-8 , 0.6 , 1.0 , 1.0 , 1 , 0 , 0.0 }; */

  /*   method myMethod; */

  /*   /\* #1 PGS TEST *\/ */
  /* #ifdef BAVARD */
  /*   printf("**** PGS TEST ****\n"); */
  /* #endif */

  /*   for( i = 0 ; i < dim ; ++i ) z1[i] = 0.0; */

  /*   myMethod.lcp =  method_lcp1; */
  /*   info1 = lcp_driver_block( blmat , q , &myMethod , z1 , w1 , &iter1 , &titer1 , &err1 ); */

  /*   /\* #2 CPG TEST *\/ */
  /* #ifdef BAVARD */
  /*   printf("**** CPG TEST *****\n"); */
  /* #endif */
  /*   for( i = 0 ; i < dim ; ++i ) z2[i] = 0.0; */

  /*   myMethod.lcp =  method_lcp2; */
  /*   info2 = lcp_driver_block( blmat , q , &myMethod , z2 , w2 , &iter2 , &titer2 , &err2 ); */

  /*   /\* #3 QP TEST *\/ */
  /* #ifdef BAVARD */
  /*   printf("**** QP TEST ******\n"); */
  /* #endif */
  /*   for( i = 0 ; i < dim ; ++i ) z4[i] = 0.0; */

  /*   myMethod.lcp =  method_lcp4; */
  /*   info4 = lcp_driver_block( blmat , q , &myMethod , z4 , w4 , &iter4 , &titer4 , &err4 ); */

  /*   /\* #4 NSQP TEST *\/ */
  /* #ifdef BAVARD */
  /*   printf("**** NSQP TEST ****\n"); */
  /* #endif */
  /*   for( i = 0 ; i < dim ; ++i ) z5[i] = 0.0; */

  /*   myMethod.lcp =  method_lcp5; */
  /*   info5 = lcp_driver_block( blmat , q , &myMethod , z5 , w5 , &iter5 , &titer5 , &err5 ); */

  /*   /\* #5 LEXICO LEMKE TEST *\/ */
  /* #ifdef BAVARD */
  /*   printf("**** Lemke TEST ***\n"); */
  /* #endif */
  /*   for( i = 0 ; i < dim ; ++i ) {z6[i] = 0.0; w6[i] = 0.0; } */

  /*   iter6 = 0; */
  /*   titer6 = 0; */
  /*   err6 = 0.; */

  /*   myMethod.lcp =  method_lcp6; */
  /*   info6 = lcp_driver_block( blmat , q , &myMethod , z6 , w6 , &iter6 , &titer6 , &err6 ); */

  /*   /\* #6 NEWTONMIN TEST *\/ */
  /* #ifdef BAVARD */
  /*   printf("**** Newton TEST **\n"); */
  /* #endif */
  /*   for( i = 0 ; i < dim ; ++i ) z7[i] = 0.0; */

  /*   myMethod.lcp =  method_lcp7; */
  /*   info7 = lcp_driver_block( blmat , q , &myMethod , z7 , w7 , &iter7 , &titer7 , &err7 ); */

  /*   /\* #7 LATIN TEST *\/ */
  /* #ifdef BAVARD */
  /*   printf("**** LATIN TEST ***\n"); */
  /* #endif */
  /*   for( i = 0 ; i < dim ; ++i ) z3[i] = 0.0; */

  /*   myMethod.lcp =  method_lcp3; */
  /*   info3 = lcp_driver_block( blmat , q , &myMethod , z3 , w3 , &iter3 , &titer3 , &err3 ); */

  /*   /\* #8 RPGS TEST *\/ */
  /* #ifdef BAVARD */
  /*   printf("**** RPGS TEST ****\n"); */
  /* #endif */

  /*   for( i = 0 ; i < dim ; ++i ) z8[i] = 0.0; */

  /*   myMethod.lcp =  method_lcp8; */
  /*   info8 = lcp_driver_block( blmat , q , &myMethod, z8 , w8 , &iter8 , &titer8 , &err8 ); */


  /* #ifdef BAVARD */
  /*   printf(" *** z ************************************** ***\n"); */
  /*   printf("\n   PGS RESULT : ");  for( i = 0 ; i < dim ; ++i ) printf(" %14.7e " , z1[i] ); */
  /*   printf("\n    CPG RESULT : ");  for( i = 0 ; i < dim ; ++i ) printf(" %14.7e " , z2[i] ); */
  /*   printf("\n     QP RESULT : ");  for( i = 0 ; i < dim ; ++i ) printf(" %14.7e " , z4[i] ); */
  /*   printf("\n   NSQP RESULT : ");  for( i = 0 ; i < dim ; ++i ) printf(" %14.7e " , z5[i] ); */
  /*   printf("\n  Lemke RESULT : ");  for( i = 0 ; i < dim ; ++i ) printf(" %14.7e " , z6[i] ); */
  /*   printf("\n Newton RESULT : ");  for( i = 0 ; i < dim ; ++i ) printf(" %14.7e " , z7[i] ); */
  /*   printf("\n  LATIN RESULT : ");  for( i = 0 ; i < dim ; ++i ) printf(" %14.7e " , z3[i] ); */
  /*   printf("\n   RPGS RESULT : ");  for( i = 0 ; i < dim ; ++i ) printf(" %14.7e " , z8[i] ); */
  /*   printf("\n\n"); */
  /*   printf(" *** w ************************************** ***\n"); */
  /*   printf("\n   PGS RESULT : ");  for( i = 0 ; i < dim ; ++i ) printf(" %14.7e " , w1[i] ); */
  /*   printf("\n    CPG RESULT : ");  for( i = 0 ; i < dim ; ++i ) printf(" %14.7e " , w2[i] ); */
  /*   printf("\n     QP RESULT : ");  for( i = 0 ; i < dim ; ++i ) printf(" %14.7e " , w4[i] ); */
  /*   printf("\n   NSQP RESULT : ");  for( i = 0 ; i < dim ; ++i ) printf(" %14.7e " , w5[i] ); */
  /*   printf("\n  Lemke RESULT : ");  for( i = 0 ; i < dim ; ++i ) printf(" %14.7e " , w6[i] ); */
  /*   printf("\n Newton RESULT : ");  for( i = 0 ; i < dim ; ++i ) printf(" %14.7e " , w7[i] ); */
  /*   printf("\n  LATIN RESULT : ");  for( i = 0 ; i < dim ; ++i ) printf(" %14.7e " , w3[i] ); */
  /*   printf("\n   RPGS RESULT : ");  for( i = 0 ; i < dim ; ++i ) printf(" %14.7e " , w8[i] ); */
  /*   printf("\n\n"); */
  /*   printf(" INFO RESULT\n"); */

  /*   printf(" -- SOLVEUR ------ BLOCK I/P ----- TOTAL I/P ----- ERR ----- COMP "); */

  /*   comp = DDOT( dim , z1 , incx , w1 , incy ); */

  /*   printf("\n    PGS   (LOG:%1d)|      %5d | %5d | %10.4g | %10.4g |",info1,iter1,titer1,err1,comp); */

  /*   comp = DDOT( dim , z2 , incx , w2 , incy ); */

  /*   printf("\n    CPG    (LOG:%1d)|      %5d | %5d | %10.4g | %10.4g |",info2,iter2,titer2,err2,comp); */

  /*   comp = DDOT( dim , z4 , incx , w4 , incy ); */

  /*   printf("\n    QP     (LOG:%1d)|      %5d | %5d | %10.4g | %10.4g |",info4,iter4,titer4,err4,comp); */

  /*   comp = DDOT( dim , z5 , incx , w5 , incy ); */

  /*   printf("\n    NSQP   (LOG:%1d)|      %5d | %5d | %10.4g | %10.4g |",info5,iter5,titer5,err5,comp); */

  /*   comp = DDOT( dim , z6 , incx , w6 , incy ); */

  /*   /\*  printf("\n    Lemke  (LOG:%1d)|",info6); */
  /*       printf("\n    Lemke    %5d ",iter6); */
  /*       printf("\n    Lemke %5d |",titer6); */
  /*       printf("\n    Lemke %10.4g",err6); */
  /*       printf("\n    Lemke %10.4g |",comp);*\/ */
  /*   printf("\n    Lemke  (LOG:%1d)|      %5d | %5d | %10.4g | %10.4g |",info6,iter6,titer6,err6,comp); */

  /*   comp = DDOT( dim , z7 , incx , w7 , incy ); */

  /*   printf("\n    Newton (LOG:%1d)|      %5d | %5d | %10.4g | %10.4g |",info7,iter7,titer7,err7,comp); */

  /*   comp = DDOT( dim , z3 , incx , w3 , incy ); */

  /*   printf("\n    LATIN  (LOG:%1d)|      %5d | %5d | %10.4g | %10.4g |",info3,iter3,titer3,err3,comp); */

  /*   comp = DDOT( dim , z8 , incx , w8 , incy ); */

  /*   printf("\n    RPGS   (LOG:%1d)|      %5d | %5d | %10.4g | %10.4g |",info8,iter8,titer8,err8,comp); */

  /*   printf("\n \n"); */

  /* #endif */

  /*   free( z1 ); */
  /*   free( w1 ); */
  /*   free( z2 ); */
  /*   free( w2 ); */
  /*   free( z3 ); */
  /*   free( w3 ); */
  /*   free( z4 ); */
  /*   free( w4 ); */
  /*   free( z5 ); */
  /*   free( w5 ); */
  /*   free( z6 ); */
  /*   free( w6 ); */
  /*   free( z7 ); */
  /*   free( w7 ); */
  /*   free( z8 ); */
  /*   free( w8 ); */
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
  printf("* *** STARTING TEST MMC... *** * \n");
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

  vecM = (double*)malloc(n * n * sizeof(double));

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


  q = (double*)malloc(n * sizeof(double));

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
  LinearComplementarity_Problem problem;
  problem.size = n;
  problem.q = q;
  NumericsMatrix MM;
  MM.matrix0 = vecM;
  MM.size0 = n;
  MM.size1 = n;
  MM.storageType = 0;

  problem.M = &MM;

  test_lcp_series(&problem);

  free(vecM);
  free(q);

  printf("* *** ******************** *** * \n");
  printf("* *** END OF TEST MMC      *** * \n");
  printf("* *** ******************** *** * \n");


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

  printf("* *** ******************** *** * \n");
  printf("* *** STARTING TEST MATRIX *** * \n");
  printf("* *** ******************** *** * \n");


  iter  = 0;
  criteria = 0.0;

  NBTEST = 10;

  /****************************************************************/
#ifdef BAVARD
  printf("\n ********** BENCHMARK FOR LCP_DRIVER ********** \n\n");
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
    case 8:
      printf("\n\n TRIVIAL 3 LCP ");
      if ((LCPfile = fopen("MATRIX/trivial3.dat", "r")) == NULL)
      {
        perror("fopen LCPfile: trivial3.dat");
        exit(1);
      }
      break;
    case 9:
      printf("\n\n BUCKCONVERTERREGUL2 LCP ");
      if ((LCPfile = fopen("MATRIX/buckconverterregul2.dat", "r")) == NULL)
      {
        perror("fopen LCPfile: buckconverterregul2.dat");
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

    LinearComplementarity_Problem problem;
    problem.size = dim;
    problem.q = q;
    NumericsMatrix MM;
    MM.matrix0 = vecM;
    MM.size0 = dim;
    MM.size1 = dim;
    MM.storageType = 0;

    problem.M = &MM;

    test_lcp_series(&problem);

    free(sol);
    free(vecM);
    free(q);
  }
  printf("* *** ******************** *** * \n");
  printf("* *** END OF TEST MATRIX   *** * \n");
  printf("* *** ******************** *** * \n");


}

/*
******************************************************************************
*/

void test_blockmatrix(void)
{

  FILE *LCPfile;

  int i, j, isol, NBTEST, itest;
  int dim;
  int sizebl;

  double *q , *sol;

  char val[20];

  int iter, titer;
  double criteria;

  SparseBlockStructuredMatrix blmat;

  printf("* *** ********************         *** * \n");
  printf("* *** STARTING TEST BLOCKMATRIX... *** * \n");
  printf("* *** ********************         *** * \n");

  iter  = 0;
  titer = 0;
  criteria = 0.0;

  NBTEST = 6;

  /****************************************************************/
#ifdef BAVARD
  printf("\n\n ******** BENCHMARK FOR LCP_DRIVER_BLOCK ******** \n\n");
#endif
  /****************************************************************/

  for (itest = 0 ; itest < NBTEST ; ++itest)
  {

    switch (itest)
    {
    case 0:
      printf("\n\n BLOC22_MATHIEU1 LCP BLOCK");
      if ((LCPfile = fopen("BLOCKMATRIX_NEW/bloc22_mathieu1.dat", "r")) == NULL)
      {
        perror("fopen LCPfile: bloc22_mathieu1.dat");
        exit(1);
      }
      break;

    case 1:
      printf("\n\n MONOBLOC_MATHIEU1 LCP BLOCK");
      if ((LCPfile = fopen("BLOCKMATRIX_NEW/monobloc_mathieu1.dat", "r")) == NULL)
      {
        perror("fopen LCPfile: monobloc_mathieu1.dat");
        exit(1);
      }
      break;

    case 2:
      printf("\n\n MONOBLOC_ORTIZ LCP BLOCK");
      if ((LCPfile = fopen("BLOCKMATRIX_NEW/monobloc_ortiz.dat", "r")) == NULL)
      {
        perror("fopen LCPfile: monobloc_ortiz.dat");
        exit(1);
      }
      break;

    case 3:
      printf("\n\n BLOC22_ORTIZ LCP BLOCK");
      if ((LCPfile = fopen("BLOCKMATRIX_NEW/bloc22_ortiz.dat", "r")) == NULL)
      {
        perror("fopen LCPfile: bloc22_ortiz.dat");
        exit(1);
      }
      break;

    case 4:
      printf("\n\n BLOC31_ORTIZ LCP BLOCK");
      if ((LCPfile = fopen("BLOCKMATRIX_NEW/bloc31_ortiz.dat", "r")) == NULL)
      {
        perror("fopen LCPfile: bloc31_ortiz.dat");
        exit(1);
      }
      break;

    case 5:
      printf("\n\n BLOC3333_MATHIEU2 LCP BLOCK");
      if ((LCPfile = fopen("BLOCKMATRIX_NEW/bloc3333_mathieu2.dat", "r")) == NULL)
      {
        perror("fopen LCPfile: bloc3333_mathieu2.dat");
        exit(1);
      }
      break;

    }

    fscanf(LCPfile , "%d" , &blmat.nbblocks);
    fscanf(LCPfile , "%d" , &blmat.size);

    blmat.blocksize = (int*) malloc(blmat.size * sizeof(int));
    for (i = 0 ; i < blmat.size ; i++) fscanf(LCPfile , "%d" , &blmat.blocksize[i]);

    blmat.RowIndex = (int*)malloc(blmat.nbblocks * sizeof(int));
    blmat.ColumnIndex = (int*)malloc(blmat.nbblocks * sizeof(int));

    for (i = 0 ; i < blmat.nbblocks ; i++)
    {
      fscanf(LCPfile , "%d" , &blmat.RowIndex[i]);
      blmat.RowIndex[i]--;
      fscanf(LCPfile , "%d" , &blmat.ColumnIndex[i]);
      blmat.ColumnIndex[i]--;
    }

    dim = blmat.blocksize[blmat.size - 1];

    q = (double*)malloc(dim * sizeof(double));
    blmat.block = (double**)malloc(blmat.nbblocks * sizeof(double*));
    int numberOfRows, numberOfColumns, pos;

    for (i = 0 ; i < blmat.nbblocks ; i++)
    {
      pos = blmat.RowIndex[i];
      numberOfRows = blmat.blocksize[pos] - blmat.blocksize[pos - 1];
      pos = blmat.ColumnIndex[i];
      numberOfColumns = blmat.blocksize[pos] - blmat.blocksize[pos - 1];
      sizebl = numberOfRows * numberOfColumns;
      blmat.block[i] = (double*)malloc(sizebl * sizeof(double));
      for (j = 0 ; j < sizebl ; j++)
      {
        fscanf(LCPfile, "%s", val);
        blmat.block[i][j] = atof(val);
      }
    }

    for (i = 0 ; i < dim ; i++)
    {
      fscanf(LCPfile , "%s" , val);
      q[i] = atof(val);
    }

    fscanf(LCPfile , "%s" , val);

    if (!feof(LCPfile))
    {
      sol  = (double*)malloc(dim * sizeof(double));
      sol[0] = atof(val);
      isol = 1;
      for (i = 1 ; i < dim ; i++)
      {
        fscanf(LCPfile , "%s" , val);
        sol[i] = atof(val);
      }
    }
    else
      isol = 0;

    fclose(LCPfile);

#ifdef BAVARD
    printf("\n exact solution : ");
    if (isol) for (i = 0 ; i < dim ; i++) printf(" %10.4g " , sol[i]);
    else printf(" unknown ");
    printf("\n");
#endif

    test_lcp_block_series(&blmat , q);

    if (isol) free(sol);
    free(q);
    free(blmat.RowIndex);
    free(blmat.ColumnIndex);
    free(blmat.blocksize);
    for (i = 0 ; i < blmat.nbblocks ; i++) free(blmat.block[i]);
    free(blmat.block);

  }
  printf("* *** ********************     *** * \n");
  printf("* *** END OF TEST BLOCKMATRIX  *** * \n");
  printf("* *** ********************     *** * \n");
}

/*
******************************************************************************
*/

int main(void)
{

  test_mmc();
  test_matrix();
  //  test_blockmatrix();

  return 0;
}

