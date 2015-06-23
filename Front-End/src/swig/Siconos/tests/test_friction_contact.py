#!/usr/bin/env python

import numpy as np

# import Siconos.Numerics * fails with py.test!
import Siconos.Numerics as N


NC = 1

M = np.eye(3*NC)

q = np.array([-1., 1., 3.])

mu = np.array([0.1])

z = np.array([0.,0.,0.])

reactions = np.array([0.,0.,0.])

velocities = np.array([0.,0.,0.])


def test_fc3dnsgs():
    N.setNumericsVerbose(2)
    FCP = N.FrictionContactProblem(3,M,q,mu)
    SO=N.SolverOptions(N.SICONOS_FRICTION_3D_NSGS)
    r=N.frictionContact3D_nsgs(FCP, reactions, velocities, SO)
    assert SO.dparam[1] < 1e-10
    assert not r


def test_fc3dlocalac():
    N.setNumericsVerbose(2)
    FCP = N.FrictionContactProblem(3,M,q,mu)
    SO=N.SolverOptions(N.SICONOS_FRICTION_3D_LOCALAC)

    r = N.frictionContact3D_localAlartCurnier(FCP, reactions, velocities, SO)
    assert SO.dparam[1] < 1e-10
    assert not r


def test_fc3dfischer():
    N.setNumericsVerbose(2)
    FCP = N.FrictionContactProblem(3,M,q,mu)
    SO=N.SolverOptions(N.SICONOS_FRICTION_3D_LOCALFB)

    r = N.frictionContact3D_localFischerBurmeister(FCP, reactions, velocities, SO)
    assert SO.dparam[1] < 1e-10
    assert not r
