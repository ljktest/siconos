/* Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.
 *
 * Copyright 2018 INRIA.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

/*
  Tests functions for NumericsMatrix structure

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "NumericsMatrix.h"
#include <math.h>
#include "numericsMatrixTestFunction.h"
#include "SiconosLapack.h"

static int test_NM_row_prod_no_diag_non_square(NumericsMatrix* M3, NumericsMatrix* M4)
{

  printf("== Numerics tests: NM_row_prod_no_diag_non_square(NumericsMatrix,vector) == \n");
  int i ,  m = M3->size1;
  double * x = (double *)malloc(m * sizeof(double));

  for (i = 0; i < m; i++)
  {
    x[i] = i + 1;
  }

  int min = 2;
  int max = 6;
  int sizeY = max - min;
  int sizeX = m;
  /* Computes yRef = subA*x, subA = A limited to row min to max*/
  double * y = (double *)malloc(sizeY * sizeof(double));
  double yref[4];
  //  int incx = n, incy =1;
  double tol = 1e-12;
  int info = 0;
  /*   for(i=0;i<sizeY;i++) */
  /*     yref[i]=cblas_ddot(n, &(M3->matrix0[min+i]), incx, x, incy); */

  /*   NM_row_prod_no_diag(n,sizeY,min,M3,x,y,1); */
  /*   for(i = 0; i< sizeY; i++) */
  /*     { */
  /*       if( fabs(y[i]-yref[i])>tol) info = 1;  */
  /* /\*       printf("%lf\n", fabs(y[i]-yref[i])); *\/ */
  /*     } */
  /*   if(info ==0) */
  /*     printf("Step 0 ( y = subA*x, double* storage) ok ...\n"); */
  /*   else */
  /*     printf("Step 0 ( y = subA*x, double* storage) failed ...\n"); */

  /*   /\* += *\/ */
  /*   NM_row_prod_no_diag(n,sizeY,min,M3,x,y,0); */
  /*   for(i = 0; i< sizeY; i++) */
  /*     { */
  /*       if( fabs(y[i]-2*yref[i])>tol) info = 1;  */
  /*       /\*       printf("%lf\n", fabs(y[i]-2*yref[i]));  *\/ */
  /*     } */
  /*   if(info ==0) */
  /*     printf("Step 1 ( y += subA*x, double* storage) ok ...\n"); */
  /*   else */
  /*     printf("Step 1 ( y += subA*x, double* storage) failed ...\n"); */

  free(y);
  sizeY = 2;
  int pos = 1; // pos of the required row of blocks
  y = (double *)malloc(sizeY * sizeof(double));
  y[0] = 0;
  y[1] = 0;
  yref[0] = 0;
  yref[1] = 0;

  /* Sparse ... */
  NM_row_prod_no_diag(sizeX, sizeY, pos, SIZE_MAX, M4, x, y, NULL, 1);
  for (i = 0; i < sizeY; i++)
  {
    if (fabs(y[i] - yref[i]) > tol) info = 1;
    /*       printf("%lf\n", fabs(y[i]-yref[i])); */
    /*       printf("%lf\n", y[i]); */
    /*       printf("%lf\n", yref[i]); */
  }

  printf("\n");
  yref[0] = 10;
  yref[1] = 14;

  NM_row_prod_no_diag(sizeX, sizeY, pos + 1, SIZE_MAX, M4, x, y, NULL, 1);

  for (i = 0; i < sizeY; i++)
  {
    if (fabs(y[i] - yref[i]) > tol) info = 1;
    /*      printf("%lf\n", fabs(y[i]-yref[i])); */
    /*             printf("%lf\n", y[i]); */
    /*             printf("%lf\n", yref[i]); */
  }

  if (info == 0)
    printf("Step 2 ( y = subA*x, sparse storage) ok ...\n");
  else
    printf("Step 2 ( y = subA*x,  sparse storage) failed ...\n");

  /* Sparse, += ... */
  NM_row_prod_no_diag(sizeX, sizeY, pos + 1, SIZE_MAX, M4, x, y, NULL, 0);
  for (i = 0; i < sizeY; i++)
  {
    if (fabs(y[i] - 2 * yref[i]) > tol) info = 1;
    /*           printf("%lf\n", fabs(y[i]-yref[i])); */
    /*             printf("%lf\n", y[i]); */
    /*             printf("%lf\n", yref[i]); */
  }
  if (info == 0)
    printf("Step 3 ( y += subA*x, sparse storage) ok ...\n");
  else
    printf("Step 3 ( y += subA*x,  sparse storage) failed ...\n");


  free(x);
  free(y);
  printf("== End of test NM_row_prod_no_diag_non_square(NumericsMatrix,vector), result = %d\n", info);

  return info;
}
int main(void)
{

  printf("========= Starts Numerics tests for NumericsMatrix ========= \n");

  int i, nmm = 4 ;
  NumericsMatrix ** NMM = (NumericsMatrix **)malloc(nmm * sizeof(NumericsMatrix *)) ;
  int info = test_build_first_4_NM(NMM);
  if (info != 0)
  {
    printf("Construction failed ...\n");
    return info;
  }
  printf("Construction ok ...\n");

  info = test_NM_row_prod_no_diag_non_square(NMM[2], NMM[3]);
  printf("End of Sub-Prod no diag Non Square...\n");
  if (info != 0) return info;

  /* free memory */

  for (i = 0 ; i < nmm; i++)
  {
    NM_free(NMM[i]);
    free(NMM[i]);
  }

  free(NMM);



  printf("========= End Numerics tests for NumericsMatrix ========= \n");
  return info;
}

