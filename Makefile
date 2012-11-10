.SUFFIXES:
.DEFAULT: default
.PHONY: default debug release build clean graph config cmake
.NOTPARALLEL:
.SILENT:
.ONESHELL:

BUILD_TYPE := build
BUILD_DIR := build
ifeq ($(findstring debug,${MAKECMDGOALS}),debug)
BUILD_TYPE := debug
BUILD_DIR := debug
else
ifeq ($(findstring release,${MAKECMDGOALS}),release)
BUILD_TYPE := release
BUILD_DIR := release
endif
endif
ifeq ($(findstring cmake,${MAKECMDGOALS}),cmake)
$(warning CMake is no longer used, just run make)
endif

TARGETS := $(filter-out default debug release build clean graph config cmake,${MAKECMDGOALS})
override MAKECMDGOALS :=

default:
	./configure ${BUILD_TYPE}
	ninja ${NINJA_ARGS} -C ${BUILD_DIR} ${TARGETS}

debug: default

release: default

build: default

cmake: config

config:
	./configure build
	./configure release
	./configure debug

clean:
	rm -rf debug release build

graph:
	ninja -C build -t graph ${TARGETS} | sed s\"`pwd`/\"\"g > deps.dot

ifneq ("${TARGETS}","")

% :: default
	@

endif
