// generated with build_from_doxygen.py
#ifndef SiconosFullKernelGenerated_hpp
#define SiconosFullKernelGenerated_hpp
#include <SiconosConfig.h>
#ifdef WITH_SERIALIZATION
#include "SiconosKernel.hpp"
SICONOS_IO_REGISTER(SiconosException,
  (_reportMsg))
SICONOS_IO_REGISTER(SiconosMemory,
  (_indx)
  (_nbVectorsInMemory))
SICONOS_IO_REGISTER(BlockVector,
  (_sizeV)
  (_tabIndex)
  (_vect))
SICONOS_IO_REGISTER_WITH_BASES(BlockMatrix,(SiconosMatrix),
  (_dimCol)
  (_dimRow)
  (_mat)
  (_tabCol)
  (_tabRow))
SICONOS_IO_REGISTER(SiconosMatrix,
  (_num))
SICONOS_IO_REGISTER(GraphProperties,
  (symmetric))
SICONOS_IO_REGISTER(DynamicalSystemProperties,
  (W)
  (WBoundaryConditions)
  (absolute_position)
  (lower_block)
  (osi)
  (upper_block)
  (workMatrices)
  (workVectors))
SICONOS_IO_REGISTER(InteractionProperties,
  (DSlink)
  (absolute_position)
  (absolute_position_proj)
  (block)
  (forControl)
  (source)
  (source_pos)
  (target)
  (target_pos)
  (workMatrices)
  (workVectors))
SICONOS_IO_REGISTER(MatrixIntegrator,
  (_DS)
  (_E)
  (_OSI)
  (_TD)
  (_isConst)
  (_mat)
  (_model)
  (_plugin)
  (_sim))
SICONOS_IO_REGISTER_WITH_BASES(DynamicalSystemsGraph,(_DynamicalSystemsGraph),
  (Ad)
  (AdInt)
  (B)
  (Bd)
  (L)
  (Ld)
  (dummy)
  (e)
  (groupId)
  (jacgx)
  (name)
  (pluginB)
  (pluginJacgx)
  (pluginL)
  (pluginU)
  (tmpXdot)
  (u))
SICONOS_IO_REGISTER_WITH_BASES(InteractionsGraph,(_InteractionsGraph),
  (blockProj)
  (dummy)
  (lower_blockProj)
  (name)
  (upper_blockProj))
SICONOS_IO_REGISTER(Topology,
  (_DSG)
  (_IG)
  (_hasChanged)
  (_isTopologyUpToDate)
  (_numberOfConstraints)
  (_symmetric))
SICONOS_IO_REGISTER_WITH_BASES(MultipleImpactNSL,(NonSmoothLaw),
  (_ElasCof)
  (_ResCof)
  (_Stiff))
SICONOS_IO_REGISTER_WITH_BASES(ComplementarityConditionNSL,(NonSmoothLaw),
)
SICONOS_IO_REGISTER_WITH_BASES(FixedBC,(BoundaryCondition),
)
SICONOS_IO_REGISTER_WITH_BASES(HarmonicBC,(BoundaryCondition),
  (_a)
  (_aV)
  (_b)
  (_bV)
  (_omega)
  (_omegaV)
  (_phi)
  (_phiV))
SICONOS_IO_REGISTER(NSLawMatrix,
)
SICONOS_IO_REGISTER_WITH_BASES(EqualityConditionNSL,(NonSmoothLaw),
)
SICONOS_IO_REGISTER_WITH_BASES(NewtonImpactFrictionNSL,(NonSmoothLaw),
  (_en)
  (_et)
  (_mu))
SICONOS_IO_REGISTER_WITH_BASES(MixedComplementarityConditionNSL,(NonSmoothLaw),
  (_equalitySize))
SICONOS_IO_REGISTER(NonSmoothDynamicalSystem,
  (_BVP)
  (_mIsLinear)
  (_topology))
SICONOS_IO_REGISTER(PluggedObject,
  (_pluginName))
SICONOS_IO_REGISTER_WITH_BASES(NewtonEulerFrom3DLocalFrameR,(NewtonEulerFrom1DLocalFrameR),
)
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderLinearTIR,(FirstOrderR),
  (_e))
SICONOS_IO_REGISTER(BoundaryCondition,
  (_pluginPrescribedVelocity)
  (_prescribedVelocity)
  (_prescribedVelocityOld)
  (_velocityIndices))
