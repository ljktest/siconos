#!/usr/bin/env python


def test_getVector():
    from Siconos.Kernel import getVector, SimpleVector
    from numpy import array

    assert (getVector([1,2,3]) == array([1,2,3])).all()

    v = SimpleVector(3)
    v.setValue(0,1)
    v.setValue(1,2)
    v.setValue(2,4)

    assert (getVector(v) != array([1,2,3])).any()

    assert (getVector(v) == array([1,2,4])).all()
    

def test_getMatrix():
    from Siconos.Kernel import getMatrix, SimpleMatrix
    from numpy import array
    
    assert (getMatrix([[1,2,3]]) == array([[1,2,3]])).all()

    m = SimpleMatrix(1,3)

    m.setValue(0,0,1)

    m.setValue(0,1,2)

    m.setValue(0,2,3)
    
    assert (getMatrix(m) == array([[1,2,3]])).all()

    assert (getMatrix(m) != array([[1,0,3]])).any()


def test_LagrangianDS_setMassPtr():
    from Siconos.Kernel import LagrangianDS
    from numpy import array

    class LDS(LagrangianDS):
        pass

    lds = LDS()

    lds.setMassPtr([[1,2,3],[4,5,6]])
    
    assert (lds.mass() == array([[1,2,3],[4,5,6]])).all()
    

def test_LagrangianScleronomousR_setJachqPtr():
    import Siconos.Kernel as K
    from numpy import array

    class Rel(K.LagrangianScleronomousR):
        pass
    
    r = Rel()
    j = array([[1,2,3],[4,5,6]])
    r.setJachqPtr(j)

    import numpy as np

    # C is transposed()
    r.C()

    assert np.max(r.C() - array([[1,2,3],[4,5,6]])) == 0.
    assert np.max(r.C() - array([[0,2,3],[4,5,6]])) == 1.

    r.setJachqPtr(r.C())

    r.C()

    assert np.max(r.C() - array([[1,2,3],[4,5,6]])) == 0.
    assert np.max(r.C() - array([[0,2,3],[4,5,6]])) == 1.


