
%include "SolverOptions.h"
%include "MLCP_Solvers.h"

%extend SolverOptions_
{
  SolverOptions_(enum FRICTION_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));

    /* cf Friction_cst.h */
    if(id >= 400 && id < 500)
    {
      fc2d_setDefaultSolverOptions(SO, id);
    }
    else if (id >= 500 && id < 600)
    {
      fc3d_setDefaultSolverOptions(SO, id);
    }
    else if (id >= 600 && id < 700)
    {
      gfc3d_setDefaultSolverOptions(SO, id);
    }
    else
    {
      PyErr_SetString(PyExc_RuntimeError, "Unknown friction contact problem solver");
      free(SO);
      return NULL;
    }


    return SO;
  }

  SolverOptions_()
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    return SO;
  }

  SolverOptions_(LinearComplementarityProblem* lcp, enum LCP_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    solver_options_set(SO, id);
    return SO;
  }

  SolverOptions_(MixedLinearComplementarityProblem* mlcp, enum MLCP_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    SO->solverId=id;
    mixedLinearComplementarity_setDefaultSolverOptions(mlcp, SO);
    return SO;
  }

  SolverOptions_(MixedComplementarityProblem* mlcp, enum MCP_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    SO->solverId=id;
    mixedComplementarity_setDefaultSolverOptions(mlcp, SO);
    return SO;
  }

  SolverOptions_(MixedComplementarityProblem2* mcp, enum MCP_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    solver_options_set(SO, id);
    return SO;
  }

  SolverOptions_(VariationalInequality* vi, enum VI_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    solver_options_set(SO, id);
    return SO;
  }

  SolverOptions_(AffineVariationalInequalities* vi, enum AVI_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    SO->solverId=id;
    solver_options_set(SO, id);
    return SO;
  }

  ~SolverOptions_()
  {
    solver_options_delete($self);
    free($self);
  }

};


