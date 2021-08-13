SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic

#------------------------------------------------------------
#
# When you adapt this makefile to compile your CPLEX programs
# please copy this makefile and set CPLEXDIR and CONCERTDIR to
# the directories where CPLEX and CONCERT are installed.
#
#------------------------------------------------------------
CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Studio1262/cplex
CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Studio1262/concert

# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------

CCC = g++

# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

CCOPT = -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD
COPT  = -m64 -fPIC 
JOPT  = -classpath $(CPLEXDIR)/lib/cplex.jar -O

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -g -lpthread

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

EXDIR         = .
EXSRC         = .
EXINC         = $(EXDIR)/include
EXDATA        = .

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 

all:
	make main

clean:
	rm -rf *.o *~ *.class Main
	rm -rf *.mps *.ord *.sos *.lp *.sav *.net *.msg *.clp


main: Worker.o Task.o Model.o Station.o ReadInstance.o Individual.o Sequencing.o GeneticAlgorithm.o Line.o Main.o
	$(CCC) $(CCFLAGS)Worker.o Task.o Model.o Station.o ReadInstance.o Individual.o Sequencing.o GeneticAlgorithm.o Line.o Main.o -o Main $(CCLNFLAGS)
Worker.o: $(EXSRC)/AssemblyLine/Worker.cpp
	$(CCC) -c $(CCFLAGS) $(EXSRC)/AssemblyLine/Worker.cpp -o Worker.o
Task.o: $(EXSRC)/AssemblyLine/Task.cpp
	$(CCC) -c $(CCFLAGS) $(EXSRC)/AssemblyLine/Task.cpp -o Task.o
Model.o: $(EXSRC)/AssemblyLine/Model.cpp
	$(CCC) -c $(CCFLAGS) $(EXSRC)/AssemblyLine/Model.cpp -o Model.o
Station.o: $(EXSRC)/AssemblyLine/Station.cpp $(EXSRC)/AssemblyLine/Worker.h $(EXSRC)/AssemblyLine/Task.h $(EXSRC)/AssemblyLine/Model.h
	$(CCC) -c $(CCFLAGS) $(EXSRC)/AssemblyLine/Station.cpp -o Station.o
ReadInstance.o: $(EXSRC)/InputOutputFile/ReadInstance.cpp
	$(CCC) -c $(CCFLAGS) $(EXSRC)/InputOutputFile/ReadInstance.cpp -o ReadInstance.o
Line.o: $(EXSRC)/AssemblyLine/Line.cpp $(EXSRC)/AssemblyLine/Worker.h $(EXSRC)/AssemblyLine/Task.h $(EXSRC)/AssemblyLine/Model.h $(EXSRC)/AssemblyLine/Station.h $(EXSRC)/InputOutputFile/ReadInstance.h
	$(CCC) -c $(CCFLAGS) $(EXSRC)/AssemblyLine/Line.cpp -o Line.o
Individual.o: $(EXSRC)/GA/Individual.cpp
	$(CCC) -c $(CCFLAGS) $(EXSRC)/GA/Individual.cpp -o Individual.o
GeneticAlgorithm.o: $(EXSRC)/GA/GeneticAlgorithm.cpp $(EXSRC)/AssemblyLine/Line.h $(EXSRC)/AssemblyLine/Task.h $(EXSRC)/GA/Individual.h
	$(CCC) -c $(CCFLAGS) $(EXSRC)/GA/GeneticAlgorithm.cpp -o GeneticAlgorithm.o
Sequencing.o: $(EXSRC)/IBMCplex/Sequencing.cpp $(EXSRC)/Constants.h
	$(CCC) -c $(CCFLAGS) $(EXSRC)/IBMCplex/Sequencing.cpp -o Sequencing.o
Main.o: $(EXSRC)/Main.cpp $(EXSRC)/IBMCplex/Sequencing.h $(EXSRC)/GA/GeneticAlgorithm.h $(EXSRC)/AssemblyLine/Line.h
	$(CCC) -c $(CCFLAGS) $(EXSRC)/Main.cpp -o Main.o

# Local Variables:
# mode: makefile
# End:
