#ifndef _SCHED_DEADLINE_H
#define _SCHED_DEADLINE_H

/*
 * SCHED_DEADLINE tasks has negative priorities, reflecting
 * the fact that any of them has higher prio than RT and
 * NORMAL/BATCH tasks.
 */

#define MAX_DL_PRIO		0

static inline int dl_prio(int prio)
{
	if (unlikely(prio < MAX_DL_PRIO))
		return 1;
	return 0;
}

static inline int dl_task(struct task_struct *p)
{
	return dl_prio(p->prio);
}

<<<<<<< HEAD
static inline bool dl_time_before(u64 a, u64 b)
{
	return (s64)(a - b) < 0;
}

=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
#endif /* _SCHED_DEADLINE_H */