SICONOS_IO_REGISTER_WITH_BASES(NewtonImpactNSL,(NonSmoothLaw),
  (_e))
SICONOS_IO_REGISTER_WITH_BASES(NewtonEulerFrom1DLocalFrameR,(NewtonEulerR),
  (_AUX1)
  (_AUX2)
  (_NPG1)
  (_NPG2)
  (_Nc)
  (_Pc1)
  (_Pc2)
  (_RotationAbsToContactFrame)
  (_isOnContact)
  (_relPc1)
  (_relPc2)
  (_rotationMatrixAbsToBody))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianLinearTIR,(LagrangianR),
  (_F)
  (_e))
SICONOS_IO_REGISTER_WITH_BASES(NormalConeNSL,(NonSmoothLaw),
  (_H)
  (_K))
SICONOS_IO_REGISTER_WITH_BASES(NewtonEulerR,(Relation),
  (_T)
  (_contactForce)
  (_dotjachq)
  (_e)
  (_jacglambda)
  (_jachlambda)
  (_jachq)
  (_jachqDot)
  (_jachqT)
  (_plugindotjacqh)
  (_secondOrderTimeDerivativeTerms))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianCompliantLinearTIR,(LagrangianR),
  (_F)
  (_e))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderLinearR,(FirstOrderR),
  (_e))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderLinearTIDS,(FirstOrderLinearDS),
)
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderType2R,(FirstOrderR),
)
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderType1R,(FirstOrderR),
)
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderLinearDS,(FirstOrderNonLinearDS),
  (_A)
  (_pluginA)
  (_pluginb))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianCompliantR,(LagrangianR),
  (_pluginJachlambda))
SICONOS_IO_REGISTER(NonSmoothLaw,
  (_size))
SICONOS_IO_REGISTER_WITH_BASES(RelayNSL,(NonSmoothLaw),
  (_lb)
  (_ub))
SICONOS_IO_REGISTER(Relation,
  (_pluginJacglambda)
  (_pluginJacgx)
  (_pluginJachlambda)
  (_pluginJachx)
  (_pluginJachz)
  (_plugine)
  (_pluginf)
  (_pluging)
  (_pluginh)
  (_relationType)
  (_subType))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianRheonomousR,(LagrangianR),
  (_hDot)
  (_pluginhDot))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderNonLinearR,(FirstOrderR),
)
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderR,(Relation),
  (_B)
  (_C)
  (_D)
  (_F)
  (_K))
SICONOS_IO_REGISTER(Interaction,
  (__count)
  (_has2Bodies)
  (_interactionSize)
  (_lambda)
  (_lambdaMemory)
  (_lambdaOld)
  (_lowerLevelForInput)
  (_lowerLevelForOutput)
  (_nslaw)
  (_number)
  (_relation)
  (_sizeOfDS)
  (_upperLevelForInput)
  (_upperLevelForOutput)
  (_y)
  (_yMemory)
  (_yOld)
  (_y_k))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianR,(Relation),
  (_dotjachq)
  (_jachlambda)
  (_jachq)
  (_jachqDot)
  (_pluginJachq))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianLinearDiagonalDS,(LagrangianDS),
  (_damping)
  (_mu)
  (_stiffness))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderNonLinearDS,(DynamicalSystem),
  (_M)
  (_b)
  (_f)
  (_fold)
  (_invM)
  (_jacobianfx)
  (_pluginJacxf)
  (_pluginM)
  (_pluginf)
  (_rMemory))
SICONOS_IO_REGISTER(DynamicalSystem,
  (__count)
  (_jacxRhs)
  (_n)
  (_number)
  (_r)
  (_stepsInMemory)
  (_x)
  (_x0)
  (_xMemory)
  (_z))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianScleronomousR,(LagrangianR),
  (_dotjacqhXqdot)
  (_plugindotjacqh))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianLinearTIDS,(LagrangianDS),
  (_C)
  (_K))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianDS,(DynamicalSystem),
  (_boundaryConditions)
  (_fExt)
  (_fGyr)
  (_fInt)
  (_forces)
  (_forcesMemory)
  (_hasConstantFExt)
  (_hasConstantMass)
  (_inverseMass)
  (_jacobianFGyrq)
  (_jacobianFGyrqDot)
  (_jacobianFIntq)
  (_jacobianFIntqDot)
  (_jacobianqDotForces)
  (_jacobianqForces)
  (_mass)
  (_ndof)
  (_p)
  (_pMemory)
  (_pluginFExt)
  (_pluginFGyr)
  (_pluginFInt)
  (_pluginJacqDotFGyr)
  (_pluginJacqDotFInt)
  (_pluginJacqFGyr)
  (_pluginJacqFInt)
  (_pluginMass)
  (_q)
  (_q0)
  (_qMemory)
  (_reactionToBoundaryConditions)
  (_rhsMatrices)
  (_velocity0)
  (_velocityMemory))
