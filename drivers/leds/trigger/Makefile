# SPDX-License-Identifier: GPL-2.0
obj-$(CONFIG_LEDS_TRIGGER_TIMER)	+= ledtrig-timer.o
obj-$(CONFIG_LEDS_TRIGGER_ONESHOT)	+= ledtrig-oneshot.o
obj-$(CONFIG_LEDS_TRIGGER_DISK)		+= ledtrig-disk.o
obj-$(CONFIG_LEDS_TRIGGER_MTD)		+= ledtrig-mtd.o
obj-$(CONFIG_LEDS_TRIGGER_HEARTBEAT)	+= ledtrig-heartbeat.o
obj-$(CONFIG_LEDS_TRIGGER_BACKLIGHT)	+= ledtrig-backlight.o
obj-$(CONFIG_LEDS_TRIGGER_GPIO)		+= ledtrig-gpio.o
obj-$(CONFIG_LEDS_TRIGGER_CPU)		+= ledtrig-cpu.o
obj-$(CONFIG_LEDS_TRIGGER_ACTIVITY)	+= ledtrig-activity.o
obj-$(CONFIG_LEDS_TRIGGER_DEFAULT_ON)	+= ledtrig-default-on.o
obj-$(CONFIG_LEDS_TRIGGER_TRANSIENT)	+= ledtrig-transient.o
obj-$(CONFIG_LEDS_TRIGGER_CAMERA)	+= ledtrig-camera.o
obj-$(CONFIG_LEDS_TRIGGER_PANIC)	+= ledtrig-panic.o
obj-$(CONFIG_LEDS_TRIGGER_NETDEV)	+= ledtrig-netdev.o
obj-$(CONFIG_LEDS_TRIGGER_PATTERN)	+= ledtrig-pattern.o
obj-$(CONFIG_LEDS_TRIGGER_AUDIO)	+= ledtrig-audio.o

KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(M) modules $(KBUILD_OPTIONS)

modules_install:
	$(MAKE) INSTALL_MOD_STRIP=1 -C $(KERNEL_SRC) M=$(M) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean
