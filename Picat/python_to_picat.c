#ifdef PYTHON

#include "picat.h"
#include "picat_utilities.h"

#include "Python.h"

#include "python_interface_utility.h"

//Convert a Python list to a Picat list.
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

//Convert a Python string to a Picat string.
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

//Convert a Python dictionary to a Picat map.
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

	TERM t = new_picat_map(C);

	//Get dictionaray keys from python dictionary
	PyObject* keys = PyDict_Keys(v);

	for (int i = 0; i < PyList_Size(keys); i++) {
		//Get python dictionary keys and values and convert to picat
		PyObject* pykey = PyList_GetItem(keys, i);
		PyObject* pyval = PyDict_GetItem(v, pykey);

		TERM key = PythonToPicat(pykey);
		TERM value = PythonToPicat(pyval);

		add_to_picat_map(t, key, value);
	}
	return t;
}

//Convert a Python object to a Picat term.
TERM PythonToPicat(PyObject* v) {
	if (v) {
		char* t = v->ob_type->tp_name;
		if (t) {
			if (strcmp(t, "int") == 0) {
				return picat_build_integer(PyLong_AsLong(v));
			}
			else if (strcmp(t, "float") == 0) {
				return picat_build_float(PyFloat_AsDouble(v));
			}
			else if (strcmp(t, "list") == 0 || strcmp(t, "tuple") == 0) {
				return PythonListToPicatList(v);
			}
			else if (strcmp(t, "range") == 0) {
				return picat_build_nil();
			}
			else if (strcmp(t, "str") == 0) {
				return PythonStringToPicatString(v);
			}
			else if (strcmp(t, "dict") == 0) {
				return PythonDictToPicatMap(v);
			}
			else {
				//SOMETHING WENT WRONG, DID NOT TRANSLATE.
				printf("Cannot translate type: '%s' into appropriate Picat type.\n", t);
				return picat_build_nil();
			}
		}
	}
	//SOMETHING WENT WRONG, DID NOT TRANSLATE.
	return picat_build_nil();
}

#endif