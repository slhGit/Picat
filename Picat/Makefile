SAT_V = 2 # 1=lingeling 2=glucose 3=maple 4=clasp
CC     = gcc -DGC -DGCC -DPICAT -DSATS=$(SAT_V) -DM64BITS
CPP    = g++ -DGC -DGCC -DPICAT -DSATS=$(SAT_V) -DM64BITS

CFLAGS = -c -O3 -fno-strict-aliasing -Dunix -DLINUX -DPOSIX -DCYGWIN 

LGLFLAG = -Wall -DNLGLOG -DNLGLSTATS -DNDEBUG -DNLGLPICOSAT
LFLAGS = -lm -lpthread

OBJ = dis.o init.o init_sym.o loader.o inst_inf.o main.o toam.o unify.o \
    file.o domain.o cfd.o float1.o arith.o token.o global.o \
    builtins.o mic.o numbervars.o cpreds.o univ.o assert.o findall.o clause.o \
    delay.o clpfd.o clpfd_libs.o event.o toamprofile.o \
    kapi.o getline.o table.o gcstack.o gcheap.o gcqueue.o debug.o \
    expand.o bigint.o sapi.o sat_bp.o espresso_bp.o lglbnr.o lglib.o plglib.o \
	application.o aspif.o aspif_text.o pclingo.o convert.o match_basic_types.o \
	program_options.o rule_utils.o smodels.o string_convert.o theory_data.o value_store.o \
	asp_preprocessor.o cb_enumerator.o clasp_facade.o clasp_options.o clasp_output.o \
	clasp_clause.o clingo.o constraint.o dependency_graph.o enumerator.o heuristics.o \
	logic_program.o logic_program_types.o lookahead.o minimize_constraint.o \
	model_enumerators.o parallel_solve.o parser.o program_builder.o satelite.o \
	shared_context.o solve_algorithms.o solver.o solver_strategies.o solver_types.o \
	statistics.o timer.o unfounded_check.o weight_constraint.o clasp_interface.o \
	mSolver.o mOptions.o mSystem.o mSimpSolver.o Maple_Interface.o \
	gSolver.o gOptions.o gSystem.o gSimpSolver.o \
	SharedCompanion.o SolverCompanion.o SolverConfiguration.o ParallelSolver.o MultiSolvers.o ClausesBuffer.o Glucose_Interface.o \
	

ESP = espresso/

ESPRESSO_OBJ = $(ESP)black_white.o $(ESP)canonical.o $(ESP)cofactor.o $(ESP)cols.o $(ESP)compl.o $(ESP)contain.o \
			 $(ESP)cpu_time.o $(ESP)cubestr.o $(ESP)cvrin.o $(ESP)cvrm.o $(ESP)cvrmisc.o $(ESP)cvrout.o \
			 $(ESP)dominate.o $(ESP)equiv.o $(ESP)essen.o $(ESP)essentiality.o $(ESP)exact.o $(ESP)espresso_expand.o \
			 $(ESP)gasp.o $(ESP)gimpel.o $(ESP)globals.o $(ESP)hack.o $(ESP)indep.o $(ESP)irred.o $(ESP)map.o \
			 $(ESP)matrix.o $(ESP)mincov.o $(ESP)opo.o $(ESP)pair.o $(ESP)part.o $(ESP)primes.o $(ESP)prtime.o \
			 $(ESP)reduce.o $(ESP)rows.o $(ESP)set.o $(ESP)setc.o $(ESP)sharp.o $(ESP)sigma.o $(ESP)signature.o \
			 $(ESP)signature_exact.o $(ESP)sminterf.o $(ESP)solution.o $(ESP)sparse.o $(ESP)unate.o $(ESP)util_signature.o \
			 $(ESP)verify.o $(ESP)espresso.o

GLU = glucose/

MAPLE = maple/

CLACP = claspLib/

picat.exe : $(OBJ)
	cd espresso && make && cd .. && $(CPP) -o picat.exe $(OBJ) $(ESPRESSO_OBJ) $(LFLAGS)