SICONOS_IO_REGISTER_WITH_BASES(NewtonEulerDS,(DynamicalSystem),
  (_I)
  (_T)
  (_Tdot)
  (_boundaryConditions)
  (_computeJacobianFIntqByFD)
  (_computeJacobianFInttwistByFD)
  (_computeJacobianMIntqByFD)
  (_computeJacobianMInttwistByFD)
  (_dotq)
  (_dotqMemory)
  (_epsilonFD)
  (_fExt)
  (_fInt)
  (_forcesMemory)
  (_hasConstantFExt)
  (_hasConstantMExt)
  (_inverseMass)
  (_isMextExpressedInInertialFrame)
  (_jacobianFIntq)
  (_jacobianFInttwist)
  (_jacobianMExtq)
  (_jacobianMGyrtwist)
  (_jacobianMIntq)
  (_jacobianMInttwist)
  (_jacobianWrenchTwist)
  (_jacobianWrenchq)
  (_mExt)
  (_mGyr)
  (_mInt)
  (_massMatrix)
  (_nullifyMGyr)
  (_p)
  (_pluginFExt)
  (_pluginFInt)
  (_pluginJacqFInt)
  (_pluginJacqMInt)
  (_pluginJactwistFInt)
  (_pluginJactwistMInt)
  (_pluginMExt)
  (_pluginMInt)
  (_q)
  (_q0)
  (_qDim)
  (_qMemory)
  (_reactionToBoundaryConditions)
  (_scalarMass)
  (_twist)
  (_twist0)
  (_twistMemory)
  (_wrench))
SICONOS_IO_REGISTER(ExtraAdditionalTerms,
)
SICONOS_IO_REGISTER_WITH_BASES(MoreauJeanBilbaoOSI,(OneStepIntegrator),
)
SICONOS_IO_REGISTER(InteractionManager,
  (_nslaws))
SICONOS_IO_REGISTER_WITH_BASES(TimeDiscretisationEvent,(Event),
)
SICONOS_IO_REGISTER_WITH_BASES(TimeSteppingCombinedProjection,(TimeStepping),
  (_constraintTol)
  (_constraintTolUnilateral)
  (_cumulatedNewtonNbIterations)
  (_doCombinedProj)
  (_doCombinedProjOnEquality)
  (_indexSetLevelForProjection)
  (_isIndexSetsStable)
  (_kIndexSetMax)
  (_maxViolationEquality)
  (_maxViolationUnilateral)
  (_nbCumulatedProjectionIteration)
  (_nbIndexSetsIteration)
  (_nbProjectionIteration)
  (_projectionMaxIteration))
SICONOS_IO_REGISTER_WITH_BASES(EventDriven,(Simulation),
  (_DSG0)
  (_TOL_ED)
  (_indexSet0)
  (_isNewtonConverge)
  (_istate)
  (_localizeEventMaxIter)
  (_newtonMaxIteration)
  (_newtonNbIterations)
  (_newtonResiduDSMax)
  (_newtonResiduYMax)
  (_newtonTolerance)
  (_numberOfOneStepNSproblems))
SICONOS_IO_REGISTER_WITH_BASES(NonSmoothEvent,(Event),
)
SICONOS_IO_REGISTER_WITH_BASES(QP,(OneStepNSProblem),
  (_Q)
  (_p))
SICONOS_IO_REGISTER_WITH_BASES(TimeSteppingD1Minus,(Simulation),
)
SICONOS_IO_REGISTER_WITH_BASES(MLCPProjectOnConstraints,(MLCP),
  (_alpha)
  (_doProjOnEquality)
  (_useMassNormalization))
