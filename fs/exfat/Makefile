# SPDX-License-Identifier: GPL-2.0-or-later
#
# Makefile for the linux exFAT filesystem support.
#
obj-$(CONFIG_EXFAT_FS) += exfat.o

exfat-y	:= inode.o namei.o dir.o super.o fatent.o cache.o nls.o misc.o \
	   file.o balloc.o

KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(shell pwd) modules $(KBUILD_OPTIONS)

modules_install:
	$(MAKE) INSTALL_MOD_STRIP=1 -C $(KERNEL_SRC) M=$(shell pwd) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean
