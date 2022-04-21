//-------------------------------------------------------------------------------------------------
// Simple FLI demo calling a Python Numpy library
//                                                                         
//  https://github.com/htminuslab                                                   
//
//-------------------------------------------------------------------------------------------------
//
// Update: Created 30/11/2017
//       : Fix the path to the python module in line 67 before executing it
// Update: Added matplotlib 04/12/2018
//       : Must use TK version from Python and not the (default) one from Modelsim. This can
//       : be done by setting the TCLLIBPATH env variable before starting Modelsim.
// Update: updated for Python3.8/Modelsim 2022.1 14/03/2022 
//-------------------------------------------------------------------------------------------------

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <mti.h>

PyObject *pModule;

//-------------------------------------------------------------------------------------------------
// Convert VHDL string into a NULL terminated C string 
//-------------------------------------------------------------------------------------------------
static char *get_string(mtiVariableIdT id)
{
	static char buf[1000];
	mtiTypeIdT type;
	int len;
	mti_GetArrayVarValue(id, buf);
	type = mti_GetVarType(id);
	len = mti_TickLength(type);

	buf[len] = 0;
	return buf;
}

//-------------------------------------------------------------------------------------------------
// Free python memory when simulation stops
// This is called automatically
//-------------------------------------------------------------------------------------------------
void call_python_cleanup(void * param)
{
	mti_PrintMessage("Cleaning up Python\n");
	Py_DECREF(pModule);	
	Py_FinalizeEx();
}

//-------------------------------------------------------------------------------------------------
// Load Python module  E:\Modelsim-Python-Demo2
//-------------------------------------------------------------------------------------------------
int call_python_module(mtiVariableIdT module_name) 
{
    PyObject *pName;

	mti_PrintMessage("Python FLI Test Numpy\n");

    Py_Initialize();
	if (!Py_IsInitialized()) {
		mti_PrintFormatted("Unable to initialize Python3.8 interpreter\n");
		return 1;
	} else {		
		mti_PrintFormatted("Python3.8 interpreter initialized\n");
	}
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append(r'E:\\GitHub\\Modelsim-Python-Demo2')"); // Change me!!!

	mti_PrintFormatted("Loading Module %s\n",get_string(module_name));
	pName = PyUnicode_DecodeFSDefault(get_string(module_name));

    pModule = PyImport_Import(pName);						// Global variable
    Py_DECREF(pName);

    if (pModule == NULL) {
		PyErr_Print();
        mti_PrintFormatted("Failed to load \"%s\"\n" ,get_string(module_name));
        return 1;
    }
	
	mti_AddQuitCB(call_python_cleanup,NULL);
	
	return 0;
}

//-------------------------------------------------------------------------------------------------
// Call function from Python module 
//-------------------------------------------------------------------------------------------------
int call_python_function(mtiVariableIdT function_name, mtiVariableIdT vhdl_array_in, mtiVariableIdT vhdl_array_out)
{
	PyObject *pFunc;
	PyObject *pArgs, *pValue;
	void   *array_val;
	
	pFunc = PyObject_GetAttrString(pModule,get_string(function_name));
	
	if (pFunc && PyCallable_Check(pFunc)) {
		
		array_val = mti_GetArrayVarValue(vhdl_array_in, 0);
		double *val = array_val;

		pArgs = PyList_New(500);				// TODO Update to vhdl_array_in length
        for (int i = 0; i < 500; ++i) {			
			pValue = PyFloat_FromDouble(val[i]);
            if (!pValue) {
                Py_DECREF(pArgs);
                Py_DECREF(pModule);
                fprintf(stderr, "Cannot convert double argument\n");
                return 1;
            }
			PyList_SetItem(pArgs, i, pValue);
        }

		pValue = PyObject_CallFunctionObjArgs(pFunc, pArgs, NULL);

		Py_DECREF(pArgs);
		
        if (pValue != NULL) {			
			array_val = mti_GetArrayVarValue(vhdl_array_out, 0);
			val = array_val;
            for (int i=0; i<(int)PyList_Size(pValue); i++) {
				val[i]=PyFloat_AsDouble(PyList_GetItem(pValue, (Py_ssize_t)i));
			}
            Py_DECREF(pValue);
			
        } else {
            Py_DECREF(pFunc);
            Py_DECREF(pModule);
            PyErr_Print();
            mti_PrintFormatted("Error, function %s returned an error\n",get_string(function_name));          
			return 1;
        }
	} else {
		if (PyErr_Occurred()) PyErr_Print();
		mti_PrintFormatted("Fatal error, cannot find function \"%s\"\n", get_string(function_name));
		mti_FatalError();
	}
	Py_XDECREF(pFunc);
	return 0;	
}
	