SICONOS_IO_REGISTER_WITH_BASES(OSNSMultipleImpact,(LinearOSNS),
  (_DataMatrix)
  (_IsImpactEnd)
  (_Kcontact)
  (_Tol_Ener)
  (_Tol_Vel)
  (_WorkcContact)
  (_ZeroEner_EndIm)
  (_ZeroVel_EndIm)
  (_deltaImpulseContact)
  (_deltaP)
  (_distributionVector)
  (_elasticyCoefficientcontact)
  (_energyContact)
  (_energyPrimaryContact)
  (_forceContact)
  (_impulseContactUpdate)
  (_impulseVariable)
  (_isPrimaryContactEnergy)
  (_nContact)
  (_nStepMax)
  (_nStepSave)
  (_namefile)
  (_oldVelocityContact)
  (_primaryContactId)
  (_relativeVelocityPrimaryContact)
  (_restitutionContact)
  (_saveData)
  (_selectPrimaConInVel)
  (_sizeDataSave)
  (_stateContact)
  (_stepMaxSave)
  (_stepMinSave)
  (_timeVariable)
  (_tolImpact)
  (_tolImpulseContact)
  (_typeCompLaw)
  (_velocityContact))
SICONOS_IO_REGISTER_WITH_BASES(NewMarkAlphaOSI,(OneStepIntegrator),
  (_IsVelocityLevel)
  (_alpha_f)
  (_alpha_m)
  (_beta)
  (_gamma)
  (_orderDenseOutput))
SICONOS_IO_REGISTER_WITH_BASES(AVI,(LinearOSNS),
)
SICONOS_IO_REGISTER_WITH_BASES(TimeSteppingDirectProjection,(TimeStepping),
  (_constraintTol)
  (_constraintTolUnilateral)
  (_doOnlyProj)
  (_doProj)
  (_indexSetLevelForProjection)
  (_maxViolationEquality)
  (_maxViolationUnilateral)
  (_nbProjectionIteration)
  (_projectionMaxIteration))
SICONOS_IO_REGISTER_WITH_BASES(LinearOSNS,(OneStepNSProblem),
  (_M)
  (_keepLambdaAndYState)
  (_q)
  (_w)
  (_z))
SICONOS_IO_REGISTER_WITH_BASES(ZeroOrderHoldOSI,(OneStepIntegrator),
  (_useGammaForRelation))
SICONOS_IO_REGISTER_WITH_BASES(Equality,(LinearOSNS),
)
SICONOS_IO_REGISTER_WITH_BASES(GenericMechanical,(LinearOSNS),
)
SICONOS_IO_REGISTER_WITH_BASES(MoreauJeanCombinedProjectionOSI,(MoreauJeanOSI),
)
SICONOS_IO_REGISTER_WITH_BASES(MoreauJeanDirectProjectionOSI,(MoreauJeanOSI),
  (_activateYPosThreshold)
  (_activateYVelThreshold)
  (_deactivateYPosThreshold)
  (_deactivateYVelThreshold))
SICONOS_IO_REGISTER_WITH_BASES(TimeStepping,(Simulation),
  (_computeResiduR)
  (_computeResiduY)
  (_displayNewtonConvergence)
  (_explicitJacobiansOfRelation)
  (_isNewtonConverge)
  (_newtonCumulativeNbIterations)
  (_newtonMaxIteration)
  (_newtonNbIterations)
  (_newtonOptions)
  (_newtonResiduDSMax)
  (_newtonResiduRMax)
  (_newtonResiduYMax)
  (_newtonTolerance)
  (_newtonUpdateInteractionsPerIteration))
SICONOS_IO_REGISTER(Simulation,
  (_T)
  (_allNSProblems)
  (_allOSI)
  (_eventsManager)
  (_interman)
  (_linkOrUnlink)
  (_name)
  (_nsds)
  (_numberOfIndexSets)
  (_printStat)
  (_relativeConvergenceCriterionHeld)
  (_relativeConvergenceTol)
  (_staticLevels)
  (_tend)
  (_tinit)
  (_tolerance)
  (_tout)
  (_useRelativeConvergenceCriterion)
  (statOut))
SICONOS_IO_REGISTER(OneStepIntegrator,
  (_dynamicalSystemsGraph)
  (_extraAdditionalTerms)
  (_integratorType)
  (_levelMaxForInput)
  (_levelMaxForOutput)
  (_levelMinForInput)
  (_levelMinForOutput)
  (_simulation)
  (_sizeMem)
  (_steps))
SICONOS_IO_REGISTER_WITH_BASES(Relay,(LinearOSNS),
  (_lb)
  (_ub))
SICONOS_IO_REGISTER_WITH_BASES(LCP,(LinearOSNS),
)
SICONOS_IO_REGISTER_WITH_BASES(MLCP,(LinearOSNS),
  (_curBlock)
  (_m)
  (_n))
