#ifndef CLASP_INTERFACE_H
#define CLASP_INTERFACE_H

#include "clasp_interface.h"

#include "clasp\clasp_facade.h"
#include "clasp\solver.h"

#include <vector>
#include <iostream>

#include <ctime>


void printModel(const Clasp::OutputTable& out, const Clasp::Model& model) {
	std::cout << "Model " << model.num << ": \n";
	// Always print facts.
	for (Clasp::OutputTable::fact_iterator it = out.fact_begin(), end = out.fact_end(); it != end; ++it) {
		std::cout << *it << " ";
	}
	// Print elements that are true wrt the current model.
	for (Clasp::OutputTable::pred_iterator it = out.pred_begin(), end = out.pred_end(); it != end; ++it) {
		if (model.isTrue(it->cond)) {
			std::cout << it->name << " ";
		}
	}
	// Print additional output variables.
	for (Clasp::OutputTable::range_iterator it = out.vars_begin(), end = out.vars_end(); it != end; ++it) {
		std::cout << (model.isTrue(Clasp::posLit(*it)) ? int(*it) : -int(*it)) << " ";
	}
	std::cout << std::endl;
}

// In order to get models from the ClaspFacade, we must provide a suitable
// event handler.
class ClaspModel : public Clasp::EventHandler {
public:
	ClaspModel() {}
	bool onModel(const Clasp::Solver& s, const Clasp::Model& model) {
		Clasp::OutputTable out = s.outputTable();

		for (Clasp::OutputTable::range_iterator it = out.vars_begin(), end = out.vars_end(); it != end; ++it) {
			if (model.isTrue(Clasp::posLit(*it)))
				m.push_back(1);
			else
				m.push_back(0);
		}
		return true;
	}

	std::vector<int> m;
};


static Clasp::ClaspFacade* clasp_s;
static ClaspModel* clasp_m;

static Clasp::LitVec clasp_clause;
static std::vector<Clasp::LitVec> clasp_clauses;
static int clasp_vars = 0;


extern "C" {
	void clasp_init(int nvrs, int ncls) {
		if (clasp_s) 
			delete clasp_s;
		if (clasp_m)
			delete clasp_m;

		clasp_clause.clear();
		clasp_clauses.clear();
		clasp_vars = 0;
		
		clasp_s = new Clasp::ClaspFacade();
		clasp_m = new ClaspModel();
	}

	void clasp_add_lit(int lit0) {
		if (lit0 == 0) {
			clasp_clauses.push_back(clasp_clause);
			clasp_clause.clear();
		}
		else {
			clasp_vars = clasp_vars > abs(lit0) ? clasp_vars : abs(lit0);
			clasp_clause.push_back((Clasp::Literal(abs(lit0), (lit0 < 0))));
		}
	}


	int clasp_start_solver() {
		printf("solve, time: ");
		
		
		Clasp::ClaspConfig clasp_config;
		Clasp::SatBuilder& clasp_sat = clasp_s->startSat(clasp_config);
		clasp_sat.prepareProblem(clasp_vars, 0, clasp_clauses.size());

		for (auto& cls : clasp_clauses) {
			clasp_sat.addClause(cls);
		}
		std::clock_t    start;
		start = std::clock();

		Clasp::SolveResult ret = clasp_s->solve(clasp_m);

		printf("%d ms, ", (int)((std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000)));

		if (ret.sat())
			printf("SAT\n");
		else
			printf("UNSAT\n");

		return ret.sat() ? 1 : 0;
	}

	int clasp_get_binding(int varNum) {
		//printf("clasp_get_binding: %d\n", varNum);
		if (varNum > clasp_m->m.size()) {
			return 0;
		}
		return clasp_m->m[varNum - 1];
	}

}


#endif