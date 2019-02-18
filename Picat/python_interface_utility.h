#ifndef PYTHON_INTERFACE_UTILITY_H
#define PYTHON_INTERFACE_UTILITY_H

#ifdef PYTHON

#include "picat.h"
#include "Python.h"

PyObject* PicatToPython(TERM);
char* PicatStringToChar(TERM);

TERM PythonToPicat(PyObject*);

#endif //PYTHON
#endif //PYTHON_INTERFACE_UTILITY_H