clean :
	rm -f *.o picat.exe
dis.o   : dis.c term.h inst.h basic.h 
	$(CC) $(CFLAGS) dis.c 
init.o  : init.c term.h inst.h basic.h
	$(CC) $(CFLAGS) init.c
init_sym.o  : init_sym.c term.h inst.h basic.h
	$(CC) $(CFLAGS) init_sym.c
loader.o : loader.c term.h basic.h inst.h picat_bc.h
	$(CC) $(CFLAGS) loader.c 
inst_inf.o  : inst_inf.c term.h inst.h basic.h
	$(CC) $(CFLAGS) inst_inf.c
main.o  : main.c term.h inst.h basic.h
	$(CC) $(CFLAGS) main.c
toam.o  : toam.c term.h inst.h basicd.h basic.h toam.h event.h frame.h emu_inst.h
	$(CC) $(CFLAGS) toam.c
toamprofile.o  : toamprofile.c term.h inst.h basicd.h basic.h toam.h event.h frame.h
	$(CC) $(CFLAGS) toamprofile.c
unify.o : unify.c term.h basic.h event.h bapi.h frame.h
	$(CC) $(CFLAGS) unify.c 
file.o    : file.c term.h basic.h bapi.h
	$(CC) $(CFLAGS) file.c 
domain.o    : domain.c term.h basic.h bapi.h event.h frame.h
	$(CC) $(CFLAGS) domain.c 
cfd.o    : cfd.c term.h basic.h bapi.h event.h frame.h
	$(CC) $(CFLAGS) cfd.c 
float1.o    : float1.c term.h basic.h bapi.h
	$(CC) $(CFLAGS) float1.c 
arith.o    : arith.c term.h basic.h bapi.h
	$(CC) $(CFLAGS) arith.c 
bigint.o    : bigint.c term.h basic.h bapi.h
	$(CC) $(CFLAGS) bigint.c 
token.o    : token.c term.h basic.h bapi.h
	$(CC) $(CFLAGS) token.c 
global.o    : global.c term.h basic.h bapi.h 
	$(CC) $(CFLAGS) global.c 
builtins.o    : builtins.c term.h basic.h bapi.h
	$(CC) $(CFLAGS) builtins.c 
mic.o    : mic.c term.h basic.h bapi.h
	$(CC) $(CFLAGS) mic.c 
numbervars.o    : numbervars.c term.h basic.h bapi.h
	$(CC) $(CFLAGS) numbervars.c 
cpreds.o    : cpreds.c term.h basic.h bapi.h
	$(CC) $(CFLAGS) cpreds.c
univ.o    : univ.c term.h basic.h bapi.h
	$(CC) $(CFLAGS) univ.c
assert.o    : assert.c term.h basic.h bapi.h
	$(CC) $(CFLAGS) assert.c
findall.o    : findall.c term.h basic.h bapi.h
	$(CC) $(CFLAGS) findall.c
clause.o    : clause.c term.h basic.h bapi.h dynamic.h
	$(CC) $(CFLAGS) clause.c
delay.o    : delay.c term.h basic.h bapi.h 
	$(CC) $(CFLAGS) delay.c
clpfd.o : clpfd.c term.h basic.h bapi.h event.h frame.h
	$(CC) $(CFLAGS) clpfd.c 
clpfd_libs.o : clpfd_libs.c term.h basic.h bapi.h event.h frame.h
	$(CC) $(CFLAGS) clpfd_libs.c 
kapi.o : kapi.c term.h basic.h bapi.h kapi.h
	$(CC) $(CFLAGS) kapi.c 
event.o : event.c term.h basic.h bapi.h kapi.h event.h
	$(CC) $(CFLAGS) event.c 
table.o : table.c term.h basic.h bapi.h frame.h
	$(CC) $(CFLAGS) table.c 
debug.o : debug.c term.h basic.h bapi.h frame.h
	$(CC) $(CFLAGS) debug.c 
gcstack.o : gcstack.c gc.h term.h basic.h bapi.h frame.h
	$(CC) $(CFLAGS) gcstack.c 
