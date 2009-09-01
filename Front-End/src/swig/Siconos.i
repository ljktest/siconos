// -*- C++ -*-
// Siconos-Front-End version 3.0.0, Copyright INRIA 2005-2008.
// Siconos is a program dedicated to modeling, simulation and control
// of non smooth dynamical systems.	
// Siconos is a free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// Siconos is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Siconos; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
// Contact: Vincent ACARY vincent.acary@inrialpes.fr 
//	

// Siconos.i - SWIG interface
%module Siconos

%{
#define SWIG_FILE_WITH_INIT
#include <sstream>
#include "SiconosNumerics.h"
%} 

%rename (LCP) LinearComplementarity_Problem;

// numpy macros
%include numpy.i 	

%init %{
  import_array();
%}

%apply (int DIM1 , int DIM2 , double* IN_FARRAY2)
{(int nrows, int ncols, double* data          )};

%apply (int DIM1 , double* INPLACE_ARRAY1)
{(int sizex, double *x          )};

%apply (int DIM1 , double* INPLACE_ARRAY1)
{(int sizey, double *y          )};

%apply (int DIM1 , double* INPLACE_ARRAY1)
{(int sizeq, double *q         )};

%apply (int DIM1 , double* INPLACE_ARRAY1)
{(int sizez, double *z        )};

%apply (int DIM1 , double* INPLACE_ARRAY1)
{(int sizew, double *w        )};


// Handle standard exceptions
%include "exception.i"
%exception
{
  try
  {
    $action
  }
  catch (const std::invalid_argument& e)
  {
    SWIG_exception(SWIG_ValueError, e.what());
  }
  catch (const std::out_of_range& e)
  {
    SWIG_exception(SWIG_IndexError, e.what());
  }
}

// std python sequence -> C array

// int
%{
  static int convert_iarray(PyObject *input, int *ptr) {
  int i;
  if (!PySequence_Check(input)) {
      PyErr_SetString(PyExc_TypeError,"Expecting a sequence");
      return NULL;
  }
  for (i =0; i <  PyObject_Length(input); i++) {
      PyObject *o = PySequence_GetItem(input,i);
      if (!PyInt_Check(o)) {
        Py_XDECREF(o);
        PyErr_SetString(PyExc_ValueError,"Expecting a sequence of ints");
        return NULL;
      }
      ptr[i] = (int) PyInt_AsLong(o);

      if (ptr[i] == -1 && PyErr_Occurred())
        return NULL;
      Py_DECREF(o);
  }
  return 1;
}
%}

// double
%{
static int convert_darray(PyObject *input, double *ptr) {
  int i;
  if (!PySequence_Check(input)) {
      PyErr_SetString(PyExc_TypeError,"Expecting a sequence");
      return NULL;
  }

  for (i =0; i < PyObject_Length(input); i++) {
      PyObject *o = PySequence_GetItem(input,i);
      if (!PyFloat_Check(o)) {
         Py_XDECREF(o);
         PyErr_SetString(PyExc_ValueError,"Expecting a sequence of floats");
         return NULL;
      }
      ptr[i] = PyFloat_AsDouble(o);

      if (ptr[i] == -1 && PyErr_Occurred())
        return NULL;

      Py_DECREF(o);
  }
  return 1;
  
}
%}

// conversions
%typemap(in) (int sizei, int *iparam) (int *temp) {
  
  temp = (int *) malloc(sizeof(int)*PyObject_Length($input));
    if (!convert_iarray($input,temp)) {
      SWIG_fail;
  }
  $1 = PyObject_Length($input);
  $2 = &temp[0];

  }

%typemap(in) (int sized, double *dparam) (double *temp) {

  temp = (double *) malloc(sizeof(double)*PyObject_Length($input));
    if (!convert_darray($input,temp)) {
      SWIG_fail;
    }
    $1 = PyObject_Length($input);
    $2 = &temp[0];
 }


// cleanup
%typemap(freearg) (int sized, double *dparam) {
  free($2);
 }

%typemap(freearg) (int sizei, int *iparam) {
  free($2);
 }


// info result
%typemap(in, numinputs=0) int *info (int temp) {
  $1 = &temp;
}

%typemap(argout) (int *info) {
  $result = PyInt_FromLong(*$1);
 }

// vectors
%typemap(in) (double *z) (PyArrayObject* array=NULL, int is_new_object) {

  array = obj_to_array_contiguous_allow_conversion($input, NPY_DOUBLE,&is_new_object);

  if (!array || !require_dimensions(array,1) ||
      !require_native(array) || !require_contiguous(array)) SWIG_fail;
  
  $1 = (double *) array_data(array);

 }

%apply (double *z) { (double *w) }; 

%apply (double *z) { (double *zlem) };

%apply (double *z) { (double *wlem) };


