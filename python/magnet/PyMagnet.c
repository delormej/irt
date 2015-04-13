#include <Python.h>
#include "magnet.c"
/*
To build: 
	$ gcc -shared -I/c/Python34/include -I../../firmware/emotion/libraries/math -L/c/Python34/libs ../../firmware/emotion/libraries/math/*.c pyMagnet.c -lpython34 -std=gnu99 -o magnet.pyd
*/

/*
 * Python wrapper for C magnet_watts call.  Returns the magnet watts for 
 * a given speed and magnet position.
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
 * Python wrapper for C magnet_position call.  Returns the position given
 * speed in meters per second and desired additional watts from magnet.
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

/*
 * Python wrapper for setting coefficients.
 *
 */
static PyObject* PyMagnet_set_coeff(PyObject* self, PyObject* args)
{
	float low_speed_mps, 
		low_a,
		low_b,
		low_c,
		low_d,
		high_speed_mps, 
		high_a,
		high_b,
		high_c,
		high_d;
	
	// Parse the python arguments.
	PyArg_ParseTuple(args, "ffffffffff", 
		&low_speed_mps, 
		&low_a,
		&low_b,
		&low_c,
		&low_d,
		&high_speed_mps, 
		&high_a,
		&high_b,
		&high_c,
		&high_d		
		);
	
	// Build the low and high speed coefficient structures.
	poly_coeff_t low;
	low.speed_mps = low_speed_mps;
	low.coeff[0] = low_a;
	low.coeff[1] = low_b;
	low.coeff[2] = low_c;
	low.coeff[3] = low_d;
	
	poly_coeff_t high;
	high.speed_mps = high_speed_mps;
	high.coeff[0] = high_a;
	high.coeff[1] = high_b;
	high.coeff[2] = high_c;
	high.coeff[3] = high_d;
	
	// Call the actual magnet module function.
	magnet_set_coeff(low, high);
	
	// Build python variable for return value.
	return Py_BuildValue("i", 0);
}

static PyMethodDef MagnetMethods[] = {
    {"watts",  PyMagnet_watts, METH_VARARGS,
     "Calculate Watts."},
    {"position",  PyMagnet_position, METH_VARARGS,
     "Calculate position from target watts."},	 
    {"set_coeff",  PyMagnet_set_coeff, METH_VARARGS,
     "Sets the coefficients."},	 
	 
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
