# Picat
This is a Visual Studio Project to compile Picat in Win x64. Additionally it has been embedded with the Python Interpreter.
This must be compiled as Release x64 or it will not compile or run.

The basics of Python use:

bp.python_init()		Initializes the Python Interpreter, must be run before any Python can be used.

bp.python_exit()		Closes python, Python commands will no longer work. (frees up memory used by python)

bp.python_run_interpreter()	Will switch Picat into a interactive Python Interpreter, all commands entered will have to be Python.
				You can exit this mode by entering 2 consecutive empty lines in the command prompt.

bp.python_run_file(X)		X must be a string, will call the python script described in X, exclude file extention. 
				(instead of "pyfile.py", just write "pyfile")

bp.python_get_value(X,Y)	X must be a free Variable, Y must be a string. If possible will convert the Python variable described in Y
				and store it in X.
                                
				ex:
							
				// python_test.py //
				def fact(n):
					if (n == 1):
						return 1
					return n * fact(n -1)
                                
				f = fact(5)
                               
                                
				// picat_test.pi //
				main =>
					bp.python_init(),
					bp.python_run_file("python_test"),
					bp.python_get_value(X,"f"),
					println(X),
					bp.python_exit().
                                  
                                Will print:
                                120
                                As thats the value assigned to f in python_test.py
                                
                                
//WARNING!!!  As of now, there is very little error handling in the Python interface, so there are chances of crashing.
                                
                                
