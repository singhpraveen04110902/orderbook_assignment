SRC = $(wildcard *.cpp)
OBJS = $(SRC:.cpp=.o)

all: tester

tester: $(OBJS)
	g++ -W -O2 -I../include -I../../include -L../../lib -lorderBook -std=c++14 -o $@ $^
	mv tester ../bin/
	rm *.o

%.o : %.cpp
	g++ -W -O2 -c -I../include -I../../include -L../../lib -lorderBook -std=c++14 -o $@ $<


clean:
	rm *.o
	rm -f ../bin/tester
