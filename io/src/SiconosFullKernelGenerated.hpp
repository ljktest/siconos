// generated with the command : ./builder.py --targets=kernel -I/usr/local/include/siconos -I/usr/include/openmpi --output=../src/SiconosFullKernelGenerated.hpp --source=..
#ifndef SiconosFullKernelGenerated_hpp
#define SiconosFullKernelGenerated_hpp
#include <SiconosConfig.h>
#ifdef WITH_SERIALIZATION
#include "SiconosKernel.hpp"
SICONOS_IO_REGISTER(BlockVector,
  (_sizeV)
  (vect)
  (_tabIndex))
SICONOS_IO_REGISTER_WITH_BASES(NewtonEulerFrom1DLocalFrameR,(NewtonEulerR),
  (_isOnContact)
  (_Pc1)
  (_Pc2)
  (_Nc)
  (_Mabs_C)
  (_NPG1)
  (_NPG2)
  (_AUX1)
  (_AUX2))
SICONOS_IO_REGISTER_WITH_BASES(NonSmoothEvent,(Event),
)
SICONOS_IO_REGISTER(Relation,
  (_pluginh)
  (_pluginJachx)
  (_pluginJachz)
  (_pluginJachlambda)
  (_pluging)
  (_pluginJacglambda)
  (_pluginJacgx)
  (_pluginf)
  (_plugine)
  (_relationType)
  (_subType))
SICONOS_IO_REGISTER(DynamicalSystem,
  (_number)
  (_n)
  (_x0)
  (_r)
  (_normRef)
  (_x)
  (_jacxRhs)
  (_z)
  (_pluging)
  (_pluginJacgx)
  (_pluginJacxDotG)
  (_xMemory)
  (_stepsInMemory)
  (_workspace)
  (_workMatrix)
  (count))
SICONOS_IO_REGISTER(SiconosException,
  (reportMsg))
SICONOS_IO_REGISTER_WITH_BASES(MLCP,(LinearOSNS),
  (_n)
  (_m)
  (_curBlock))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianRheonomousR,(LagrangianR),
  (_hDot)
  (_pluginhDot))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderNonLinearR,(FirstOrderR),
)
SICONOS_IO_REGISTER_WITH_BASES(TimeSteppingD1Minus,(Simulation),
)
SICONOS_IO_REGISTER(Interaction,
  (_initialized)
  (_number)
  (_lowerLevelForOutput)
  (_upperLevelForOutput)
  (_lowerLevelForInput)
  (_upperLevelForInput)
  (_interactionSize)
  (_sizeOfDS)
  (_has2Bodies)
  (_absolutePosition)
  (_absolutePositionProj)
  (_y)
  (_yOld)
  (_y_k)
  (_yMemory)
  (_lambdaMemory)
  (_steps)
  (_lambda)
  (_lambdaOld)
  (_nslaw)
  (_relation)
  (_residuY)
  (_h_alpha)
  (_yForNSsolver))
SICONOS_IO_REGISTER_WITH_BASES(NewMarkAlphaOSI,(OneStepIntegrator),
  (_beta)
  (_gamma)
  (_alpha_m)
  (_alpha_f)
  (WMap)
  (_orderDenseOutput)
  (_IsVelocityLevel))
SICONOS_IO_REGISTER(TimeDiscretisation,
  (_h)
  (_tkV)
  (_t0)
  (_hgmp)
  (_tkp1)
  (_tk)
  (_t0gmp))
SICONOS_IO_REGISTER_WITH_BASES(AVI,(LinearOSNS),
)
SICONOS_IO_REGISTER(BlockCSRMatrix,
  (_nr)
  (_nc)
  (_sparseBlockStructuredMatrix)
  (_diagsize0)
  (_diagsize1)
  (rowPos)
  (colPos))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianLinearTIR,(LagrangianR),
  (_F)
  (_e))
SICONOS_IO_REGISTER_WITH_BASES(NewtonEulerFrom3DLocalFrameR,(NewtonEulerFrom1DLocalFrameR),
)
SICONOS_IO_REGISTER(GraphProperties,
  (symmetric))
SICONOS_IO_REGISTER_WITH_BASES(BlockMatrix,(SiconosMatrix),
  (_mat)
  (_tabRow)
  (_tabCol)
  (dimRow)
  (dimCol))
SICONOS_IO_REGISTER_WITH_BASES(ZeroOrderHoldOSI,(OneStepIntegrator),
  (_useGammaForRelation))
SICONOS_IO_REGISTER(Topology,
  (_DSG)
  (_IG)
  (_isTopologyUpToDate)
  (_hasChanged)
  (_numberOfConstraints)
  (_symmetric))
SICONOS_IO_REGISTER_WITH_BASES(MoreauJeanOSI,(OneStepIntegrator),
  (WMap)
  (_WBoundaryConditionsMap)
  (_theta)
  (_gamma)
  (_useGamma)
  (_useGammaForRelation))
SICONOS_IO_REGISTER(InteractionProperties,
  (block)
  (source)
  (source_pos)
  (target)
  (target_pos)
  (osi)
  (forControl)
  (DSlink)
  (workVectors)
  (workMatrices))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianScleronomousR,(LagrangianR),
  (_plugindotjacqh)
  (_dotjacqhXqdot))
SICONOS_IO_REGISTER_WITH_BASES(NewtonImpactNSL,(NonSmoothLaw),
  (_e))
SICONOS_IO_REGISTER(Model,
  (_t)
  (_t0)
  (_T)
  (_simulation)
  (_nsds)
  (_title)
  (_author)
  (_description)
  (_date))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderNonLinearDS,(DynamicalSystem),
  (_M)
  (_f)
  (_b)
  (_fold)
  (_jacobianfx)
  (_pluginf)
  (_pluginJacxf)
  (_pluginM)
  (_rMemory)
  (_invM))
SICONOS_IO_REGISTER_WITH_BASES(Relay,(LinearOSNS),
  (_lb)
  (_ub))
SICONOS_IO_REGISTER(OneStepNSProblem,
  (_sizeOutput)
  (_simulation)
  (_indexSetLevel)
  (_inputOutputLevel)
  (_maxSize)
  (_nbIter)
  (_hasBeenUpdated))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderLinearDS,(FirstOrderNonLinearDS),
  (_A)
  (_pluginA)
  (_pluginb))
