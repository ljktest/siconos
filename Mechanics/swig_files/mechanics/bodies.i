// -*- c++ -*-
// SWIG interface for Siconos Mechanics/bodies
%module(directors="1", allprotected="1") Bodies

%include start.i

#undef WITH_IO
#undef WITH_SERIALIZATION

#ifdef WITH_IO
%{
#include <SiconosFull.hpp>
%}
#endif

%include sharedPointers.i

%{
#include <MechanicsFwd.hpp>
%}
%include <MechanicsFwd.hpp>

%include picklable.i

%include path.i

%include handleException.i

%include KernelTypes.i

%{
#include <SiconosKernel.hpp>
%}
%import Kernel/Kernel.i

%include pyRegister.i



// force the definition of SWIGTYPE_p_Interaction...
typedef Interaction Interaction;

PY_FULL_REGISTER(Disk);
PY_FULL_REGISTER(Circle);
PY_FULL_REGISTER(DiskDiskR);
PY_FULL_REGISTER(DiskPlanR);
PY_FULL_REGISTER(DiskMovingPlanR);
PY_FULL_REGISTER(SphereLDS);
PY_FULL_REGISTER(SphereNEDS);
PY_FULL_REGISTER(SphereLDSPlanR);
PY_FULL_REGISTER(SphereNEDSPlanR);
PY_FULL_REGISTER(SphereLDSSphereLDSR);
PY_FULL_REGISTER(SphereNEDSSphereNEDSR);


