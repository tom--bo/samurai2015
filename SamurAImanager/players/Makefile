CPPFLAGS= -std=c++11 -g
CPP= c++

COMMONOBJECTS= players.o
PLANNINGOBJECTS= planning.o
GREEDYOBJECTS= ${COMMONOBJECTS} ${PLANNINGOBJECTS} greedyPlayer.o
GREEDYOBJECTS0= ${COMMONOBJECTS} ${PLANNINGOBJECTS} greedyPlayer0.o
GREEDYOBJECTS1= ${COMMONOBJECTS} ${PLANNINGOBJECTS} greedyPlayer1.o
GREEDYOBJECTS2= ${COMMONOBJECTS} ${PLANNINGOBJECTS} greedyPlayer2.o
GREEDYOBJECTS3= ${COMMONOBJECTS} ${PLANNINGOBJECTS} greedyPlayer3.o
GREEDYOBJECTS4= ${COMMONOBJECTS} ${PLANNINGOBJECTS} greedyPlayer4.o
GREEDYOBJECTS5= ${COMMONOBJECTS} ${PLANNINGOBJECTS} greedyPlayer5.o
FILEOBJECTS= ${COMMONOBJECTS} ${PLANNINGOBJECTS} filePlayer.o
 
TARGET= greedyPlayer0  greedyPlayer1  greedyPlayer2 greedyPlayer3 greedyPlayer4 greedyPlayer5 greedyPlayer filePlayer

all: ${TARGET}

greedyPlayer: ${GREEDYOBJECTS}
	${CPP} ${CPPFLAGS} -o $@ $^

greedyPlayer0: ${GREEDYOBJECTS0}
	${CPP} ${CPPFLAGS} -o $@ $^

greedyPlayer1: ${GREEDYOBJECTS1}
	${CPP} ${CPPFLAGS} -o $@ $^

greedyPlayer2: ${GREEDYOBJECTS2}
	${CPP} ${CPPFLAGS} -o $@ $^

greedyPlayer3: ${GREEDYOBJECTS3}
	${CPP} ${CPPFLAGS} -o $@ $^

greedyPlayer4: ${GREEDYOBJECTS4}
	${CPP} ${CPPFLAGS} -o $@ $^

greedyPlayer5: ${GREEDYOBJECTS5}
	${CPP} ${CPPFLAGS} -o $@ $^

filePlayer: ${FILEOBJECTS}
	${CPP} ${CPPFLAGS} -o $@ $^


-include ${OBJECTS:.o=.d}

%.o: %.cpp
	${CPP} -c ${CPPFLAGS} $*.cpp -o $*.o
	${CPP} -MM ${CPPFLAGS} $*.cpp > $*.d

clean:
	rm -f ${TARGET}
	rm -f *.o *.d
	rm -rf *~ */*~ \#*\#