SICONOS_IO_REGISTER_WITH_BASES(LinearOSNS,(OneStepNSProblem),
  (_w)
  (_z)
  (_M)
  (_q)
  (_MStorageType)
  (_keepLambdaAndYState))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderType1R,(FirstOrderR),
)
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderType2R,(FirstOrderR),
)
SICONOS_IO_REGISTER_WITH_BASES(SchatzmanPaoliOSI,(OneStepIntegrator),
  (WMap)
  (_WBoundaryConditionsMap)
  (_theta)
  (_gamma)
  (_useGamma)
  (_useGammaForRelation))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianCompliantR,(LagrangianR),
  (_pluginJachlambda))
SICONOS_IO_REGISTER(NonSmoothLaw,
  (_size)
  (_sizeProjectOnConstraints))
SICONOS_IO_REGISTER(BoundaryCondition,
  (_velocityIndices)
  (_prescribedVelocity)
  (_prescribedVelocityOld)
  (_pluginPrescribedVelocity))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderLinearR,(FirstOrderR),
  (_e))
SICONOS_IO_REGISTER_WITH_BASES(EulerMoreauOSI,(OneStepIntegrator),
  (WMap)
  (_WBoundaryConditionsMap)
  (_theta)
  (_gamma)
  (_useGamma)
  (_useGammaForRelation))
SICONOS_IO_REGISTER_WITH_BASES(InteractionsGraph,(_InteractionsGraph),
  (blockProj)
  (upper_blockProj)
  (lower_blockProj)
  (name)
  (dummy))
SICONOS_IO_REGISTER_WITH_BASES(MoreauJeanCombinedProjectionOSI,(MoreauJeanOSI),
)
SICONOS_IO_REGISTER_WITH_BASES(DynamicalSystemsGraph,(_DynamicalSystemsGraph),
  (Ad)
  (AdInt)
  (Ld)
  (Bd)
  (B)
  (L)
  (pluginB)
  (pluginL)
  (e)
  (u)
  (pluginU)
  (pluginJacgx)
  (tmpXdot)
  (jacgx)
  (name)
  (groupId)
  (dummy))
SICONOS_IO_REGISTER_WITH_BASES(NormalConeNSL,(NonSmoothLaw),
  (_H)
  (_K))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderLinearTIR,(FirstOrderR),
  (_e))
SICONOS_IO_REGISTER(OSNSMatrix,
  (_dimRow)
  (_dimColumn)
  (_storageType)
  (_M1)
  (_M2))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderR,(Relation),
  (_C)
  (_D)
  (_F)
  (_B)
  (_K))
SICONOS_IO_REGISTER(Event,
  (_timeOfEvent)
  (_tickIncrement)
  (_type)
  (_dTime)
  (_tick)
  (_eventCreated)
  (_k)
  (_td)
  (_reschedule))
SICONOS_IO_REGISTER_WITH_BASES(MLCPProjectOnConstraints,(MLCP),
  (_alpha)
  (_doProjOnEquality)
  (_useMassNormalization))
SICONOS_IO_REGISTER_WITH_BASES(OSNSMultipleImpact,(LinearOSNS),
  (Impulse_variable)
  (Time_variable)
  (Ncontact)
  (NstepMax)
  (TOL_IMPACT)
  (TypeCompLaw)
  (VelContact)
  (OldVelContact)
  (EnerContact)
  (WcContact)
  (DistriVector)
  (StateContact)
  (Kcontact)
  (ResContact)
  (ElasCoefContact)
  (DelImpulseContact)
  (TolImpulseContact)
  (ImpulseContact_update)
  (ForceContact)
  (SelectPrimaConInVel)
  (IdPrimaContact)
  (IsPrimaConEnergy)
  (VelAtPrimaCon)
  (EnerAtPrimaCon)
  (DeltaP)
  (NameFile)
  (YesSaveData)
  (NstepSave)
  (_DataMatrix)
  (SizeDataSave)
  (_IsImpactEnd)
  (_Tol_Vel)
  (_Tol_Ener)
  (_ZeroVel_EndIm)
  (_ZeroEner_EndIm)
  (Step_min_save)
  (Step_max_save))
SICONOS_IO_REGISTER_WITH_BASES(OSNSMatrixProjectOnConstraints,(OSNSMatrix),
)
SICONOS_IO_REGISTER_WITH_BASES(MoreauJeanOSI2,(MoreauJeanOSI),
)
SICONOS_IO_REGISTER_WITH_BASES(NewtonEulerDS,(DynamicalSystem),
  (_v)
  (_v0)
  (_vMemory)
  (_qMemory)
  (_forcesMemory)
  (_dotqMemory)
  (_qDim)
  (_q)
  (_q0)
  (_dotq)
  (_MObjToAbs)
  (_I)
  (_mass)
  (_massMatrix)
  (_luW)
  (_T)
  (_Tdot)
  (_p)
  (_fExt)
  (_fInt)
  (_mExt)
  (_mInt)
  (_jacobianFIntq)
  (_jacobianFIntv)
  (_jacobianMIntq)
  (_jacobianMIntv)
  (_fGyr)
  (_jacobianFGyrv)
  (_computeJacobianFIntqByFD)
  (_computeJacobianFIntvByFD)
  (_computeJacobianMIntqByFD)
  (_computeJacobianMIntvByFD)
  (_epsilonFD)
  (_pluginFExt)
  (_pluginMExt)
  (_pluginFInt)
  (_pluginMInt)
  (_pluginJacqFInt)
  (_pluginJacvFInt)
  (_pluginJacqMInt)
  (_pluginJacvMInt)
  (_forces)
  (_jacobianqForces)
  (_jacobianvForces)
  (_boundaryConditions)
  (_reactionToBoundaryConditions))
SICONOS_IO_REGISTER_WITH_BASES(RelayNSL,(NonSmoothLaw),
  (_lb)
  (_ub))
SICONOS_IO_REGISTER_WITH_BASES(EqualityConditionNSL,(NonSmoothLaw),
)
SICONOS_IO_REGISTER(OneStepIntegrator,
  (_integratorType)
  (_dynamicalSystemsGraph)
  (_sizeMem)
  (_simulation)
  (_extraAdditionalTerms))
