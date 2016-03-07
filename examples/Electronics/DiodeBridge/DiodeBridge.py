#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Siconos-sample , Copyright INRIA 2005-2011.
# Siconos is a program dedicated to modeling, simulation and control
# of non smooth dynamical systems.
# Siconos is a free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# Siconos is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Siconos; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# Contact: Vincent ACARY vincent.acary@inrialpes.fr
#
# -----------------------------------------------------------------------
#
#  DiodeBridge  : sample of an electrical circuit involving :
#    - a linear dynamical system consisting of an LC oscillator (1 µF , 10 mH)
#    - a non smooth system (a 1000 Ohm resistor supplied through a 4
#	diodes bridge) in parallel with the oscillator
#
#  Expected behavior :
#
#  The initial state (Vc = 10 V , IL = 0) of the oscillator provides
#  an initial energy.
#  The period is 2 Pi sqrt(LC) ~ 0,628 ms.
#  The non smooth system is a full wave rectifier :
#  each phase (positive and negative) of the oscillation allows current to flow
#  through the resistor in a constant direction, resulting in an energy loss :
#  the oscillation damps.
#
#  State variables :
#    - the voltage across the capacitor (or inductor)
#    - the current through the inductor
#
#  Since there is only one dynamical system, the interaction is defined by :
#    - complementarity laws between diodes current and
#  voltage. Depending on the diode position in the bridge, y stands
#  for the reverse voltage across the diode or for the diode
#  current (see figure in the template file)
#    - a linear time invariant relation between the state variables and
#	y and lambda (derived from Kirchhoff laws)
#
# -----------------------------------------------------------------------

with_plot = True
if with_plot:
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt

from siconos.kernel import FirstOrderLinearDS, FirstOrderLinearTIR, \
    ComplementarityConditionNSL, Interaction,\
    Model, EulerMoreauOSI, TimeDiscretisation, LCP,  \
    TimeStepping



t0 = 0.0
T = 5.0e-3       # Total simulation time
h_step = 1.0e-6  # Time step
Lvalue = 1e-2    # inductance
Cvalue = 1e-6    # capacitance
Rvalue = 1e3     # resistance
Vinit = 10.0     # initial voltage
Modeltitle = "DiodeBridge"

#
# dynamical system
#
init_state = [Vinit, 0]

A = [[0,          -1.0/Cvalue],
     [1.0/Lvalue, 0          ]]

LSDiodeBridge = FirstOrderLinearDS(init_state, A)

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

LTIRDiodeBridge = FirstOrderLinearTIR(C, B)
LTIRDiodeBridge.setDPtr(D)

nslaw = ComplementarityConditionNSL(4)
InterDiodeBridge = Interaction(nslaw, LTIRDiodeBridge)


#
# Model
#
DiodeBridge = Model(t0, T, Modeltitle)

#   add the dynamical system in the non smooth dynamical system
DiodeBridge.nonSmoothDynamicalSystem().insertDynamicalSystem(LSDiodeBridge)

#   link the interaction and the dynamical system
DiodeBridge.nonSmoothDynamicalSystem().link(InterDiodeBridge, LSDiodeBridge)

#
# Simulation
#

# (1) OneStepIntegrators
theta = 0.5
aOSI = EulerMoreauOSI(theta)
aOSI.insertDynamicalSystem(LSDiodeBridge)
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
N = (T - t0) / h
print("Number of steps : ", N)

# Get the values to be plotted
# ->saved in a matrix dataPlot

from numpy import zeros
dataPlot = zeros([N, 8])

x = LSDiodeBridge.x()
print("Initial state : ", x)
y = InterDiodeBridge.y(0)
print("First y : ", y)
lambda_ = InterDiodeBridge.lambda_(0)

# For the initial time step:
# time

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

# comparison with reference file
from siconos.kernel import SimpleMatrix, getMatrix
from numpy.linalg import norm

ref = getMatrix(SimpleMatrix("result.ref"))

assert (norm(dataPlot - ref) < 1e-12)

if with_plot:
    #
    # plots
    #
    #plt.ion()
    plt.subplot(411)
    plt.title('inductor voltage')
    plt.plot(dataPlot[0:k - 1, 0], dataPlot[0:k - 1, 1])
    plt.grid()
    plt.subplot(412)
    plt.title('inductor current')
    plt.plot(dataPlot[0:k - 1, 0], dataPlot[0:k - 1, 2])
    plt.grid()
    plt.subplot(413)
    plt.title('diode R1 (blue) and F2 (green) voltage')
    plt.plot(dataPlot[0:k - 1, 0], -dataPlot[0:k - 1, 4])
    plt.plot(dataPlot[0:k - 1, 0], dataPlot[0:k - 1, 5])
    plt.grid()
    plt.subplot(414)
    plt.title('resistor current')
    plt.plot(dataPlot[0:k - 1, 0], dataPlot[0:k - 1, 7])
    plt.grid()
    plt.savefig("diode_bridge.png")
