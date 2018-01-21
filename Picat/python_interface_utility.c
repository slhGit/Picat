#include <string.h>
#include <stdio.h>
#include "Python.h"
#include "picat.h"

#define DEBUG 0

PyObject *main_module = NULL;
PyObject *main_dict = NULL;

void dprint(char* str) {
	if (DEBUG) {
		printf(str);
	}
}
char* PicatStringToChar(TERM t) {
	int mult = 1;
	char* str = calloc(512, sizeof(char));

	TERM tList = t;
	TERM x = picat_get_car(tList);
	int i = 0;
	while (x != picat_build_integer(0)) {
		i++;
		if (i > 512 * mult) {
			mult++;
			realloc(str, 512 * mult);
		}
		strcat(str, picat_get_atom_name(x));
		tList = picat_get_cdr(tList);
		x = picat_get_car(tList);
	}

	return str;
}


PyObject* PicatToPython(TERM t) {
	/*if (picat_is_var(t))
		dprint("is var\n");
	if (picat_is_attr_var(t))
		dprint("is attributed var\n");
	if (picat_is_dvar(t))
		dprint("is domain var\n");
	if (picat_is_bool_dvar(t))
		dprint("is bool\n");*/
	if (picat_is_integer(t)) {
		return PyLong_FromLong(picat_get_integer(t));
	}
	if (picat_is_float(t)) {
		return PyFloat_FromDouble(picat_get_float(t));
	}
	if (picat_is_atom(t)) {
		return PyUnicode_FromString(picat_get_atom_name(t));
	}
	if (picat_is_nil(t)) {
		dprint("is nil\n");
		return PyList_New(0);
	}
	if (picat_is_string(t)) {
		return PyUnicode_FromString(PicatStringToChar(t));
	}
	if (picat_is_list(t)) {
		PyObject* list = PyList_New(0);

		TERM tList = t;
		TERM x = picat_get_car(tList);
		while (x != picat_build_integer(0)) {
			PyList_Append(list, PicatToPython(x));
			tList = picat_get_cdr(tList);
			x = picat_get_car(tList);
		}
		return list;
	}
	if (b_IS_MAP_c(t)){
		PyObject* dict = PyDict_New();

		TERM hashtable = picat_get_arg(2, t);

		for (int i = 1; i <= picat_get_struct_arity(hashtable); i++) {
			PyObject* bucket = PicatToPython(picat_get_arg(i, hashtable));
			if (bucket) {
				int num = PyList_Size(bucket);
				for (int j = 0; j < num; j++) {
					PyObject* pair = PyList_GetItem(bucket, j);
					PyObject* key = PyList_GetItem(pair, 0);
					PyObject* value = PyList_GetItem(pair, 1);
					PyDict_SetItem(dict, key, value);
				}
			}
		}
		return dict;
	}
	if (picat_is_structure(t)) {
		dprint("%s\n", picat_get_struct_name(t));
		int size = picat_get_struct_arity(t);
		PyObject* s = PyList_New(size);
		for (int i = 0; i < size; i++) {
			PyObject* x = PicatToPython(picat_get_arg(i + 1, t));
			PyList_SetItem(s, i, x);
		}
		return s;
	}
	if (picat_is_array(t)) {
		dprint("is array\n");
		return NULL;
	}
	if (picat_is_compound(t)) {
		dprint("is compound\n");
		return NULL;
	}

	//SOME KIND OF ERROR SHOULD GO HERE AS NO PICAT VALUE WAS GIVEN.
	dprint("is nothing\n");
	return NULL;
}

TERM NewPicatMap(int C) {
	dprint("new map\n");
	TERM t = picat_build_structure("$hshtb", 2);

	picat_unify(picat_get_arg(1, t), picat_build_integer(0));
	picat_unify(picat_get_arg(2, t), picat_build_structure("hashtable", C));

	return t;
}

TERM PythonListToPicatList(PyObject* v) {
	int len = PyList_Size(v);
	if (len) {
		TERM t = picat_build_list(), car, cdr;
		car = picat_get_car(t);
		cdr = picat_get_cdr(t);

		for (int i = 0; i < len - 1; i++) {
			picat_unify(car, PythonToPicat(PyList_GetItem(v, i)));
			TERM temp = picat_build_list();
			picat_unify(cdr, temp);

			car = picat_get_car(temp);
			cdr = picat_get_cdr(temp);
		}
		picat_unify(car, PythonToPicat(PyList_GetItem(v, len - 1)));
		picat_unify(cdr, picat_build_nil());

		return t;
	}
	else {
		return picat_build_nil();
	}
}