SICONOS_IO_REGISTER(MatrixIntegrator,
  (_mat)
  (_E)
  (_plugin)
  (_isConst)
  (_DS)
  (_model)
  (_TD)
  (_sim)
  (_OSI))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianR,(Relation),
  (_jachlambda)
  (_jachq)
  (_jachqDot)
  (_dotjachq)
  (_pluginJachq))
SICONOS_IO_REGISTER_WITH_BASES(NewtonImpactFrictionNSL,(NonSmoothLaw),
  (_en)
  (_et)
  (_mu))
SICONOS_IO_REGISTER_WITH_BASES(LCP,(LinearOSNS),
)
SICONOS_IO_REGISTER_WITH_BASES(NewtonEulerR,(Relation),
  (_plugindotjacqh)
  (_secondOrderTimeDerivativeTerms)
  (_jachq)
  (_jachqDot)
  (_dotjachq)
  (_jachlambda)
  (_jacglambda)
  (_e)
  (_contactForce)
  (_jachqT))
SICONOS_IO_REGISTER_WITH_BASES(EventDriven,(Simulation),
  (TOL_ED)
  (_isNewtonConverge)
  (_newtonMaxIteration)
  (_newtonNbIterations)
  (_newtonResiduDSMax)
  (_newtonResiduYMax)
  (_newtonTolerance)
  (_localizeEventMaxIter)
  (_numberOfOneStepNSproblems)
  (_indexSet0)
  (_DSG0)
  (_istate))
SICONOS_IO_REGISTER_WITH_BASES(TimeStepping,(Simulation),
  (_newtonTolerance)
  (_newtonMaxIteration)
  (_newtonNbIterations)
  (_newtonCumulativeNbIterations)
  (_newtonOptions)
  (_newtonResiduDSMax)
  (_newtonResiduYMax)
  (_newtonResiduRMax)
  (_computeResiduY)
  (_computeResiduR)
  (_isNewtonConverge))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderLinearTIDS,(FirstOrderLinearDS),
)
SICONOS_IO_REGISTER(SiconosMemory,
  (_size)
  (_nbVectorsInMemory)
  (_vectorMemory)
  (_indx))
SICONOS_IO_REGISTER(SystemProperties,
  (upper_block)
  (lower_block)
  (workVectors)
  (workMatrices))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianLinearTIDS,(LagrangianDS),
  (_K)
  (_C))
SICONOS_IO_REGISTER_WITH_BASES(GenericMechanical,(LinearOSNS),
)
SICONOS_IO_REGISTER_WITH_BASES(MoreauJeanDirectProjectionOSI,(MoreauJeanOSI),
  (_deactivateYPosThreshold)
  (_deactivateYVelThreshold)
  (_activateYPosThreshold)
  (_activateYVelThreshold))
SICONOS_IO_REGISTER_WITH_BASES(TimeSteppingDirectProjection,(TimeStepping),
  (_indexSetLevelForProjection)
  (_nbProjectionIteration)
  (_constraintTol)
  (_constraintTolUnilateral)
  (_maxViolationUnilateral)
  (_maxViolationEquality)
  (_projectionMaxIteration)
  (_doProj)
  (_doOnlyProj))
SICONOS_IO_REGISTER_WITH_BASES(ComplementarityConditionNSL,(NonSmoothLaw),
)
SICONOS_IO_REGISTER(NonSmoothDynamicalSystem,
  (_BVP)
  (_topology)
  (_mIsLinear))
SICONOS_IO_REGISTER_WITH_BASES(MultipleImpactNSL,(NonSmoothLaw),
  (_ResCof)
  (_Stiff)
  (_ElasCof))
SICONOS_IO_REGISTER_WITH_BASES(TimeSteppingCombinedProjection,(TimeStepping),
  (_indexSetLevelForProjection)
  (_cumulatedNewtonNbIterations)
  (_nbProjectionIteration)
  (_nbCumulatedProjectionIteration)
  (_nbIndexSetsIteration)
  (_constraintTol)
  (_constraintTolUnilateral)
  (_maxViolationUnilateral)
  (_maxViolationEquality)
  (_projectionMaxIteration)
  (_kIndexSetMax)
  (_doCombinedProj)
  (_doCombinedProjOnEquality)
  (_isIndexSetsStable))
SICONOS_IO_REGISTER(Simulation,
  (_name)
  (_eventsManager)
  (_tinit)
  (_tend)
  (_tout)
  (_T)
  (_allOSI)
  (_osiMap)
  (_allNSProblems)
  (_model)
  (_levelMinForOutput)
  (_levelMaxForOutput)
  (_levelMinForInput)
  (_levelMaxForInput)
  (_numberOfIndexSets)
  (_tolerance)
  (_printStat)
  (_staticLevels)
  (statOut)
  (_useRelativeConvergenceCriterion)
  (_relativeConvergenceCriterionHeld)
  (_relativeConvergenceTol))
SICONOS_IO_REGISTER(ExtraAdditionalTerms,
)
SICONOS_IO_REGISTER_WITH_BASES(MixedComplementarityConditionNSL,(NonSmoothLaw),
  (EqualitySize))
SICONOS_IO_REGISTER(SiconosMatrix,
  (num))
SICONOS_IO_REGISTER_WITH_BASES(Equality,(LinearOSNS),
)
SICONOS_IO_REGISTER(PluggedObject,
  (_pluginName))
SICONOS_IO_REGISTER_WITH_BASES(QP,(OneStepNSProblem),
  (_Q)
  (_p))
SICONOS_IO_REGISTER(EventsManager,
  (_events)
  (_eNonSmooth)
  (_k)
  (_td)
  (_T)
  (_GapLimit2Events)
  (_NSeventInsteadOfTD))
