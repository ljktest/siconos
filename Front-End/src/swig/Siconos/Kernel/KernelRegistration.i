// Warning: 
// You have to PY_REGISTER base classe before derivated classes
#undef PY_REGISTER
#define KERNEL_REGISTRATION()                                           \
  PY_REGISTER_WITHOUT_DIRECTOR_REF_ONLY(SiconosMatrix);                 \
  PY_REGISTER_SIMPLEMATRIX(SimpleMatrix);                               \
  PY_REGISTER_WITHOUT_DIRECTOR_REF(SiconosVector);                      \
  PY_REGISTER_WITHOUT_DIRECTOR(BlockVector);                            \
  PY_REGISTER(SiconosMemory)                                            \
  PY_REGISTER(NonSmoothLaw);                                            \
  PY_REGISTER(NewtonImpactNSL);                                         \
  PY_REGISTER(NewtonImpactFrictionNSL);                                 \
  PY_REGISTER(MixedComplementarityConditionNSL);                        \
  PY_REGISTER(ComplementarityConditionNSL);                             \
  PY_REGISTER(EqualityConditionNSL);                                    \
  PY_REGISTER(MultipleImpactNSL);                                       \
  PY_REGISTER(RelayNSL);                                                \
  PY_REGISTER(NormalConeNSL);                                           \
  PY_REGISTER(DynamicalSystem);                                         \
  PY_REGISTER(NonSmoothDynamicalSystem);                                \
  PY_REGISTER(Topology);                                                \
  PY_REGISTER(LagrangianDS);                                            \
  PY_REGISTER(LagrangianLinearTIDS);                                    \
  PY_REGISTER(NewtonEulerDS);                                           \
  PY_REGISTER(FirstOrderNonLinearDS);                                   \
  PY_REGISTER(FirstOrderLinearDS);                                      \
  PY_REGISTER(FirstOrderLinearTIDS);                                    \
  PY_REGISTER(Relation);                                                \
  PY_REGISTER(LagrangianR);                                             \
  PY_REGISTER(LagrangianLinearTIR);                                     \
  PY_REGISTER(LagrangianRheonomousR);                                   \
  PY_REGISTER(LagrangianScleronomousR);                                 \
  PY_REGISTER(LagrangianCompliantR);                                    \
  PY_REGISTER(NewtonEulerR);                                            \
  PY_REGISTER(NewtonEulerFrom1DLocalFrameR);                            \
  PY_REGISTER(NewtonEulerFrom3DLocalFrameR);                            \
  PY_REGISTER(FirstOrderR);                                             \
  PY_REGISTER(FirstOrderNonLinearR);                                    \
  PY_REGISTER(FirstOrderType1R);                                        \
  PY_REGISTER(FirstOrderType2R);                                        \
  PY_REGISTER(FirstOrderLinearR);                                       \
  PY_REGISTER(FirstOrderLinearTIR);                                     \
  PY_REGISTER(Interaction);                                             \
  PY_REGISTER(TimeDiscretisation);                                      \
  PY_REGISTER(OneStepNSProblem);                                        \
  PY_REGISTER(OneStepIntegrator);                                       \
  PY_REGISTER(LinearOSNS);                                              \
  PY_REGISTER(LsodarOSI);                                               \
  PY_REGISTER(LCP);                                                     \
  PY_REGISTER(AVI);                                                     \
  PY_REGISTER(QP);                                                      \
  PY_REGISTER(Relay);                                                   \
  PY_REGISTER(MLCP);                                                    \
  PY_REGISTER(MLCPProjectOnConstraints);                                \
  PY_REGISTER(GenericMechanical);                                       \
  PY_REGISTER(FrictionContact);                                         \
  PY_REGISTER(EulerMoreauOSI);                                          \
  PY_REGISTER(MoreauJeanOSI);                                           \
  PY_REGISTER(MoreauJeanCombinedProjectionOSI);                         \
  PY_REGISTER(ZeroOrderHoldOSI);                                        \
  PY_REGISTER(Simulation);                                              \
  PY_REGISTER(TimeStepping);                                            \
  PY_REGISTER(TimeSteppingCombinedProjection);                          \
  PY_REGISTER(EventDriven);                                             \
  PY_REGISTER(EventsManager);                                           \
  PY_REGISTER(Event);                                                   \
  PY_REGISTER(Model);                                                   \
  PY_REGISTER(BoundaryCondition);                                       \
  PY_REGISTER(OSNSMatrix);                                              \
  PY_REGISTER(BlockCSRMatrix);
