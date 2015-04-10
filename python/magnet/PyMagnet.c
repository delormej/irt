#include <Python.h>
#include "magnet.c"
/*
To build: 
	$ gcc -shared -I/c/Python34/include -I../../firmware/emotion/libraries/math -L/c/Python34/libs ../../firmware/emotion/libraries/math/*.c pyMagnet.c -lpython34 -std=gnu99 -o magnet.pyd
*/

/*
 * Function to be called from Python
 */
static PyObject* PyMagnet_watts(PyObject* self, PyObject* args)
{
	float watts = 0.0f;
	float speed_mps = 0.0f;
	uint16_t position = 0;
	
	// Parse the python arguments.
	PyArg_ParseTuple(args, "fi", &speed_mps, &position);
	
	// Call the actual magnet module function.
	watts = magnet_watts(speed_mps, position);

	// Build python variable for return value.
	return Py_BuildValue("f", watts);
}

/*
 * Another function to be called from Python
 */
static PyObject* PyMagnet_position(PyObject* self, PyObject* args)
{
	float speed_mps = 0.0f;
	float watts = 0.0f;
	uint16_t position = 0;
	
	// Parse the python arguments.
	PyArg_ParseTuple(args, "ff", &speed_mps, &watts);
	
	// Call the actual magnet module function.
	position = magnet_position(speed_mps, watts);

	// Build python variable for return value.
	return Py_BuildValue("i", position);
}

static PyMethodDef MagnetMethods[] = {
    {"watts",  PyMagnet_watts, METH_VARARGS,
     "Calculate Watts."},
    {"position",  PyMagnet_position, METH_VARARGS,
     "Calculate position from target watts."},	 
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef magnet_module = {
   PyModuleDef_HEAD_INIT,
   "magnet",   /* name of module */
   NULL, /* module documentation, may be NULL */
   -1,       /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
   MagnetMethods
};

PyMODINIT_FUNC
PyInit_magnet(void)
{
	return PyModule_Create(&magnet_module);
}
