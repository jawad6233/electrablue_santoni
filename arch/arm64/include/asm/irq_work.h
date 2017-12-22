#ifndef __ASM_IRQ_WORK_H
#define __ASM_IRQ_WORK_H

<<<<<<< HEAD
#ifdef CONFIG_SMP

=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
#include <asm/smp.h>

static inline bool arch_irq_work_has_interrupt(void)
{
	return !!__smp_cross_call;
}

<<<<<<< HEAD
#else

static inline bool arch_irq_work_has_interrupt(void)
{
	return false;
}

#endif

=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
#endif /* __ASM_IRQ_WORK_H */