gcheap.o : gcheap.c gc.h term.h basic.h bapi.h frame.h
	$(CC) $(CFLAGS) gcheap.c 
gQueue.o : gcqueue.c gc.h term.h basic.h bapi.h frame.h
	$(CC) $(CFLAGS) gcqueue.c 
expand.o : expand.c gc.h term.h basic.h bapi.h frame.h
	$(CC) $(CFLAGS) expand.c 
espresso_bp.o : espresso_bp.c term.h basic.h bapi.h frame.h
	$(CC) $(CFLAGS) -Iespresso espresso_bp.c
sapi.o : sapi.c sapi.h term.h basic.h bapi.h frame.h
	$(CC) $(CFLAGS) sapi.c 
sat_bp.o : sat_bp.c term.h basic.h bapi.h frame.h
	$(CC) $(CFLAGS) sat_bp.c
lglbnr.o : lingeling/lglbnr.c 
	$(CC) $(CFLAGS) $(LGLFLAG) -Ilingeling lingeling/lglbnr.c
lglib.o : lingeling/lglib.c 
	$(CC) $(CFLAGS)  $(LGLFLAG) -Ilingeling lingeling/lglib.c
plglib.o : lingeling/plglib.c 
	$(CC) $(CFLAGS)  $(LGLFLAG) -Ilingeling lingeling/plglib.c
application.o : claspLib/libpotassco/src/application.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/libpotassco/src/application.cpp
aspif.o : claspLib/libpotassco/src/aspif.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/libpotassco/src/aspif.cpp
aspif_text.o : claspLib/libpotassco/src/aspif_text.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/libpotassco/src/aspif_text.cpp
pclingo.o : claspLib/libpotassco/src/clingo.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco -o pclingo.o claspLib/libpotassco/src/clingo.cpp
convert.o : claspLib/libpotassco/src/convert.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/libpotassco/src/convert.cpp
match_basic_types.o : claspLib/libpotassco/src/match_basic_types.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/libpotassco/src/match_basic_types.cpp
program_options.o : claspLib/libpotassco/src/program_options.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/libpotassco/src/program_options.cpp
rule_utils.o : claspLib/libpotassco/src/rule_utils.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/libpotassco/src/rule_utils.cpp
smodels.o : claspLib/libpotassco/src/smodels.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/libpotassco/src/smodels.cpp
string_convert.o : claspLib/libpotassco/src/string_convert.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/libpotassco/src/string_convert.cpp
theory_data.o : claspLib/libpotassco/src/theory_data.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/libpotassco/src/theory_data.cpp
value_store.o : claspLib/libpotassco/src/value_store.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/libpotassco/src/value_store.cpp
asp_preprocessor.o : claspLib/src/asp_preprocessor.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/asp_preprocessor.cpp
cb_enumerator.o : claspLib/src/cb_enumerator.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/cb_enumerator.cpp
clasp_facade.o : claspLib/src/clasp_facade.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/clasp_facade.cpp
clasp_options.o : claspLib/src/clasp_options.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/clasp_options.cpp
clasp_output.o : claspLib/src/clasp_output.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/clasp_output.cpp
clasp_clause.o : claspLib/src/clause.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco -o clasp_clause.o claspLib/src/clause.cpp
clingo.o : claspLib/src/clingo.cpp   
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/clingo.cpp
constraint.o : claspLib/src/constraint.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/constraint.cpp
dependency_graph.o : claspLib/src/dependency_graph.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/dependency_graph.cpp
enumerator.o : claspLib/src/enumerator.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/enumerator.cpp
heuristics.o : claspLib/src/heuristics.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/heuristics.cpp
logic_program.o : claspLib/src/logic_program.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/logic_program.cpp
logic_program_types.o : claspLib/src/logic_program_types.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/logic_program_types.cpp
lookahead.o : claspLib/src/lookahead.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/lookahead.cpp
minimize_constraint.o : claspLib/src/minimize_constraint.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/minimize_constraint.cpp
model_enumerators.o : claspLib/src/model_enumerators.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/model_enumerators.cpp
parallel_solve.o : claspLib/src/parallel_solve.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/parallel_solve.cpp
parser.o : claspLib/src/parser.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/parser.cpp
program_builder.o : claspLib/src/program_builder.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/program_builder.cpp
satelite.o : claspLib/src/satelite.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/satelite.cpp
shared_context.o : claspLib/src/shared_context.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/shared_context.cpp
solve_algorithms.o : claspLib/src/solve_algorithms.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/solve_algorithms.cpp
solver.o : claspLib/src/solver.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/solver.cpp
solver_strategies.o : claspLib/src/solver_strategies.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/solver_strategies.cpp
solver_types.o : claspLib/src/solver_types.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/solver_types.cpp
statistics.o : claspLib/src/statistics.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/statistics.cpp
timer.o : claspLib/src/timer.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/timer.cpp
unfounded_check.o : claspLib/src/unfounded_check.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/unfounded_check.cpp
weight_constraint.o : claspLib/src/weight_constraint.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco claspLib/src/weight_constraint.cpp
clasp_interface.o : clasp_interface.cpp
	$(CPP) $(CFLAGS) -IclaspLib -IclaspLib/libpotassco clasp_interface.cpp