TERM PythonStringToPicatString(PyObject* v) {
	char* str = PyUnicode_AsUTF8(v);
	TERM t = picat_build_list(), car, cdr;
	car = picat_get_car(t);
	cdr = picat_get_cdr(t);

	int len = strlen(str);
	for (int i = 0; i < len - 1; i++) {
		char c[] = { str[i],"\0" };
		picat_unify(car, picat_build_atom(c));
		TERM temp = picat_build_list();
		picat_unify(cdr, temp);

		car = picat_get_car(temp);
		cdr = picat_get_cdr(temp);
	}
	char c[] = { str[len - 1],"\0" };
	picat_unify(car, picat_build_atom(c));
	picat_unify(cdr, picat_build_nil());

	return t;
}

TERM PythonDictToPicatMap(PyObject* v) {
	int C = PyDict_Size(v);

	//size of hash table, min 11, to match picat, else next largest prime number.
	if (C <= 11) {
		C = 11;
	}
	else {
		C = bp_prime(picat_build_integer(C));
	}

	dprint("%d\n", C);

	TERM t = NewPicatMap(C);

	//Get dictionaray keys from python dictionary
	PyObject* keys = PyDict_Keys(v);

	for (int i = 0; i < PyList_Size(keys); i++) {
		//Get python dictionary keys and values and convert to picat
		PyObject* pykey = PyList_GetItem(keys, i);
		PyObject* pyval = PyDict_GetItem(v, pykey);

		TERM key = PythonToPicat(pykey);
		TERM value = PythonToPicat(pyval);

		//Get hash value
		TERM HashVal = MAKEINT(bp_hashval(key));
		int Index = picat_get_integer(HashVal) % C + 1;

		//Create pair to add to map
		TERM Pair = picat_build_structure("=", 2);
		picat_unify(picat_get_arg(1, Pair), key);
		picat_unify(picat_get_arg(2, Pair), value);

		//Place pair in a list and that list into the hashtable
		TERM Bucket = picat_build_list();

		picat_unify(picat_get_car(Bucket), Pair);

		TERM Hashtable = picat_get_arg(2, t);
		picat_unify(picat_get_arg(Index, Hashtable), Bucket);

		//Update count of map
		TERM Count = picat_get_arg(1, t);
		TERM newCount = picat_build_integer(picat_get_integer(Count) + 1);
		picat_unify(Count, newCount);
	}
	return t;
}

TERM PythonToPicat(PyObject* v) {
	if (!v) {
		return picat_build_nil();
	}
	char* t = v->ob_type->tp_name;
	if (t) {
		//dprint(t); dprint("\n");
		if (strcmp(t,"int") == 0) {
			dprint("int\n");
			return picat_build_integer(PyLong_AsLong(v));
		}
		else if (strcmp(t,"float") == 0) {
			dprint("float\n");
			return picat_build_float(PyFloat_AsDouble(v));
		}
		else if (strcmp(t,"complex") == 0) {
			dprint("complex\n");

			return picat_build_nil();
		}
		else if (strcmp(t,"list") == 0 || strcmp(t, "tuple") == 0) {
			dprint("list or tuple\n");
			return PythonListToPicatList(v);
		}
		else if (strcmp(t,"range") == 0) {
			dprint("range\n");
			return picat_build_nil();
		}
		else if (strcmp(t, "str") == 0) {
			dprint("string\n");
			return PythonStringToPicatString(v);
		}
		else if (strcmp(t, "dict") == 0) {
			dprint("dict\n");
			return PythonDictToPicatMap(v);
		}
		else {
			dprint("cannot translate\n");
			return picat_build_nil();
		}
	}
	else {
		dprint("t not a thing\n");
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
		char* name = PicatStringToChar(arg);
		int success = _python_import(name);

		return success ? PICAT_TRUE : PICAT_FALSE;
	}
	else {
		//error arg should be String
		return PICAT_FALSE;
	}
}

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
		char* f = PicatStringToChar(arg);

		FILE* fp = _Py_fopen(f, "r");

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

			PyObject* py_val = PyDict_GetItemString(main_dict, PicatStringToChar(value));

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