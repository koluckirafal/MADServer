RM:=rm -f
CC?=gcc
CXX?=g++
CONTAINER_TOOL:=$(shell command -v podman >/dev/null 2>&1 && echo podman || echo docker)

BUILD_REVISION?=$(shell git describe --tags --always --dirty 2> /dev/null)
CONTAINER_IMAGE:=localhost/madserver-gcc2.95
SRC_DIR:=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))
BUILD_DIR:=/mnt/build
IMAGE_EXISTS=$(shell $(CONTAINER_TOOL) images -q $(CONTAINER_IMAGE))

DATA_DIR ?= ~/shogo
DATA_MOUNT=/mnt/shogo

USER_ID=$(shell id -u)
GROUP_ID=$(shell id -g)

CPPFLAGS:=-std=c++98 -I. -Itclap/include -Ishogo_src/Misc -Ishogo_src/AppHeaders -g -pthread
LDFLAGS:=-pthread -g
LDLIBS:=-ldl -lpthread

SRCS=main.cc MADServer.cc Logger.cc Utils.cc GameVariables.cc
HDRS=MADServer.h Logger.h Utils.h GameVariables.h consts.h build.h
OBJS=$(subst .cc,.o,$(SRCS))

.PHONY: all lint format docker-make docker-build depend clean distclean

all: docker-make

build.h:
	printf "#ifndef _BUILD_H\n#define _BUILD_H\n" > $@
	printf "#define BUILD_REVISION \"$(BUILD_REVISION)\"\n" >> $@
	printf "#define LITH_GAME \"shogo\"\n" >> $@
	printf "#endif // _BUILD_H\n" >> $@

madserv: $(OBJS) $(HDRS)
	$(CXX) $(LDFLAGS) -o madserv $(OBJS) $(LDLIBS)

compile_commands.json:
	bear -- make madserv

lint: compile_commands.json
	clang-tidy $(SRCS)

format:
	clang-format -style=microsoft -i $(SRCS) $(HDRS)

depend: .depend

.depend: $(SRCS) $(HDRS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^ >> ./.depend;

docker-make: docker-build
	$(CONTAINER_TOOL) run --rm --pull never -v $(SRC_DIR):$(BUILD_DIR) \
		$(if $(filter podman,$(CONTAINER_TOOL)),--userns=keep-id,--user $(USER_ID):$(GROUP_ID)) \
		-w $(BUILD_DIR) $(CONTAINER_IMAGE) \
		make madserv CC=gcc-2.95 CXX=g++-2.95 BUILD_REVISION=$(BUILD_REVISION)

docker-build:
	@if [ -z "$(IMAGE_EXISTS)" ]; then \
		echo "Building $(CONTAINER_TOOL) image $(CONTAINER_IMAGE)..."; \
		$(CONTAINER_TOOL) build -t $(CONTAINER_IMAGE) .; \
	else \
		echo "$(CONTAINER_TOOL) image $(CONTAINER_IMAGE) already exists."; \
	fi

docker-gdb: docker-build
	$(CONTAINER_TOOL) run --rm -it \
		-v $(SRC_DIR):$(BUILD_DIR) -v $(DATA_DIR):$(DATA_MOUNT) -w $(DATA_MOUNT) \
		$(if $(filter podman,$(CONTAINER_TOOL)),--userns=keep-id,--user $(USER_ID):$(GROUP_ID)) \
		--cap-add=SYS_PTRACE --security-opt seccomp=unconfined \
		-e LD_LIBRARY_PATH=$(DATA_MOUNT) \
		$(CONTAINER_IMAGE) gdb --args $(BUILD_DIR)/madserv

docker-clean:
	@if [ -n "$(IMAGE_EXISTS)" ]; then \
		echo "Removing $(CONTAINER_TOOL) image $(CONTAINER_IMAGE)..."; \
		$(CONTAINER_TOOL) rmi -f $(CONTAINER_IMAGE); \
	else \
		echo "$(CONTAINER_TOOL) image $(CONTAINER_IMAGE) not found."; \
	fi

clean:
	$(RM) $(OBJS) build.h

distclean: clean
	$(RM) *~ .depend compile_commands.json

include .depend
