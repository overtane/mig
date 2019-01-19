SRCS = mig_tests.cpp msg_tests.cpp ../migmsg.cpp sampleproto.cpp
OBJS = $(SRCS:.cpp=.o)
GTEST_DIR?=../../googletest/googletest
GTEST_SRC= ${GTEST_DIR}/src/gtest-all.cc
GTEST_OBJ=gtest-all.o

CPP=g++
CPPFLAGS ?= -DDEBUG -std=c++14 -g -isystem ${GTEST_DIR}/include -I..

all: libgtest.a testrunner

libgtest.a: ${GTEST_OBJ}
	ar -rv $@ ${GTEST_OBJ} 

msg_tests.msg.h: msg_tests.msg ../mig
	../mig -o $@ $< 

${GTEST_OBJ}: ${GTEST_SRC}
	$(CPP) $(CPPFLAGS) -I${GTEST_DIR} -pthread -c $<

mig_tests.o: mig_tests.cpp ../mig

msg_tests.o: msg_tests.cpp msg_tests.msg.h ../migmsg.h

testrunner: libgtest.a $(OBJS)
	$(CPP) $(CPPFLAGS) -o $@ libgtest.a $(OBJS)
	./testrunner

clean:
	rm libgtest.a ${GTEST_OBJ}
	rm $(OBJS)
	rm testrunner

