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
#include <string.h>
#include <time.h>
#include <float.h>

#include "NumericsOptions.h"
#include "SOCLCP_Solvers.h"
#include "NonSmoothDrivers.h"

char *  SICONOS_SOCLCP_NSGS_STR = "SOCLCP_NSGS";
char *  SICONOS_SOCLCP_NSGSV_STR = "SOCLCP_NSGSV";
char *  SICONOS_SOCLCP_TFP_STR = "SOCLCP_TFP";
char *  SICONOS_SOCLCP_LOCALAC_STR = "SOCLCP_LOCALAC";
char *  SICONOS_SOCLCP_LOCALFB_STR = "SOCLCP_LOCALFB";
char *  SICONOS_SOCLCP_DSFP_STR = "SOCLCP_DeSaxceFixedPoint";
char *  SICONOS_SOCLCP_NCPGlockerFBFixedPoint_STR = "SOCLCP_NCPGlockerFBFixedPoint";
char *  SICONOS_SOCLCP_AlartCurnierNewton_STR = "SOCLCP_AlartCurnierNewton";
char *  SICONOS_SOCLCP_DampedAlartCurnierNewton_STR = "SOCLCP_DampedAlartCurnierNewton";
char *  SICONOS_SOCLCP_NCPGlockerFBNewton_STR = "SOCLCP_NCPGlockerFBNewton";
char * SICONOS_SOCLCP_ProjectionOnConeWithDiagonalization_STR = "SOCLCP_ProjectionOnConeWithDiagonalization";
char * SICONOS_SOCLCP_ProjectionOnCone_STR = "SOCLCP_ProjectionOnCone";
char * SICONOS_SOCLCP_ProjectionOnConeWithLocalIteration_STR = "SOCLCP_ProjectionOnConeWithLocalIteration";
char * SICONOS_SOCLCP_projectionOnConeWithRegularization_STR = "SOCLCP_projectionOnConeWithRegularization";
char * SICONOS_SOCLCP_NCPGlockerFBPATH_STR = "SOCLCP_NCPGlockerFBPATH";
char * SICONOS_SOCLCP_projectionOnCylinder_STR = "SOCLCP_projectionOnCylinder";
char * SICONOS_SOCLCP_ProjectionOnCone_velocity_STR = "SOCLCP_ProjectionOnCone_velocity";
char * SICONOS_SOCLCP_PGoC_STR = "SOCLCP_PGoC";
char * SICONOS_SOCLCP_DeSaxceFixedPoint_STR = "SOCLCP_DeSaxceFixedPoint";
char * SICONOS_SOCLCP_EG_STR = "SOCLCP_ExtraGradient";
char * SICONOS_SOCLCP_FPP_STR = "SOCLCP_FixedPointProjection";
char * SICONOS_SOCLCP_VI_EG_STR = "SOCLCP_VI_ExtraGradient";
char * SICONOS_SOCLCP_VI_FPP_STR = "SOCLCP_VI_FixedPointProjection";
char * SICONOS_SOCLCP_HP_STR = "SOCLCP_HyperplaneProjection";
char * SICONOS_SOCLCP_PROX_STR = "SOCLCP_PROX";
char * SICONOS_SOCLCP_QUARTIC_STR = "SOCLCP_QUARTIC";
char * SICONOS_SOCLCP_QUARTIC_NU_STR = "SOCLCP_QUARTIC_NU";

void snPrintf(int level, SolverOptions* opts, const char *fmt, ...);