SICONOS_IO_REGISTER_WITH_BASES(SchatzmanPaoliOSI,(OneStepIntegrator),
  (_gamma)
  (_theta)
  (_useGamma)
  (_useGammaForRelation))
SICONOS_IO_REGISTER(Event,
  (_dTime)
  (_eventCreated)
  (_k)
  (_reschedule)
  (_td)
  (_tick)
  (_tickIncrement)
  (_timeOfEvent)
  (_type))
SICONOS_IO_REGISTER(TimeDiscretisation,
  (_h)
  (_hgmp)
  (_t0)
  (_t0gmp)
  (_tk)
  (_tkV)
  (_tkp1))
SICONOS_IO_REGISTER(EventsManager,
  (_GapLimit2Events)
  (_NSeventInsteadOfTD)
  (_T)
  (_eNonSmooth)
  (_events)
  (_k)
  (_td))
SICONOS_IO_REGISTER(OneStepNSProblem,
  (_hasBeenUpdated)
  (_indexSetLevel)
  (_inputOutputLevel)
  (_maxSize)
  (_simulation)
  (_sizeOutput))
SICONOS_IO_REGISTER(OSNSMatrix,
  (_M1)
  (_M2)
  (_dimColumn)
  (_dimRow)
  (_storageType))
SICONOS_IO_REGISTER_WITH_BASES(OSNSMatrixProjectOnConstraints,(OSNSMatrix),
)
SICONOS_IO_REGISTER(BlockCSRMatrix,
  (_diagsize0)
  (_diagsize1)
  (_nc)
  (_nr)
  (_sparseBlockStructuredMatrix)
  (colPos)
  (rowPos))
SICONOS_IO_REGISTER_WITH_BASES(MoreauJeanGOSI,(OneStepIntegrator),
  (_WBoundaryConditionsMap)
  (_explicitNewtonEulerDSOperators)
  (_gamma)
  (_theta)
  (_useGamma)
  (_useGammaForRelation))
SICONOS_IO_REGISTER_WITH_BASES(MoreauJeanOSI,(OneStepIntegrator),
  (_explicitNewtonEulerDSOperators)
  (_gamma)
  (_theta)
  (_useGamma)
  (_useGammaForRelation))
SICONOS_IO_REGISTER_WITH_BASES(EulerMoreauOSI,(OneStepIntegrator),
  (_gamma)
  (_theta)
  (_useGamma)
  (_useGammaForRelation))
SICONOS_IO_REGISTER(Model,
  (_T)
  (_author)
  (_date)
  (_description)
  (_nsds)
  (_simulation)
  (_t)
  (_t0)
  (_title))

