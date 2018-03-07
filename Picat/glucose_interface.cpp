#include "glucose_interface.h"

#include "glucose\core\Solver.h"
#include "glucose\parallel\MultiSolvers.h"


#include "glucose/utils/System.h"
#include "glucose/utils/ParseUtils.h"
#include "glucose/utils/Options.h"
#include "glucose/core/Dimacs.h"
#include "glucose/simp/SimpSolver.h"

#include <vector>
#include <iostream>
using namespace Glucose;


static SimpSolver* glu_s;
static MultiSolvers* pglu_s;

static vec<Lit> lits;


static float realTimeStart;
static double initial_time;

extern "C" {
	void glu_init() {
		if (glu_s) {
			delete glu_s;
		}
		
		glu_s = new SimpSolver();

		glu_s->parsing = 1;
		glu_s->verbosity = -1;
		
	}

	void glu_add_lit(int lit0) {		
		if (lit0 == 0) {
			glu_s->addClause(lits);
			lits.clear();
		}
		else {
			int var = abs(lit0) - 1;
			while (var >= glu_s->nVars())
				glu_s->newVar();
			lits.push((lit0 > 0) ? mkLit(var) : ~mkLit(var));
		}
	}


	int glu_start_solver() {
		glu_s->parsing = 0;
		return glu_s->solve();
	}

	int glu_get_binding(int varNum) {
		return glu_s->model[varNum - 1] == l_True ? 1 : 0;
	}





	void pglu_init() {
		if (pglu_s) {
			delete pglu_s;
		}

		pglu_s = new MultiSolvers();

		pglu_s->setVerbosity(0);
	}

	void pglu_add_lit(int lit0) {
		if (lit0 == 0) {
			pglu_s->addClause(lits);
			lits.clear();
		}
		else {
			int var = abs(lit0) - 1;
			while (var >= pglu_s->nVars())
				pglu_s->newVar();
			lits.push((lit0 > 0) ? mkLit(var) : ~mkLit(var));
		}
	}

	int pglu_start_solver() {
		return pglu_s->solve() == l_True ? 1 : 0;
	}

	int pglu_get_binding(int varNum) {
		int ret = (pglu_s->model[varNum - 1] == l_True) ? 1 : 0;
		return ret;
	}
}