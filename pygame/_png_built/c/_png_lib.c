
#include <Python.h>
#include <stddef.h>

#if defined(MS_WIN32) && !defined(_STDINT_H)
#include <malloc.h>   /* for alloca() */
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#if !defined(_MSC_VER) || _MSC_VER <= 1700
typedef unsigned char _Bool;
#endif
#else
#if (defined (__SVR4) && defined (__sun)) || defined(_AIX)
#  include <alloca.h>
#endif
#endif

#if PY_MAJOR_VERSION < 3
# undef PyCapsule_CheckExact
# undef PyCapsule_GetPointer
# define PyCapsule_CheckExact(capsule) (PyCObject_Check(capsule))
# define PyCapsule_GetPointer(capsule, name) \
    (PyCObject_AsVoidPtr(capsule))
#endif

#if PY_MAJOR_VERSION >= 3
# define PyInt_FromLong PyLong_FromLong
#endif

#define _cffi_from_c_double PyFloat_FromDouble
#define _cffi_from_c_float PyFloat_FromDouble
#define _cffi_from_c_long PyInt_FromLong
#define _cffi_from_c_ulong PyLong_FromUnsignedLong
#define _cffi_from_c_longlong PyLong_FromLongLong
#define _cffi_from_c_ulonglong PyLong_FromUnsignedLongLong

#define _cffi_to_c_double PyFloat_AsDouble
#define _cffi_to_c_float PyFloat_AsDouble

#define _cffi_from_c_int_const(x)                                        \
    (((x) > 0) ?                                                         \
        ((unsigned long long)(x) <= (unsigned long long)LONG_MAX) ?      \
            PyInt_FromLong((long)(x)) :                                  \
            PyLong_FromUnsignedLongLong((unsigned long long)(x)) :       \
        ((long long)(x) >= (long long)LONG_MIN) ?                        \
            PyInt_FromLong((long)(x)) :                                  \
            PyLong_FromLongLong((long long)(x)))

#define _cffi_from_c_int(x, type)                                        \
    (((type)-1) > 0 ?   /* unsigned */                                   \
        (sizeof(type) < sizeof(long) ? PyInt_FromLong(x) :               \
         sizeof(type) == sizeof(long) ? PyLong_FromUnsignedLong(x) :     \
                                        PyLong_FromUnsignedLongLong(x))  \
      : (sizeof(type) <= sizeof(long) ? PyInt_FromLong(x) :              \
                                        PyLong_FromLongLong(x)))

