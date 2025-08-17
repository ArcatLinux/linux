// SPDX-License-Identifier: GPL-2.0-only
/*
 * Uniprocessor-only support functions.  The counterpart to kernel/smp.c
 */

#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/export.h>
#include <linux/smp.h>
#include <linux/hypervisor.h>

static inline int up_call_helper(void (*func)(void *), void *info)
{
    unsigned long flags;
    if (!func)
        return -EINVAL;

    local_irq_save(flags);
    func(info);
    local_irq_restore(flags);

    return 0;
}

int smp_call_function_single(int cpu, void (*func)(void *info), void *info, int wait)
{
    if (cpu != 0)
        return -ENXIO; 
    return up_call_helper(func, info);
}
EXPORT_SYMBOL(smp_call_function_single);

int smp_call_function_single_async(int cpu, call_single_data_t *csd)
{
    if (!csd)
        return -EINVAL;
    if (cpu != 0)
        return -ENXIO;
    return up_call_helper(csd->func, csd->info);
}
EXPORT_SYMBOL(smp_call_function_single_async);


/*
 * Preemption is disabled here to make sure the cond_func is called under the
 * same conditions in UP and SMP.
 */
void on_each_cpu_cond_mask(smp_cond_func_t cond_func, smp_call_func_t func,
			   void *info, bool wait, const struct cpumask *mask)
{
	unsigned long flags;

	preempt_disable();
	if ((!cond_func || cond_func(0, info)) && cpumask_test_cpu(0, mask)) {
		local_irq_save(flags);
		func(info);
		local_irq_restore(flags);
	}
	preempt_enable();
}
EXPORT_SYMBOL(on_each_cpu_cond_mask);

int smp_call_on_cpu(unsigned int cpu, int (*func)(void *), void *par, bool phys)
{
	int ret;

	if (cpu != 0)
		return -ENXIO;

	if (phys)
		hypervisor_pin_vcpu(0);
	ret = func(par);
	if (phys)
		hypervisor_pin_vcpu(-1);

	return ret;
}
EXPORT_SYMBOL_GPL(smp_call_on_cpu);
