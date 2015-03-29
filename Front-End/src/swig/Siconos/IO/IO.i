// -*- c++ -*-
// SWIG interface for Siconos IO
%module(directors="1", allprotected="1") IO

%include start.i

%{
#include <SiconosKernel.hpp>
#include <SiconosRestart.hpp>
%}

%include handleException.i

%include sharedPointers.i

%include KernelTypes.i

%include "SiconosRestart.hpp"

#ifdef HAVE_SICONOS_MECHANICS
%include <MechanicsIO.hpp>
%{
#include <MechanicsIO.hpp>
%}
#endif
