
%include "SolverOptions.h"
%include "NumericsOptions.h"



%extend NumericsOptions
{
  NumericsOptions()
  {
    NumericsOptions *numerics_options;
    numerics_options = (NumericsOptions *) malloc(sizeof(NumericsOptions));
    setDefaultNumericsOptions(numerics_options);
    return numerics_options;
  }

  ~NumericsOptions()
  {
    free($self);
  }
}

%extend _SolverOptions
{
  _SolverOptions(enum FRICTION_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));

    /* cf Friction_cst.h */
    if(id >= 400 && id < 500)
    {
      fc2d_setDefaultSolverOptions(SO, id);
    }
    else
    {
      fc3d_setDefaultSolverOptions(SO, id);
    }


    return SO;
  }

  _SolverOptions()
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    return SO;
  }

  _SolverOptions(LinearComplementarityProblem* lcp, enum LCP_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    set_SolverOptions(SO, id);
    return SO;
  }

  _SolverOptions(MixedLinearComplementarityProblem* mlcp, enum MLCP_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    SO->solverId=id;
    mixedLinearComplementarity_setDefaultSolverOptions(mlcp, SO);
    return SO;
  }

  _SolverOptions(MixedComplementarityProblem* mlcp, enum MCP_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    SO->solverId=id;
    mixedComplementarity_setDefaultSolverOptions(mlcp, SO);
    return SO;
  }

  _SolverOptions(MixedComplementarityProblem2* mcp, enum MCP_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    set_SolverOptions(SO, id);
    return SO;
  }

  _SolverOptions(VariationalInequality* vi, enum VI_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    set_SolverOptions(SO, id);
    return SO;
  }

  _SolverOptions(AffineVariationalInequalities* vi, enum AVI_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    SO->solverId=id;
    set_SolverOptions(SO, id);
    return SO;
  }

  ~_SolverOptions()
  {
    deleteSolverOptions($self);
    free($self);
  }

};


