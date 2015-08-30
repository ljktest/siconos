#ifndef FRICTION_CST_H
#define FRICTION_CST_H
/** \file Friction_cst.h */
/** \enum FRICTION_SOLVER Friction_cst.h
 * Enum that allows one to encode the list of solvers in a proper to avoid mispelling
 * with char * variables
 */
enum FRICTION_SOLVER
{
  /** 2D Frictional Contact solvers */
  SICONOS_FRICTION_2D_NSGS = 400,
  SICONOS_FRICTION_2D_PGS = 401,
  SICONOS_FRICTION_2D_CPG = 402,
  SICONOS_FRICTION_2D_LATIN = 403,
  SICONOS_FRICTION_2D_LEMKE = 404,
  SICONOS_FRICTION_2D_ENUM = 405,

  /** 3D frictional contact solvers on local formulation */
  SICONOS_FRICTION_3D_NSGS = 500,
  SICONOS_FRICTION_3D_NSGSV = 501,
  SICONOS_FRICTION_3D_PROX = 502,
  SICONOS_FRICTION_3D_TFP = 503,
  SICONOS_FRICTION_3D_LOCALAC = 504,
  SICONOS_FRICTION_3D_DSFP = 505,
  SICONOS_FRICTION_3D_VI_FPP = 506,
  SICONOS_FRICTION_3D_VI_EG = 507,
  SICONOS_FRICTION_3D_HP = 508,
  SICONOS_FRICTION_3D_NCPGlockerFBFixedPoint = 510,
  SICONOS_FRICTION_3D_FPP = 511,
  SICONOS_FRICTION_3D_EG = 512,
  SICONOS_FRICTION_3D_LOCALFB = 513,

  /** 3D Frictional Contact solvers for one contact (used mainly inside NSGS solvers) */
  SICONOS_FRICTION_3D_AlartCurnierNewton = 550,
  SICONOS_FRICTION_3D_NCPGlockerFBNewton = 551,
  SICONOS_FRICTION_3D_ProjectionOnConeWithDiagonalization = 552,
  SICONOS_FRICTION_3D_ProjectionOnCone = 553,
  SICONOS_FRICTION_3D_ProjectionOnConeWithLocalIteration = 554,
  SICONOS_FRICTION_3D_projectionOnConeWithRegularization = 555,
  SICONOS_FRICTION_3D_NCPGlockerFBPATH = 556,
  SICONOS_FRICTION_3D_projectionOnCylinder = 557,
  SICONOS_FRICTION_3D_ProjectionOnCone_velocity = 558,
  SICONOS_FRICTION_3D_PGoC = 559,
  SICONOS_FRICTION_3D_DeSaxceFixedPoint = 560,
  SICONOS_FRICTION_3D_DampedAlartCurnierNewton = 561,
  SICONOS_FRICTION_3D_QUARTIC = 562,
  SICONOS_FRICTION_3D_QUARTIC_NU = 563,

  /** 3D Frictional contact local solvers on global formulation */
  SICONOS_FRICTION_3D_GLOBAL_NSGS_WR = 600,
  SICONOS_FRICTION_3D_GLOBAL_NSGSV_WR = 601,
  SICONOS_FRICTION_3D_GLOBAL_PROX_WR = 602,
  SICONOS_FRICTION_3D_GLOBAL_DSFP_WR = 603,
  SICONOS_FRICTION_3D_GLOBAL_TFP_WR = 604,
  SICONOS_FRICTION_3D_GLOBAL_NSGS = 605,
  SICONOS_FRICTION_3D_GLOBAL_LOCALAC_WR = 606,
  SICONOS_FRICTION_3D_GLOBAL_AC = 607

};



extern char *  SICONOS_FRICTION_2D_NSGS_STR ;
extern char *  SICONOS_FRICTION_2D_PGS_STR ;
extern char *  SICONOS_FRICTION_2D_CPG_STR ;
extern char *  SICONOS_FRICTION_2D_LATIN_STR ;
extern char *  SICONOS_FRICTION_2D_LEMKE_STR ;
extern char *  SICONOS_FRICTION_2D_ENUM_STR ;
extern char *  SICONOS_FRICTION_3D_NSGS_STR ;
extern char *  SICONOS_FRICTION_3D_NSGSV_STR ;
extern char *  SICONOS_FRICTION_3D_PROX_STR;
extern char *  SICONOS_FRICTION_3D_TFP_STR ;
extern char *  SICONOS_FRICTION_3D_LOCALAC_STR ;
extern char *  SICONOS_FRICTION_3D_LOCALFB_STR ;
extern char *  SICONOS_FRICTION_3D_DSFP_STR ;
extern char *  SICONOS_FRICTION_3D_VI_EG_STR ;
extern char *  SICONOS_FRICTION_3D_VI_FPP_STR ;
extern char *  SICONOS_FRICTION_3D_EG_STR ;
extern char *  SICONOS_FRICTION_3D_FPP_STR ;
extern char *  SICONOS_FRICTION_3D_HP_STR ;
extern char *  SICONOS_FRICTION_3D_NCPGlockerFBFixedPoint_STR;
extern char *  SICONOS_FRICTION_3D_AlartCurnierNewton_STR;
extern char *  SICONOS_FRICTION_3D_DampedAlartCurnierNewton_STR;
extern char *  SICONOS_FRICTION_3D_NCPGlockerFBNewton_STR;
extern char *  SICONOS_FRICTION_3D_ProjectionOnConeWithDiagonalization_STR;
extern char *  SICONOS_FRICTION_3D_ProjectionOnCone_STR;
extern char *  SICONOS_FRICTION_3D_ProjectionOnConeWithLocalIteration_STR;
extern char *  SICONOS_FRICTION_3D_projectionOnConeWithRegularization_STR;
extern char *  SICONOS_FRICTION_3D_NCPGlockerFBPATH_STR;
extern char *  SICONOS_FRICTION_3D_projectionOnCylinder_STR;
extern char *  SICONOS_FRICTION_3D_ProjectionOnCone_velocity_STR;
extern char *  SICONOS_FRICTION_3D_PGoC_STR;
extern char *  SICONOS_FRICTION_3D_DeSaxceFixedPoint_STR;
extern char *  SICONOS_FRICTION_3D_GLOBAL_NSGS_WR_STR ;
extern char *  SICONOS_FRICTION_3D_GLOBAL_NSGSV_WR_STR ;
extern char *  SICONOS_FRICTION_3D_GLOBAL_PROX_WR_STR ;
extern char *  SICONOS_FRICTION_3D_GLOBAL_DSFP_WR_STR ;
extern char *  SICONOS_FRICTION_3D_GLOBAL_TFP_WR_STR ;
extern char *  SICONOS_FRICTION_3D_GLOBAL_NSGS_STR ;
extern char *  SICONOS_FRICTION_3D_GLOBAL_LOCALAC_WR_STR ;
extern char *  SICONOS_FRICTION_3D_GLOBAL_AC_STR;
extern char *  SICONOS_FRICTION_3D_QUARTIC_STR ;
extern char *  SICONOS_FRICTION_3D_QUARTIC_NU_STR ;

#endif
