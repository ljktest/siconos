// -*- c++ -*-
%module(package="contact_detection", directors="1", allprotected="1") base

%include MechanicsBase.i


%fragment("NumPy_Fragments");

// suppress warning
%ignore  STD11::enable_shared_from_this< Hashed >;
%template (sharedHashed) STD11::enable_shared_from_this< Hashed >;

PY_FULL_REGISTER(SpaceFilter);
PY_FULL_REGISTER(SiconosBodies);
PY_FULL_REGISTER(ExternalBody);
