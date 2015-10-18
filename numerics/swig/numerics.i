// -*- C++ -*-
// Siconos-Front-End , Copyright INRIA 2005-2012.
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
// Contact: Vincent ACARY, siconos-team@lists.gforge.inria.fr
//

// Siconos.i - SWIG interface for Siconos
%module(package="siconos", directors="1") numerics

%include start.i

// generated docstrings from doxygen xml output
%include numerics-docstrings.i

%include <typemaps/swigmacros.swg>

%define %SN_INPUT_CHECK_RETURN(var, output, type_)
{ void* _ptr = NULL; if (!SWIG_IsOK(SWIG_ConvertPtr(var, &_ptr, SWIGTYPE_p_##type_, 0 |  0 ))) {
    char errmsg[1024];
    snprintf(errmsg, sizeof(errmsg), "Argument check failed! Argument %s has the wrong type, should be %s", #var, #type_);
    PyErr_SetString(PyExc_TypeError, errmsg); PyErr_PrintEx(0); return NULL; }
    output = %static_cast(_ptr, type_*);
   }
%enddef

%{
#include "SiconosNumerics.h"
#include "SiconosConfig.h"
#include "SolverOptions.h"
#include "SparseMatrix.h"
#include "SparseBlockMatrix.h"
#include "FrictionContact3D_Solvers.h"
#include "Friction_cst.h"
#include "FrictionContact3D_AlartCurnier.h"
#include "FrictionContact3D_localAlartCurnier.h"
#include "FrictionContact3D_localFischerBurmeister.h"
#include "AlartCurnierGenerated.h"
#include "FischerBurmeisterGenerated.h"
#include "FrictionContact3D_compute_error.h"
#include "fclib_interface.h"
#include "Numerics_functions.h"
#include "SiconosSets.h"
#include <numpy/npy_3kcompat.h>


#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/cat.hpp>

#include <limits>
%}

#ifdef WITH_IO
%{
#include <SiconosFullNumerics.hpp>
%}
#endif
%include "picklable.i"

%include "std_string.i"

 // needed macros
 %include "SiconosConfig.h"



 // more convenient
 %rename (LCP) LinearComplementarityProblem;
 %rename (MLCP) MixedLinearComplementarityProblem;
 %rename (MCP) MixedComplementarityProblem;
 %rename (VI) VariationalInequality_;
 %rename (AVI) AffineVariationalInequalities;

 %ignore lcp_compute_error_only;

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

 %apply (double IN_ARRAY1[ANY])
 {(double reaction[3])}

 %apply (double IN_ARRAY1[ANY])
 {(double velocity[3])}

 %apply (double IN_ARRAY1[ANY])
 {(double rho[3])}

 %apply (double ARGOUT_ARRAY1[ANY])
 {(double ACresult[3])}

 %apply (double ARGOUT_ARRAY1[ANY])
 {(double result[3])}

 %apply (double ARGOUT_ARRAY1[ANY])
 {(double A[9])}

 %apply (double ARGOUT_ARRAY1[ANY])
 {(double B[9])}


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

%include solverOptions.i

%include Numerics_typemaps_problems.i
%include Numerics_typemaps_basic_linalg.i
%include Numerics_typemaps_numericsmatrices.i

// this has to die --xhub
// info param
%typemap(in, numinputs=0) (int *info) (int temp_info = -1)
{
  // a default initialization : solver may stop if *info = 0 (checkTrivialCase)
  // checkTrivialCase => better if directly in solvers, not in driver.
  $1 = &temp_info;
}

%inline
%{
 unsigned int isqrt(unsigned int n)
  {
    unsigned int c = 0x8000;
    unsigned int g = 0x8000;

    for(;;)
    {
      if(g*g > n)
      g ^= c;
      c >>= 1;
      if(c == 0)
        return g;
      g |= c;
    }
  }

  int compiled_in_debug_mode()
  {
#ifdef NDEBUG
    return 0;
#else
    return 1;
#endif
  }

  extern "C" void set_cstruct(uintptr_t p_env, void* p_struct)
  {
    *(void**)p_env = p_struct;
  }
%}

%fragment("NumPy_Fragments");

// LCP
%include "LinearComplementarityProblem.h"
%include "LCP_Solvers.h"
%include "lcp_cst.h"
%include "SolverOptions.h"
%include "NumericsOptions.h"

//Relay
%include "relay_cst.h"
%include Numerics_AVI.i


// redefine typemap on q for MLCP
%typemap(out) (double* q) {
  npy_intp dims[2];

  dims[0] = arg1->m + arg1->n;
  dims[1] = 1;
  if ($1)
  {
    PyObject *obj = PyArray_SimpleNewFromData(2, dims, NPY_DOUBLE, $1);
    PyArrayObject *array = (PyArrayObject*) obj;
    if (!array || !require_fortran(array)) SWIG_fail;
    $result = obj;
  }
  else
    SWIG_fail;
 }



// MLCP
%include "MixedLinearComplementarityProblem.h"
%include "MLCP_Solvers.h"
%include "mlcp_cst.h"

%inline %{


#include <stdio.h>
  FrictionContactProblem* frictionContactProblemFromFile
    (const char * filename)
  {
    FILE * finput = fopen(filename, "r");
    if (finput)
    {
      FrictionContactProblem* problem =
        (FrictionContactProblem *) malloc(sizeof(FrictionContactProblem));
      if (frictionContact_newFromFile(problem,finput))
      {
      char msg[1024];
      snprintf(msg, sizeof(msg), "frictionContactProblemFromFile: cannot load %s\n",filename);
      PyErr_SetString(PyExc_RuntimeError, msg);
      PyErr_PrintEx(0);
      free(problem);
      fclose(finput);
      return NULL;
      }
      else
      {
        fclose(finput);
        return problem;
      }
    }
    else
    {
      char msg[1024];
      snprintf(msg, sizeof(msg), "frictionContactProblemFromFile: cannot open %s\n",filename);
      PyErr_SetString(PyExc_RuntimeError, msg);
      PyErr_PrintEx(0);
      return NULL;
    }
    
  }

  MixedLinearComplementarityProblem* mixedLinearComplementarityProblemFromFile
    (const char * filename)
  {
    FILE * finput = fopen(filename, "r");
    if (finput)
    {
      MixedLinearComplementarityProblem* problem =
        (MixedLinearComplementarityProblem *) malloc(sizeof(MixedLinearComplementarityProblem));
      if (mixedLinearComplementarity_newFromFile(problem,finput))
      {
      fprintf(stderr, "mixedLinearComplementarityProblemFromFile: cannot load %s\n",filename);
      free(problem);
      fclose(finput);
      return 0;
      }
      else
      {
        fclose(finput);
        return problem;
      }
    }
    else
    {
      fclose(finput);
      fprintf(stderr, "mixedLinearComplementarityProblemFromFile: cannot open %s\n",filename);
      return 0;
    }
  }
%}

%include Numerics_for_python_callback.i

// MCP
%include "MixedComplementarityProblem.h"
%include "MCP_Solvers.h"
%include "MCP_cst.h"

%extend NumericsOptions
{
  NumericsOptions()
  {
    NumericsOptions *numerics_options;
    numerics_options = (NumericsOptions *) malloc(sizeof(NumericsOptions));
    return numerics_options;
  }

  ~NumericsOptions()
  {
    delete($self);
  }
}

%extend _SolverOptions
{
  _SolverOptions(FRICTION_SOLVER id)
  {
    _SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));

    /* cf Friction_cst.h */
    if(id >= 400 && id < 500)
    {
      frictionContact2D_setDefaultSolverOptions(SO, id);
    }
    else
    {
      frictionContact3D_setDefaultSolverOptions(SO, id);
    }

    return SO;
  }

  _SolverOptions()
    {
      SolverOptions *SO;
      SO = (SolverOptions *) malloc(sizeof(SolverOptions));
      return SO;
    }

  _SolverOptions(LinearComplementarityProblem* lcp, LCP_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    set_SolverOptions(SO, id);
    return SO;
  }

  _SolverOptions(MixedLinearComplementarityProblem* mlcp, MLCP_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    SO->solverId=id;
    mixedLinearComplementarity_setDefaultSolverOptions(mlcp, SO);
    return SO;
  }

  _SolverOptions(MixedComplementarityProblem* mlcp, MCP_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    SO->solverId=id;
    mixedComplementarity_setDefaultSolverOptions(mlcp, SO);
    return SO;
  }

  _SolverOptions(MixedComplementarityProblem2* mcp, MCP_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    set_SolverOptions(SO, id);
    return SO;
  }

  _SolverOptions(VariationalInequality* vi, VI_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    set_SolverOptions(SO, id);
    return SO;
  }

  _SolverOptions(AffineVariationalInequalities* vi, AVI_SOLVER id)
  {
    SolverOptions *SO;
    SO = (SolverOptions *) malloc(sizeof(SolverOptions));
    SO->solverId=id;
    set_SolverOptions(SO, id);
    return SO;
  }
  // SolverOptions(FRICTION_SOLVER id)
  // {
  //   return BOOST_PP_CAT(FE_SWIG_INTERNAL_MEMBER,SolverOptions_makeSolverOptions)(NULL, NULL, id);
  // }

  // SolverOptions(FrictionContactProblem* fcp, FRICTION_SOLVER id)
  // {
  //   return BOOST_PP_CAT(FE_SWIG_INTERNAL_MEMBER,SolverOptions_makeSolverOptions)(NULL, fcp, id);
  // }

  ~_SolverOptions()
    {
      deleteSolverOptions($self);
      free($self);
    }

};

%extend LinearComplementarityProblem
{
  LinearComplementarityProblem(PyObject *o1, PyObject *o2)
    {

      int is_new_object1=0;
      int is_new_object2=0;
      PyArrayObject* array = obj_to_array_fortran_allow_conversion(o1, NPY_DOUBLE,&is_new_object1);
      assert(array);
      PyArrayObject* vector = obj_to_array_contiguous_allow_conversion(o2, NPY_DOUBLE, &is_new_object2);
      assert(vector);
      LinearComplementarityProblem *LC;
      // return pointer : free by std swig destructor
      LC = (LinearComplementarityProblem *) malloc(sizeof(LinearComplementarityProblem));
      size_t size0 = array_size(array,0);
      size_t size1 = array_size(array,1);
      LC->M = createNumericsMatrix(NM_DENSE, size0, size1);

      memcpy(LC->M->matrix0,array_data(array),size0*size1*sizeof(double));
      LC->size = size0;
      LC->q = (double *) malloc(size0*sizeof(double));
      memcpy(LC->q,array_data(vector),size0*sizeof(double));

      // python mem management
      if(is_new_object1 && array)
      {
        Py_DECREF(array);
      }

      if(is_new_object2 && vector)
      {
        Py_DECREF(vector);
       }

      return LC;

    }


  ~LinearComplementarityProblem()
  {
    freeLinearComplementarityProblem($self);
  }

};


%exception MixedLinearComplementarityProblem {
    $action
    if (PyErr_Occurred()) SWIG_fail;
}

%extend MixedLinearComplementarityProblem
{
  MixedLinearComplementarityProblem()
   {
     MixedLinearComplementarityProblem* MLCP;
     MLCP =  (MixedLinearComplementarityProblem *) malloc(sizeof(MixedLinearComplementarityProblem));
     return MLCP;
   }

  MixedLinearComplementarityProblem(PyObject *dim, PyObject *o1, PyObject *o2)
    {

      int is_new_object1=0;
      int is_new_object2=0;

      PyArrayObject* array = obj_to_array_fortran_allow_conversion(o1, NPY_DOUBLE,&is_new_object1);
      PyArrayObject* vector = obj_to_array_contiguous_allow_conversion(o2, NPY_DOUBLE, &is_new_object2);

      if ( array_size(array,0) !=  array_size(array,1))
      {
        PyErr_Format(PyExc_ValueError,
                     "A non square matrix (%ld,%ld) has been given",
                     array_size(array,0), array_size(array,1));
      }


      MixedLinearComplementarityProblem *MLCP;
      // return pointer : free by std swig destructor
      MLCP = (MixedLinearComplementarityProblem *) malloc(sizeof(MixedLinearComplementarityProblem));

      MLCP->M = createNumericsMatrix(NM_DENSE, array_size(array,0), array_size(array,1));

      memcpy(MLCP->M->matrix0,array_data(array),MLCP->M->size0*MLCP->M->size1*sizeof(double));

      MLCP->n = (int) PyInt_AsLong(dim);
      MLCP->m = MLCP->M->size0 - MLCP->n;
      MLCP->blocksRows = (int *) malloc(3*sizeof(int));
      MLCP->blocksIsComp = (int *) malloc(2*sizeof(int));


      MLCP->blocksRows[0]=0;
      MLCP->blocksRows[1]=MLCP->n;
      MLCP->blocksRows[2]=MLCP->n+MLCP->m;
      MLCP->blocksIsComp[0]=0;
      MLCP->blocksIsComp[1]=1;


      MLCP->isStorageType1 = 1;
      MLCP->isStorageType2 = 0;
      MLCP->A = NULL;
      MLCP->B = NULL;
      MLCP->C = NULL;
      MLCP->D = NULL;
      MLCP->a = NULL;
      MLCP->b = NULL;

      if ( array_size(array,0) !=  array_size(vector,0))
      {
        //printf("size of q = %i\n",  array_size(vector,0));
        //printf("size of M = %i\n",  array_size(array,0));

        PyErr_Format(PyExc_ValueError,
                     "Matrix and vector of incompatible lengths (%ld != %ld) ",
                     array_size(array,0), array_size(vector,0) );
      }
      MLCP->q = (double *) malloc(MLCP->M->size0*sizeof(double));
      memcpy(MLCP->q,array_data(vector),MLCP->M->size0*sizeof(double));

      // python mem management
      if(is_new_object1 && array)
      {
        Py_DECREF(array);
      }

      if(is_new_object2 && vector)
      {
        Py_DECREF(vector);
      }

      return MLCP;

    }



  ~MixedLinearComplementarityProblem()
  {
    freeMixedLinearComplementarityProblem($self);
  }

  // MixedLinearComplementarityProblem * newFromFilename(PyObject * o1)
  // {
  //   int result;
  //   MixedLinearComplementarityProblem *MLCP;
  //   // return pointer : free by std swig destructor
  //   MLCP = (MixedLinearComplementarityProblem *) malloc(sizeof(MixedLinearComplementarityProblem));

  //   char *arg1 = (char *) 0 ;
  //   int res1 ;
  //   char *buf1 = 0 ;
  //   int alloc1 = 0 ;

  //   res1 = SWIG_AsCharPtrAndSize(o1, &buf1, NULL, &alloc1);
  //   // if (!SWIG_IsOK(res1)) {
  //   //   SWIG_exception_fail(SWIG_ArgError(res1), "in method '" "MixedLinearComplementarity_newFromFilename" "', argument " "1"" of type '" "char *""'");
  //   // }
  //   arg1 = reinterpret_cast< char * >(buf1);
  //   {
  //     try
  //     {
  //       result = (int)mixedLinearComplementarity_newFromFilename(MLCP,arg1);
  //     }
  //     catch (const std::invalid_argument& e)
  //     {
  //       // SWIG_exception(SWIG_ValueError, e.what());
  //     }
  //   }

  //   return MLCP;

  // }

};


%extend MixedComplementarityProblem
{



  MixedComplementarityProblem()
   {
     MixedComplementarityProblem* MCP;
     MCP =  (MixedComplementarityProblem *) malloc(sizeof(MixedComplementarityProblem));
     MCP->Fmcp=NULL;
     MCP->nablaFmcp=NULL;
     MCP->computeFmcp=NULL;
     MCP->computeNablaFmcp=NULL;
     return MCP;
   }

  void set_computeFmcp(PyObject *o)
  {
    set_my_callback_Fmcp(o);
    $self->computeFmcp = (my_call_to_callback_Fmcp);
  }

  void set_computeNablaFmcp(PyObject *o)
  {

    set_my_callback_NablaFmcp(o);
    $self->computeNablaFmcp = (my_call_to_callback_NablaFmcp);
  }

  void test_call_to_callback()
  {
    printf("I am in test_call_to_callback()\n");

    int size =   $self->sizeEqualities +  $self->sizeInequalities;

    double * z = (double *)malloc(size*sizeof(double));
    double * F = (double *)malloc(size*sizeof(double));
    double * nablaF = (double *)malloc(size*size*sizeof(double));

    for (int i=0; i < size; i++) z[i]=i;
    printf("Input \n");
    for (int i=0; i < size; i++) printf("z[%i] = %lf\t", i, z[i]);
    printf("\n");
    $self->computeFmcp(size,z,F);
    if  (!PyErr_Occurred())
    {
      $self->computeNablaFmcp(size,z,nablaF);
    }
    printf("Output \n");
    for (int i=0; i < size; i++) printf("F[%i] =  %lf\t", i, F[i]);
    printf("\n");
    for (int i=0; i < size*size; i++) printf("nablaF[%i] =  %lf\t", i, nablaF[i]);

    printf("\n");
    free(z);
    free(F);
    free(nablaF);



    printf("I leave test_call_to_callback()\n");
  }



  MixedComplementarityProblem(PyObject *sizeEq, PyObject *sizeIneq, PyObject *o1, PyObject *o2)
  {
     MixedComplementarityProblem* MCP;
     MCP =  (MixedComplementarityProblem *) malloc(sizeof(MixedComplementarityProblem));

     MCP->sizeEqualities = (int) PyInt_AsLong(sizeEq);
     MCP->sizeInequalities = (int) PyInt_AsLong(sizeIneq);
     int size =  MCP->sizeEqualities +  MCP->sizeInequalities;

     if (size<1)
     {
       PyErr_SetString(PyExc_RuntimeError, "sizeEqualities + sizeInequalities has to be positive");
       MCP->Fmcp = NULL;
       MCP->nablaFmcp = NULL;
       freeMixedComplementarityProblem(MCP);
       return NULL;
     }
     else
     {
       MCP->Fmcp = (double *) malloc(size*sizeof(double));
       MCP->nablaFmcp = (double *) malloc(size*size*sizeof(double));
     }

     if (PyCallable_Check(o1))
     {
       set_my_callback_Fmcp(o1);
       MCP->computeFmcp = (my_call_to_callback_Fmcp);
     }
     else
     {
       PyErr_SetString(PyExc_TypeError, "argument 3 must be callable");
       free(MCP->Fmcp);
       free(MCP->nablaFmcp);
       MCP->Fmcp = NULL;
       MCP->nablaFmcp = NULL;
       freeMixedComplementarityProblem(MCP);
       return NULL;
     }


     if (PyCallable_Check(o2))
     {
       set_my_callback_NablaFmcp(o2);
       MCP->computeNablaFmcp = (my_call_to_callback_NablaFmcp);
     }
     else
     {
       PyErr_SetString(PyExc_TypeError, "argument 4 must be callable");
       free(MCP->Fmcp);
       free(MCP->nablaFmcp);
       MCP->Fmcp = NULL;
       MCP->nablaFmcp = NULL;
       freeMixedComplementarityProblem(MCP);
       return NULL;
     }

     return MCP;
   }

  ~MixedComplementarityProblem()
  {
    free($self->Fmcp);
    free($self->nablaFmcp);
    $self->Fmcp = NULL;
    $self->nablaFmcp = NULL;
    freeMixedComplementarityProblem($self);
  }
};


%include Numerics_MCP2.i
%include Numerics_VI.i

%typemap(out) (double* q) {
  npy_intp dims[2];

  dims[0] = arg1->dimension * arg1->numberOfContacts;
  dims[1] = 1;
  if ($1)
  {
    PyObject *obj = PyArray_SimpleNewFromData(2, dims, NPY_DOUBLE, $1);
    PyArrayObject *array = (PyArrayObject*) obj;
    if (!array || !require_fortran(array)) SWIG_fail;
    $result = obj;
  }
  else
    SWIG_fail;
 }

// for b in GlobalFrictionContact
%apply (double* q) { (double* b) };

%typemap(out) (double* mu) {
  npy_intp dims[2];

  dims[0] = arg1->numberOfContacts;
  dims[1] = 1;
  if ($1)
  {
    PyObject *obj = PyArray_SimpleNewFromData(2, dims, NPY_DOUBLE, $1);
    PyArrayObject *array = (PyArrayObject*) obj;
    if (!array || !require_fortran(array)) SWIG_fail;
    $result = obj;
  }
  else
    SWIG_fail;
 }





// FrictionContact
%ignore LocalNonsmoothNewtonSolver; //signature problem (should be SolverOption
                          //instead of *iparam, *dparam).
%ignore DampedLocalNonsmoothNewtonSolver; // signature problem idem.

%ignore frictionContactProblem_new; // signature issue with mu param

%include FrictionContactProblem.i

%include "typemaps.i"

%apply double *OUTPUT { double *error };
%apply double *OUTPUT { double *result };

// Callback (see SolverOptions.i) needed here
%typemap(in, numinputs=0) (FischerBurmeisterFun3x3Ptr computeACFun3x3) () {
  // Callback (see SolverOptions.i) needed here
  $1 = &frictionContact3D_FischerBurmeisterFunctionGenerated;
 }

%typemap(in, numinputs=0) (AlartCurnierFun3x3Ptr computeACFun3x3) () {
  // Callback (see SolverOptions.i) needed here
  $1 = &frictionContact3D_AlartCurnierFunctionGenerated;
 }

%include "FrictionContact3D_Solvers.h"
%include "FrictionContact3D_unitary_enumerative.h"
%include "FrictionContact2D_Solvers.h"
%include "Friction_cst.h"
%include "FrictionContact3D_AlartCurnier.h"
%include "FrictionContact3D_localAlartCurnier.h"
%include "FrictionContact3D_localFischerBurmeister.h"
%include "AlartCurnierGenerated.h"
%include "FischerBurmeisterGenerated.h"
%include "FrictionContact3D_compute_error.h"
%include "fclib_interface.h"




%extend FrictionContactProblem
{
  FrictionContactProblem()
  {
    FrictionContactProblem * FCP = (FrictionContactProblem *) malloc(sizeof(FrictionContactProblem));
    FCP->M = NULL;
    FCP->q = NULL;
    FCP->mu = NULL;

    return FCP;
  }


  /* copy constructor */
  FrictionContactProblem(PyObject *o)
  {
    FrictionContactProblem* fcp;
    FrictionContactProblem* FCP;

    %SN_INPUT_CHECK_RETURN(o, fcp, FrictionContactProblem);

    FCP = (FrictionContactProblem *) malloc(sizeof(FrictionContactProblem));
    FCP->dimension = fcp->dimension;
    FCP->M = fcp->M;
    FCP->numberOfContacts = fcp->numberOfContacts;
    FCP->q =  fcp->q;
    FCP->mu = fcp->mu;

    Py_INCREF(o);

    return FCP;
  }

  /* */
  FrictionContactProblem(PyObject *dim, PyObject *numberOfContacts, PyObject *M, PyObject *q, PyObject *mu)
  {
    FrictionContactProblem * FC = (FrictionContactProblem *) malloc(sizeof(FrictionContactProblem));
    FC->dimension = PyInt_AsLong(dim);
    FC->numberOfContacts = PyInt_AsLong(numberOfContacts);

    %SN_INPUT_CHECK_RETURN(q, FC->M, NumericsMatrix);
    %SN_INPUT_CHECK_RETURN(q, FC->q, double);
    %SN_INPUT_CHECK_RETURN(mu, FC->mu, double);

    return FC;

  }

  NumericsMatrix* rawM()
  {
    return $self->M;
  }

  FrictionContactProblem(PyObject *dim, PyObject *o1, PyObject *o2, PyObject *o3)
    {

      int is_new_object1=0;
      int is_new_object2=0;
      int is_new_object3=0;

      PyArrayObject* array = obj_to_array_fortran_allow_conversion(o1, NPY_DOUBLE,&is_new_object1);
      PyArrayObject* vector = obj_to_array_contiguous_allow_conversion(o2, NPY_DOUBLE, &is_new_object2);
      PyArrayObject* mu_vector = obj_to_array_contiguous_allow_conversion(o3, NPY_DOUBLE, &is_new_object3);
      FrictionContactProblem *FC;
      // return pointer : free by std swig destructor
      FC = (FrictionContactProblem *) malloc(sizeof(FrictionContactProblem));
      size_t size0 = array_size(array,0);
      size_t size1 = array_size(array,1);

      FC->M = createNumericsMatrix(NM_DENSE, size0, size1);

      memcpy(FC->M->matrix0,array_data(array),size0*size1*sizeof(double));
      FC->dimension = (int) PyInt_AsLong(dim);
      FC->numberOfContacts = size0 / FC->dimension;
      FC->q = (double *) malloc(size0*sizeof(double));
      memcpy(FC->q,array_data(vector),size0*sizeof(double));
      FC->mu = (double *) malloc(FC->numberOfContacts*sizeof(double));
      memcpy(FC->mu,array_data(mu_vector),FC->numberOfContacts*sizeof(double));


      // python mem management
      if(is_new_object1 && array)
      {
        Py_DECREF(array);
      }

      if(is_new_object2 && vector)
      {
        Py_DECREF(vector);
      }

      if(is_new_object3 && mu_vector)
      {
        Py_DECREF(mu_vector);
      }

      return FC;
    }

  ~FrictionContactProblem()
  {
    freeFrictionContactProblem($self);
  }

};


%extend GlobalFrictionContactProblem
{

  GlobalFrictionContactProblem()
    {

      GlobalFrictionContactProblem *FC;
      // return pointer : free by std swig destructor
      FC = (GlobalFrictionContactProblem *) malloc(sizeof(GlobalFrictionContactProblem));

      FC->M = NULL;
      FC->H = NULL;
      FC->q = NULL;
      FC->b = NULL;
      FC->mu = NULL;
      FC->env = NULL;
      FC->numberOfContacts = 0;
      FC->dimension = 0;

      return FC;
    }

  GlobalFrictionContactProblem(PyObject *dim)
    {

      GlobalFrictionContactProblem *FC;
      // return pointer : free by std swig destructor
      FC = (GlobalFrictionContactProblem *) malloc(sizeof(GlobalFrictionContactProblem));

      FC->M = NULL;
      FC->H = NULL;
      FC->q = NULL;
      FC->b = NULL;
      FC->mu = NULL;
      FC->env = NULL;
      FC->numberOfContacts = 0;
      FC->dimension = (int) PyInt_AsLong(dim);

      return FC;
    }

  GlobalFrictionContactProblem(PyObject *dim, PyObject *o1, PyObject *o2, PyObject *o3)
    {

      int is_new_object1=0;
      int is_new_object2=0;
      int is_new_object3=0;

      PyArrayObject* array = obj_to_array_fortran_allow_conversion(o1, NPY_DOUBLE,&is_new_object1);
      PyArrayObject* vector = obj_to_array_contiguous_allow_conversion(o2, NPY_DOUBLE, &is_new_object2);
      PyArrayObject* mu_vector = obj_to_array_contiguous_allow_conversion(o3, NPY_DOUBLE, &is_new_object3);
      GlobalFrictionContactProblem *FC;
      // return pointer : free by std swig destructor
      FC = (GlobalFrictionContactProblem *) malloc(sizeof(GlobalFrictionContactProblem));
      size_t size0 = array_size(array,0);
      size_t size1 = array_size(array,1);
      FC->M = createNumericsMatrix(NM_DENSE, size0, size1);

      memcpy(FC->M->matrix0,array_data(array),size0*size1*sizeof(double));
      FC->dimension = (int) PyInt_AsLong(dim);
      FC->numberOfContacts = size0 / FC->dimension;
      FC->q = (double *) malloc(size0*sizeof(double));
      memcpy(FC->q,array_data(vector),size0*sizeof(double));
      FC->mu = (double *) malloc(FC->numberOfContacts*sizeof(double));
      memcpy(FC->mu,array_data(mu_vector),FC->numberOfContacts*sizeof(double));


      // python mem management
      if(is_new_object1 && array)
      {
        Py_DECREF(array);
      }

      if(is_new_object2 && vector)
      {
        Py_DECREF(vector);
      }

      if(is_new_object3 && mu_vector)
      {
        Py_DECREF(mu_vector);
      }

      return FC;
    }

  ~GlobalFrictionContactProblem()
  {
    freeGlobalFrictionContactProblem($self);
  }

};

//GenericMechanical
//%include GMPReduced.h
//%include GenericMechanicalProblem.h
//%include GenericMechanical_Solvers.h
%{
#include <GenericMechanical_cst.h>
%}
%include GenericMechanical_cst.h


 // Matrices
%include "SparseMatrix.h"
%include "SparseBlockMatrix.h"
%include "NumericsMatrix.h"

// some extensions but numpy arrays should be used instead
%extend NumericsMatrix
{

  NumericsMatrix(int nrows, int ncols, double *data)
    {
      NumericsMatrix *M;

      // return pointer : free by std swig destructor
      M = newNumericsMatrix();
      M->storageType = 0;
      M->size0 = nrows;
      M->size1 = ncols;
      M->matrix0 = data;
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

  PyObject * __str__()
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
    %#if PY_MAJOR_VERSION < 3
    return PyString_FromString(result.str().c_str());
    %#else
    return PyUnicode_FromString(result.str().c_str());
    %#endif
  }

};


#define GET_ATTR(OBJ,ATTR)                                              \
  ATTR = PyObject_GetAttrString(OBJ,BOOST_PP_STRINGIZE(ATTR));          \
  if(!ATTR)                                                             \
  {                                                                     \
    throw(std::invalid_argument(BOOST_PP_STRINGIZE(need a ATTR attr))); \
  }

/* I'm not sure it's a good idea to duplicate thise here ... it is already defined in csparse.h */
typedef struct cs_sparse    /* matrix in compressed-column or triplet form */
{
  csi nzmax ;	    /* maximum number of entries */
  csi m ;	    /* number of rows */
  csi n ;	    /* number of columns */
  csi *p ;	    /* column pointers (size n+1) or col indices (size nzmax) */
  csi *i ;	    /* row indices, size nzmax */
  double *x ;	    /* numerical values, size nzmax */
  csi nz ;	    /* # of entries in triplet matrix, -1 for compressed-col */
} cs ;

%extend cs_sparse
{
  %fragment("NumPy_Fragments");

  // from a scipy.sparse csr
  cs_sparse(PyObject *obj)
  {

    PyObject *shape = NULL;
    PyObject *nnz = NULL;
    PyObject *data = NULL;
    PyObject *indices = NULL;
    PyObject *indptr = NULL;
    int is_new_object1=0;
    int is_new_object2=0;
    int is_new_object3=0;
    PyArrayObject *array_data, *array_indices, *array_indptr;
    cs_sparse* M;

    try
    {

      M = (cs_sparse *) malloc(sizeof(cs_sparse));

      GET_ATTR(obj,shape);
      GET_ATTR(obj,nnz);
      GET_ATTR(obj,data);
      GET_ATTR(obj,indices);
      GET_ATTR(obj,indptr);

      int dim0, dim1, nzmax;

      GET_INTS(shape,0,dim0);
      GET_INTS(shape,1,dim1);
//      GET_INT(nnz,nzmax); fail: type is numpy.int32!
      nzmax = PyInt_AsLong(nnz);

      // the return NULL is a hack, we should raise an exception, but I'm lazy --xhub
      array_data = obj_to_array_allow_conversion(data, NPY_DOUBLE, &is_new_object1);
      if (!array_data) { return NULL; }
      array_indices = obj_to_array_allow_conversion(indices, NPY_INT32, &is_new_object2);
      if (!array_indices) { return NULL; }
      array_indptr = obj_to_array_allow_conversion(indptr, NPY_INT32, &is_new_object3);
      if (!array_indptr) { return NULL; }

      M->m = dim0;
      M->n = dim1;

      M->nzmax = nzmax;

      M->nz = -2; // csr only for the moment

      M->p = (csi *) malloc((M->m+1) * sizeof(csi));
      M->i = (csi *) malloc(M->nzmax * sizeof(csi));
      M->x = (double *) malloc(M->nzmax * sizeof(double));

      for(unsigned int i = 0; i < (M->m+1); i++)
      {
        M->p[i] = ((int *) array_data(array_indptr)) [i];
      }

      for(unsigned int i = 0; i< M->nzmax; i++)
      {
        M->i[i] = ((int *) array_data(array_indices)) [i];
      }

      memcpy(M->x, (double *) array_data(array_data), M->nzmax * sizeof(double));

      Py_DECREF(shape);
      Py_DECREF(nnz);
      Py_DECREF(data);
      Py_DECREF(indices);
      Py_DECREF(indptr);

      if (array_data && is_new_object1)
      {
        Py_DECREF(array_data);
      }

      if (array_indptr && is_new_object2)
      {
        Py_DECREF(array_indptr);
      }

      if (array_indices && is_new_object3)
      {
        Py_DECREF(array_indices);
      }


      return M;
    }
    catch (const std::invalid_argument& e)
    {
      Py_XDECREF(shape);
      Py_XDECREF(nnz);
      Py_XDECREF(data);
      Py_XDECREF(indices);
      Py_XDECREF(indptr);

      if (array_data && is_new_object1)
      {
        Py_DECREF(array_data);
      }

      if (array_indptr && is_new_object2)
      {
        Py_DECREF(array_indptr);
      }

      if (array_indices && is_new_object3)
      {
        Py_DECREF(array_indices);
      }

      assert(!M->p);
      assert(!M->i);
      assert(!M->x);
      assert(M);
      free(M);
      throw(e);
    }
  }

  ~cs_sparse()
  {
    cs_spfree($self);
  }
}


#ifdef WITH_IO
%include picklable.i

%make_picklable(Callback, Numerics);
%make_picklable(_SolverOptions, Numerics);
%make_picklable(FrictionContactProblem, Numerics);
%make_picklable(NumericsMatrix, Numerics);
%make_picklable(SparseBlockStructuredMatrix, Numerics);
#endif