template <class Archive>
void siconos_io_register_generated_Kernel(Archive& ar)
{
  ar.register_type(static_cast<SiconosException*>(NULL));
  ar.register_type(static_cast<SiconosMemory*>(NULL));
  ar.register_type(static_cast<BlockVector*>(NULL));
  ar.register_type(static_cast<BlockMatrix*>(NULL));
  ar.register_type(static_cast<GraphProperties*>(NULL));
  ar.register_type(static_cast<DynamicalSystemProperties*>(NULL));
  ar.register_type(static_cast<InteractionProperties*>(NULL));
  ar.register_type(static_cast<MatrixIntegrator*>(NULL));
  ar.register_type(static_cast<DynamicalSystemsGraph*>(NULL));
  ar.register_type(static_cast<InteractionsGraph*>(NULL));
  ar.register_type(static_cast<Topology*>(NULL));
  ar.register_type(static_cast<MultipleImpactNSL*>(NULL));
  ar.register_type(static_cast<ComplementarityConditionNSL*>(NULL));
  ar.register_type(static_cast<FixedBC*>(NULL));
  ar.register_type(static_cast<HarmonicBC*>(NULL));
  ar.register_type(static_cast<NSLawMatrix*>(NULL));
  ar.register_type(static_cast<EqualityConditionNSL*>(NULL));
  ar.register_type(static_cast<NewtonImpactFrictionNSL*>(NULL));
  ar.register_type(static_cast<MixedComplementarityConditionNSL*>(NULL));
  ar.register_type(static_cast<NonSmoothDynamicalSystem*>(NULL));
  ar.register_type(static_cast<PluggedObject*>(NULL));
  ar.register_type(static_cast<NewtonEulerFrom3DLocalFrameR*>(NULL));
  ar.register_type(static_cast<FirstOrderLinearTIR*>(NULL));
  ar.register_type(static_cast<BoundaryCondition*>(NULL));
  ar.register_type(static_cast<NewtonImpactNSL*>(NULL));
  ar.register_type(static_cast<NewtonEulerFrom1DLocalFrameR*>(NULL));
  ar.register_type(static_cast<LagrangianLinearTIR*>(NULL));
  ar.register_type(static_cast<NormalConeNSL*>(NULL));
  ar.register_type(static_cast<NewtonEulerR*>(NULL));
  ar.register_type(static_cast<LagrangianCompliantLinearTIR*>(NULL));
  ar.register_type(static_cast<FirstOrderLinearR*>(NULL));
  ar.register_type(static_cast<FirstOrderLinearTIDS*>(NULL));
  ar.register_type(static_cast<FirstOrderType2R*>(NULL));
  ar.register_type(static_cast<FirstOrderType1R*>(NULL));
  ar.register_type(static_cast<FirstOrderLinearDS*>(NULL));
  ar.register_type(static_cast<LagrangianCompliantR*>(NULL));
  ar.register_type(static_cast<RelayNSL*>(NULL));
  ar.register_type(static_cast<LagrangianRheonomousR*>(NULL));
  ar.register_type(static_cast<FirstOrderNonLinearR*>(NULL));
  ar.register_type(static_cast<Interaction*>(NULL));
  ar.register_type(static_cast<LagrangianLinearDiagonalDS*>(NULL));
  ar.register_type(static_cast<FirstOrderNonLinearDS*>(NULL));
  ar.register_type(static_cast<LagrangianScleronomousR*>(NULL));
  ar.register_type(static_cast<LagrangianLinearTIDS*>(NULL));
  ar.register_type(static_cast<LagrangianDS*>(NULL));
  ar.register_type(static_cast<NewtonEulerDS*>(NULL));
  ar.register_type(static_cast<MoreauJeanBilbaoOSI*>(NULL));
  ar.register_type(static_cast<InteractionManager*>(NULL));
  ar.register_type(static_cast<TimeDiscretisationEvent*>(NULL));
  ar.register_type(static_cast<TimeSteppingCombinedProjection*>(NULL));
  ar.register_type(static_cast<EventDriven*>(NULL));
  ar.register_type(static_cast<NonSmoothEvent*>(NULL));
  ar.register_type(static_cast<QP*>(NULL));
  ar.register_type(static_cast<TimeSteppingD1Minus*>(NULL));
  ar.register_type(static_cast<MLCPProjectOnConstraints*>(NULL));
  ar.register_type(static_cast<OSNSMultipleImpact*>(NULL));
  ar.register_type(static_cast<NewMarkAlphaOSI*>(NULL));
  ar.register_type(static_cast<AVI*>(NULL));
  ar.register_type(static_cast<TimeSteppingDirectProjection*>(NULL));
  ar.register_type(static_cast<ZeroOrderHoldOSI*>(NULL));
  ar.register_type(static_cast<Equality*>(NULL));
  ar.register_type(static_cast<GenericMechanical*>(NULL));
  ar.register_type(static_cast<MoreauJeanCombinedProjectionOSI*>(NULL));
  ar.register_type(static_cast<MoreauJeanDirectProjectionOSI*>(NULL));
  ar.register_type(static_cast<TimeStepping*>(NULL));
  ar.register_type(static_cast<Relay*>(NULL));
  ar.register_type(static_cast<LCP*>(NULL));
  ar.register_type(static_cast<MLCP*>(NULL));
  ar.register_type(static_cast<SchatzmanPaoliOSI*>(NULL));
  ar.register_type(static_cast<TimeDiscretisation*>(NULL));
  ar.register_type(static_cast<EventsManager*>(NULL));
  ar.register_type(static_cast<OSNSMatrix*>(NULL));
  ar.register_type(static_cast<OSNSMatrixProjectOnConstraints*>(NULL));
  ar.register_type(static_cast<BlockCSRMatrix*>(NULL));
  ar.register_type(static_cast<MoreauJeanGOSI*>(NULL));
  ar.register_type(static_cast<MoreauJeanOSI*>(NULL));
  ar.register_type(static_cast<EulerMoreauOSI*>(NULL));
  ar.register_type(static_cast<Model*>(NULL));
}
#endif
#endif
