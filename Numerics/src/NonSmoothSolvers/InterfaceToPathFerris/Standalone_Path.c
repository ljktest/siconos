#include "Standalone_Path.h"
#include "NumericsOptions.h"
#include "include/MCP_Interface.h"

#include "include/Path.h"
#include "include/PathOptions.h"

#include "include/Macros.h"
#include "include/Output.h"
#include "include/Output_Interface.h"
#include "include/Options.h"
#include "NumericsConfig.h"
#include <limits.h>
#include <stdio.h>

static FuncEvalPtr funcEval = NULL;
static JacEvalPtr jacEval = NULL;

void setFuncEval(FuncEvalPtr myFunc)
{
  funcEval = myFunc;
}

void setJacEval(JacEvalPtr myJac)
{
  jacEval = myJac;
}

#ifdef HAVE_PATHFERRIS
typedef struct
{
  int n;
  int nnz;

  double *z;
  double *f;

  double *lb;
  double *ub;
} Problem;

static Problem problem;

static CB_FUNC(void) problem_size(void *id, int *n, int *nnz)
{
  *n = problem.n;
  *nnz = problem.nnz;
  return;
}

static CB_FUNC(void) bounds(void *id, int n, double *z, double *lb, double *ub)
{
  int i;

  for (i = 0; i < n; i++)
  {
    z[i] = problem.z[i];
    lb[i] = problem.lb[i];
    ub[i] = problem.ub[i];
  }
  return;
}

static CB_FUNC(int) function_evaluation(void *id, int n, double *z, double *f)
{
  int err;

  err = (*funcEval)(n, z, f);
  return err;
}

static CB_FUNC(int) jacobian_evaluation(void *id, int n, double *z, int wantf,
                                        double *f, int *nnz,
                                        int *col_start, int *col_len,
                                        int *row, double *data)
{
  int i, err = 0;

  if (wantf)
  {
    err += function_evaluation(id, n, z, f);
  }

  err += (*jacEval)(n, *nnz, z, col_start, col_len, row, data);

  (*nnz) = 0;
  for (i = 0; i < n; i++)
  {
    (*nnz) += col_len[i];
  }
  return err;
}

static MCP_Interface m_interface =
{
  NULL,
  problem_size, bounds,
  function_evaluation, jacobian_evaluation,
  NULL, NULL,
  NULL, NULL,
  NULL
};

static void install_interface(MCP *m)
{
  MCP_SetInterface(m, &m_interface);
  return;
}

void create(int n, int nnz, double *z, double *f, double *lb, double *ub)
{
  problem.n = n;
  problem.nnz = nnz;
  problem.z = z;
  problem.f = f;
  problem.lb = lb;
  problem.ub = ub;
}

void postMCP(int n, MCP *m, double *z, double *f)
{
  double *tempZ = MCP_GetX(m);
  double *tempF = MCP_GetF(m);

  for (int i = 0; i < n; i++)
  {
    z[i] = tempZ[i];
    f[i] = tempF[i];
  }
}

void pathMain(int n, int nnz, int *status,
              double *z, double *f, double *lb, double *ub)
{
  Options_Interface *o;
  MCP *m;
  MCP_Termination t;
  Information info;

  double *tempZ;
  double *tempF;
  double dnnz;
  int i;

  /* File name for output log */
  char name[9] = "path.log";
  FILE* pathLog = fopen(name, "w");
  Output_SetLog(pathLog);

  //  Output_SetLog(stdout);

  o = Options_Create();
  Path_AddOptions(o);
  Options_Default(o);

  Output_Printf(Output_Log | Output_Status | Output_Listing,
                "%s: Standalone-C Link\n", Path_Version());

  if (n == 0)
  {
    Output_Printf(Output_Log | Output_Status,
                  "\n ** EXIT - solution found (degenerate model).\n");
    (*status) = MCP_Solved;
    Options_Destroy(o);
    return;
  }

  dnnz = MIN(1.0 * nnz, 1.0 * n * n);
  if (dnnz > INT_MAX)
  {
    Output_Printf(Output_Log | Output_Status,
                  "\n ** EXIT - model too large.\n");
    (*status) = MCP_Error;
    Options_Destroy(o);
    return;
  }
  nnz = (int) dnnz;

  Output_Printf(Output_Log | Output_Status | Output_Listing,
                "%d row/cols, %d non-zeros, %3.2f%% dense.\n\n",
                n, nnz, 100.0 * nnz / (1.0 * n * n));
  nnz++;

  /* Form the problem */
  create(n, nnz, z, f, lb, ub);

  /* Create and setup an MCP structure */
  m = MCP_Create(n, nnz);
  install_interface(m);

  Options_Read(o, "path.opt");
  Options_Display(o);

  info.generate_output = Output_Log | Output_Status | Output_Listing;
  info.use_start = True;
  info.use_basics = True;

  /*** SOLVE THE PROBLEM ***/
  t = Path_Solve(m, &info);

  /** Work with the results **/
  postMCP(n, m, z, f);
  *status = t;

  *status = t;

  MCP_Destroy(m);
  Options_Destroy(o);
  return;
}

#else
void pathMain(int n, int nnz, int *status, double *z, double *f, double *lb, double *ub)
{
  numericsError("Standalone_Path:pathMain - ", "Path lib not found");
}
#endif
