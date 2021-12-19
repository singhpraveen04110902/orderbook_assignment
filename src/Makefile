SRC = $(wildcard *.cpp)
OBJS = $(SRC:.cpp=.o)

all: ../lib/liborderBook.a


%.o : %.cpp
	g++ -W -O2 -c -I../include -std=c++14 -o $@ $<

../lib/liborderBook.a: $(OBJS)
	ar rcs liborderBook.a $^
	mv liborderBook.a ../lib/
	rm *.o

clean:
	rm -f *.o ../lib/*.a ../bin/feed