mSolver.o : maple/core/Solver.cc 
	$(CPP) $(CFLAGS) -Imaple -o mSolver.o maple/core/Solver.cc 
mOptions.o : maple/utils/Options.cc 
	$(CPP) $(CFLAGS) -Imaple -o mOptions.o maple/utils/Options.cc 
mSystem.o : maple/utils/System.cc 
	$(CPP) $(CFLAGS) -Imaple -o mSystem.o maple/utils/System.cc 
mSimpSolver.o : maple/simp/SimpSolver.cc 
	$(CPP) $(CFLAGS) -Imaple -o mSimpSolver.o maple/simp/SimpSolver.cc 
Maple_Interface.o : maple_interface.cpp
	$(CPP) $(CFLAGS) -Imaple maple_Interface.cpp
gSolver.o : glucose/core/Solver.cc 
	$(CPP) $(CFLAGS) -Iglucose -o gSolver.o glucose/core/Solver.cc 
gOptions.o : glucose/utils/Options.cc 
	$(CPP) $(CFLAGS) -Iglucose -o gOptions.o glucose/utils/Options.cc 
gSystem.o : glucose/utils/System.cc 
	$(CPP) $(CFLAGS) -Iglucose -o gSystem.o glucose/utils/System.cc 
gSimpSolver.o : glucose/simp/SimpSolver.cc 
	$(CPP) $(CFLAGS) -Iglucose -o gSimpSolver.o glucose/simp/SimpSolver.cc 
SharedCompanion.o : glucose/parallel/SharedCompanion.cc 
	$(CPP) $(CFLAGS) -Iglucose glucose/parallel/SharedCompanion.cc 
SolverCompanion.o : glucose/parallel/SolverCompanion.cc 
	$(CPP) $(CFLAGS) -Iglucose glucose/parallel/SolverCompanion.cc 
SolverConfiguration.o : glucose/parallel/SolverConfiguration.cc 
	$(CPP) $(CFLAGS) -Iglucose glucose/parallel/SolverConfiguration.cc 
ParallelSolver.o : glucose/parallel/ParallelSolver.cc
	$(CPP) $(CFLAGS) -Iglucose glucose/parallel/ParallelSolver.cc 
MultiSolvers.o : glucose/parallel/MultiSolvers.cc 
	$(CPP) $(CFLAGS) -Iglucose glucose/parallel/MultiSolvers.cc 
ClausesBuffer.o : glucose/parallel/ClausesBuffer.cc 
	$(CPP) $(CFLAGS) -Iglucose glucose/parallel/ClausesBuffer.cc 
Glucose_Interface.o : glucose_interface.cpp
	$(CPP) $(CFLAGS) -Iglucose Glucose_Interface.cpp



