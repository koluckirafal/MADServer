CC?=gcc-2.95
CXX?=g++-2.95
RM=rm -f
CPPFLAGS=-std=c++98 -Ishogo_src/Misc -Ishogo_src/AppHeaders
LDFLAGS=
LDLIBS=-ldl

SRCS=main.cc MADServer.cc
OBJS=$(subst .cc,.o,$(SRCS))

all: madserv

madserv: $(OBJS)
	$(CXX) $(LDFLAGS) -o madserv $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend

include .depend