SICONOS_IO_REGISTER_WITH_BASES(TimeDiscretisationEvent,(Event),
)
SICONOS_IO_REGISTER_WITH_BASES(LagrangianDS,(DynamicalSystem),
  (_ndof)
  (_q)
  (_q0)
  (_velocity0)
  (_qMemory)
  (_velocityMemory)
  (_jacxRhs)
  (_p)
  (_pMemory)
  (_mass)
  (_fInt)
  (_jacobianFIntq)
  (_jacobianFIntqDot)
  (_fExt)
  (_fGyr)
  (_jacobianFGyrq)
  (_jacobianFGyrqDot)
  (_forces)
  (_jacobianqForces)
  (_jacobianqDotForces)
  (_forcesMemory)
  (_boundaryConditions)
  (_reactionToBoundaryConditions)
  (_pluginMass)
  (_pluginFInt)
  (_pluginFExt)
  (_pluginFGyr)
  (_pluginJacqFInt)
  (_pluginJacqDotFInt)
  (_pluginJacqFGyr)
  (_pluginJacqDotFGyr))

template <class Archive>
void siconos_io_register_generated(Archive& ar)
{
  ar.register_type(static_cast<BlockVector*>(NULL));
  ar.register_type(static_cast<NewtonEulerFrom1DLocalFrameR*>(NULL));
  ar.register_type(static_cast<NonSmoothEvent*>(NULL));
  ar.register_type(static_cast<SiconosException*>(NULL));
  ar.register_type(static_cast<MLCP*>(NULL));
  ar.register_type(static_cast<LagrangianRheonomousR*>(NULL));
  ar.register_type(static_cast<FirstOrderNonLinearR*>(NULL));
  ar.register_type(static_cast<TimeSteppingD1Minus*>(NULL));
  ar.register_type(static_cast<Interaction*>(NULL));
  ar.register_type(static_cast<NewMarkAlphaOSI*>(NULL));
  ar.register_type(static_cast<TimeDiscretisation*>(NULL));
  ar.register_type(static_cast<AVI*>(NULL));
  ar.register_type(static_cast<BlockCSRMatrix*>(NULL));
  ar.register_type(static_cast<LagrangianLinearTIR*>(NULL));
  ar.register_type(static_cast<NewtonEulerFrom3DLocalFrameR*>(NULL));
  ar.register_type(static_cast<BlockMatrix*>(NULL));
  ar.register_type(static_cast<ZeroOrderHoldOSI*>(NULL));
  ar.register_type(static_cast<MoreauJeanOSI*>(NULL));
  ar.register_type(static_cast<LagrangianScleronomousR*>(NULL));
  ar.register_type(static_cast<NewtonImpactNSL*>(NULL));
  ar.register_type(static_cast<Model*>(NULL));
  ar.register_type(static_cast<FirstOrderNonLinearDS*>(NULL));
  ar.register_type(static_cast<Relay*>(NULL));
  ar.register_type(static_cast<FirstOrderLinearDS*>(NULL));
  ar.register_type(static_cast<FirstOrderType1R*>(NULL));
  ar.register_type(static_cast<FirstOrderType2R*>(NULL));
  ar.register_type(static_cast<SchatzmanPaoliOSI*>(NULL));
  ar.register_type(static_cast<LagrangianCompliantR*>(NULL));
  ar.register_type(static_cast<BoundaryCondition*>(NULL));
  ar.register_type(static_cast<FirstOrderLinearR*>(NULL));
  ar.register_type(static_cast<EulerMoreauOSI*>(NULL));
  ar.register_type(static_cast<MoreauJeanCombinedProjectionOSI*>(NULL));
  ar.register_type(static_cast<NormalConeNSL*>(NULL));
  ar.register_type(static_cast<FirstOrderLinearTIR*>(NULL));
  ar.register_type(static_cast<OSNSMatrix*>(NULL));
  ar.register_type(static_cast<MLCPProjectOnConstraints*>(NULL));
  ar.register_type(static_cast<OSNSMultipleImpact*>(NULL));
  ar.register_type(static_cast<OSNSMatrixProjectOnConstraints*>(NULL));
  ar.register_type(static_cast<MoreauJeanOSI2*>(NULL));
  ar.register_type(static_cast<NewtonEulerDS*>(NULL));
  ar.register_type(static_cast<RelayNSL*>(NULL));
  ar.register_type(static_cast<EqualityConditionNSL*>(NULL));
  ar.register_type(static_cast<NewtonImpactFrictionNSL*>(NULL));
  ar.register_type(static_cast<LCP*>(NULL));
  ar.register_type(static_cast<NewtonEulerR*>(NULL));
  ar.register_type(static_cast<EventDriven*>(NULL));
  ar.register_type(static_cast<TimeStepping*>(NULL));
  ar.register_type(static_cast<FirstOrderLinearTIDS*>(NULL));
  ar.register_type(static_cast<SiconosMemory*>(NULL));
  ar.register_type(static_cast<LagrangianLinearTIDS*>(NULL));
  ar.register_type(static_cast<GenericMechanical*>(NULL));
  ar.register_type(static_cast<MoreauJeanDirectProjectionOSI*>(NULL));
  ar.register_type(static_cast<TimeSteppingDirectProjection*>(NULL));
  ar.register_type(static_cast<ComplementarityConditionNSL*>(NULL));
  ar.register_type(static_cast<NonSmoothDynamicalSystem*>(NULL));
  ar.register_type(static_cast<MultipleImpactNSL*>(NULL));
  ar.register_type(static_cast<TimeSteppingCombinedProjection*>(NULL));
  ar.register_type(static_cast<MixedComplementarityConditionNSL*>(NULL));
  ar.register_type(static_cast<Equality*>(NULL));
  ar.register_type(static_cast<PluggedObject*>(NULL));
  ar.register_type(static_cast<QP*>(NULL));
  ar.register_type(static_cast<EventsManager*>(NULL));
  ar.register_type(static_cast<TimeDiscretisationEvent*>(NULL));
  ar.register_type(static_cast<LagrangianDS*>(NULL));
  ar.register_type(static_cast<GraphProperties*>(NULL));
  ar.register_type(static_cast<SystemProperties*>(NULL));
  ar.register_type(static_cast<InteractionProperties*>(NULL));
  ar.register_type(static_cast<MatrixIntegrator*>(NULL));
  ar.register_type(static_cast<DynamicalSystemsGraph*>(NULL));
  ar.register_type(static_cast<InteractionsGraph*>(NULL));
  ar.register_type(static_cast<Topology*>(NULL));
}
#endif
#endif
// generated with the command : ./builder.py --targets=kernel -I/usr/local/include/siconos -I/usr/include/openmpi --output=../src/SiconosFullKernelGenerated.hpp --source=..
#ifndef SiconosFullKernelGenerated_hpp
#define SiconosFullKernelGenerated_hpp
#include <SiconosConfig.h>
#ifdef WITH_SERIALIZATION
#include "SiconosKernel.hpp"
SICONOS_IO_REGISTER(BlockVector,
  (_sizeV)
  (vect)
  (_tabIndex))
