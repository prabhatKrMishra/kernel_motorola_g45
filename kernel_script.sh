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

# Set RPS (Receive Packet Steering) for each rmnet interface
echo fe > /sys/class/net/rmnet0/queues/rx-0/rps_cpus
echo fe > /sys/class/net/rmnet1/queues/rx-0/rps_cpus
echo fe > /sys/class/net/rmnet2/queues/rx-0/rps_cpus
echo fe > /sys/class/net/rmnet3/queues/rx-0/rps_cpus
echo fe > /sys/class/net/rmnet4/queues/rx-0/rps_cpus
echo fe > /sys/class/net/rmnet5/queues/rx-0/rps_cpus
echo fe > /sys/class/net/rmnet6/queues/rx-0/rps_cpus
echo fe > /sys/class/net/rmnet7/queues/rx-0/rps_cpus

# Pin Wi-Fi RX to both big cores
echo c0 > /sys/class/net/wlan0/queues/rx-0/rps_cpus

# Push control packets earlier (safe, low overhead)
echo 0 > /proc/sys/net/ipv4/tcp_ecn
echo 0 > /proc/sys/net/ipv4/tcp_slow_start_after_idle
echo 1 > /proc/sys/net/ipv4/tcp_no_metrics_save

# ==== VM Tuning 8GB RAM ====
# Cache reclaim
echo 60 > /proc/sys/vm/vfs_cache_pressure

# Dirty writeback
echo 6    > /proc/sys/vm/dirty_ratio
echo 3    > /proc/sys/vm/dirty_background_ratio
echo 1000 > /proc/sys/vm/dirty_writeback_centisecs
echo 3000 > /proc/sys/vm/dirty_expire_centisecs

# Swap behavior
echo 30 > /proc/sys/vm/swappiness
echo 0  > /proc/sys/vm/page-cluster

# Reclaim smoothness
echo 25 > /proc/sys/vm/watermark_scale_factor
echo 131072 > /proc/sys/vm/extra_free_kbytes

# Fragmentation control
echo 20 > /proc/sys/vm/compaction_proactiveness

# ===========================

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

