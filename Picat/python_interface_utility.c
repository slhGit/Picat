#include <string.h>
#include <stdio.h>
#include "Python.h"
#include "picat.h"
#include "picat_utilities.h"
#include "python_interface_utility.h"

#define DEBUG 0

PyObject *main_module = NULL;
PyObject *main_dict = NULL;

void dprint(char* str) {
	if (DEBUG) {
		printf(str);
	}
}

int _python_import(char* mod) {
	if (main_module) {
		PyObject* module = PyImport_ImportModule(mod);
		if (module) {
			PyDict_SetItemString(main_dict, mod, module);
			return 1;
		}
		else {
			//Trouble importing module
			PyErr_Print();
			return 0;
		}
	}
	else {
		//ERROR PYTHON WAS NOT INITIALIZED
		return 0;
	}
}
python_import() {
	TERM arg = picat_get_call_arg(1, 1);
	if (picat_is_string(arg)) {
		char* name = picat_string_to_cstring(arg);
		int success = _python_import(name);

		return success ? PICAT_TRUE : PICAT_FALSE;
	}
	else {
		//error arg should be String
		return PICAT_FALSE;
	}
}

/*NOT YET FULLY IMPLEMENTED
PyObject* PythonParsePicatArgs(TERM t) {
	if (picat_is_list(t)) {
		PyObject* temp = PicatToPython(t);
		int size = PyList_Size(temp);
		PyObject* ret = PyTuple_New(size);
		for (int i = 0; i < size; i++) {
			PyTuple_SetItem(ret, i, PyList_GetItem(temp, i));
		}
		Py_DECREF(temp);
		return ret;
	} else {
		//Args should be in list form
		return NULL;
	}
}

python_call_function() {
	TERM t_module, t_function, t_args, ret;
	t_module = picat_get_call_arg(1, 4);
	t_function = picat_get_call_arg(2, 4);
	t_args = picat_get_call_arg(3, 4);
	ret = picat_get_call_arg(4, 4);

	if (picat_is_string(t_module) && picat_is_string(t_function) && picat_is_list(t_args)) {
		if (main_module) {
			PyObject* mod = PyDict_GetItem(main_dict, PicatToPython(t_module));
			if (mod) {
				PyObject* func = PyObject_GetAttrString(mod, PicatToPython(t_function));
				if (func && PyCallable_Check(func)) {
					PyObject* args = PythonParsePicatArgs(t_args);
					PyObject* value = PyObject_CallObject(func, args);
					PyObject_Print(value, stdout, Py_PRINT_RAW);
					return PICAT_TRUE;

				}
				else {
					//function not callable
					return PICAT_FALSE;
				}
			} else {
				//Module not imported
				return PICAT_FALSE;
			}
		} else {
			// Python not init
			return PICAT_FALSE;
		}
	}
	else {
		// Arguments not currect format
		return PICAT_FALSE;
	}
}
*/

//Start the python.
python_init() {
	Py_SetProgramName("picat");

	Py_Initialize();

	char** args = calloc(1, sizeof(char*));
	args[0] = "";
	PySys_SetArgvEx(1, args, 0);
	free(args);
	
	main_module = PyImport_ImportModule("__main__");
	main_dict = PyModule_GetDict(main_module);

	return PICAT_TRUE;
}

//run the interacter python interpreter.
python_run_interpreter() {
	int picat_python = 0;
	char* cmd = calloc(1, 1);
	char line[512] = { 0 };

	printf("Now running Python interpreter, press Ctrl-C to exit and execute commands.\n");
	printf("Picat_Python> ");
	while (fgets(line,512,stdin)) {
		if (strcmp(line, "\n") == 0) {
			if (picat_python == 0) {
				picat_python = 1;
			}
			else {
				picat_python = 2;
			}
		}
		else {
			cmd = realloc(cmd, strlen(cmd) + 1 + strlen(line));
			strcat(cmd, line);
			picat_python = 0;
		}
		printf("Picat_Python> ");
	}
	printf("\n");

	PyRun_SimpleString(cmd);
	if (PyErr_Occurred()) {
		PyErr_Print();
		return PICAT_FALSE;
	}

	return PICAT_TRUE;
}
python_run_file() {
	dprint("run file\n");
	TERM arg = picat_get_call_arg(1, 1);

	if (picat_is_string(arg)) {
		dprint("is string\n");
		char* f = picat_string_to_cstring(arg);

		if (main_module) {
			dprint("is init\n");
			PyObject* module = PyImport_ImportModule(f);
			if (module) {
				dprint("module import\n");
				PyDict_Merge(main_dict, PyModule_GetDict(module), 1);

				if (PyErr_Occurred()) {
					PyErr_Print();
					return PICAT_FALSE;
				}
				dprint("all good\n");
				return PICAT_TRUE;
			}
			else {
				//Trouble importing module
				dprint("no module import\n");
				if (PyErr_Occurred()) {
					PyErr_Print();
				}
				return PICAT_FALSE;
			}
		}
		else {
			//ERROR PYTHON WAS NOT INITIALIZED
			return PICAT_FALSE;
		}
	}
	else {
		//Not a file
		dprint("Not a file\n");
		return PICAT_FALSE;
	}
}

python_get_value() {
	dprint("get value\n");
	TERM ret, value;
	
	ret = picat_get_call_arg(1, 2);

	if (picat_is_var(ret)) {
		dprint("is var\n");
		value = picat_get_call_arg(2, 2);
		if (picat_is_string(value)) {

			PyObject* py_val = PyDict_GetItemString(main_dict, picat_string_to_cstring(value));

			if (py_val) {
				dprint("got value\n");
				TERM t = PythonToPicat(py_val);
				return picat_unify(ret, t);
			}
			else {
				// no such python value
				dprint("no such value\n");
				return PICAT_FALSE;
			}
		}
		else {
			// this should be a string
			dprint("should be string\n");
			return PICAT_FALSE;
		}
	}
	else {
		// this should be a Var
		dprint("should be var\n");
		return PICAT_FALSE;
	}
}

python_set_value() {
	TERM arg, name;

	arg = picat_get_call_arg(1, 2);

	if (main_module) {
		dprint("main mod\n");
		PyObject* py_arg = PicatToPython(arg);

		if (py_arg) {
			dprint("arg\n");
			name = picat_get_call_arg(2, 2);
			if (picat_is_string(name)) {
				dprint("name\n");
				PyDict_SetItem(main_dict, PicatToPython(name), py_arg);
				return PICAT_TRUE;
			}
			else {
				//name needs to be a string
				return PICAT_FALSE;
			}
		}
		else {
			//arg needs to have a value
			return PICAT_FALSE;
		}
	}
	else {
		//python not init
		return PICAT_FALSE;
	}
}

python_exit() {
	Py_Finalize();

	//PyMem_RawFree("picat");

	return PICAT_TRUE;
}


python_cpreds() {
	insert_cpred("python_init", 0, python_init);
	insert_cpred("python_run_interpreter", 0, python_run_interpreter);
	insert_cpred("python_run_file", 1, python_run_file);
	insert_cpred("python_get_value", 2, python_get_value);
	insert_cpred("python_set_value", 2, python_set_value);
	insert_cpred("python_exit", 0, python_exit);
}