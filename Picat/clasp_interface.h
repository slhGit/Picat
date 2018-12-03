#ifndef CLASP_INTERFACE_H
#define CLASP_INTERFACE_H


#include "bprolog.h"


#ifdef __cplusplus
extern "C" {
#endif
	void clasp_init(int nvrs, int ncls);
	void clasp_add_lit(int lit0);
	int clasp_start_solver();
	int clasp_get_binding(int varNum);


#ifdef __cplusplus
}
#endif

#endif