int soclcp_driver(SecondOrderConeLinearComplementarityProblem* problem,
                  double *r, double *v,
                  SolverOptions* options,
                  NumericsOptions* global_options)
{
  if(options == NULL)
    numericsError("soclcp_driver", "null input for solver and/or global options");

  int setnumericsoptions=0;

  /* Set global options */
  if(global_options)
  {
    setNumericsOptions(global_options);
    options->numericsOptions = (NumericsOptions*) malloc(sizeof(NumericsOptions));
    options->numericsOptions->verboseMode = global_options->verboseMode;
    setnumericsoptions=1;
  }

  int NoDefaultOptions = options->isSet; /* true(1) if the SolverOptions structure has been filled in else false(0) */

  if(!NoDefaultOptions)
    readSolverOptions(3, options);

  if(verbose > 0)
    printSolverOptions(options);

  /* Solver name */
  /*char * name = options->solverName;*/

  int info = -1 ;

  /* Check for trivial case */
  info = soclcp_checkTrivialCase(problem, v, r, options);


  if(info == 0)
    return info;


  switch(options->solverId)
  {
  /* Non Smooth Gauss Seidel (NSGS) */
  case SICONOS_SOCLCP_NSGS:
  {
    snPrintf(1, options,
             " ========================== Call NSGS solver for Second Order Cone LCP problem ==========================\n");
    soclcp_nsgs(problem, r , v , &info , options);
    break;
  }
  /* case SICONOS_SOCLCP_NSGSV: */
  /* { */
  /*   snPrintf(1, options, */
  /*            " ========================== Call NSGSV solver for Second Order Cone LCP problem ==========================\n"); */
  /*   soclcp_nsgs_v(problem, r , v , &info , options); */
  /*   break; */
  /* } */
  /* /\* Proximal point algorithm *\/ */
  /* case SICONOS_SOCLCP_PROX: */
  /* { */
  /*   snPrintf(1, options, */
  /*            " ========================== Call PROX (Proximal Point) solver for Second Order Cone LCP problem ==========================\n"); */
  /*   soclcp_proximal(problem, r , v , &info , options); */
  /*   break; */
  /* } */
  /* /\* Tresca Fixed point algorithm *\/ */
  /* case SICONOS_SOCLCP_TFP: */
  /* { */
  /*   snPrintf(1, options,  */
  /*            " ========================== Call TFP (Tresca Fixed Point) solver for Second Order Cone LCP problem ==========================\n"); */
  /*   soclcp_TrescaFixedPoint(problem, r , v , &info , options); */
  /*   break; */
  /* } */
  /* /\* De Saxce Fixed point algorithm *\/ */
  /* case SICONOS_SOCLCP_DSFP: */
  /* { */
  /*   snPrintf(1, options,  */
  /*            " ========================== Call DeSaxce Fixed Point (DSFP) solver for Second Order Cone LCP problem ==========================\n"); */
  /*   soclcp_DeSaxceFixedPoint(problem, r , v , &info , options); */
  /*   break; */
  /* } */
  /* /\* Fixed point projection algorithm *\/ */
  /* case SICONOS_SOCLCP_FPP: */
  /* { */
  /*   snPrintf(1, options,  */
  /*            " ========================== Call Fixed Point Projection (FPP) solver for Second Order Cone LCP problem ==========================\n"); */
  /*   soclcp_fixedPointProjection(problem, r , v , &info , options); */
  /*   break; */
  /* } */

  /* /\* Extra Gradient algorithm *\/ */
  /* case SICONOS_SOCLCP_EG: */
  /* { */
  /*   snPrintf(1, options,  */
  /*            " ========================== Call ExtraGradient (EG) solver for Second Order Cone LCP problem ==========================\n"); */
  /*   soclcp_ExtraGradient(problem, r , v , &info , options); */
  /*   break; */
  /* } */
  /* /\* VI Fixed Point Projection algorithm *\/ */
  /* case SICONOS_SOCLCP_VI_FPP: */
  /* { */
  /*   snPrintf(1, options, */
  /*            " ========================== Call VI_FixedPointProjection (VI_FPP) solver for Second Order Cone LCP problem ==========================\n"); */
  /*   soclcp_VI_FixedPointProjection(problem, r , v , &info , options); */
  /*   break; */
  /* } */
  /* /\* VI Extra Gradient algorithm *\/ */
  /* case SICONOS_SOCLCP_VI_EG: */
  /* { */
  /*   snPrintf(1, options, */
  /*            " ========================== Call VI_ExtraGradient (VI_EG) solver for Second Order Cone LCP problem ==========================\n"); */
  /*   soclcp_VI_ExtraGradient(problem, r , v , &info , options); */
  /*   break; */
  /* } */
  /* /\* Hyperplane Projection algorithm *\/ */
  /* case SICONOS_SOCLCP_HP: */
  /* { */
  /*   snPrintf(1, options,  */
  /*            " ========================== Call Hyperplane Projection (HP) solver for Second Order Cone LCP problem ==========================\n"); */
  /*   soclcp_HyperplaneProjection(problem, r , v , &info , options); */
  /*   break; */
  /* } */
  /* /\* Alart Curnier in local coordinates *\/ */
  /* case SICONOS_SOCLCP_LOCALAC: */
  /* { */
  /*   snPrintf(1, options,  */
  /*            " ========================== Call Alart Curnier solver for Second Order Cone LCP problem ==========================\n"); */
  /*   if (problem->M->matrix0) */
  /*   { */
  /*     soclcp_localAlartCurnier(problem, r , v , &info , options); */
  /*   } */
  /*   else */
  /*   { */
  /*     soclcp_localAlartCurnier(problem, r , v , &info , options); */
  /*   } */
  /*   break; */
  /* } */
  /* /\* Fischer Burmeister in local coordinates *\/ */
  /* case SICONOS_SOCLCP_LOCALFB: */
  /* { */
  /*   snPrintf(1, options,  */
  /*            " ========================== Call Fischer Burmeister solver for Second Order Cone LCP problem ==========================\n"); */
  /*   soclcp_localFischerBurmeister(problem, r , v , &info , options); */
  /*   break; */
  /* } */
  /* case SICONOS_SOCLCP_QUARTIC_NU: */
  /* case SICONOS_SOCLCP_QUARTIC: */
  /* { */
  /*   snPrintf(1, options,  */
  /*            " ========================== Call Quartic solver for Second Order Cone LCP problem ==========================\n"); */
  /*   soclcp_unitary_enumerative(problem, r , v , &info , options); */
  /*   break; */
  /* } */
  /* case SICONOS_SOCLCP_AlartCurnierNewton: */
  /* case SICONOS_SOCLCP_DampedAlartCurnierNewton: */
  /* { */
  /*   snPrintf(1, options,  */
  /*            " ========================== Call Quartic solver for Second Order Cone LCP problem ==========================\n"); */
  /*   info =soclcp_Newton_solve(problem, r , options); */
  /*   break; */
  /* } */
  default:
  {
    fprintf(stderr, "Numerics, SecondOrderConeLinearComplementarity_driver failed. Unknown solver.\n");
    exit(EXIT_FAILURE);

  }
  }

  if(setnumericsoptions)
  {
    free(options->numericsOptions);
    options->numericsOptions = NULL;
  }

  return info;

}

int soclcp_checkTrivialCase(SecondOrderConeLinearComplementarityProblem* problem, double* v,
                            double* r, SolverOptions* options)
{
  /* Number of contacts */
  int nc = problem->nc;
  double* q = problem->q;

  int i = 0;
  /* Dimension of the problem */
  int n = problem->n;

  /* r=0 ?*/
  int normal=0;
  for(i = 0; i < nc; i++)
  {
    normal = problem->coneIndex[i];
    if(q[normal] < -DBL_EPSILON)
      return -1;

  }
  for(i = 0 ; i < n ; ++i)
  {
    v[i] = q[i];
    r[i] = 0.;
  }
  options->iparam[2] = 0;
  options->iparam[4] = 0;
  options->dparam[1] = 0.0;
  options->dparam[3] = 0.0;
  if(verbose == 1)
    printf("SecondOrderConeLinearComplementarity driver, trivial solution r = 0, v = q.\n");
  return 0;
}
