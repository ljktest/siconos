/* Siconos is a program dedicated to modeling, simulation and control
 * of non smooth dynamical systems.

 * Copyright 2016 INRIA.

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

 * http://www.apache.org/licenses/LICENSE-2.0

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
#include "NumericsMatrix.h"
#include <math.h>
#include "numericsMatrixTestFunction.h"
int main(void)
{

  printf("========= Starts Numerics tests for NumericsMatrix ========= \n");

  int i, nmm = 4 ;
  NumericsMatrix ** NMM = (NumericsMatrix **)malloc(nmm * sizeof(NumericsMatrix *)) ;


  for (i = 0 ; i < nmm; i++)
  {
    NMM[i] = newNumericsMatrix();
  }


  int info = test_BuildNumericsMatrix(NMM);
  if (info != 0)
  {
    printf("Construction failed ...\n");
    return info;
  }
  printf("Construction ok ...\n");

  info = test_rowProdNoDiagNonSquare(NMM[2], NMM[3]);
  printf("End of Sub-Prod no diag Non Square...\n");
  if (info != 0) return info;

  /* free memory */

  for (i = 0 ; i < nmm; i++)
  {
    freeNumericsMatrix(NMM[i]);
    free(NMM[i]);
  }

  free(NMM);



  printf("========= End Numerics tests for NumericsMatrix ========= \n");
  return info;
}

