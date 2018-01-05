# Picat
This is a Visual Studio Project to compile Picat in Win x64. Additionally it has been embedded with the Python Interpreter.
This must be compiled as Release x64 or it will not compile or run.

The basics of Python use:

bp.python_init()		Initializes the Python Interpreter, must be run before any Python can be used.

bp.python_exit()		Closes python, Python commands will no longer work. (frees up memory used by python)

bp.python_run_interpreter()	Will switch Picat into a interactive Python Interpreter, all commands entered will have to be Python.
				You can exit this mode by entering Ctrl-C.

bp.python_run_file(X)		X must be a string, will call the python script described in X, exclude file extention. 
				(instead of "pyfile.py", just write "pyfile")

bp.python_get_value(X,Y)	X must be a free Variable, Y must be a string. If possible will convert the Python variable described in Y
				and store it in X.

bp.python_set_value(X,Y)		X is a picat value, Y must be a string. Will convert the Picat Value to Python and store it in the
				Python environment as Y.

				ex:
							
				// python_test.py //
				def fact(n):
					if (n == 1):
						return 1
					return n * fact(n -1)
                                
				f = fact(5)
				print(i)
                               
                                
				// picat_test.pi //
				main =>
					bp.python_init(),
					I = 2.5,
					bp.python_set_value(I,"i")
					bp.python_run_file("python_test"),
					bp.python_get_value(X,"f"),
					println(X),
					bp.python_exit().
                                  
                Will print:
                2.5
                120

								                                
                                
//WARNING!!!  As of now, there is very little error handling in the Python interface, so there are chances of crashing.
                                
                                
