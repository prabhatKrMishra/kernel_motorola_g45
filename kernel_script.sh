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

# Network response is more important than network efficiency
echo 64 > /proc/sys/net/core/netdev_budget
echo 7000 > /proc/sys/net/core/netdev_budget_usecs
echo 1 > /proc/sys/net/ipv4/tcp_low_latency
echo 16384 > /proc/sys/net/ipv4/tcp_notsent_lowat

# Set governor settings for CPU scaling
echo "schedutil" > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
echo 500 > /sys/devices/system/cpu/cpufreq/policy0/schedutil/up_rate_limit_us
echo 1000 > /sys/devices/system/cpu/cpufreq/policy0/schedutil/down_rate_limit_us

echo "schedutil" > /sys/devices/system/cpu/cpufreq/policy6/scaling_governor
echo 500 > /sys/devices/system/cpu/cpufreq/policy6/schedutil/up_rate_limit_us
echo 1000 > /sys/devices/system/cpu/cpufreq/policy6/schedutil/down_rate_limit_us

# Use 10ms polling_interval for silver and gold latfloor
echo 50 > /sys/devices/platform/soc/soc:qcom,cpu-cpu-ddr-bw/devfreq/soc:qcom,cpu-cpu-ddr-bw/polling_interval
echo 10 > /sys/devices/platform/soc/soc:qcom,cpu0-cpu-ddr-lat/devfreq/soc:qcom,cpu0-cpu-ddr-lat/polling_interval
echo 10 > /sys/devices/platform/soc/soc:qcom,cpu6-cpu-ddr-lat/devfreq/soc:qcom,cpu6-cpu-ddr-lat/polling_interval
echo 10 > /sys/devices/platform/soc/soc:qcom,cpu0-cpu-ddr-latfloor/devfreq/soc:qcom,cpu0-cpu-ddr-latfloor/polling_interval
echo 10 > /sys/devices/platform/soc/soc:qcom,cpu6-cpu-ddr-latfloor/devfreq/soc:qcom,cpu6-cpu-ddr-latfloor/polling_interval

# VM Tuning
echo 5 > /proc/sys/vm/dirty_ratio
echo 2 > /proc/sys/vm/dirty_background_ratio
echo 500 > /proc/sys/vm/dirty_writeback_centisecs
echo 300 > /proc/sys/vm/dirty_expire_centisecs

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

