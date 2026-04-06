MCU_IP_ADDR ?= 192.168.1.71
PROXY_TCP_PORT ?= 3002


THIS_ROOT := $(realpath .)
ifeq "x$(BUILD_DIR)" "x"
   BUILD_DIR ?= $(realpath .)/build
   JS_BLD_DIR ?= $(THIS_ROOT)/njs/build
   JS_BLD_DIR_DEV ?= /tmp/rv/njs/build
else
   JS_BLD_DIR ?= $(BUILD_DIR)/njs/build
   JS_BLD_DIR_DEV ?= /tmp/rv/njs/build
endif

JS_SRC_DIR := $(THIS_ROOT)/njs/src
NJS_DIR :=$(THIS_ROOT)/njs



include sources.mk
include targets.mk
include rules.mk

