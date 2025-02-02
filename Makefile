RM:=rm -f
CC?=gcc
CXX?=g++
DOCKER?=docker

DOCKER_IMAGE:=madserver-gcc2.95
SRC_DIR:=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))
BUILD_DIR:=/mnt/build
IMAGE_EXISTS=$(shell $(DOCKER) images -q $(DOCKER_IMAGE))

CPPFLAGS:=-std=c++98 -Ishogo_src/Misc -Ishogo_src/AppHeaders
LDFLAGS:=
LDLIBS:=-ldl

SRCS=main.cc MADServer.cc
OBJS=$(subst .cc,.o,$(SRCS))

.PHONY: all madserv docker-make docker-build depend clean distclean

all: docker-make

madserv: $(OBJS)
	$(CXX) $(LDFLAGS) -o madserv $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^ >> ./.depend;

docker-make: docker-build
	docker run --rm -v $(SRC_DIR):$(BUILD_DIR) \
		-w $(BUILD_DIR) $(DOCKER_IMAGE) \
		make madserv CC=gcc-2.95 CXX=g++-2.95

docker-build:
	@if [ -z "$(IMAGE_EXISTS)" ]; then \
		echo "Building Docker image $(DOCKER_IMAGE)..."; \
		docker build -t $(DOCKER_IMAGE) .; \
	else \
		echo "Docker image $(DOCKER_IMAGE) already exists."; \
	fi

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend

include .depend