// Numpy array -> NumericsMatrix (dense storage only!)
%typemap(in) 
(NumericsMatrix* A) 
(PyArrayObject* array=NULL, 
 int is_new_object=0,

 // free in typemap(freearg)
 NumericsMatrix *nummat = (NumericsMatrix *) malloc(sizeof(NumericsMatrix)))
{
  array = obj_to_array_fortran_allow_conversion($input, NPY_DOUBLE,&is_new_object);

  if (!array || !require_dimensions(array,2) ||
      !require_native(array) || !require_fortran(array)) SWIG_fail;

  nummat->storageType = 0;
  nummat->size0 =  array_size(array,0);
  nummat->size1 =  array_size(array,1);
  nummat->matrix0 = (double *)array_data(array);
  $1 = nummat;
}

%typemap(freearg) (NumericsMatrix* A) {
    free($1);
}

%apply (NumericsMatrix *A) { (NumericsMatrix *m) };
%apply (NumericsMatrix *A) { (NumericsMatrix *M) };

%feature("autodoc", "1");

%include "NumericsMatrix.h"
%include "LinearComplementarity_Problem.h"
%include "LCP_Solvers.h"
%include "Solver_Options.h"
%include "Numerics_Options.h"



%extend LinearComplementarity_Problem
{
  LinearComplementarity_Problem(PyObject *o1, PyObject *o2)
    {

      int is_new_object1, is_new_object2;
      PyArrayObject* array = obj_to_array_fortran_allow_conversion(o1, NPY_DOUBLE,&is_new_object1);
      PyArrayObject* vector = obj_to_array_contiguous_allow_conversion(o2, NPY_DOUBLE, &is_new_object2); 
      LinearComplementarity_Problem *LC;
      // free in std swig destructor
      LC = (LinearComplementarity_Problem *) malloc(sizeof(LinearComplementarity_Problem));
      NumericsMatrix *M = (NumericsMatrix *) malloc(sizeof(NumericsMatrix));

      M->storageType = 0;
      M->size0 = array_size(array,0);
      M->size1 = array_size(array,1);
      M->matrix0 = (double *) array_data(array);
      LC->size = M->size0;
      LC->M = M;
      LC->q = (double *) array_data(vector);

      return LC;



    }


  ~LinearComplementarity_Problem()
    {
      // fail?
      //free(self->M);
      //free(self->q);
     }

};



%extend Solver_Options
{
  Solver_Options(char solverName[64], int sizei, int *iparam, int sized, double *dparam, bool filterOn)
    {
      Solver_Options *SO;
      SO = (Solver_Options *) malloc(sizeof(Solver_Options));

      // copy (allocated args are free with freearg)
      int *niparam = (int *) malloc(sizeof(int)*sizei);
      double *ndparam = (double *) malloc(sizeof(double)*sized);
      memmove(niparam,iparam,sizeof(int)*sizei);
      memmove(ndparam,dparam,sizeof(double)*sized);

      SO->isSet = 1;
      SO->iSize = sizei;
      SO->iparam = niparam;
      SO->dSize = sized;
      SO->dparam = ndparam;
      SO->filterOn = filterOn;
      return SO;
    }

  ~Solver_Options() 
    { 
      free(self->iparam); 
      free(self->dparam);
     }
};

// some extensions but numpy arrays should be used instead 
%extend NumericsMatrix
{

  NumericsMatrix(int nrows, int ncols, double *data)
    {
      NumericsMatrix *M;
      
      // free in std swig destructor
      M = (NumericsMatrix *) malloc(sizeof(NumericsMatrix));
      M->storageType=0;
      M->size0 = nrows;
      M->size1 = ncols;
      M->matrix0=data;
      return M;
    }

  void set_matrix0(int i, int j, double v)
  {
    self->matrix0[i+j*self->size1] = v;
  }

  double get_matrix0(int i, int j)
  {
    return self->matrix0[i+j*self->size1];
  }


  PyObject * __setitem__(PyObject* index, double v)
  {
    int i, j;
    if (!PyArg_ParseTuple(index, "ii:NumericsMatrix___setitem__",&i,&j)) return NULL;
    NumericsMatrix_set_matrix0(self,i,j,v);
    return Py_BuildValue("");
  }

  PyObject * __getitem__(PyObject * index)
  {
    int i, j;
    if (!PyArg_ParseTuple(index, "ii:NumericsMatrix___getitem__",&i,&j)) return NULL;
    return SWIG_From_double(NumericsMatrix_get_matrix0(self,i,j));
  }

  int __len__()
  {
    return self->size0 * self->size1;
  }

  std::string __str__()
  {
    std::stringstream result;
    result << "[ ";
    for (int i=0; i < self->size0; ++i)
      {
        if (i > 0) result << "  ";
        result << "[";
        for (int j=0; j < self->size1; ++j)
          {
            result << " " << NumericsMatrix_get_matrix0(self,i,j);
            if (j < self->size1-1) result << ",";
          }
        result << " ]";
        if (i < self->size0-1) result << "," << std::endl;
      }
    result << " ]" << std::endl;
    return result.str();
  }
  
}; 

