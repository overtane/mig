CPP=g++
CPPFLAGS ?= -DDEBUG -std=c++11 -isystem ${GTEST_DIR}/include -I..
SFLAGS = -Wno-unused-function -Wno-unneeded-internal-declaration
OBJS = mig_test.o
SRCS = mig_test.cpp
GTEST_DIR?=../../googletest/googletest
GTEST_SRC= ${GTEST_DIR}/src/gtest-all.cc
GTEST_OBJ=gtest-all.o

all: libgtest.a testrunner

libgtest.a: ${GTEST_OBJ}
	ar -rv $@ ${GTEST_OBJ} 

${GTEST_OBJ}: ${GTEST_SRC}
	$(CPP) $(CPPFLAGS) -I${GTEST_DIR} -pthread -c ${GTEST_SRC}

$(OBJS): $(SRCS)
	$(CPP) $(CPPFLAGS) -c -o $@ $<

testrunner: libgtest.a $(OBJS)
	$(CPP) $(CPPFLAGS) -o $@ libgtest.a $(OBJS)

clean:
	rm libgtest.a ${GTEST_OBJ}
	rm $(OBJS)
	rm testrunner