SICONOS_IO_REGISTER_WITH_BASES(NewtonEulerFrom1DLocalFrameR,(NewtonEulerR),
  (_isOnContact)
  (_Pc1)
  (_Pc2)
  (_Nc)
  (_Mabs_C)
  (_NPG1)
  (_NPG2)
  (_AUX1)
  (_AUX2))
SICONOS_IO_REGISTER_WITH_BASES(NonSmoothEvent,(Event),
)
SICONOS_IO_REGISTER(Relation,
  (_pluginh)
  (_pluginJachx)
  (_pluginJachz)
  (_pluginJachlambda)
  (_pluging)
  (_pluginJacglambda)
  (_pluginJacgx)
  (_pluginf)
  (_plugine)
  (_relationType)
  (_subType))
SICONOS_IO_REGISTER(DynamicalSystem,
  (_number)
  (_n)
  (_x0)
  (_r)
  (_normRef)
  (_x)
  (_jacxRhs)
  (_z)
  (_pluging)
  (_pluginJacgx)
  (_pluginJacxDotG)
  (_xMemory)
  (_stepsInMemory)
  (_workspace)
  (_workMatrix)
  (count))
SICONOS_IO_REGISTER(SiconosException,
  (reportMsg))
SICONOS_IO_REGISTER_WITH_BASES(MLCP,(LinearOSNS),
  (_n)
  (_m)
  (_curBlock))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianRheonomousR,(LagrangianR),
  (_hDot)
  (_pluginhDot))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderNonLinearR,(FirstOrderR),
)
SICONOS_IO_REGISTER_WITH_BASES(TimeSteppingD1Minus,(Simulation),
)
SICONOS_IO_REGISTER(Interaction,
  (_initialized)
  (_number)
  (_lowerLevelForOutput)
  (_upperLevelForOutput)
  (_lowerLevelForInput)
  (_upperLevelForInput)
  (_interactionSize)
  (_sizeOfDS)
  (_has2Bodies)
  (_absolutePosition)
  (_absolutePositionProj)
  (_y)
  (_yOld)
  (_y_k)
  (_yMemory)
  (_lambdaMemory)
  (_steps)
  (_lambda)
  (_lambdaOld)
  (_nslaw)
  (_relation)
  (_residuY)
  (_h_alpha)
  (_yForNSsolver))
SICONOS_IO_REGISTER_WITH_BASES(NewMarkAlphaOSI,(OneStepIntegrator),
  (_beta)
  (_gamma)
  (_alpha_m)
  (_alpha_f)
  (WMap)
  (_orderDenseOutput)
  (_IsVelocityLevel))
SICONOS_IO_REGISTER(TimeDiscretisation,
  (_h)
  (_tkV)
  (_t0)
  (_hgmp)
  (_tkp1)
  (_tk)
  (_t0gmp))
SICONOS_IO_REGISTER_WITH_BASES(AVI,(LinearOSNS),
)
SICONOS_IO_REGISTER(BlockCSRMatrix,
  (_nr)
  (_nc)
  (_sparseBlockStructuredMatrix)
  (_diagsize0)
  (_diagsize1)
  (rowPos)
  (colPos))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianLinearTIR,(LagrangianR),
  (_F)
  (_e))
SICONOS_IO_REGISTER_WITH_BASES(NewtonEulerFrom3DLocalFrameR,(NewtonEulerFrom1DLocalFrameR),
)
SICONOS_IO_REGISTER(GraphProperties,
  (symmetric))
SICONOS_IO_REGISTER_WITH_BASES(BlockMatrix,(SiconosMatrix),
  (_mat)
  (_tabRow)
  (_tabCol)
  (dimRow)
  (dimCol))
SICONOS_IO_REGISTER_WITH_BASES(ZeroOrderHoldOSI,(OneStepIntegrator),
  (_useGammaForRelation))
SICONOS_IO_REGISTER(Topology,
  (_DSG)
  (_IG)
  (_isTopologyUpToDate)
  (_hasChanged)
  (_numberOfConstraints)
  (_symmetric))
SICONOS_IO_REGISTER_WITH_BASES(MoreauJeanOSI,(OneStepIntegrator),
  (WMap)
  (_WBoundaryConditionsMap)
  (_theta)
  (_gamma)
  (_useGamma)
  (_useGammaForRelation))
SICONOS_IO_REGISTER(InteractionProperties,
  (block)
  (source)
  (source_pos)
  (target)
  (target_pos)
  (osi)
  (forControl)
  (DSlink)
  (workVectors)
  (workMatrices))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianScleronomousR,(LagrangianR),
  (_plugindotjacqh)
  (_dotjacqhXqdot))
SICONOS_IO_REGISTER_WITH_BASES(NewtonImpactNSL,(NonSmoothLaw),
  (_e))
SICONOS_IO_REGISTER(Model,
  (_t)
  (_t0)
  (_T)
  (_simulation)
  (_nsds)
  (_title)
  (_author)
  (_description)
  (_date))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderNonLinearDS,(DynamicalSystem),
  (_M)
  (_f)
  (_b)
  (_fold)
  (_jacobianfx)
  (_pluginf)
  (_pluginJacxf)
  (_pluginM)
  (_rMemory)
  (_invM))
SICONOS_IO_REGISTER_WITH_BASES(Relay,(LinearOSNS),
  (_lb)
  (_ub))
SICONOS_IO_REGISTER(OneStepNSProblem,
  (_sizeOutput)
  (_simulation)
  (_indexSetLevel)
  (_inputOutputLevel)
  (_maxSize)
  (_nbIter)
  (_hasBeenUpdated))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderLinearDS,(FirstOrderNonLinearDS),
  (_A)
  (_pluginA)
  (_pluginb))
