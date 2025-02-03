RM:=rm -f
CC?=gcc
CXX?=g++
CONTAINER_TOOL?=docker

CONTAINER_IMAGE:=localhost/madserver-gcc2.95
SRC_DIR:=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))
BUILD_DIR:=/mnt/build
IMAGE_EXISTS=$(shell $(CONTAINER_TOOL) images -q $(CONTAINER_IMAGE))
USER_ID=$(shell id -u)
GROUP_ID=$(shell id -g)

CPPFLAGS:=-std=c++98 -Ishogo_src/Misc -Ishogo_src/AppHeaders
LDFLAGS:=
LDLIBS:=-ldl

SRCS=main.cc MADServer.cc Logger.cc
HDRS=MADServer.h Logger.h
OBJS=$(subst .cc,.o,$(SRCS))

.PHONY: all lint format docker-make docker-build depend clean distclean

all: docker-make

madserv: $(OBJS)
	$(CXX) $(LDFLAGS) -o madserv $(OBJS) $(LDLIBS)

compile_commands.json:
	bear -- make madserv

lint: compile_commands.json
	clang-tidy $(SRCS)

format:
	clang-format -style=microsoft -i $(SRCS) $(HDRS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^ >> ./.depend;

docker-make: docker-build
	$(CONTAINER_TOOL) run --rm --pull never -v $(SRC_DIR):$(BUILD_DIR) \
		$(if $(filter podman,$(CONTAINER_TOOL)),--userns=keep-id,--user $(USER_ID):$(GROUP_ID)) \
		-w $(BUILD_DIR) $(CONTAINER_IMAGE) \
		make madserv CC=gcc-2.95 CXX=g++-2.95

docker-build:
	@if [ -z "$(IMAGE_EXISTS)" ]; then \
		echo "Building $(CONTAINER_TOOL) image $(CONTAINER_IMAGE)..."; \
		$(CONTAINER_TOOL) build -t $(CONTAINER_IMAGE) .; \
	else \
		echo "$(CONTAINER_TOOL) image $(CONTAINER_IMAGE) already exists."; \
	fi

docker-clean:
	@if [ -n "$(IMAGE_EXISTS)" ]; then \
		echo "Removing $(CONTAINER_TOOL) image $(CONTAINER_IMAGE)..."; \
		$(CONTAINER_TOOL) rmi -f $(CONTAINER_IMAGE); \
	else \
		echo "$(CONTAINER_TOOL) image $(CONTAINER_IMAGE) not found."; \
	fi

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend compile_commands.json

include .depend
