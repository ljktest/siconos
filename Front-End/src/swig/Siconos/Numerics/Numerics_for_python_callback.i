
%{

#include <open_lib.h>

static PyObject *my_callback_NablaFmcp = NULL;

static PyObject * set_my_callback_NablaFmcp(PyObject *o)
{
  PyObject *result = NULL;
  PyObject *temp;
  if (!PyCallable_Check(o)) {
    PyErr_SetString(PyExc_TypeError, "parameter must be callable");
    return NULL;
  }
  Py_XINCREF(o);         /* Add a reference to new callback */
  Py_XDECREF(my_callback_NablaFmcp);  /* Dispose of previous callback */
  my_callback_NablaFmcp = o;       /* Remember new callback */
  
  /* Boilerplate to return "None" */
  Py_INCREF(Py_None);
  result = Py_None;

  return result;
}

static void  my_call_to_callback_NablaFmcp (int size, double *z, double *nablaF)
{  
//  printf("I am in my_call_to_callback_NablaFmcp (int size, double *z, double *NablaF)\n");

  npy_intp this_matrix_dim[1];
  this_matrix_dim[0]=size;
  
  PyObject* pyarray = FPyArray_SimpleNewFromData(1,this_matrix_dim, NPY_DOUBLE, z);   
  PyObject* tuple = PyTuple_New(1);
  PyTuple_SetItem(tuple, 0, pyarray);  
  PyObject* result; 

  if (PyCallable_Check(my_callback_NablaFmcp))
  {
    result = PyObject_CallObject(my_callback_NablaFmcp, tuple);
  }
  else
  {
    PyErr_SetString(PyExc_TypeError, "parameter must be callable");
  }

   // PyTuple_SetItem steals a reference to the object
   // I'm leaving this commented so that people don't do the mistake twice -- xhub
   //Py_DECREF(pyarray);
   Py_DECREF(tuple);

//Comment :  it will be safier to use  obj_to_array_fortran_allow_conversion

  if (is_array(result))
  {
    if (array_size(result,0) != size || array_size(result,1) != size )
    {
      char message[240];
      sprintf(message, "Wrong size for  the return value of callback function. Expected size is %i x %i", size,size);
      PyErr_SetString(PyExc_RuntimeError,message);
    }
    else if (array_numdims(result) != 2)
    {
      char message[240];
      sprintf(message, "Wrong dimension for  the return value of callback function. Expected dimension is 2");
      PyErr_SetString(PyExc_RuntimeError,message);
    }
    else
    { 

      int is_new_object0=0;      
      PyArrayObject* result2 = make_fortran((PyArrayObject *)result, &is_new_object0);
      // if (is_new_object0)
      // {
      //   Py_DECREF(result);
      //   printf ("the object is new !!\n");
      // }
      memcpy(nablaF, (double *)array_data(result2), size*size * sizeof(double));
      
    }
   Py_DECREF(result);
  }
  else
  {      
    const char * desired_type = typecode_string(NPY_DOUBLE);
    const char * actual_type  = pytype_string(result);
    PyErr_Format(PyExc_TypeError,
                 "Array of type '%s' required as return value fo callback function. A '%s' was returned",
                   desired_type, actual_type);
    if (result != NULL) Py_DECREF(result); // things can go really south ...
  }

  return;

}

static PyObject *my_callback_Fmcp = NULL;

static PyObject * set_my_callback_Fmcp(PyObject *o1)
{
  PyObject *result = NULL;
  PyObject *temp;
  if (!PyCallable_Check(o1)) {
    PyErr_SetString(PyExc_TypeError, "parameter must be callable");
    return NULL;
  }
  Py_XINCREF(o1);         /* Add a reference to new callback */
  Py_XDECREF(my_callback_Fmcp);  /* Dispose of previous callback */
  my_callback_Fmcp = o1;       /* Remember new callback */
  
  /* Boilerplate to return "None" */
  Py_INCREF(Py_None);
  result = Py_None;

  return result;
}

static void  my_call_to_callback_Fmcp (int size, double *z, double *F)
{

//  printf("I am in my_call_to_callback_Fmcp (int size, double *z, double *F)\n");

  npy_intp this_matrix_dim[1];
  this_matrix_dim[0]=size;
  
  PyObject* pyarray = FPyArray_SimpleNewFromData(1,this_matrix_dim, NPY_DOUBLE, z);   
  PyObject* tuple = PyTuple_New(1);
  PyTuple_SetItem(tuple, 0, pyarray);  
  PyObject* result; 

  if (PyCallable_Check(my_callback_Fmcp))
  {
    result = PyObject_CallObject(my_callback_Fmcp, tuple);
  }
  else
  {
    PyErr_SetString(PyExc_TypeError, "parameter must be callable");
  }

  // PyTuple_SetItem steals a reference to the object
  // I'm leaving this commented so that people don't do the mistake twice -- xhub
  //Py_DECREF(pyarray);
  Py_DECREF(tuple);

  if (is_array(result))
  {
    if (array_size(result,0) != size)
    {
      char message[240];
      sprintf(message, "Wrong size for  the return value of callback function. Expected size is %i", size);
      PyErr_SetString(PyExc_RuntimeError,message);
    }
    else if (array_numdims(result) != 1)
    {
      char message[240];
      sprintf(message, "Wrong dimension for  the return value of callback function. Expected dimension is 1");
      PyErr_SetString(PyExc_RuntimeError,message);
    }
    else
    { 
      int is_new_object0=0;      
      PyArrayObject* result2 = make_fortran((PyArrayObject *)result, &is_new_object0);
      memcpy(F, (double *)array_data(result2), size * sizeof(double));
    }
    Py_DECREF(result);
  }
  else
  {
    const char * desired_type = typecode_string(NPY_DOUBLE);
    const char * actual_type  = pytype_string(result);
    PyErr_Format(PyExc_TypeError,
                 "Array of type '%s' required as return value fo callback function. A '%s' was returned",
                   desired_type, actual_type);
    if (result != NULL) Py_DECREF(result); // things can go really south ...
  }
  
  return;

}

  static void call_py_compute_Fmcp(void *env, int n1, int n2, double* z, double* Fmcp)
  {
    npy_intp dim[1];
    dim[0] = n1 + n2;

    PyObject* py_z = FPyArray_SimpleNewFromData(1, dim, NPY_DOUBLE, z);
    PyObject* py_Fmcp = FPyArray_SimpleNewFromData(1, dim, NPY_DOUBLE, Fmcp);

    PyObject* py_n1 = PyInt_FromLong(n1);
    PyObject* py_n2 = PyInt_FromLong(n2);

    PY_CALL_METHOD_OR_FUNCTION(env, "compute_Fmcp", env_compute_function, py_n1, py_n2, py_z, py_Fmcp)

    Py_DECREF(py_n1);
    Py_DECREF(py_n2);
    Py_DECREF(py_z);
    Py_DECREF(py_Fmcp);

  };

  static void call_py_compute_nabla_Fmcp(void *env, int n1, int n2, double* z, double* nabla_Fmcp)
  {
    npy_intp dim[1];
    dim[0] = n1 + n2;

    npy_intp dim2[2];
    dim2[0] = n1 + n2;
    dim2[1] = n1 + n2;

    PyObject* py_z = FPyArray_SimpleNewFromData(1, dim, NPY_DOUBLE, z);
    PyObject* py_nabla_Fmcp = FPyArray_SimpleNewFromData(2, dim2, NPY_DOUBLE, nabla_Fmcp);

    PyObject* py_n1 = PyInt_FromLong(n1);
    PyObject* py_n2 = PyInt_FromLong(n2);

    PY_CALL_METHOD_OR_FUNCTION(env, "compute_nabla_Fmcp", env_compute_jacobian, py_n1, py_n2, py_z, py_nabla_Fmcp)
    Py_DECREF(py_n1);
    Py_DECREF(py_n2);
    Py_DECREF(py_z);
    Py_DECREF(py_nabla_Fmcp);

  };

  static void call_py_compute_F(void *env, int n, double* z, double* F)
  {
    npy_intp dim[1];
    dim[0] = n;


    PyObject* py_z = FPyArray_SimpleNewFromData(1, dim, NPY_DOUBLE, z);
    PyObject* py_F = FPyArray_SimpleNewFromData(1, dim, NPY_DOUBLE, F);

    PyObject* py_n = PyInt_FromLong(n);

    PY_CALL_METHOD_OR_FUNCTION(env, "compute_Fmcp", env_compute_function, py_n, py_z, py_F)

    Py_DECREF(py_z);
    Py_DECREF(py_F);
    Py_DECREF(py_n);
  };

  static void call_py_compute_nabla_F(void *env, int n, double* z, double* nabla_F)
  {
    npy_intp dim[1];
    dim[0] = n;

    npy_intp dim2[2];
    dim2[0] = n;
    dim2[1] = n;


    PyObject* py_z = FPyArray_SimpleNewFromData(1, dim, NPY_DOUBLE, z);
    PyObject* py_nabla_F = FPyArray_SimpleNewFromData(2, dim2, NPY_DOUBLE, nabla_F);

    PyObject* py_n = PyInt_FromLong(n);

    PY_CALL_METHOD_OR_FUNCTION(env, "compute_nabla_Fmcp", env_compute_jacobian, py_n, py_z, py_nabla_F)

    Py_DECREF(py_z);
    Py_DECREF(py_nabla_F);
    Py_DECREF(py_n);
  };

  static void* get_c_functions(PyObject* lib_name, PyObject* f1_name, PyObject* f2_name, void** void_ptr1, void** void_ptr2)
  {

    // implementation note: if something related to the library opening fails,
    // the called functions "handle" it and exit.

    char* name_str;
    PyObject* tmp_ascii;
    void* handle_lib;

#if PY_MAJOR_VERSION < 3
    name_str = PyString_AsString(lib_name);
#else
    tmp_ascii = PyUnicode_AsASCIIString(lib_name);
    name_str = PyBytes_AsString(tmp_ascii);
#endif

    handle_lib = open_library(name_str);

#if PY_MAJOR_VERSION < 3
    name_str = PyString_AsString(f1_name);
#else
    Py_DECREF(tmp_ascii);
    tmp_ascii = PyUnicode_AsASCIIString(f1_name);
    name_str = PyBytes_AsString(tmp_ascii);
#endif

    *void_ptr1 = get_function_address(handle_lib, name_str);

#if PY_MAJOR_VERSION < 3
    name_str = PyString_AsString(f2_name);
#else
    Py_DECREF(tmp_ascii);
    tmp_ascii = PyUnicode_AsASCIIString(f2_name);
    name_str = PyBytes_AsString(tmp_ascii);
#endif

    *void_ptr2 = get_function_address(handle_lib, name_str);

    Py_XDECREF(tmp_ascii);

    return handle_lib;
  }

/*  if (ptr == NULL)
  {
    PyErr_SetString("can not find procedure " + procedure);
    PyErr_PrintEx(0);
    exit(err);
  }
*/
%}


