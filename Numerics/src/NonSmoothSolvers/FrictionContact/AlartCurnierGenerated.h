#ifndef AlartCurnierGenerated_h
#define AlartCurnierGenerated_h
#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
extern "C"
{
#endif

void frictionContact3D_AlartCurnierFABGenerated(
  double rn,
  double rt1,
  double rt2,
  double un,
  double ut1,
  double ut2,
  double mu,
  double rhon,
  double rhot1,
  double rhot2,
  double *result);


void frictionContact3D_AlartCurnierFGenerated(
  double rn,
  double rt1,
  double rt2,
  double un,
  double ut1,
  double ut2,
  double mu,
  double rhon,
  double rhot1,
  double rhot2,
  double *result);

void frictionContact3D_AlartCurnierABGenerated(
  double rn,
  double rt1,
  double rt2,
  double un,
  double ut1,
  double ut2,
  double mu,
  double rhon,
  double rhot1,
  double rhot2,
  double *result);


void frictionContact3D_AlartCurnierJeanMoreauFABGenerated(
  double rn,
  double rt1,
  double rt2,
  double un,
  double ut1,
  double ut2,
  double mu,
  double rhon,
  double rhot1,
  double rhot2,
  double *result);

void frictionContact3D_AlartCurnierJeanMoreauFGenerated(
  double rn,
  double rt1,
  double rt2,
  double un,
  double ut1,
  double ut2,
  double mu,
  double rhon,
  double rhot1,
  double rhot2,
  double *result);

void frictionContact3D_AlartCurnierJeanMoreauABGenerated(
  double rn,
  double rt1,
  double rt2,
  double un,
  double ut1,
  double ut2,
  double mu,
  double rhon,
  double rhot1,
  double rhot2,
  double *result);

void frictionContact3D_AlartCurnierFunctionGenerated(
  double *reaction,
  double *velocity,
  double mu,
  double *rho,
  double *f,
  double *A,
  double *B);

void frictionContact3D_AlartCurnierJeanMoreauFunctionGenerated(
  double *reaction,
  double *velocity,
  double mu,
  double *rho,
  double *f,
  double *A,
  double *B);

#if defined(__cplusplus) && !defined(BUILD_AS_CPP)
}
#endif
#endif