SICONOS_IO_REGISTER_WITH_BASES(LinearOSNS,(OneStepNSProblem),
  (_w)
  (_z)
  (_M)
  (_q)
  (_MStorageType)
  (_keepLambdaAndYState))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderType1R,(FirstOrderR),
)
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderType2R,(FirstOrderR),
)
SICONOS_IO_REGISTER_WITH_BASES(SchatzmanPaoliOSI,(OneStepIntegrator),
  (WMap)
  (_WBoundaryConditionsMap)
  (_theta)
  (_gamma)
  (_useGamma)
  (_useGammaForRelation))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianCompliantR,(LagrangianR),
  (_pluginJachlambda))
SICONOS_IO_REGISTER(NonSmoothLaw,
  (_size)
  (_sizeProjectOnConstraints))
SICONOS_IO_REGISTER(BoundaryCondition,
  (_velocityIndices)
  (_prescribedVelocity)
  (_prescribedVelocityOld)
  (_pluginPrescribedVelocity))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderLinearR,(FirstOrderR),
  (_e))
SICONOS_IO_REGISTER_WITH_BASES(EulerMoreauOSI,(OneStepIntegrator),
  (WMap)
  (_WBoundaryConditionsMap)
  (_theta)
  (_gamma)
  (_useGamma)
  (_useGammaForRelation))
SICONOS_IO_REGISTER_WITH_BASES(InteractionsGraph,(_InteractionsGraph),
  (blockProj)
  (upper_blockProj)
  (lower_blockProj)
  (name)
  (dummy))
SICONOS_IO_REGISTER_WITH_BASES(MoreauJeanCombinedProjectionOSI,(MoreauJeanOSI),
)
SICONOS_IO_REGISTER_WITH_BASES(DynamicalSystemsGraph,(_DynamicalSystemsGraph),
  (Ad)
  (AdInt)
  (Ld)
  (Bd)
  (B)
  (L)
  (pluginB)
  (pluginL)
  (e)
  (u)
  (pluginU)
  (pluginJacgx)
  (tmpXdot)
  (jacgx)
  (name)
  (groupId)
  (dummy))
SICONOS_IO_REGISTER_WITH_BASES(NormalConeNSL,(NonSmoothLaw),
  (_H)
  (_K))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderLinearTIR,(FirstOrderR),
  (_e))
SICONOS_IO_REGISTER(OSNSMatrix,
  (_dimRow)
  (_dimColumn)
  (_storageType)
  (_M1)
  (_M2))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderR,(Relation),
  (_C)
  (_D)
  (_F)
  (_B)
  (_K))
SICONOS_IO_REGISTER(Event,
  (_timeOfEvent)
  (_tickIncrement)
  (_type)
  (_dTime)
  (_tick)
  (_eventCreated)
  (_k)
  (_td)
  (_reschedule))
SICONOS_IO_REGISTER_WITH_BASES(MLCPProjectOnConstraints,(MLCP),
  (_alpha)
  (_doProjOnEquality)
  (_useMassNormalization))
SICONOS_IO_REGISTER_WITH_BASES(OSNSMultipleImpact,(LinearOSNS),
  (Impulse_variable)
  (Time_variable)
  (Ncontact)
  (NstepMax)
  (TOL_IMPACT)
  (TypeCompLaw)
  (VelContact)
  (OldVelContact)
  (EnerContact)
  (WcContact)
  (DistriVector)
  (StateContact)
  (Kcontact)
  (ResContact)
  (ElasCoefContact)
  (DelImpulseContact)
  (TolImpulseContact)
  (ImpulseContact_update)
  (ForceContact)
  (SelectPrimaConInVel)
  (IdPrimaContact)
  (IsPrimaConEnergy)
  (VelAtPrimaCon)
  (EnerAtPrimaCon)
  (DeltaP)
  (NameFile)
  (YesSaveData)
  (NstepSave)
  (_DataMatrix)
  (SizeDataSave)
  (_IsImpactEnd)
  (_Tol_Vel)
  (_Tol_Ener)
  (_ZeroVel_EndIm)
  (_ZeroEner_EndIm)
  (Step_min_save)
  (Step_max_save))
SICONOS_IO_REGISTER_WITH_BASES(OSNSMatrixProjectOnConstraints,(OSNSMatrix),
)
SICONOS_IO_REGISTER_WITH_BASES(MoreauJeanOSI2,(MoreauJeanOSI),
)
SICONOS_IO_REGISTER_WITH_BASES(NewtonEulerDS,(DynamicalSystem),
  (_v)
  (_v0)
  (_vMemory)
  (_qMemory)
  (_forcesMemory)
  (_dotqMemory)
  (_qDim)
  (_q)
  (_q0)
  (_dotq)
  (_MObjToAbs)
  (_I)
  (_mass)
  (_massMatrix)
  (_luW)
  (_T)
  (_Tdot)
  (_p)
  (_fExt)
  (_fInt)
  (_mExt)
  (_mInt)
  (_jacobianFIntq)
  (_jacobianFIntv)
  (_jacobianMIntq)
  (_jacobianMIntv)
  (_fGyr)
  (_jacobianFGyrv)
  (_computeJacobianFIntqByFD)
  (_computeJacobianFIntvByFD)
  (_computeJacobianMIntqByFD)
  (_computeJacobianMIntvByFD)
  (_epsilonFD)
  (_pluginFExt)
  (_pluginMExt)
  (_pluginFInt)
  (_pluginMInt)
  (_pluginJacqFInt)
  (_pluginJacvFInt)
  (_pluginJacqMInt)
  (_pluginJacvMInt)
  (_forces)
  (_jacobianqForces)
  (_jacobianvForces)
  (_boundaryConditions)
  (_reactionToBoundaryConditions))
SICONOS_IO_REGISTER_WITH_BASES(RelayNSL,(NonSmoothLaw),
  (_lb)
  (_ub))
SICONOS_IO_REGISTER_WITH_BASES(EqualityConditionNSL,(NonSmoothLaw),
)
SICONOS_IO_REGISTER(OneStepIntegrator,
  (_integratorType)
  (OSIDynamicalSystems)
  (_sizeMem)
  (_simulation)
  (_extraAdditionalTerms))
