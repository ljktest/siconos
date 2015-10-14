import os
from siconos.tests_setup import working_dir

# need a ref file.
def test_diodebridge1():
    from siconos.kernel import FirstOrderLinearDS, FirstOrderLinearTIR, \
                               ComplementarityConditionNSL, Interaction,\
                               Model, EulerMoreauOSI, TimeDiscretisation, LCP,  \
                               TimeStepping
    from numpy import empty
    from siconos.kernel import SimpleMatrix, getMatrix
    from numpy.linalg import norm

    t0 = 0.0
    T = 5.0e-3       # Total simulation time
    h_step = 1.0e-6  # Time step
    Lvalue = 1e-2  # inductance
    Cvalue = 1e-6   # capacitance
    Rvalue = 1e3    # resistance
    Vinit = 10.0    # initial voltage
    Modeltitle = "DiodeBridge"

    #
    # dynamical system
    #

    init_state = [Vinit, 0]

    A = [[0,          -1.0/Cvalue],
         [1.0/Lvalue, 0          ]]

    LSDiodeBridge=FirstOrderLinearDS(init_state, A)

    #
    # Interactions
    #

    C = [[0.,   0.],
         [0,    0.],
         [-1.,  0.],
         [1.,   0.]]

    D = [[1./Rvalue, 1./Rvalue, -1.,  0.],
         [1./Rvalue, 1./Rvalue,  0., -1.],
         [1.,        0.,         0.,  0.],
         [0.,        1.,         0.,  0.]]

    B = [[0.,        0., -1./Cvalue, 1./Cvalue],
         [0.,        0.,  0.,        0.       ]]

    LTIRDiodeBridge=FirstOrderLinearTIR(C, B)
    LTIRDiodeBridge.setDPtr(D)

    LTIRDiodeBridge.display()
    nslaw=ComplementarityConditionNSL(4)
    InterDiodeBridge=Interaction(4, nslaw, LTIRDiodeBridge, 1)


    #
    # Model
    #
    DiodeBridge=Model(t0, T, Modeltitle)

    #   add the dynamical system in the non smooth dynamical system
    DiodeBridge.nonSmoothDynamicalSystem().insertDynamicalSystem(LSDiodeBridge)

    #   link the interaction and the dynamical system
    DiodeBridge.nonSmoothDynamicalSystem().link(InterDiodeBridge, LSDiodeBridge)

    #
    # Simulation
    #

    # (1) OneStepIntegrators
    theta = 0.5
    aOSI = EulerMoreauOSI(LSDiodeBridge, theta)

    # (2) Time discretisation
    aTiDisc = TimeDiscretisation(t0, h_step)

    # (3) Non smooth problem
    aLCP = LCP()

    # (4) Simulation setup with (1) (2) (3)
    aTS = TimeStepping(aTiDisc, aOSI, aLCP)

    # end of model definition

    #
    # computation
    #

    # simulation initialization
    DiodeBridge.initialize(aTS)

    k = 0
    h = aTS.timeStep()
    print("Timestep : ", h)
    # Number of time steps
    N = (T-t0)/h
    print("Number of steps : ", N)

    # Get the values to be plotted
    # ->saved in a matrix dataPlot

    dataPlot = empty([N, 8])

    x = LSDiodeBridge.x()
    print("Initial state : ", x)
    y = InterDiodeBridge.y(0)
    print("First y : ", y)
    lambda_ = InterDiodeBridge.lambda_(0)

    # For the initial time step:
    # time
    dataPlot[k, 0] = t0

    #  inductor voltage
    dataPlot[k, 1] = x[0]

    # inductor current
    dataPlot[k, 2] = x[1]

    # diode R1 current
    dataPlot[k, 3] = y[0]

    # diode R1 voltage
    dataPlot[k, 4] = - lambda_[0]

    # diode F2 voltage
    dataPlot[k, 5] = - lambda_[1]

    # diode F1 current
    dataPlot[k, 6] = lambda_[2]

    # resistor current
    dataPlot[k, 7] = y[0] + lambda_[2]

    k += 1
    while (k < N):
        aTS.computeOneStep()
        #aLCP.display()
        dataPlot[k, 0] = aTS.nextTime()
        #  inductor voltage
        dataPlot[k, 1] = x[0]
        # inductor current
        dataPlot[k, 2] = x[1]
        # diode R1 current
        dataPlot[k, 3] = y[0]
        # diode R1 voltage
        dataPlot[k, 4] = - lambda_[0]
        # diode F2 voltage
        dataPlot[k, 5] = - lambda_[1]
        # diode F1 current
        dataPlot[k, 6] = lambda_[2]
        # resistor current
        dataPlot[k, 7] = y[0] + lambda_[2]
        k += 1
        aTS.nextStep()

    #
    # comparison with the reference file
    #
    ref = getMatrix(SimpleMatrix(os.path.join(working_dir,"data/diode_bridge.ref")))

    print(norm(dataPlot - ref))
    assert (norm(dataPlot - ref) < 1e-12)
    return ref, dataPlot


