CPP    = g++ -c 

Glucose: Solver.o Options.o System.o SimpSolver.o SharedCompanion.o SolverCompanion.o \
	SolverConfiguration.o ParallelSolver.o MultiSolvers.o ClausesBuffer.o Glucose.o
	$(CPP) -I../glucose *.o
Solver.o : core/Solver.cc 
	$(CPP) -I../glucose core/Solver.cc 
Options.o : utils/Options.cc 
	$(CPP) -I../glucose utils/Options.cc 
System.o : utils/System.cc 
	$(CPP) -I../glucose utils/System.cc 
SimpSolver.o : simp/SimpSolver.cc 
	$(CPP) -I../glucose simp/SimpSolver.cc 
SharedCompanion.o : parallel/SharedCompanion.cc 
	$(CPP) -I../glucose parallel/SharedCompanion.cc 
SolverCompanion.o : parallel/SolverCompanion.cc 
	$(CPP) -I../glucose parallel/SolverCompanion.cc 
SolverConfiguration.o : parallel/SolverConfiguration.cc 
	$(CPP) -I../glucose parallel/SolverConfiguration.cc 
ParallelSolver.o : parallel/ParallelSolver.cc parallel/SharedCompanion.h
	$(CPP) -I../glucose parallel/ParallelSolver.cc 
MultiSolvers.o : parallel/MultiSolvers.cc 
	$(CPP) -I../glucose parallel/MultiSolvers.cc 
ClausesBuffer.o : parallel/ClausesBuffer.cc 
	$(CPP) -I../glucose parallel/ClausesBuffer.cc 
Glucose.o : glucose.cpp
	$(CPP) -I../glucose glucose.cpp
