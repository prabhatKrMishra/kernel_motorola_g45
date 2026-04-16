/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_ENERGY_MODEL_H
#define _LINUX_ENERGY_MODEL_H
#include <linux/cpumask.h>
#include <linux/jump_label.h>
#include <linux/kobject.h>
#include <linux/rcupdate.h>
#include <linux/sched/cpufreq.h>
#include <linux/sched/topology.h>
#include <linux/types.h>

#ifdef CONFIG_ENERGY_MODEL
/**
 * em_cap_state - Capacity state of a performance domain
 * @frequency:	The CPU frequency in KHz, for consistency with CPUFreq
 * @power:	The power consumed by 1 CPU at this level, in milli-watts
 * @cost:	The cost coefficient associated with this level, used during
 *		energy calculation. Equal to: power * max_frequency / frequency
 */
struct em_cap_state {
	unsigned long frequency;
	unsigned long power;
	unsigned long cost;
};

/**
 * em_perf_domain - Performance domain
 * @table:		List of capacity states, in ascending order
 * @nr_cap_states:	Number of capacity states
 * @cpus:		Cpumask covering the CPUs of the domain
 *
 * A "performance domain" represents a group of CPUs whose performance is
 * scaled together. All CPUs of a performance domain must have the same
 * micro-architecture. Performance domains often have a 1-to-1 mapping with
 * CPUFreq policies.
 */
struct em_perf_domain {
	struct em_cap_state *table;
	int nr_cap_states;
	unsigned long cpus[0];
};

#define EM_CPU_MAX_POWER 0xFFFF

/*
 * Increase resolution of energy estimation calculations for 64-bit
 * architectures. The extra resolution improves decision made by EAS for the
 * task placement when two Performance Domains might provide similar energy
 * estimation values (w/o better resolution the values could be equal).
 *
 * We increase resolution only if we have enough bits to allow this increased
 * resolution (i.e. 64-bit). The costs for increasing resolution when 32-bit
 * are pretty high and the returns do not justify the increased costs.
 */
#ifdef CONFIG_64BIT
#define em_scale_power(p) ((p) * 1000)
#else
#define em_scale_power(p) (p)
#endif

struct em_data_callback {
	/**
	 * active_power() - Provide power at the next capacity state of a CPU
	 * @power	: Active power at the capacity state in mW (modified)
	 * @freq	: Frequency at the capacity state in kHz (modified)
	 * @cpu		: CPU for which we do this operation
	 *
	 * active_power() must find the lowest capacity state of 'cpu' above
	 * 'freq' and update 'power' and 'freq' to the matching active power
	 * and frequency.
	 *
	 * The power is the one of a single CPU in the domain, expressed in
	 * milli-watts. It is expected to fit in the [0, EM_CPU_MAX_POWER]
	 * range.
	 *
	 * Return 0 on success.
	 */
	int (*active_power)(unsigned long *power, unsigned long *freq, int cpu);
};
#define EM_DATA_CB(_active_power_cb) { .active_power = &_active_power_cb }

struct em_perf_domain *em_cpu_get(int cpu);
int em_register_perf_domain(cpumask_t *span, unsigned int nr_states,
						struct em_data_callback *cb);

#ifdef CONFIG_CPU_FREQ_GOV_SCHEDUTIL
extern unsigned long schedutil_get_eas_bias(int cpu, unsigned long cap);
#else
static inline unsigned long schedutil_get_eas_bias(int cpu,
						   unsigned long cap)
{
	return 0;
}
#endif

#ifdef CONFIG_CPU_FREQ_GOV_SCHEDUTIL
extern unsigned int util_scale;
#else
#define util_scale SCHED_CAPACITY_SCALE
#endif

/**
 * em_pd_energy() - Estimates the energy consumed by the CPUs of a perf. domain
 * @pd		: performance domain for which energy has to be estimated
 * @max_util	: highest utilization among CPUs of the domain
 * @sum_util	: sum of the utilization of all CPUs in the domain
 *
 * Estimate the energy consumed by a performance domain for the given
 * utilization. The utilization is scaled by the schedutil margin and
 * mapped to the lowest OPP whose capacity satisfies the demand.
 *
 * Capacity is derived from a linear frequency mapping. A per-CPU bias,
 * provided by schedutil, is added to account for deviations from the
 * linear model. The bias is precomputed and retrieved via an O(1) lookup.
 *
 * Return: energy estimate for the domain.
 */
static inline unsigned long em_pd_energy(struct em_perf_domain *pd,
				unsigned long max_util, unsigned long sum_util)
{
	unsigned long scale_cpu;
	struct em_cap_state *cs;
	int i, cpu;
	unsigned long max_freq;
	unsigned long scaled_util;

	if (!sum_util)
		return 0;

	cpu = cpumask_first(to_cpumask(pd->cpus));
	scale_cpu = arch_scale_cpu_capacity(cpu);

	max_freq = pd->table[pd->nr_cap_states - 1].frequency;
	scaled_util = (max_util * util_scale) >> SCHED_CAPACITY_SHIFT;

	if (scaled_util > scale_cpu)
		scaled_util = scale_cpu;

	cs = &pd->table[pd->nr_cap_states - 1];

	for (i = 0; i < pd->nr_cap_states; i++) {
		unsigned long cs_cap;

		cs = &pd->table[i];
		cs_cap = mult_frac(cs->frequency, scale_cpu, max_freq);
		cs_cap += schedutil_get_eas_bias(cpu, cs_cap);
		cs_cap = min(cs_cap, scale_cpu);

		if (cs_cap >= scaled_util)
			break;
	}

	/*
	 * Calculate energy using the selected capacity state (cs).
	 * cs->cost = cs->power * max_freq / cs->frequency
	 * Energy = cs->cost * sum_util / scale_cpu
	 */
	return cs->cost * sum_util / scale_cpu;
}

/**
 * em_pd_nr_cap_states() - Get the number of capacity states of a perf. domain
 * @pd		: performance domain for which this must be done
 *
 * Return: the number of capacity states in the performance domain table
 */
static inline int em_pd_nr_cap_states(struct em_perf_domain *pd)
{
	return pd->nr_cap_states;
}

#else
struct em_perf_domain {};
struct em_data_callback {};
#define EM_DATA_CB(_active_power_cb) { }

static inline int em_register_perf_domain(cpumask_t *span,
			unsigned int nr_states, struct em_data_callback *cb)
{
	return -EINVAL;
}
static inline struct em_perf_domain *em_cpu_get(int cpu)
{
	return NULL;
}
static inline unsigned long em_pd_energy(struct em_perf_domain *pd,
			unsigned long max_util, unsigned long sum_util)
{
	return 0;
}
static inline int em_pd_nr_cap_states(struct em_perf_domain *pd)
{
	return 0;
}
#endif

#endif
