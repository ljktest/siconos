/* Siconos-Numerics version 3.0.0, Copyright INRIA 2005-2008.
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

|A C| |u| |a| |0|
|   |*| |+| |=| |
|D B| |v| |b| |w|
0<z*v>0
dim(u)=mm
dim(v)=nn

**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LA.h"
#include "MLCP_Solvers.h"
#include <math.h>
#include "mlcp_direct_FB.h"
#include "mlcp_direct.h"
#include "mlcp_tool.h"

static int sN;
static int sM;

static int * siWorkFB = 0;
static int * siWorkDirect = 0;
static double * sdWorkFB = 0;
static double * sdWorkDirect = 0;



int mlcp_direct_FB_getNbIWork(MixedLinearComplementarity_Problem* problem, Solver_Options* options)
{
  int aux = mlcp_FB_getNbIWork(problem, options);
  return mlcp_direct_getNbIWork(problem, options) + aux;
}
int mlcp_direct_FB_getNbDWork(MixedLinearComplementarity_Problem* problem, Solver_Options* options)
{
  int aux = mlcp_FB_getNbDWork(problem, options);
  return mlcp_direct_getNbDWork(problem, options) + aux;
}



/*
 *options->iparam[5] : n0 number of possible configuration.
 * dparam[5] : (in) a positive value, tolerane about the sign.
 *options->iWork : double work memory of  mlcp_direct_FB_getNbIWork() integers
 *options->dWork : double work memory of mlcp_direct_FB_getNbDWork() doubles
 *
 *
 */

void mlcp_direct_FB_init(MixedLinearComplementarity_Problem* problem, Solver_Options* options)
{
  sN = problem->n;
  sM = problem->m;
  int iOffset = mlcp_direct_getNbIWork(problem, options);
  int dOffset = mlcp_direct_getNbDWork(problem, options);
  siWorkFB = options->iWork + iOffset;
  siWorkDirect = options->iWork;
  sdWorkFB = options->dWork + dOffset;
  sdWorkDirect = options->dWork;

  mlcp_direct_init(problem, options);
  options->dWork = sdWorkFB;
  options->iWork = siWorkFB;

  mlcp_FB_init(problem, options);
  options->dWork = sdWorkDirect;
  options->iWork = siWorkDirect;


}
void mlcp_direct_FB_reset()
{
  mlcp_direct_reset();
  mlcp_FB_reset();
}

/*
 * The are no memory allocation in mlcp_direct, all necessary memory must be allocated by the user.
 *
 *options:
 * iparam[0] : (in) verbose.
 * dparam[0] : (in) a positive value, tolerane about the sign used by the path algo.
 * iparam[5] : (in)  n0 number of possible configuration.
 * dparam[5] : (in) a positive value, tolerane about the sign.
 * dWork : working float zone size : n + m + n0*(n+m)*(n+m)  . MUST BE ALLOCATED BY THE USER.
 * iWork : working int zone size : (n + m)*(n0+1) + nO*m. MUST BE ALLOCATED BY THE USER.
 * double *z : size n+m
 * double *w : size n+m
 * info : output. info == 0 if success
 */
void mlcp_direct_FB(MixedLinearComplementarity_Problem* problem, double *z, double *w, int *info, Solver_Options* options)
{
  /*First, try direct solver*/
  mlcp_direct(problem, z, w, info, options);
  if (*info)
  {
    /*solver direct failed, so run the path solver.*/
    mlcp_FB(problem, z, w, info, options);
    if (!(*info))
    {
      /*       for (i=0;i<problem->n+problem->m;i++){ */
      /*  printf("w[%d]=%f z[%d]=%f\t",i,w[i],i,z[i]);  */
      /*       } */
      mlcp_direct_addConfigFromWSolution(problem, w + sN);
    }
  }
}
