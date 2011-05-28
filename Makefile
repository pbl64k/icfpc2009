CXX = g++
CXXFLAGS = -ansi -pedantic -Wall -Wextra \
	-O4 \
	-ffast-math
HPRJ = om.hpp
HACT = act.hpp orcl.hpp sptn.hpp phys.hpp page.hpp $(HPRJ)
HALL = proc.hpp actm.hpp 3clc.hpp 2clc.hpp 1wkd.hpp dumb.hpp $(HACT)
OALL = main.o proc.o actm.o 3clc.o 2clc.o 1wkd.o dumb.o act.o orcl.o sptn.o phys.o page.o

om : $(OALL)
	$(CXX) -o om $(OALL)

.PHONY : clean all

clean :
	-rm om $(OALL)

all : om

main.o : $(HALL)

proc.o : $(HALL)

actm.o : actm.hpp 3clc.hpp 2clc.hpp 1wkd.hpp dumb.hpp $(HACT)

3clc.o : 3clc.hpp 2clc.hpp 1wkd.hpp $(HACT)

2clc.o : 2clc.hpp 1wkd.hpp $(HACT)

1wkd.o : 1wkd.hpp $(HACT)

dumb.o : dumb.hpp $(HACT)

act.o : $(HACT)

orcl.o : $(HPRJ)

phys.o : phys.hpp $(HPRJ)

page.o : page.hpp $(HPRJ)

