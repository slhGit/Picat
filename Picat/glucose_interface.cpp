

#include "glucose_interface.h"


#include "glucose/core/Solver.h"


#include "glucose/utils/System.h"
#include "glucose/utils/ParseUtils.h"
#include "glucose/utils/Options.h"
#include "glucose/core/Dimacs.h"
#include "glucose/simp/SimpSolver.h"



#include "glucose/parallel/MultiSolvers.h"

#include <vector>
#include <iostream>

#include <ctime>
using namespace Glucose;


static SimpSolver* glu_s;
static MultiSolvers* pglu_s;

static vec<Lit> lits;


extern "C" {
	void glu_init() {
		if (glu_s) {
			delete glu_s;
		}
		
		glu_s = new SimpSolver();

		glu_s->parsing = 1;
		glu_s->verbosity = -1;

		glu_s->use_simplification = true;
		glu_s->certifiedUNSAT = false;
		glu_s->vbyte = false;
		
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
		printf("solve, time: ");
		
		std::clock_t    start;
		start = std::clock();

		glu_s->parsing = 0;
		glu_s->eliminate(true);

		vec<Lit> dummy;
		lbool ret = glu_s->solveLimited(dummy);

		printf("%d ms, ", (int)((std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000)));

		if (ret == l_True)
			printf("SAT\n");
		else
			printf("UNSAT\n");

		return ret == l_True ? 1 : 0;

		//glu_s->parsing = 0;
		//return glu_s->solve();
	}

	int glu_get_binding(int varNum) {
		if (varNum > glu_s->model.size()) {
			return 0;
		}
		return glu_s->model[varNum - 1] == l_True ? 1 : 0;
	}

	void pglu_init(int num_threads) {
		if (pglu_s) {
			delete pglu_s;
		}

		pglu_s = new MultiSolvers();
#if not defined(__linux__) and not defined(__FreeBSD__) and not defined(__APPLE__)
		pglu_s->forceThreads(num_threads);
#endif
		pglu_s->setVerbosity(-1);
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
		printf("solve, time: ");

		std::clock_t    start;
		start = std::clock();


		int ret2 = pglu_s->simplify();
		pglu_s->use_simplification = true;
		if (ret2)
			pglu_s->eliminate();

		lbool ret = pglu_s->solve();

		printf("%d ms, ", (int)((std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000)));

		if (ret == l_True)
			printf("SAT\n");
		else
			printf("UNSAT\n");


		return ret == l_True ? 1 : 0;
		//return pglu_s->solve() == l_True ? 1 : 0;
	}

	int pglu_get_binding(int varNum) {
		if (varNum > pglu_s->model.size()) {
			return 0;
		}
		int ret = (pglu_s->model[varNum - 1] == l_True) ? 1 : 0;
		return ret;
	}
}