def test_diodebridge2():
    from siconos.kernel import FirstOrderLinearDS, FirstOrderLinearR, \
                               ComplementarityConditionNSL, Interaction,\
                               Model, EulerMoreauOSI, TimeDiscretisation, LCP,  \
                               TimeStepping, SiconosVector
    from numpy import empty
    from siconos.kernel import SimpleMatrix, getMatrix
    from numpy.linalg import norm

    t0 = 0.0
    T = 5.0e-3       # Total simulation time
    h_step = 1.0e-6  # Time step
    Lvalue = 1e-2  # inductance
    Cvalue = 1e-6   # capacitance
    Rvalue = 1e3    # resistance
    Vinit = 10.0    # initial voltage
    Modeltitle = "DiodeBridge"


    #
    # dynamical system
    #
    init_state = [Vinit, 0]

    A = [[0,          -1.0/Cvalue],
         [1.0/Lvalue, 0          ]]

    LSDiodeBridge=FirstOrderLinearDS(init_state, A)

    #
    # Interactions
    #

    C = [[0.,   0.],
         [0,    0.],
         [-1.,  0.],
         [1.,   0.]]

    D = [[1./Rvalue, 1./Rvalue, -1.,  0.],
         [1./Rvalue, 1./Rvalue,  0., -1.],
         [1.,        0.,         0.,  0.],
         [0.,        1.,         0.,  0.]]

    B = [[0.,        0., -1./Cvalue, 1./Cvalue],
         [0.,        0.,  0.,        0.       ]]

    class VoltageSource(FirstOrderLinearR):

        omega = 1e4
        Voffset = 0.0

        def __init__(self, *args):
            super(VoltageSource, self).__init__(*args)

        def initcomponents(self, inter, DSlink, workV, workM):
            super(VoltageSource, self).initcomponents(inter, DSlink, workV, workM)
            self._e = SiconosVector(inter.getSizeOfY())

#        def computee(self, z, e):
            # then compute self._e

    LTIRDiodeBridge=VoltageSource(C, B)
    LTIRDiodeBridge.setDPtr(D)

    nslaw=ComplementarityConditionNSL(4)
    InterDiodeBridge=Interaction(4, nslaw, LTIRDiodeBridge, 1)


    #
    # Model
    #
    DiodeBridge=Model(t0, T, Modeltitle)

    #   add the dynamical system in the non smooth dynamical system
    DiodeBridge.nonSmoothDynamicalSystem().insertDynamicalSystem(LSDiodeBridge)

    #   link the interaction and the dynamical system
    DiodeBridge.nonSmoothDynamicalSystem().link(InterDiodeBridge, LSDiodeBridge)

    #
    # Simulation
    #

    # (1) OneStepIntegrators
    theta = 0.5
    aOSI = EulerMoreauOSI(LSDiodeBridge, theta)

    # (2) Time discretisation
    aTiDisc = TimeDiscretisation(t0, h_step)

    # (3) Non smooth problem
    aLCP = LCP()

    # (4) Simulation setup with (1) (2) (3)
    aTS = TimeStepping(aTiDisc, aOSI, aLCP)

    # end of model definition

    #
    # computation
    #

    # simulation initialization
    DiodeBridge.initialize(aTS)

    k = 0
    h = aTS.timeStep()
    print("Timestep : ", h)
    # Number of time steps
    N = (T-t0)/h
    print("Number of steps : ", N)

    # Get the values to be plotted
    # ->saved in a matrix dataPlot

    dataPlot = empty([N, 8])

    x = LSDiodeBridge.x()
    print("Initial state : ", x)
    y = InterDiodeBridge.y(0)
    print("First y : ", y)
    lambda_ = InterDiodeBridge.lambda_(0)

    # For the initial time step:
    # time
    dataPlot[k, 0] = t0

    #  inductor voltage
    dataPlot[k, 1] = x[0]

    # inductor current
    dataPlot[k, 2] = x[1]

    # diode R1 current
    dataPlot[k, 3] = y[0]

    # diode R1 voltage
    dataPlot[k, 4] = - lambda_[0]

    # diode F2 voltage
    dataPlot[k, 5] = - lambda_[1]

    # diode F1 current
    dataPlot[k, 6] = lambda_[2]

    # resistor current
    dataPlot[k, 7] = y[0] + lambda_[2]

    k += 1
    while (k < N):
        aTS.computeOneStep()
        #aLCP.display()
        dataPlot[k, 0] = aTS.nextTime()
        #  inductor voltage
        dataPlot[k, 1] = x[0]
        # inductor current
        dataPlot[k, 2] = x[1]
        # diode R1 current
        dataPlot[k, 3] = y[0]
        # diode R1 voltage
        dataPlot[k, 4] = - lambda_[0]
        # diode F2 voltage
        dataPlot[k, 5] = - lambda_[1]
        # diode F1 current
        dataPlot[k, 6] = lambda_[2]
        # resistor current
        dataPlot[k, 7] = y[0] + lambda_[2]
        k += 1
        aTS.nextStep()

    #
    # comparison with the reference file
    #

    ref = getMatrix(SimpleMatrix(os.path.join(working_dir,"data/diode_bridge.ref")))

    assert (norm(dataPlot - ref) < 1e-12)

#test_diodebridge2()

#test_diodebridge1()
