#ifndef SOCLCP_CST_H
#define SOCLCP_CST_H
/** \file SOCLCP_cst.h */
/** \enum SOCLCP_SOLVER encode the list of solvers as integers to avoid mispelling
 * with char * variables
 */
enum SOCLCP_SOLVER
{

  /** SOCLCP solvers on local formulation */
  SICONOS_SOCLCP_NSGS = 1100,
  /* SICONOS_SOCLCP_NSGSV = 501, */
  /* SICONOS_SOCLCP_PROX = 502, */
  /* SICONOS_SOCLCP_TFP = 503, */
  /* SICONOS_SOCLCP_NSN_AC = 504, */
  SICONOS_SOCLCP_VI_FPP = 1106,
  SICONOS_SOCLCP_VI_EG = 1107,
  /* SICONOS_SOCLCP_HP = 508, */
  /* SICONOS_SOCLCP_NCPGlockerFBFixedPoint = 510, */
  /* SICONOS_SOCLCP_NSN_FB = 513, */

  /** SOCLCP for one cone (used mainly inside NSGS solvers) */
  SICONOS_SOCLCP_ProjectionOnCone = 1150,
  SICONOS_SOCLCP_ProjectionOnConeWithLocalIteration = 1151,
  SICONOS_SOCLCP_projectionOnConeWithRegularization = 1152
  /* SICONOS_SOCLCP_AlartCurnierNewton = 550, */
  /* SICONOS_SOCLCP_NCPGlockerFBNewton = 551, */
  /* SICONOS_SOCLCP_NCPGlockerFBPATH = 556, */
  /* SICONOS_SOCLCP_projectionOnCylinder = 557, */
  /* SICONOS_SOCLCP_ProjectionOnCone_velocity = 558, */
  /* SICONOS_SOCLCP_PGoC = 559, */
  /* SICONOS_SOCLCP_DeSaxceFixedPoint = 560, */
  /* SICONOS_SOCLCP_DampedAlartCurnierNewton = 561, */
  /* SICONOS_SOCLCP_QUARTIC = 562, */
  /* SICONOS_SOCLCP_QUARTIC_NU = 563, */

  /** SOCLCP local solvers on global formulation */
  /* SICONOS_SOCLCP_GLOBAL_NSGS_WR = 600, */
  /* SICONOS_SOCLCP_GLOBAL_NSGSV_WR = 601, */
  /* SICONOS_SOCLCP_GLOBAL_PROX_WR = 602, */
  /* SICONOS_SOCLCP_GLOBAL_DSFP_WR = 603, */
  /* SICONOS_SOCLCP_GLOBAL_TFP_WR = 604, */
  /* SICONOS_SOCLCP_GLOBAL_NSGS = 605, */
  /* SICONOS_SOCLCP_GLOBAL_NSN_AC_WR = 606, */
  /* SICONOS_SOCLCP_GLOBAL_AC = 607 */

};



extern char *  SICONOS_FRICTION_2D_NSGS_STR ;
extern char *  SICONOS_FRICTION_2D_PGS_STR ;
extern char *  SICONOS_FRICTION_2D_CPG_STR ;
extern char *  SICONOS_FRICTION_2D_LATIN_STR ;
extern char *  SICONOS_FRICTION_2D_LEMKE_STR ;
extern char *  SICONOS_FRICTION_2D_ENUM_STR ;
extern char *  SICONOS_SOCLCP_NSGS_STR ;
extern char *  SICONOS_SOCLCP_NSGSV_STR ;
extern char *  SICONOS_SOCLCP_PROX_STR;
extern char *  SICONOS_SOCLCP_TFP_STR ;
extern char *  SICONOS_SOCLCP_NSN_AC_STR ;
extern char *  SICONOS_SOCLCP_NSN_FB_STR ;
extern char *  SICONOS_SOCLCP_DSFP_STR ;
extern char *  SICONOS_SOCLCP_VI_EG_STR ;
extern char *  SICONOS_SOCLCP_VI_FPP_STR ;
extern char *  SICONOS_SOCLCP_EG_STR ;
extern char *  SICONOS_SOCLCP_FPP_STR ;
extern char *  SICONOS_SOCLCP_HP_STR ;
extern char *  SICONOS_SOCLCP_NCPGlockerFBFixedPoint_STR;
extern char *  SICONOS_SOCLCP_AlartCurnierNewton_STR;
extern char *  SICONOS_SOCLCP_DampedAlartCurnierNewton_STR;
extern char *  SICONOS_SOCLCP_NCPGlockerFBNewton_STR;
extern char *  SICONOS_SOCLCP_ProjectionOnConeWithDiagonalization_STR;
extern char *  SICONOS_SOCLCP_ProjectionOnCone_STR;
extern char *  SICONOS_SOCLCP_ProjectionOnConeWithLocalIteration_STR;
extern char *  SICONOS_SOCLCP_projectionOnConeWithRegularization_STR;
extern char *  SICONOS_SOCLCP_NCPGlockerFBPATH_STR;
extern char *  SICONOS_SOCLCP_projectionOnCylinder_STR;
extern char *  SICONOS_SOCLCP_ProjectionOnCone_velocity_STR;
extern char *  SICONOS_SOCLCP_PGoC_STR;
extern char *  SICONOS_SOCLCP_DeSaxceFixedPoint_STR;
extern char *  SICONOS_SOCLCP_GLOBAL_NSGS_WR_STR ;
extern char *  SICONOS_SOCLCP_GLOBAL_NSGSV_WR_STR ;
extern char *  SICONOS_SOCLCP_GLOBAL_PROX_WR_STR ;
extern char *  SICONOS_SOCLCP_GLOBAL_DSFP_WR_STR ;
extern char *  SICONOS_SOCLCP_GLOBAL_TFP_WR_STR ;
extern char *  SICONOS_SOCLCP_GLOBAL_NSGS_STR ;
extern char *  SICONOS_SOCLCP_GLOBAL_NSN_AC_WR_STR ;
extern char *  SICONOS_SOCLCP_QUARTIC_STR ;
extern char *  SICONOS_SOCLCP_QUARTIC_NU_STR ;

#endif
