#ifdef PYTHON

#include "picat.h"
#include "Python.h"
#include "picat_utilities.h"
#include "python_interface_utility.h"

//Translate Picat list to Python list.
PyObject* PicatListToPythonList(TERM t) {
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

//Translate Picat map to a Python dictionary
PyObject* PicatMapToPythonDict(TERM t) {
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

//Translate a Picat structure to a Python list.
PyObject* PicatStructureToPythonList(TERM t) {
	int size = picat_get_struct_arity(t);
	PyObject* s = PyList_New(size);
	for (int i = 0; i < size; i++) {
		PyObject* x = PicatToPython(picat_get_arg(i + 1, t));
		PyList_SetItem(s, i, x);
	}
	return s;
}

//Translate a Picat term into a Python Object.
PyObject* PicatToPython(TERM t) {
	if (picat_is_integer(t)) {
		return PyLong_FromLong(picat_get_integer(t));
	}
	else if (picat_is_float(t)) {
		return PyFloat_FromDouble(picat_get_float(t));
	}
	else if (picat_is_atom(t)) {
		return PyUnicode_FromString(picat_get_atom_name(t));
	}
	else if (picat_is_nil(t)) {
		return PyList_New(0);
	}
	else if (picat_is_string(t)) {
		return PyUnicode_FromString(picat_string_to_cstring(t));
	}
	else if (picat_is_list(t)) {
		return PicatListToPythonList(t);
	}
	else if (b_IS_MAP_c(t)) {
		return PicatMapToPythonDict(t);
	}
	else if (picat_is_structure(t)) {
		return PicatStructureToPythonList(t);
	}

	//SOME KIND OF ERROR SHOULD GO HERE AS NO TRANSLATABLE PICAT VALUE WAS GIVEN.
	return NULL;
}


#endif //PYTHON