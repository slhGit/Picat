#ifndef PYTHON_INTERFACE_UTILITY_H
#define PYTHON_INTERFACE_UTILITY_H

#include "picat.h"
#include "Python.h"

PyObject* PicatToPython(TERM);
char* PicatStringToChar(TERM);

TERM PythonToPicat(PyObject*);

#endif