SICONOS_IO_REGISTER(MatrixIntegrator,
  (_mat)
  (_E)
  (_plugin)
  (_isConst)
  (_DS)
  (_model)
  (_TD)
  (_sim)
  (_OSI))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianR,(Relation),
  (_jachlambda)
  (_jachq)
  (_jachqDot)
  (_dotjachq)
  (_pluginJachq))
SICONOS_IO_REGISTER_WITH_BASES(NewtonImpactFrictionNSL,(NonSmoothLaw),
  (_en)
  (_et)
  (_mu))
SICONOS_IO_REGISTER_WITH_BASES(LCP,(LinearOSNS),
)
SICONOS_IO_REGISTER_WITH_BASES(NewtonEulerR,(Relation),
  (_plugindotjacqh)
  (_secondOrderTimeDerivativeTerms)
  (_jachq)
  (_jachqDot)
  (_dotjachq)
  (_jachlambda)
  (_jacglambda)
  (_e)
  (_contactForce)
  (_jachqT))
SICONOS_IO_REGISTER_WITH_BASES(EventDriven,(Simulation),
  (TOL_ED)
  (_isNewtonConverge)
  (_newtonMaxIteration)
  (_newtonNbIterations)
  (_newtonResiduDSMax)
  (_newtonResiduYMax)
  (_newtonTolerance)
  (_localizeEventMaxIter)
  (_numberOfOneStepNSproblems)
  (_indexSet0)
  (_DSG0)
  (_istate))
SICONOS_IO_REGISTER_WITH_BASES(TimeStepping,(Simulation),
  (_newtonTolerance)
  (_newtonMaxIteration)
  (_newtonNbIterations)
  (_newtonCumulativeNbIterations)
  (_newtonOptions)
  (_newtonResiduDSMax)
  (_newtonResiduYMax)
  (_newtonResiduRMax)
  (_computeResiduY)
  (_computeResiduR)
  (_isNewtonConverge))
SICONOS_IO_REGISTER_WITH_BASES(FirstOrderLinearTIDS,(FirstOrderLinearDS),
)
SICONOS_IO_REGISTER(SiconosMemory,
  (_size)
  (_nbVectorsInMemory)
  (_vectorMemory)
  (_indx))
SICONOS_IO_REGISTER(SystemProperties,
  (upper_block)
  (lower_block)
  (workVectors)
  (workMatrices))
SICONOS_IO_REGISTER_WITH_BASES(LagrangianLinearTIDS,(LagrangianDS),
  (_K)
  (_C))
SICONOS_IO_REGISTER_WITH_BASES(GenericMechanical,(LinearOSNS),
)
SICONOS_IO_REGISTER_WITH_BASES(MoreauJeanDirectProjectionOSI,(MoreauJeanOSI),
  (_deactivateYPosThreshold)
  (_deactivateYVelThreshold)
  (_activateYPosThreshold)
  (_activateYVelThreshold))
SICONOS_IO_REGISTER_WITH_BASES(TimeSteppingDirectProjection,(TimeStepping),
  (_indexSetLevelForProjection)
  (_nbProjectionIteration)
  (_constraintTol)
  (_constraintTolUnilateral)
  (_maxViolationUnilateral)
  (_maxViolationEquality)
  (_projectionMaxIteration)
  (_doProj)
  (_doOnlyProj))
SICONOS_IO_REGISTER_WITH_BASES(ComplementarityConditionNSL,(NonSmoothLaw),
)
SICONOS_IO_REGISTER(NonSmoothDynamicalSystem,
  (_BVP)
  (_topology)
  (_mIsLinear))
SICONOS_IO_REGISTER_WITH_BASES(MultipleImpactNSL,(NonSmoothLaw),
  (_ResCof)
  (_Stiff)
  (_ElasCof))
SICONOS_IO_REGISTER_WITH_BASES(TimeSteppingCombinedProjection,(TimeStepping),
  (_indexSetLevelForProjection)
  (_cumulatedNewtonNbIterations)
  (_nbProjectionIteration)
  (_nbCumulatedProjectionIteration)
  (_nbIndexSetsIteration)
  (_constraintTol)
  (_constraintTolUnilateral)
  (_maxViolationUnilateral)
  (_maxViolationEquality)
  (_projectionMaxIteration)
  (_kIndexSetMax)
  (_doCombinedProj)
  (_doCombinedProjOnEquality)
  (_isIndexSetsStable))
SICONOS_IO_REGISTER(Simulation,
  (_name)
  (_eventsManager)
  (_tinit)
  (_tend)
  (_tout)
  (_T)
  (_allOSI)
  (_osiMap)
  (_allNSProblems)
  (_model)
  (_levelMinForOutput)
  (_levelMaxForOutput)
  (_levelMinForInput)
  (_levelMaxForInput)
  (_numberOfIndexSets)
  (_tolerance)
  (_printStat)
  (_staticLevels)
  (statOut)
  (_useRelativeConvergenceCriterion)
  (_relativeConvergenceCriterionHeld)
  (_relativeConvergenceTol))
SICONOS_IO_REGISTER(ExtraAdditionalTerms,
)
SICONOS_IO_REGISTER_WITH_BASES(MixedComplementarityConditionNSL,(NonSmoothLaw),
  (EqualitySize))
SICONOS_IO_REGISTER(SiconosMatrix,
  (num))
SICONOS_IO_REGISTER_WITH_BASES(Equality,(LinearOSNS),
)
SICONOS_IO_REGISTER(PluggedObject,
  (_pluginName))
SICONOS_IO_REGISTER_WITH_BASES(QP,(OneStepNSProblem),
  (_Q)
  (_p))
SICONOS_IO_REGISTER(EventsManager,
  (_events)
  (_eNonSmooth)
  (_k)
  (_td)
  (_T)
  (_GapLimit2Events)
  (_NSeventInsteadOfTD))
