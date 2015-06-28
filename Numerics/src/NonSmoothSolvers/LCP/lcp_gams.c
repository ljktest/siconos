/*
   Use this command to compile the example:
   cl xp_example2.c api/gdxcc.c api/optcc.c api/gamsxcc.c -Iapi
   */

/*
   This program performs the following steps:
   1. Generate a gdx file with demand data
   2. Calls GAMS to solve a simple transportation model
   (The GAMS model writes the solution to a gdx file)
   3. The solution is read from the gdx file
   */

/* GAMS stuff */

#include "NumericsMatrix.h"
#include "LinearComplementarityProblem.h"
#include "LCP_Solvers.h"

#ifdef HAVE_GAMS_C_API

#include "GAMSlink.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>


void lcp_gams(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options)
{

  assert(problem);
  assert(problem->size > 0);
  assert(problem->M);
  assert(problem->q);

  /* Handles to the GAMSX, GDX, and Option objects */
  gamsxHandle_t Gptr = NULL;
  idxHandle_t Xptr = NULL;
  optHandle_t Optr = NULL;

  int status;
  char sysdir[GMS_SSSIZE], model[GMS_SSSIZE], msg[GMS_SSSIZE];
  const char defModel[] = SPACE_CONC(GAMS_MODELS_SHARE_DIR, "/lcp.gms");
  const char defGAMSdir[] = GAMS_DIR;

  SN_Gams_set_dirs(options->solverParameters, defModel, defGAMSdir, model, sysdir, "/lcp.gms");

  /* Create objects */
  if (! gamsxCreateD (&Gptr, sysdir, msg, sizeof(msg))) {
    printf("Could not create gamsx object: %s\n", msg);
    return;
  }

  if (! idxCreateD (&Xptr, sysdir, msg, sizeof(msg))) {
    printf("Could not create gdx object: %s\n", msg);
    return;
  }

  if (! optCreateD (&Optr, sysdir, msg, sizeof(msg))) {
    printf("Could not create opt object: %s\n", msg);
    return;
  }

  idxOpenWrite(Xptr, "lcp.gdx", "Siconos/Numerics NM_to_GDX", &status);
  if (status)
    idxerror(status, "idxOpenWrite");

  if ((status=NM_to_GDX(Xptr, "M", "M matrix", problem->M))) {
    printf("Model data not written\n");
    goto TERMINATE;
  }

  if ((status=NV_to_GDX(Xptr, "q", "q vector", problem->q, problem->size))) {
    printf("Model data not written\n");
    goto TERMINATE;
  }

  if (idxClose(Xptr))
    idxerror(idxGetLastError(Xptr), "idxClose");

  if ((status=CallGams(Gptr, Optr, sysdir, model))) {
    printf("Call to GAMS failed\n");
    goto TERMINATE;
  }


  /************************************************
   * Read back solution
   ************************************************/
  idxOpenRead(Xptr, "lcp_sol.gdx", &status);
  if (status)
    idxerror(status, "idxOpenRead");

  if ((status=GDX_to_NV(Xptr, "sol", z, problem->size))) {
    printf("Model data not read\n");
    goto TERMINATE;
  }

  if (idxClose(Xptr))
    idxerror(idxGetLastError(Xptr), "idxClose");

TERMINATE:
  optFree(&Optr);
  idxFree(&Xptr);
  gamsxFree(&Gptr);

  *info = status;
}

#else

void lcp_gams(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options)
{
  printf("lcp_gams :: gams was not enabled at compile time!\n");
  exit(EXIT_FAILURE);
}
#endif
