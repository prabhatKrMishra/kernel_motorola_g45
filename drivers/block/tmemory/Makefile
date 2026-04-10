# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the Tmemory block device driver
#
# 10 April 2026, Prabhat Kumar Mishra <mprabhat774@gmail.com>
# Rewritten to integrate in kernel tree
#

obj-$(CONFIG_TMEMORY)		+= tmemory.o

tmemory-y := main.o \
         io.o \
         shrinker.o \
         util.o \
         sysfs.o

tmemory-$(CONFIG_TMEMORY_CRYPTO)		+= crypt.o
tmemory-$(CONFIG_TMEMORY_FAULT_INJECTION)	+= fault_inject.o

# ccflags-y += -Wno-incompatible-pointer-types -Wno-format -Wno-unused-variable -Wno-implicit-function-declaration -Wno-int-conversion -Wno-declaration-after-statement