SICONOS_IO_REGISTER_WITH_BASES(TimeDiscretisationEvent,(Event),
)
SICONOS_IO_REGISTER_WITH_BASES(LagrangianDS,(DynamicalSystem),
  (_ndof)
  (_q)
  (_q0)
  (_velocity0)
  (_qMemory)
  (_velocityMemory)
  (_jacxRhs)
  (_p)
  (_pMemory)
  (_mass)
  (_fInt)
  (_jacobianFIntq)
  (_jacobianFIntqDot)
  (_fExt)
  (_fGyr)
  (_jacobianFGyrq)
  (_jacobianFGyrqDot)
  (_forces)
  (_jacobianqForces)
  (_jacobianqDotForces)
  (_forcesMemory)
  (_boundaryConditions)
  (_reactionToBoundaryConditions)
  (_pluginMass)
  (_pluginFInt)
  (_pluginFExt)
  (_pluginFGyr)
  (_pluginJacqFInt)
  (_pluginJacqDotFInt)
  (_pluginJacqFGyr)
  (_pluginJacqDotFGyr))

template <class Archive>
void siconos_io_register_generated(Archive& ar)
{
  ar.register_type(static_cast<BlockVector*>(NULL));
  ar.register_type(static_cast<NewtonEulerFrom1DLocalFrameR*>(NULL));
  ar.register_type(static_cast<NonSmoothEvent*>(NULL));
  ar.register_type(static_cast<SiconosException*>(NULL));
  ar.register_type(static_cast<MLCP*>(NULL));
  ar.register_type(static_cast<LagrangianRheonomousR*>(NULL));
  ar.register_type(static_cast<FirstOrderNonLinearR*>(NULL));
  ar.register_type(static_cast<TimeSteppingD1Minus*>(NULL));
  ar.register_type(static_cast<Interaction*>(NULL));
  ar.register_type(static_cast<NewMarkAlphaOSI*>(NULL));
  ar.register_type(static_cast<TimeDiscretisation*>(NULL));
  ar.register_type(static_cast<AVI*>(NULL));
  ar.register_type(static_cast<BlockCSRMatrix*>(NULL));
  ar.register_type(static_cast<LagrangianLinearTIR*>(NULL));
  ar.register_type(static_cast<NewtonEulerFrom3DLocalFrameR*>(NULL));
  ar.register_type(static_cast<BlockMatrix*>(NULL));
  ar.register_type(static_cast<ZeroOrderHoldOSI*>(NULL));
  ar.register_type(static_cast<MoreauJeanOSI*>(NULL));
  ar.register_type(static_cast<LagrangianScleronomousR*>(NULL));
  ar.register_type(static_cast<NewtonImpactNSL*>(NULL));
  ar.register_type(static_cast<Model*>(NULL));
  ar.register_type(static_cast<FirstOrderNonLinearDS*>(NULL));
  ar.register_type(static_cast<Relay*>(NULL));
  ar.register_type(static_cast<FirstOrderLinearDS*>(NULL));
  ar.register_type(static_cast<FirstOrderType1R*>(NULL));
  ar.register_type(static_cast<FirstOrderType2R*>(NULL));
  ar.register_type(static_cast<SchatzmanPaoliOSI*>(NULL));
  ar.register_type(static_cast<LagrangianCompliantR*>(NULL));
  ar.register_type(static_cast<BoundaryCondition*>(NULL));
  ar.register_type(static_cast<FirstOrderLinearR*>(NULL));
  ar.register_type(static_cast<EulerMoreauOSI*>(NULL));
  ar.register_type(static_cast<MoreauJeanCombinedProjectionOSI*>(NULL));
  ar.register_type(static_cast<NormalConeNSL*>(NULL));
  ar.register_type(static_cast<FirstOrderLinearTIR*>(NULL));
  ar.register_type(static_cast<OSNSMatrix*>(NULL));
  ar.register_type(static_cast<MLCPProjectOnConstraints*>(NULL));
  ar.register_type(static_cast<OSNSMultipleImpact*>(NULL));
  ar.register_type(static_cast<OSNSMatrixProjectOnConstraints*>(NULL));
  ar.register_type(static_cast<MoreauJeanOSI2*>(NULL));
  ar.register_type(static_cast<NewtonEulerDS*>(NULL));
  ar.register_type(static_cast<RelayNSL*>(NULL));
  ar.register_type(static_cast<EqualityConditionNSL*>(NULL));
  ar.register_type(static_cast<NewtonImpactFrictionNSL*>(NULL));
  ar.register_type(static_cast<LCP*>(NULL));
  ar.register_type(static_cast<NewtonEulerR*>(NULL));
  ar.register_type(static_cast<EventDriven*>(NULL));
  ar.register_type(static_cast<TimeStepping*>(NULL));
  ar.register_type(static_cast<FirstOrderLinearTIDS*>(NULL));
  ar.register_type(static_cast<SiconosMemory*>(NULL));
  ar.register_type(static_cast<LagrangianLinearTIDS*>(NULL));
  ar.register_type(static_cast<GenericMechanical*>(NULL));
  ar.register_type(static_cast<MoreauJeanDirectProjectionOSI*>(NULL));
  ar.register_type(static_cast<TimeSteppingDirectProjection*>(NULL));
  ar.register_type(static_cast<ComplementarityConditionNSL*>(NULL));
  ar.register_type(static_cast<NonSmoothDynamicalSystem*>(NULL));
  ar.register_type(static_cast<MultipleImpactNSL*>(NULL));
  ar.register_type(static_cast<TimeSteppingCombinedProjection*>(NULL));
  ar.register_type(static_cast<MixedComplementarityConditionNSL*>(NULL));
  ar.register_type(static_cast<Equality*>(NULL));
  ar.register_type(static_cast<PluggedObject*>(NULL));
  ar.register_type(static_cast<QP*>(NULL));
  ar.register_type(static_cast<EventsManager*>(NULL));
  ar.register_type(static_cast<TimeDiscretisationEvent*>(NULL));
  ar.register_type(static_cast<LagrangianDS*>(NULL));
  ar.register_type(static_cast<GraphProperties*>(NULL));
  ar.register_type(static_cast<SystemProperties*>(NULL));
  ar.register_type(static_cast<InteractionProperties*>(NULL));
  ar.register_type(static_cast<MatrixIntegrator*>(NULL));
  ar.register_type(static_cast<DynamicalSystemsGraph*>(NULL));
  ar.register_type(static_cast<InteractionsGraph*>(NULL));
  ar.register_type(static_cast<Topology*>(NULL));
}
#endif
#endif
