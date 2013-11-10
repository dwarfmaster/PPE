PROG=testcv.prog
CXXFLAGS=-Wall -Wextra -pedantic -std=c++11 `pkg-config --cflags opencv`
LDFLAGS=`pkg-config --libs opencv`
OBJS=main.o calib.o

all : $(PROG)

$(PROG) : $(OBJS)
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o : %.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

clean :
	rm -vf *.o

rec : clean all

.PHONY:rec clean all