#define _cffi_to_c_int(o, type)                                          \
    (sizeof(type) == 1 ? (((type)-1) > 0 ? (type)_cffi_to_c_u8(o)        \
                                         : (type)_cffi_to_c_i8(o)) :     \
     sizeof(type) == 2 ? (((type)-1) > 0 ? (type)_cffi_to_c_u16(o)       \
                                         : (type)_cffi_to_c_i16(o)) :    \
     sizeof(type) == 4 ? (((type)-1) > 0 ? (type)_cffi_to_c_u32(o)       \
                                         : (type)_cffi_to_c_i32(o)) :    \
     sizeof(type) == 8 ? (((type)-1) > 0 ? (type)_cffi_to_c_u64(o)       \
                                         : (type)_cffi_to_c_i64(o)) :    \
     (Py_FatalError("unsupported size for type " #type), 0))

#define _cffi_to_c_i8                                                    \
                 ((int(*)(PyObject *))_cffi_exports[1])
#define _cffi_to_c_u8                                                    \
                 ((int(*)(PyObject *))_cffi_exports[2])
#define _cffi_to_c_i16                                                   \
                 ((int(*)(PyObject *))_cffi_exports[3])
#define _cffi_to_c_u16                                                   \
                 ((int(*)(PyObject *))_cffi_exports[4])
#define _cffi_to_c_i32                                                   \
                 ((int(*)(PyObject *))_cffi_exports[5])
#define _cffi_to_c_u32                                                   \
                 ((unsigned int(*)(PyObject *))_cffi_exports[6])
#define _cffi_to_c_i64                                                   \
                 ((long long(*)(PyObject *))_cffi_exports[7])
#define _cffi_to_c_u64                                                   \
                 ((unsigned long long(*)(PyObject *))_cffi_exports[8])
#define _cffi_to_c_char                                                  \
                 ((int(*)(PyObject *))_cffi_exports[9])
#define _cffi_from_c_pointer                                             \
    ((PyObject *(*)(char *, CTypeDescrObject *))_cffi_exports[10])
#define _cffi_to_c_pointer                                               \
    ((char *(*)(PyObject *, CTypeDescrObject *))_cffi_exports[11])
#define _cffi_get_struct_layout                                          \
    ((PyObject *(*)(Py_ssize_t[]))_cffi_exports[12])
#define _cffi_restore_errno                                              \
    ((void(*)(void))_cffi_exports[13])
#define _cffi_save_errno                                                 \
    ((void(*)(void))_cffi_exports[14])
#define _cffi_from_c_char                                                \
    ((PyObject *(*)(char))_cffi_exports[15])
#define _cffi_from_c_deref                                               \
    ((PyObject *(*)(char *, CTypeDescrObject *))_cffi_exports[16])
#define _cffi_to_c                                                       \
    ((int(*)(char *, CTypeDescrObject *, PyObject *))_cffi_exports[17])
#define _cffi_from_c_struct                                              \
    ((PyObject *(*)(char *, CTypeDescrObject *))_cffi_exports[18])
#define _cffi_to_c_wchar_t                                               \
    ((wchar_t(*)(PyObject *))_cffi_exports[19])
#define _cffi_from_c_wchar_t                                             \
    ((PyObject *(*)(wchar_t))_cffi_exports[20])
#define _cffi_to_c_long_double                                           \
    ((long double(*)(PyObject *))_cffi_exports[21])
#define _cffi_to_c__Bool                                                 \
    ((_Bool(*)(PyObject *))_cffi_exports[22])
#define _cffi_prepare_pointer_call_argument                              \
    ((Py_ssize_t(*)(CTypeDescrObject *, PyObject *, char **))_cffi_exports[23])
#define _cffi_convert_array_from_object                                  \
    ((int(*)(char *, CTypeDescrObject *, PyObject *))_cffi_exports[24])
#define _CFFI_NUM_EXPORTS 25

typedef struct _ctypedescr CTypeDescrObject;

static void *_cffi_exports[_CFFI_NUM_EXPORTS];
static PyObject *_cffi_types, *_cffi_VerificationError;

static int _cffi_setup_custom(PyObject *lib);   /* forward */

static PyObject *_cffi_setup(PyObject *self, PyObject *args)
{
    PyObject *library;
    int was_alive = (_cffi_types != NULL);
    if (!PyArg_ParseTuple(args, "OOO", &_cffi_types, &_cffi_VerificationError,
                                       &library))
        return NULL;
    Py_INCREF(_cffi_types);
    Py_INCREF(_cffi_VerificationError);
    if (_cffi_setup_custom(library) < 0)
        return NULL;
    return PyBool_FromLong(was_alive);
}

static int _cffi_init(void)
{
    PyObject *module, *c_api_object = NULL;

    module = PyImport_ImportModule("_cffi_backend");
    if (module == NULL)
        goto failure;

    c_api_object = PyObject_GetAttrString(module, "_C_API");
    if (c_api_object == NULL)
        goto failure;
    if (!PyCapsule_CheckExact(c_api_object)) {
        PyErr_SetNone(PyExc_ImportError);
        goto failure;
    }
    memcpy(_cffi_exports, PyCapsule_GetPointer(c_api_object, "cffi"),
           _CFFI_NUM_EXPORTS * sizeof(void *));

    Py_DECREF(module);
    Py_DECREF(c_api_object);
    return 0;

  failure:
    Py_XDECREF(module);
    Py_XDECREF(c_api_object);
    return -1;
}

#define _cffi_type(num) ((CTypeDescrObject *)PyList_GET_ITEM(_cffi_types, num))

/**********/



    #define PNG_SKIP_SETJMP_CHECK 1
    #include <png.h>

    static int write_png(const char *file_name,
                         png_bytep *rows,
                         int w,
                         int h,
                         int colortype,
                         int bitdepth,
                         char** error)
    {
        png_structp png_ptr = NULL;
        png_infop info_ptr =  NULL;
        FILE *fp = NULL;
        char *doing = "open for writing";

        if (!(fp = fopen (file_name, "wb")))
            goto fail;

        doing = "create png write struct";
        if (!(png_ptr = png_create_write_struct
              (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)))
            goto fail;

        doing = "create png info struct";
        if (!(info_ptr = png_create_info_struct (png_ptr)))
            goto fail;
        if (setjmp (png_jmpbuf (png_ptr)))
            goto fail;

        doing = "init IO";
        png_init_io (png_ptr, fp);

        doing = "write header";
        png_set_IHDR (png_ptr, info_ptr, w, h, bitdepth, colortype,
                      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                      PNG_FILTER_TYPE_BASE);

        doing = "write info";
        png_write_info (png_ptr, info_ptr);

        doing = "write image";
        png_write_image (png_ptr, rows);

        doing = "write end";
        png_write_end (png_ptr, NULL);

        doing = "closing file";
        if(0 != fclose (fp))
            goto fail;
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return 0;

    fail:
        /*
         * I don't see how to handle the case where png_ptr
         * was allocated but info_ptr was not. However, those
         * calls should only fail if memory is out and you are
         * probably screwed regardless then. The resulting memory
         * leak is the least of your concerns.
         */
        if( png_ptr && info_ptr ) {
            png_destroy_write_struct(&png_ptr, &info_ptr);
        }
        *error = malloc(50);
        sprintf(*error, "SavePNG: could not %s", doing);
        return -1;
    }
    

static PyObject *
_cffi_f_write_png(PyObject *self, PyObject *args)
{
  char const * x0;
  unsigned char * * x1;
  int x2;
  int x3;
  int x4;
  int x5;
  char * * x6;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;

  if (!PyArg_ParseTuple(args, "OOOOOOO:write_png", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(1), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_int(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_int(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  x5 = _cffi_to_c_int(arg5, int);
  if (x5 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg6, (char **)&x6);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x6 = alloca(datasize);
    memset((void *)x6, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x6, _cffi_type(2), arg6) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = write_png(x0, x1, x2, x3, x4, x5, x6); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_int(result, int);
}

static int _cffi_const_PNG_COLOR_TYPE_RGB(PyObject *lib)
{
  PyObject *o;
  int res;
  o = _cffi_from_c_int_const(PNG_COLOR_TYPE_RGB);
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PNG_COLOR_TYPE_RGB", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return 0;
}

static int _cffi_const_PNG_COLOR_TYPE_RGB_ALPHA(PyObject *lib)
{
  PyObject *o;
  int res;
  o = _cffi_from_c_int_const(PNG_COLOR_TYPE_RGB_ALPHA);
  if (o == NULL)
    return -1;
  res = PyObject_SetAttrString(lib, "PNG_COLOR_TYPE_RGB_ALPHA", o);
  Py_DECREF(o);
  if (res < 0)
    return -1;
  return _cffi_const_PNG_COLOR_TYPE_RGB(lib);
}

static int _cffi_setup_custom(PyObject *lib)
{
  return _cffi_const_PNG_COLOR_TYPE_RGB_ALPHA(lib);
}

static PyMethodDef _cffi_methods[] = {
  {"write_png", _cffi_f_write_png, METH_VARARGS, NULL},
  {"_cffi_setup", _cffi_setup, METH_VARARGS, NULL},
  {NULL, NULL, 0, NULL}    /* Sentinel */
};

#if PY_MAJOR_VERSION >= 3

static struct PyModuleDef _cffi_module_def = {
  PyModuleDef_HEAD_INIT,
  "_png_lib",
  NULL,
  -1,
  _cffi_methods,
  NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit__png_lib(void)
{
  PyObject *lib;
  lib = PyModule_Create(&_cffi_module_def);
  if (lib == NULL)
    return NULL;
  if (0 < 0 || _cffi_init() < 0) {
    Py_DECREF(lib);
    return NULL;
  }
  return lib;
}

#else

PyMODINIT_FUNC
init_png_lib(void)
{
  PyObject *lib;
  lib = Py_InitModule("_png_lib", _cffi_methods);
  if (lib == NULL)
    return;
  if (0 < 0 || _cffi_init() < 0)
    return;
  return;
}

#endif