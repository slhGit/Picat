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
static const char* _certified = "CORE -- CERTIFIED UNSAT";

static float realTimeStart;
static double initial_time;

static 	BoolOption   pre("MAIN", "pre", "Completely turn on/off any preprocessing.", true);

static MultiSolvers* pmsolver;

// Terminate by notifying the solver and back out gracefully. This is mainly to have a test-case
// for this feature of the Solver as it may take longer than an immediate call to '_exit()'.
//static void SIGINT_interrupt(int signum) { pmsolver->interrupt(); }


// Note that '_exit()' rather than 'exit()' has to be used. The reason is that 'exit()' calls
// destructors and may cause deadlocks if a malloc/free function happens to be running (these
// functions are guarded by locks for multithreaded use).
static void SIGINT_exit(int signum) {
	printf("\n"); printf("*** INTERRUPTED ***\n");
	if (pmsolver->verbosity() > 0) {
		pmsolver->printFinalStats();
		printf("\n"); printf("*** INTERRUPTED ***\n");
	}
	_exit(1);
}

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
		glu_res = glu_s->solve();
		
		return glu_res;

	}

	int glu_get_binding(int varNum) {
		return glu_s->model[varNum - 1] == l_True ? 1 : 0;
	}





	void pglu_init() {
		if (pglu_s) {
			delete pglu_s;
		}
		realTimeStart = realTime();

		IntOption    verb("MAIN", "verb", "Verbosity level (0=silent, 1=some, 2=more).", 1, IntRange(0, 2));
		BoolOption   mod("MAIN", "model", "show model.", false);
		IntOption    vv("MAIN", "vv", "Verbosity every vv conflicts", 10000, IntRange(1, INT32_MAX));

		IntOption    cpu_lim("MAIN", "cpu-lim", "Limit on CPU time allowed in seconds.\n", INT32_MAX, IntRange(0, INT32_MAX));
		IntOption    mem_lim("MAIN", "mem-lim", "Limit on memory usage in megabytes.\n", INT32_MAX, IntRange(0, INT32_MAX));

		char a[] = { 'w',0 };
		char b[] = { '-','m','o','d','e','l',0 };
		char* argv[] = { a, b };
		int argc = 2;
		parseOptions(argc, argv, true);

		pglu_s = new MultiSolvers();
		pmsolver = pglu_s;

		pglu_s->setVerbosity(verb);
		pglu_s->setVerbEveryConflicts(vv);
		pglu_s->setShowModel(mod);

		initial_time = cpuTime();

		// Set limit on CPU-time:
		if (cpu_lim != INT32_MAX) {
			rlimit rl;
			getrlimit(RLIMIT_CPU, &rl);
			if (rl.rlim_max == RLIM_INFINITY || (rlim_t)cpu_lim < rl.rlim_max) {
				rl.rlim_cur = cpu_lim;
				if (setrlimit(RLIMIT_CPU, &rl) == -1)
					printf("c WARNING! Could not set resource limit: CPU-time.\n");
			}
		}

		// Set limit on virtual memory:
		if (mem_lim != INT32_MAX) {
			rlim_t new_mem_lim = (rlim_t)mem_lim * 1024 * 1024;
			rlimit rl;
			getrlimit(RLIMIT_AS, &rl);
			if (rl.rlim_max == RLIM_INFINITY || new_mem_lim < rl.rlim_max) {
				rl.rlim_cur = new_mem_lim;
				if (setrlimit(RLIMIT_AS, &rl) == -1)
					printf("c WARNING! Could not set resource limit: Virtual memory.\n");
			}
		}


	}

	void pglu_add_lit(int lit0) {
		if (lit0 = -0) {
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
		//glu_res = pglu_s->solve() == l_True ? 1 : 0;
		//return glu_res;


		if (pglu_s->verbosity() > 0) {
			printf("c ========================================[ Problem Statistics ]===========================================\n");
			printf("c |                                                                                                       |\n");
		}





		if (pglu_s->verbosity() > 0) {
			printf("c |  Number of variables:  %12d                                                                   |\n", pglu_s->nVars());
			printf("c |  Number of clauses:    %12d                                                                   |\n", pglu_s->nClauses());
		}

		double parsed_time = cpuTime();
		if (pglu_s->verbosity() > 0) {
			printf("c |  Parse time:           %12.2f s                                                                 |\n", parsed_time - initial_time);
			printf("c |                                                                                                       |\n");
		}

		// Change to signal-handlers that will only notify the solver and allow it to terminate
		// voluntarily:
		//signal(SIGINT, SIGINT_interrupt);
		//signal(SIGXCPU,SIGINT_interrupt);


		int ret2 = pglu_s->simplify();
		pglu_s->use_simplification = pre;
		if (ret2)
			pglu_s->eliminate();
		if (pre) {
			double simplified_time = cpuTime();
			if (pglu_s->verbosity() > 0) {
				printf("c |  Simplification time:  %12.2f s                                                                 |\n", simplified_time - parsed_time);
				printf("c |                                                                                                       |\n");
			}
		}

		if (!ret2 || !pglu_s->okay()) {
			//if (S.certifiedOutput != NULL) fprintf(S.certifiedOutput, "0\n"), fclose(S.certifiedOutput);
			if (pglu_s->verbosity() > 0) {
				printf("c =========================================================================================================\n");
				printf("Solved by unit propagation\n");
				printf("c real time : %g s\n", realTime() - realTimeStart);
				printf("c cpu time  : %g s\n", cpuTime());
				printf("\n");
			}
			printf("s UNSATISFIABLE\n");
			exit(20);
		}

		//  vec<Lit> dummy;
		lbool ret = pglu_s->solve();


		printf("c\n");
		printf("c real time : %g s\n", realTime() - realTimeStart);
		printf("c cpu time  : %g s\n", cpuTime());
		if (pglu_s->verbosity() > 0) {
			pglu_s->printFinalStats();
			printf("\n");
		}

		//-------------- Result is put in a external file
		/* I must admit I have to print the model of one thread... But which one? FIXME !!
		if (res != NULL){
		if (ret == l_True){
		fprintf(res, "SAT\n");
		for (int i = 0; i < S.nVars(); i++)
		if (S.model[i] != l_Undef)
		fprintf(res, "%s%s%d", (i==0)?"":" ", (S.model[i]==l_True)?"":"-", i+1);
		fprintf(res, " 0\n");
		}else if (ret == l_False)
		fprintf(res, "UNSAT\n");
		else
		fprintf(res, "INDET\n");
		fclose(res);

		//-------------- Want certified output
		} else {
		*/
		printf(ret == l_True ? "s SATISFIABLE\n" : ret == l_False ? "s UNSATISFIABLE\n" : "s INDETERMINATE\n");

		if (pglu_s->getShowModel() && ret == l_True) {
			printf("v ");
			for (int i = 0; i < pglu_s->model.size(); i++) {
				assert(pglu_s->model[i] != l_Undef);
				if (pglu_s->model[i] != l_Undef)
					printf("%s%s%d", (i == 0) ? "" : " ", (pglu_s->model[i] == l_True) ? "" : "-", i + 1);
			}
			printf(" 0\n");
		}


	}

	int pglu_get_binding(int varNum) {
		int ret = (pglu_s->model[varNum - 1] == l_True) ? 1 : 0;
		return ret;
	}
}