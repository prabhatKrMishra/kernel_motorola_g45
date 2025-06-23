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

# Low latency for gaming
echo 128 > /proc/sys/net/core/netdev_budget

# Set default and maximum receive buffer sizes
echo 1310720 > /proc/sys/net/core/rmem_default
echo 8388608 > /proc/sys/net/core/rmem_max

# Set default and maximum send buffer sizes
echo 327680 > /proc/sys/net/core/wmem_default
echo 8388608 > /proc/sys/net/core/wmem_max

# Set maximum size for ancillary data and options
echo 20480 > /proc/sys/net/core/optmem_max

# Increase network device input backlog
echo 10000 > /proc/sys/net/core/netdev_max_backlog

# Set TCP receive buffer sizes (min default max)
echo "2097152 4194304 8388608" > /proc/sys/net/ipv4/tcp_rmem

# Set TCP send buffer sizes (min default max)
echo "262144 524288 8388608" > /proc/sys/net/ipv4/tcp_wmem

# Set total TCP memory thresholds (low pressure high - in pages)
echo "44259 59012 88518" > /proc/sys/net/ipv4/tcp_mem

# Set total UDP memory thresholds (low pressure high - in pages)
echo "88518 118025 177036" > /proc/sys/net/ipv4/udp_mem

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

# Disable Core control on both clusters
echo 0 > /sys/devices/system/cpu/cpu0/core_ctl/enable
echo 0 > /sys/devices/system/cpu/cpu6/core_ctl/enable

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

# Set the io-scheduler to ssg on all mq support devices
echo "ssg" > /sys/block/sda/queue/scheduler
echo "ssg" > /sys/block/sdb/queue/scheduler
echo "ssg" > /sys/block/sdc/queue/scheduler
echo "ssg" > /sys/block/sdd/queue/scheduler
echo "ssg" > /sys/block/sde/queue/scheduler
echo "ssg" > /sys/block/sdf/queue/scheduler
echo "ssg" > /sys/class/block/mmcblk1/queue/scheduler

# Runtime fs tuning
echo 0 > /sys/block/sda/queue/iostats

# Turn off scheduler boost at the end
echo 0 > /proc/sys/kernel/sched_boost

