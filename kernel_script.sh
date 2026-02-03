# Copyright (c) 2025 PRABHAT KUMAR MISHR [mprabhat774@gmail.com]
# Permission is granted to use, copy, modify, and distribute this script
# provided that proper credit is given to the original author.
# Unauthorized claiming of authorship is not allowed.

# Setup runtime cpusets
echo "0-7" > /dev/cpuset/top-app/cpus
echo "0-6" > /dev/cpuset/foreground/cpus
echo "4-5" > /dev/cpuset/background/cpus
echo "2-5" > /dev/cpuset/system-background/cpus
echo "2-5" > /dev/cpuset/restricted/cpus
echo "0-7" > /dev/cpuset/h-foreground
echo "0-6" > /dev/cpuset/h-background

# VM Tuning
echo 5 > /proc/sys/vm/dirty_ratio
echo 2 > /proc/sys/vm/dirty_background_ratio
echo 500 > /proc/sys/vm/dirty_writeback_centisecs
echo 300 > /proc/sys/vm/dirty_expire_centisecs

# Swappiness tuning for low cpu consumption
echo 60 > /proc/sys/vm/swappiness
echo 0 > /proc/sys/vm/page-cluster

# Ultra-Low-Latency
echo 0 > /proc/sys/kernel/sched_schedstats

# Disable logging
echo "0" > /proc/sys/debug/exception-trace
echo "0 0 0 0" > /proc/sys/kernel/printk

# Enable powersaving
echo 0 > /sys/module/lpm_levels/parameters/sleep_disabled

# Enable suspend to RAM
echo "deep" > /sys/power/mem_sleep

# Enable console_suspend to save power
echo "Y" > /sys/module/printk/parameters/console_suspend

# Input boost settings
echo 100 > /sys/devices/system/cpu/cpu_boost/input_boost_ms

# Enable KSM
echo 1 > /sys/kernel/mm/ksm/run

# Set the io-scheduler to bfq on all mq support devices
echo "bfq" > /sys/block/sda/queue/scheduler
echo 128 > /sys/block/sda/queue/nr_requests
echo "bfq" > /sys/block/sdb/queue/scheduler
echo 128 > /sys/block/sdb/queue/nr_requests
echo "bfq" > /sys/block/sdc/queue/scheduler
echo 128 > /sys/block/sdc/queue/nr_requests
echo "bfq" > /sys/block/sdd/queue/scheduler
echo 128 > /sys/block/sdd/queue/nr_requests
echo "bfq" > /sys/block/sde/queue/scheduler
echo 128 > /sys/block/sde/queue/nr_requests
echo "bfq" > /sys/block/sdf/queue/scheduler
echo 128 > /sys/block/sdf/queue/nr_requests
echo "bfq" > /sys/class/block/mmcblk1/queue/scheduler
echo 128 > /sys/block/mmcblk1/queue/nr_requests

# Runtime fs tuning
echo 0 > /sys/block/sda/queue/iostats

# Turn off scheduler boost at the end
echo 0 > /proc/sys/kernel/sched_boost

