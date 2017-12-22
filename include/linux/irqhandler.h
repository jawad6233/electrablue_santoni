#ifndef _LINUX_IRQHANDLER_H
#define _LINUX_IRQHANDLER_H

/*
 * Interrupt flow handler typedefs are defined here to avoid circular
 * include dependencies.
 */

struct irq_desc;
struct irq_data;
<<<<<<< HEAD
typedef	void (*irq_flow_handler_t)(unsigned int irq, struct irq_desc *desc);
=======
typedef	bool (*irq_flow_handler_t)(unsigned int irq,
                                   struct irq_desc *desc);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
typedef	void (*irq_preflow_handler_t)(struct irq_data *data);

#endif
