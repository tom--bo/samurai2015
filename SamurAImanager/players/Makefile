CPPFLAGS= -std=c++11 -g
CPP= c++

COMMONOBJECTS= players.o
RANDOMOBJECTS= ${COMMONOBJECTS} randomPlayer.o
TIMEOUTOBJECTS=  ${COMMONOBJECTS} timeoutPlayer.o
TOOACTIVEOBJECTS= ${COMMONOBJECTS} tooActivePlayer.o
NONCHECKINGOBJECTS= ${COMMONOBJECTS} nonCheckingPlayer.o
ABORTINGOBJECTS= ${COMMONOBJECTS} abortingPlayer.o
PLANNINGOBJECTS= planning.o
GREEDYOBJECTS= ${COMMONOBJECTS} ${PLANNINGOBJECTS} greedyPlayer.o
GREEDYOBJECTS0= ${COMMONOBJECTS} ${PLANNINGOBJECTS} greedyPlayer0.o
GREEDYOBJECTS1= ${COMMONOBJECTS} ${PLANNINGOBJECTS} greedyPlayer1.o
GREEDYOBJECTS2= ${COMMONOBJECTS} ${PLANNINGOBJECTS} greedyPlayer2.o
TARGET= randomPlayer greedyPlayer0  greedyPlayer1  greedyPlayer2 greedyPlayer timeoutPlayer tooActivePlayer nonCheckingPlayer abortingPlayer

all: ${TARGET}

randomPlayer: ${RANDOMOBJECTS}
	${CPP} ${CPPFLAGS} -o $@ $^

timeoutPlayer: ${TIMEOUTOBJECTS}
	${CPP} ${CPPFLAGS} -pthread -o $@ $^

tooActivePlayer: ${TOOACTIVEOBJECTS}
	${CPP} ${CPPFLAGS} -o $@ $^

nonCheckingPlayer: ${NONCHECKINGOBJECTS}
	${CPP} ${CPPFLAGS} -o $@ $^

abortingPlayer: ${ABORTINGOBJECTS}
	${CPP} ${CPPFLAGS} -o $@ $^

greedyPlayer: ${GREEDYOBJECTS}
	${CPP} ${CPPFLAGS} -o $@ $^

greedyPlayer0: ${GREEDYOBJECTS0}
	${CPP} ${CPPFLAGS} -o $@ $^

greedyPlayer1: ${GREEDYOBJECTS1}
	${CPP} ${CPPFLAGS} -o $@ $^

greedyPlayer2: ${GREEDYOBJECTS2}
	${CPP} ${CPPFLAGS} -o $@ $^

-include ${OBJECTS:.o=.d}

%.o: %.cpp
	${CPP} -c ${CPPFLAGS} $*.cpp -o $*.o
	${CPP} -MM ${CPPFLAGS} $*.cpp > $*.d

clean:
	rm -f ${TARGET}
	rm -f *.o *.d
	rm -rf *~ */*~ \#*\#
