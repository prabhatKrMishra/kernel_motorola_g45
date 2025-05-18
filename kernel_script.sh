# Copyright (c) 2025 PRABHAT KUMAR MISHR [mprabhat774@gmail.com]
# Permission is granted to use, copy, modify, and distribute this script
# provided that proper credit is given to the original author.
# Unauthorized claiming of authorship is not allowed.

# Enable tcp_low_latency for even faster ACKs
echo 1 > /proc/sys/net/ipv4/tcp_low_latency

# Enable TCP Fast Open
echo 3 > /proc/sys/net/ipv4/tcp_fastopen

# Disable TCP Slow Start
echo 1 > /proc/sys/net/ipv4/tcp_slow_start_after_idle

# Use the "best-effort" scheduler for the network
echo 1 > /proc/sys/net/core/netdev_budget

# Set RPS (Receive Packet Steering) for each rmnet interface
echo fe > /sys/class/net/rmnet0/queues/rx-0/rps_cpus
echo fe > /sys/class/net/rmnet1/queues/rx-0/rps_cpus
echo fe > /sys/class/net/rmnet2/queues/rx-0/rps_cpus
echo fe > /sys/class/net/rmnet3/queues/rx-0/rps_cpus
echo fe > /sys/class/net/rmnet4/queues/rx-0/rps_cpus
echo fe > /sys/class/net/rmnet5/queues/rx-0/rps_cpus
echo fe > /sys/class/net/rmnet6/queues/rx-0/rps_cpus
echo fe > /sys/class/net/rmnet7/queues/rx-0/rps_cpus

# Set governor settings for CPU scaling
echo "schedutil" > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
echo 500 > /sys/devices/system/cpu/cpufreq/policy0/schedutil/up_rate_limit_us
echo 1000 > /sys/devices/system/cpu/cpufreq/policy0/schedutil/down_rate_limit_us

echo "schedutil" > /sys/devices/system/cpu/cpufreq/policy6/scaling_governor
echo 500 > /sys/devices/system/cpu/cpufreq/policy6/schedutil/up_rate_limit_us
echo 1000 > /sys/devices/system/cpu/cpufreq/policy6/schedutil/down_rate_limit_us

# Memory optimization
echo 5 > /proc/sys/vm/dirty_ratio
echo 3 > /proc/sys/vm/dirty_background_ratio
echo 500 > /proc/sys/vm/dirty_writeback_centisecs
echo 200 > /proc/sys/vm/dirty_expire_centisecs

# Disable Core control on silver cluster
echo 0 > /sys/devices/system/cpu/cpu0/core_ctl/enable

# Ultra-Low-Latency
echo 0 > /proc/sys/kernel/sched_schedstats

# Disable logging
echo "0" > /proc/sys/debug/exception-trace
echo "0 0 0 0" > /proc/sys/kernel/printk

# Enable suspend to RAM
echo "deep" > /sys/power/mem_sleep

# Enable console_suspend to save power
echo "Y" > /sys/module/printk/parameters/console_suspend

# Input boost settings
echo 100 > /sys/devices/system/cpu/cpu_boost/input_boost_ms

# Runtime fs tuning
echo 0 > /sys/block/sda/queue/iostats

# Turn off scheduler boost at the end
echo 0 > /proc/sys/kernel/sched_boost

